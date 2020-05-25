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
static void          _menu_set_object_mode  (ui_object_t * object, ui_object_mode_t new_mode);
static void          _menu_switch_object    (ui_object_t * old_object, ui_object_t * new_object);
static void          _menu_change_setpoint  (inc_enc_rot_event_t event);
static void          _menu_set_sys_state    (MENU_SYS_STATE_T new_state);
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
  _menu_set_focus(MENU_M2_RUN);
  _menu_set_object_mode(MENU_M2_RUN, UI_MODE_FOCUS);
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

static void _menu_set_object_mode(ui_object_t * object, ui_object_mode_t new_mode)
{
  object->mode = new_mode;
  _menu_set_object_dirty(object);
}

static void _menu_switch_object(ui_object_t * old_object, ui_object_t * new_object)
{
  _menu_set_object_mode(old_object, UI_MODE_NONE);
  _menu_set_object_mode(new_object, UI_MODE_FOCUS);
  _menu_set_focus(new_object);
}

static void _menu_change_setpoint(inc_enc_rot_event_t event)
{
  switch(event)
  {
    case INC_ENC_EVENT_CW:
      drill_ctrl_change_set_point(+10);
      MENU_M2_VSETPOINT->state = UI_STATE_DIRTY;
      break;
    case INC_ENC_EVENT_CCW:
      drill_ctrl_change_set_point(-10);
      MENU_M2_VSETPOINT->state = UI_STATE_DIRTY;
      break;
    case INC_ENC_EVENT_BTN:
    default:
      break;
  }
}

static void _menu_set_sys_state(MENU_SYS_STATE_T new_state)
{
  chSysLock();
  _menu_current_system_state = new_state;
  chSysUnlock();
}

/*
 * Callback functions
 */
static ui_object_t * _menu_m2_run_cb(inc_enc_rot_event_t event)
{
  switch(event)
  {
    case INC_ENC_EVENT_BTN:
      if (MENU_M2_SETPOINT->mode == UI_MODE_ACTIVE)
      {
        _menu_set_object_mode(MENU_M2_SETPOINT, UI_MODE_NONE);
        _menu_set_object_mode(MENU_M2_VSETPOINT, UI_MODE_NONE);
        _menu_set_object_mode(MENU_M2_RUN, UI_MODE_FOCUS);
        _menu_set_sys_state(MENU_SSTATE_INIT);
      }
      else
      {
        _menu_set_object_mode(MENU_M2_RUN, UI_MODE_ACTIVE);
        _menu_set_object_mode(MENU_M2_SETPOINT, UI_MODE_ACTIVE);
        _menu_set_object_mode(MENU_M2_VSETPOINT, UI_MODE_ACTIVE);
        _menu_set_sys_state(MENU_SSTATE_RUNNING);
      }
      break;
    case INC_ENC_EVENT_CW:
      if (MENU_M2_RUN->mode == UI_MODE_ACTIVE)
        _menu_change_setpoint(event);
      else
        _menu_switch_object(MENU_M2_RUN, MENU_M2_SETTINGS);
      break;
    case INC_ENC_EVENT_CCW:
      if (MENU_M2_RUN->mode == UI_MODE_ACTIVE)
        _menu_change_setpoint(event);
      else
      {
        _menu_switch_object(MENU_M2_RUN, MENU_M2_SETPOINT);
        _menu_set_object_mode(MENU_M2_VSETPOINT, UI_MODE_NONE);
      }
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
      break;
    case INC_ENC_EVENT_CW:
      _menu_switch_object(MENU_M2_SETTINGS, MENU_M2_SETPOINT);
      _menu_set_object_mode(MENU_M2_VSETPOINT, UI_MODE_NONE);
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
      if (MENU_M2_SETPOINT->mode == UI_MODE_ACTIVE)
      {
        _menu_set_object_mode(MENU_M2_SETPOINT, UI_MODE_FOCUS);
        _menu_set_object_mode(MENU_M2_VSETPOINT, UI_MODE_NONE);
      }
      else
      {
        _menu_set_object_mode(MENU_M2_SETPOINT, UI_MODE_ACTIVE);
        _menu_set_object_mode(MENU_M2_VSETPOINT, UI_MODE_ACTIVE);
      }
      break;
    case INC_ENC_EVENT_CW:
      if (MENU_M2_SETPOINT->mode == UI_MODE_ACTIVE)
        _menu_change_setpoint(event);
      else
      {
        _menu_switch_object(MENU_M2_SETPOINT, MENU_M2_RUN);
        _menu_set_object_mode(MENU_M2_VSETPOINT, UI_MODE_NONE);
      }
      break;
    case INC_ENC_EVENT_CCW:
      if (MENU_M2_SETPOINT->mode == UI_MODE_ACTIVE)
        _menu_change_setpoint(event);
      else
      {
        _menu_switch_object(MENU_M2_SETPOINT, MENU_M2_SETTINGS);
        _menu_set_object_mode(MENU_M2_VSETPOINT, UI_MODE_NONE);
      }
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
  _menu_set_object_dirty(MENU_M2_VCURRENT);
}

void menu_update_rpm(void)
{
  _menu_set_object_dirty(MENU_M2_VRPM);
}

void menu_update_setpoint(void)
{
  _menu_set_object_dirty(MENU_M2_VSETPOINT);
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

