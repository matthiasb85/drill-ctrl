/*
 * menu.c
 *
 *  Created on: 15.05.2020
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
#include "app/menu.h"
#include "app/menu_cmd.h"

/*
 * Include dependencies
 */
#include "app/drill_ctrl.h"
#include "u8g2.h"

/*
 * Global variables
 */

/*
 * Forward declarations of static functions
 */
static void          _menu_init_module      (void);
static void          _menu_set_object_dirty (ui_object_t * object);
static void          _menu_set_focus        (ui_object_t * object);
static void          _menu_switch_object    (ui_object_t * old_object, ui_object_t * new_object);
static ui_object_t * _menu_m2_run_cb        (inc_enc_rot_event_t event);
static ui_object_t * _menu_m2_settings_cb   (inc_enc_rot_event_t event);
static ui_object_t * _menu_m2_setpoint_cb   (inc_enc_rot_event_t event);

/*
 * Static variables
 */
static ui_object_t *    _menu_current_focus_object = NULL;
static MENU_SYS_STATE_T _menu_current_system_state = MENU_SSTATE_INIT;
/*
 * Greeting screen
 */

static const uint8_t _menu_logo[] = {
    0b10101010, 0b10101010, 0b01010101, 0b01010101, 0b10101010, 0b10101010, 0b01010101, 0b01010101, 0b10101010, 0b10101010, 0b01010101, 0b01010101, 0b10101010, 0b10101010, 0b01010101, 0b01010101,
    0b10101010, 0b10101010, 0b01010101, 0b01010101, 0b10101010, 0b10101010, 0b01010101, 0b01010101, 0b10101010, 0b10101010, 0b01010101, 0b01010101, 0b10101010, 0b10101010, 0b01010101, 0b01010101
};
static ui_object_bitmap_t _menu_m1_logo = { .cnt = 2, .height = 16, .bitmap = _menu_logo };
static ui_object_value_t  _menu_m1_ver  = { .type = UI_KV_TYPE_STR, .font = u8g2_font_5x8_mr, .fmt = DRILL_CTRL_VERSION, .value = NULL };

static ui_object_t _menu_m1_2 = { .class = UI_OBJECT_VALUE,  .state = UI_STATE_DIRTY, .mode = UI_MODE_NONE, .cb = NULL, .x_pos = 54, .y_pos = 50, .content = &_menu_m1_ver,  .next = NULL };
static ui_object_t _menu_m1_1 = { .class = UI_OBJECT_BITMAP, .state = UI_STATE_DIRTY, .mode = UI_MODE_NONE, .cb = NULL, .x_pos = 56, .y_pos = 24, .content = &_menu_m1_logo, .next = &_menu_m1_2 };

/*
 * Main menu screen
 */
static ui_object_value_t  _menu_m2_head      = { .type = UI_KV_TYPE_STR,    .font = u8g2_font_5x8_mr, .fmt = "Drill Control", .value = NULL };
static ui_object_tbox_t   _menu_m2_setpoint  = { .height = 26, .width = 63,  .t_x_offset = 5,  .t_y_offset = 9, .font = u8g2_font_5x8_mr, .text = "Set Point" };
static ui_object_tbox_t   _menu_m2_run       = { .height = 12, .width = 63,  .t_x_offset = 5,  .t_y_offset = 9, .font = u8g2_font_5x8_mr, .text = "Run" };
static ui_object_tbox_t   _menu_m2_settings  = { .height = 12, .width = 63,  .t_x_offset = 5,  .t_y_offset = 9, .font = u8g2_font_5x8_mr, .text = "Settings" };
static ui_object_tbox_t   _menu_m2_rpm       = { .height = 26, .width = 63,  .t_x_offset = 5,  .t_y_offset = 9, .font = u8g2_font_5x8_mr, .text = "RPM" };
static ui_object_tbox_t   _menu_m2_current   = { .height = 26, .width = 63,  .t_x_offset = 5,  .t_y_offset = 9, .font = u8g2_font_5x8_mr, .text = "Current" };
static ui_object_value_t  _menu_m2_vsetpoint = { .type = UI_KV_TYPE_UINT,  .font = u8g2_font_6x12_mr, .fmt = "%4d",   .value = &drill_ctrl_set_point };
static ui_object_value_t  _menu_m2_vrpm      = { .type = UI_KV_TYPE_UINT,  .font = u8g2_font_6x12_mr, .fmt = "%4d",   .value = &drill_ctrl_rpm };
static ui_object_value_t  _menu_m2_vcurrent  = { .type = UI_KV_TYPE_FLOAT, .font = u8g2_font_6x12_mr, .fmt = "%2.2f", .value = &drill_ctrl_current };


