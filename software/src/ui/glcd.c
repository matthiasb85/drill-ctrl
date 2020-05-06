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
static uint8_t          _glcd_process_objects     (void);
static uint8_t 			_glcd_u8g2_hw_spi         (u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
static uint8_t 			_glcd_u8g2_gpio_and_delay (U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);

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

static u8g2_t _glcd_u8g2;
static THD_WORKING_AREA(_glcd_update_stack, GLCD_UPDATE_THREAD_STACK);

/*
 * Tasks
 */
static __attribute__((noreturn)) THD_FUNCTION(_glcd_update_thread, arg)
{

  (void)arg;
  chRegSetThreadName("glcd_update");

  while (true)
  {
    if(_glcd_process_objects())
    {
        spiAcquireBus(GLCD_SPI_DRIVER);
    	u8g2_SendBuffer(&_glcd_u8g2);
        spiReleaseBus(GLCD_SPI_DRIVER);
    }
    chThdSleepMilliseconds(50);
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

static uint8_t _glcd_process_objects(void)
{
	return 1;
}

/*
 * Callback functions
 */
static uint8_t _glcd_u8g2_hw_spi(U8X8_UNUSED u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{

  switch(msg)
  {
    case U8X8_MSG_BYTE_SEND:
      spiSend(GLCD_SPI_DRIVER, arg_int, arg_ptr);
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
      spiUnselect(GLCD_SPI_DRIVER);
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:
      spiSelect(GLCD_SPI_DRIVER);
      break;
    default:
    return 0;
  }
  return 1;
}

static uint8_t _glcd_u8g2_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
	switch(msg){
		//Function to define the logic level of the RESET line
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
