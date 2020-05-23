/*
 * glcd.c
 *
 *  Created on: 01.04.2020
 *      Author: matti
 */


/*
 * Include ChibiOS & HAL
 */
#include "ch.h"
#include "hal.h"
#include "shell.h"
#include "chprintf.h"

/*
 * Includes module API, types & config
 */
#include "ui/glcd.h"
#include "ui/glcd_cmd.h"

/*
 * Include dependencies
 */
#include "ui/ui.h"
#include <stdlib.h>
#include "u8g2.h"

/*
 * Global variables
 */

/*
 * Forward declarations of static functions
 */
static void             _glcd_init_hal            (void);
static void             _glcd_init_module         (void);
static void             _glcd_init_display        (void);
static void             _glcd_set_bl              (uint8_t duty_cycle);
static void             _glcd_draw_tbox           (ui_object_t * object);
static void             _glcd_draw_value          (ui_object_t * object);
static uint8_t          _glcd_process_objects     (void);
static uint8_t          _glcd_u8g2_hw_spi         (u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
static uint8_t 	        _glcd_u8g2_gpio_and_delay (U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);

/*
 * Static variables
 */
static PWMConfig _glcd_pwmd_cfg = {
  GLCD_PWM_TIMER_FREQ,
  GLCD_PWM_PERIOD_TICKS,
  NULL,
  {
      GLCD_PWM_CHANNEL_CFG
  },
  0,
  0,
#if STM32_PWM_USE_ADVANCED
  0
#endif
};

static const SPIConfig _glcd_spid_cfg = {
  false,
  NULL,              // No finish callback
  GLCD_SS_LINE,      // Use config based IO line for SS
  GLCD_SPI_CR1,      // SPI_CR1
  GLCD_SPI_CR2       // SPI_CR2
};

static THD_WORKING_AREA(_glcd_update_stack, GLCD_UPDATE_THREAD_STACK);

static u8g2_t              _glcd_u8g2;
static ui_object_state_t   _glcd_screen_state = UI_STATE_DIRTY;
static ui_object_t *       _glcd_current_object_list = NULL;
static uint8_t             _glcd_msg_buffer[2148];
static uint16_t            _glcd_msg_buffer_idx = 0;
/*
 * Tasks
 */
static __attribute__((noreturn)) THD_FUNCTION(_glcd_update_thread, arg)
{

  (void)arg;

  chRegSetThreadName("glcd_update");
  chThdSleepMilliseconds(GLCD_UPDATE_THREAD_P_MS);

  while (true)
  {
    systime_t time = chVTGetSystemTimeX();

    if(_glcd_process_objects())
    {
      spiAcquireBus(GLCD_SPI_DRIVER);
      u8g2_SendBuffer(&_glcd_u8g2);
      spiReleaseBus(GLCD_SPI_DRIVER);
    }
    chThdSleepUntilWindowed(time, time + TIME_MS2I(GLCD_UPDATE_THREAD_P_MS));
  }
}

/*
 * Static helper functions
 */
static void _glcd_init_hal(void)
{
  /*
   * Configure PWM for backlight
   */
  pwmStart(GLCD_PWM_TIMER_DRIVER, &_glcd_pwmd_cfg);
  palSetLineMode(GLCD_BLPWM_LINE, PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
  _glcd_set_bl(GLCD_PWM_DEFAULT);

  /*
   * Configure SPI driver
   */
  palSetLineMode(GLCD_SCK_LINE,   PAL_MODE_STM32_ALTERNATE_PUSHPULL);     // SCK
  palSetLineMode(GLCD_MOSI_LINE,  PAL_MODE_STM32_ALTERNATE_PUSHPULL);     // MOSI
  palSetLineMode(GLCD_SS_LINE,    PAL_MODE_OUTPUT_PUSHPULL);              // SS
  palSetLineMode(GLCD_RESET_LINE, PAL_MODE_OUTPUT_PUSHPULL);              // Reset
}

static void _glcd_init_module(void)
{
  _glcd_init_display();
  chThdCreateStatic(_glcd_update_stack, sizeof(_glcd_update_stack), GLCD_UPDATE_THREAD_PRIO, _glcd_update_thread, NULL);
}

static void _glcd_init_display(void)
{
  spiAcquireBus(GLCD_SPI_DRIVER);
  spiStart(GLCD_SPI_DRIVER, &_glcd_spid_cfg);
  spiSelect(GLCD_SPI_DRIVER);

  u8g2_Setup_st7920_s_128x64_f(&_glcd_u8g2, U8G2_R0, _glcd_u8g2_hw_spi, _glcd_u8g2_gpio_and_delay);
  u8g2_InitDisplay(&_glcd_u8g2); // send init sequence to the display, display is in sleep mode after this,
  u8g2_SetPowerSave(&_glcd_u8g2, 0); // wake up display

  spiReleaseBus(GLCD_SPI_DRIVER);
}

static void _glcd_set_bl(uint8_t duty_cycle)
{
  duty_cycle = (duty_cycle > 100 ) ? 100 : duty_cycle;
  pwmEnableChannel(GLCD_PWM_TIMER_DRIVER, 0, PWM_PERCENTAGE_TO_WIDTH(GLCD_PWM_TIMER_DRIVER, duty_cycle*100));
}

static void _glcd_draw_tbox(ui_object_t * object)
{
  ui_object_tbox_t * tbox = object->content;
  uint8_t text_x_pos = object->x_pos + tbox->t_x_offset;
  uint8_t text_y_pos = object->y_pos + tbox->t_y_offset;

  u8g2_DrawFrame(&_glcd_u8g2,
      object->x_pos, object->y_pos,
      tbox->width, tbox->height);

  u8g2_SetDrawColor(&_glcd_u8g2, ((object->mode == UI_MODE_FOCUS) ? 1 : 0));
  u8g2_DrawFrame(&_glcd_u8g2,
      object->x_pos + 1, object->y_pos + 1,
      tbox->width - 2, tbox->height - 2);

  u8g2_SetDrawColor(&_glcd_u8g2, ((object->mode == UI_MODE_ACTIVE) ? 1 : 0));
  u8g2_DrawBox(&_glcd_u8g2,
      object->x_pos + 2, object->y_pos + 2,
      tbox->width - 4, tbox->height - 4);

  u8g2_SetDrawColor(&_glcd_u8g2, ((object->mode == UI_MODE_ACTIVE) ? 0 : 1));
  u8g2_SetFont(&_glcd_u8g2, tbox->font);

  u8g2_DrawStr(&_glcd_u8g2,
      text_x_pos, text_y_pos,
      tbox->text);

  u8g2_SetDrawColor(&_glcd_u8g2, 1);

}

static void _glcd_draw_value(ui_object_t * object)
{
  ui_object_value_t * value = object->content;
  u8g2_SetFont(&_glcd_u8g2, value->font);

  char buffer[GLCD_UI_KEY_LENGTH + GLCD_UI_VAL_LENGTH];
  switch(value->type)
  {
  case UI_KV_TYPE_UINT:
    chsnprintf(buffer, GLCD_UI_KEY_LENGTH + GLCD_UI_VAL_LENGTH,
        value->fmt, *((uint32_t *)value->value));
    break;
  case UI_KV_TYPE_INT:
    chsnprintf(buffer, GLCD_UI_KEY_LENGTH + GLCD_UI_VAL_LENGTH,
        value->fmt, *((int32_t *)value->value));
    break;
  case UI_KV_TYPE_FLOAT:
    chsnprintf(buffer, GLCD_UI_KEY_LENGTH + GLCD_UI_VAL_LENGTH,
        value->fmt, *((float *)value->value));
    break;
  case UI_KV_TYPE_STR:
    chsnprintf(buffer, GLCD_UI_KEY_LENGTH + GLCD_UI_VAL_LENGTH,
        value->fmt);
    break;
  default:
    break;
  }

  u8g2_SetFontMode(&_glcd_u8g2, 0);
  u8g2_SetDrawColor(&_glcd_u8g2, ((object->mode == UI_MODE_ACTIVE) ? 0 : 1));
  u8g2_DrawStr(&_glcd_u8g2,
      object->x_pos,
      object->y_pos,
      buffer);
  u8g2_SetFontMode(&_glcd_u8g2, 1);
  u8g2_SetDrawColor(&_glcd_u8g2, 1);

}

static void _glcd_draw_bitmap(ui_object_t * object)
{
  ui_object_bitmap_t * bitmap = object->content;
  u8g2_DrawBitmap(&_glcd_u8g2,
      object->x_pos, object->y_pos,
      bitmap->cnt,
      bitmap->height,
      bitmap->bitmap);
}

static uint8_t _glcd_process_objects(void)
{
  ui_object_t *       current = _glcd_current_object_list;
  ui_object_state_t   screen_state = UI_STATE_DIRTY;
  uint8_t ret = 0;

  chSysLock();
  screen_state = _glcd_screen_state;
  chSysUnlock();

  if (current)
  {
    while(current)
    {
      if(current->state == UI_STATE_DIRTY || screen_state == UI_STATE_DIRTY)
      {
        switch(current->class)
        {
        case UI_OBJECT_T_BOX:   _glcd_draw_tbox  (current);  break;
        case UI_OBJECT_VALUE:   _glcd_draw_value (current);  break;
        case UI_OBJECT_BITMAP:  _glcd_draw_bitmap(current);  break;
        default:  break;
        }
        ret = 1;
        current->state = UI_STATE_CLEAN;
      }
      current = current->next;
    }
  }
  else
  {
    if(screen_state == UI_STATE_DIRTY)
    {
      u8g2_ClearBuffer(&_glcd_u8g2);
      ret = 1;
    }
  }
  chSysLock();
  _glcd_screen_state = (screen_state != _glcd_screen_state) ?
      UI_STATE_DIRTY : UI_STATE_CLEAN;
  chSysUnlock();

  return ret;
}

/*
 * Callback functions
 */
static uint8_t _glcd_u8g2_hw_spi(U8X8_UNUSED u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{

  switch(msg)
  {
    case U8X8_MSG_BYTE_SEND:
      memcpy(&_glcd_msg_buffer[_glcd_msg_buffer_idx], arg_ptr, arg_int);
      _glcd_msg_buffer_idx += arg_int;
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
      _glcd_msg_buffer_idx = 0;
      spiUnselect(GLCD_SPI_DRIVER);
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:
      spiSend(GLCD_SPI_DRIVER, _glcd_msg_buffer_idx, _glcd_msg_buffer);
      spiSelect(GLCD_SPI_DRIVER);
      break;
    default:
      return 0;
  }
  return 1;
}

static uint8_t _glcd_u8g2_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
  switch(msg)
  {
  //Function to define the logic level of the RESET line
    case U8X8_MSG_DELAY_NANO:
      {
        uint16_t i = 0;
        for(i=0; i < arg_int; i++)
                asm volatile("nop");
      }
      break;
    case U8X8_MSG_GPIO_RESET:
      if (arg_int) palSetLine(GLCD_RESET_LINE);
      else palClearLine(GLCD_RESET_LINE);
      break;
    default:
      return 1;
  }

  return 1; // command processed successfully.
}

/*
 * Shell functions
 */

void glcd_set_backlight(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void)argv;
  if (argc != 1) {
    chprintf(chp, "Usage: glcd_backlight value (0...100)\r\n");
    return;
  }
  uint32_t value = atoi(argv[0]);
  _glcd_set_bl(value);
}

/*
 * API functions
 */

void glcd_init(void)
{
  _glcd_init_hal();
  _glcd_init_module();
}

void glcd_set_object_list(ui_object_t * head)
{
  chSysLockFromISR();
  _glcd_current_object_list = head;
  _glcd_screen_state = UI_STATE_DIRTY;
  chSysUnlockFromISR();
}