static ui_object_t _menu_m2_9 = { .class = UI_OBJECT_VALUE, .state = UI_STATE_DIRTY, .mode = UI_MODE_NONE, .cb = NULL,                 .x_pos = 79, .y_pos = 58, .content = &_menu_m2_vcurrent,  .next = NULL };
static ui_object_t _menu_m2_8 = { .class = UI_OBJECT_VALUE, .state = UI_STATE_DIRTY, .mode = UI_MODE_NONE, .cb = NULL,                 .x_pos = 79, .y_pos = 31, .content = &_menu_m2_vrpm,      .next = &_menu_m2_9 };
static ui_object_t _menu_m2_7 = { .class = UI_OBJECT_VALUE, .state = UI_STATE_DIRTY, .mode = UI_MODE_NONE, .cb = NULL,                 .x_pos = 15, .y_pos = 31, .content = &_menu_m2_vsetpoint, .next = &_menu_m2_8 };
static ui_object_t _menu_m2_6 = { .class = UI_OBJECT_T_BOX, .state = UI_STATE_DIRTY, .mode = UI_MODE_NONE, .cb = NULL,                 .x_pos = 65, .y_pos = 38, .content = &_menu_m2_current,   .next = &_menu_m2_7 };
static ui_object_t _menu_m2_5 = { .class = UI_OBJECT_T_BOX, .state = UI_STATE_DIRTY, .mode = UI_MODE_NONE, .cb = NULL,                 .x_pos = 65, .y_pos = 10, .content = &_menu_m2_rpm,       .next = &_menu_m2_6 };
static ui_object_t _menu_m2_4 = { .class = UI_OBJECT_T_BOX, .state = UI_STATE_DIRTY, .mode = UI_MODE_NONE, .cb = _menu_m2_settings_cb, .x_pos = 0,  .y_pos = 52, .content = &_menu_m2_settings,  .next = &_menu_m2_5 };
static ui_object_t _menu_m2_3 = { .class = UI_OBJECT_T_BOX, .state = UI_STATE_DIRTY, .mode = UI_MODE_NONE, .cb = _menu_m2_run_cb,      .x_pos = 0,  .y_pos = 38, .content = &_menu_m2_run,       .next = &_menu_m2_4 };
static ui_object_t _menu_m2_2 = { .class = UI_OBJECT_T_BOX, .state = UI_STATE_DIRTY, .mode = UI_MODE_NONE, .cb = _menu_m2_setpoint_cb, .x_pos = 0,  .y_pos = 10, .content = &_menu_m2_setpoint,  .next = &_menu_m2_3 };
static ui_object_t _menu_m2_1 = { .class = UI_OBJECT_VALUE, .state = UI_STATE_DIRTY, .mode = UI_MODE_NONE, .cb = NULL,                 .x_pos = 35, .y_pos = 8,  .content = &_menu_m2_head,      .next = &_menu_m2_2 };

/*
 * Tasks
 */

/*
 * Static helper functions
 */
static void _menu_init_hal(void)
{
  /*
   * Not needed for this module
   */
}

static void _menu_init_module(void)
{
  _menu_m2_3.mode = UI_MODE_FOCUS;
  _menu_set_focus(&_menu_m2_3);
}

static void _menu_set_object_dirty(ui_object_t * object)
{
  object->state = UI_STATE_DIRTY;
}

static void _menu_set_focus(ui_object_t * object)
{
  chSysLock();
  _menu_current_focus_object = object;
  chSysUnlock();
}

static void _menu_switch_object(ui_object_t * old_object, ui_object_t * new_object)
{
  old_object->mode =  UI_MODE_NONE;
  old_object->state = UI_STATE_DIRTY;
  new_object->mode =  UI_MODE_FOCUS;
  new_object->state = UI_STATE_DIRTY;
  _menu_set_focus(new_object);
}

/*
 * Callback functions
 */
static ui_object_t * _menu_m2_run_cb(inc_enc_rot_event_t event)
{
  switch(event)
  {
    case INC_ENC_EVENT_BTN:
    case INC_ENC_EVENT_CW:
      _menu_switch_object(MENU_M2_RUN, MENU_M2_SETTINGS);
      break;
    case INC_ENC_EVENT_CCW:
      _menu_switch_object(MENU_M2_RUN, MENU_M2_SETPOINT);
      break;
    default:
      break;
  }
  return MENU_ENTRY_SCREEN;
}

static ui_object_t * _menu_m2_settings_cb(inc_enc_rot_event_t event)
{
  switch(event)
  {
    case INC_ENC_EVENT_BTN:
    case INC_ENC_EVENT_CW:
      _menu_switch_object(MENU_M2_SETTINGS, MENU_M2_SETPOINT);
      break;
    case INC_ENC_EVENT_CCW:
      _menu_switch_object(MENU_M2_SETTINGS, MENU_M2_RUN);
      break;
    default:
      break;
  }
  return MENU_ENTRY_SCREEN;
}

static ui_object_t * _menu_m2_setpoint_cb(inc_enc_rot_event_t event)
{
  switch(event)
  {
    case INC_ENC_EVENT_BTN:
    case INC_ENC_EVENT_CW:
      _menu_switch_object(MENU_M2_SETPOINT, MENU_M2_RUN);
      break;
    case INC_ENC_EVENT_CCW:
      _menu_switch_object(MENU_M2_SETPOINT, MENU_M2_SETTINGS);
      break;
    default:
      break;
  }
  return MENU_ENTRY_SCREEN;
}

/*
 * Shell functions
 */

/*
 * API functions
 */

void menu_init(void)
{
  _menu_init_hal();
  _menu_init_module();
}

void menu_update_current(void)
{
  _menu_set_object_dirty(&_menu_m2_9);
}

void menu_update_rpm(void)
{
  _menu_set_object_dirty(&_menu_m2_8);
}

void menu_update_setpoint(void)
{
  _menu_set_object_dirty(&_menu_m2_7);
}

ui_object_t * menu_get_greeting_screen(void)
{
  return MENU_GREETING_SCREEN;
}

ui_object_t * menu_get_entry_screen(void)
{
  return MENU_ENTRY_SCREEN;
}

ui_object_t * menu_get_focus_object(void)
{
  return _menu_current_focus_object;
}

MENU_SYS_STATE_T menu_get_sys_state(void)
{
  chSysLock();
  MENU_SYS_STATE_T ret = _menu_current_system_state;
  chSysUnlock();
  return ret;
}

