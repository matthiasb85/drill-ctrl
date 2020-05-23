/*
 * ui.c
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
#include "ui/ui.h"
#include "ui/ui_cmd.h"

/*
 * Include dependencies
 */
#include "ui/glcd.h"
#include "ui/inc_enc.h"
#include "app/menu.h"

/*
 * Global variables
 */

/*
 * Forward declarations of static functions
 */

/*
 * Static variables
 */
static THD_WORKING_AREA(_ui_stack, UI_THREAD_STACK);
static ui_object_t *    _ui_current_object_list = NULL;

/*
 * Tasks
 */
static __attribute__((noreturn)) THD_FUNCTION(_ui_thread, arg)
{
  (void)arg;

  event_listener_t  event_listener[INC_ENC_EVENT_MAX];
  eventmask_t events;

  chRegSetThreadName("ui");

  chEvtRegister(&inc_enc_events_src[INC_ENC_EVENT_BTN], &event_listener[INC_ENC_EVENT_BTN], INC_ENC_EVENT_BTN);
  chEvtRegister(&inc_enc_events_src[INC_ENC_EVENT_CW],  &event_listener[INC_ENC_EVENT_CW],  INC_ENC_EVENT_CW);
  chEvtRegister(&inc_enc_events_src[INC_ENC_EVENT_CCW], &event_listener[INC_ENC_EVENT_CCW], INC_ENC_EVENT_CCW);


  chThdSleepMilliseconds(GLCD_UPDATE_THREAD_P_MS);
  _ui_current_object_list = menu_get_greeting_screen();
  glcd_set_object_list(_ui_current_object_list);
  chThdSleepMilliseconds(2000);

  glcd_set_object_list(NULL);
  chThdSleepMilliseconds(GLCD_UPDATE_THREAD_P_MS);
  _ui_current_object_list = menu_get_entry_screen();
  glcd_set_object_list(_ui_current_object_list);

  while (true)
  {
    events = chEvtWaitAny(EVENT_MASK(INC_ENC_EVENT_BTN)
                          | EVENT_MASK(INC_ENC_EVENT_CW)
                          | EVENT_MASK(INC_ENC_EVENT_CCW));
    ui_object_t * focus_object = menu_get_focus_object();

    if (focus_object && focus_object->cb)
    {
      ui_object_t * new_screen = _ui_current_object_list;
      if (events & EVENT_MASK(INC_ENC_EVENT_BTN)) {
        new_screen = focus_object->cb(INC_ENC_EVENT_BTN);
      }
      if (events & EVENT_MASK(INC_ENC_EVENT_CW)) {
        new_screen = focus_object->cb(INC_ENC_EVENT_CW);
      }
      if (events & EVENT_MASK(INC_ENC_EVENT_CCW)) {
        new_screen = focus_object->cb(INC_ENC_EVENT_CCW);
      }
      if(new_screen != _ui_current_object_list)
      {
        _ui_current_object_list = new_screen;
        glcd_set_object_list(_ui_current_object_list);
      }
    }
  }
}

/*
 * Static helper functions
 */
static void _ui_init_hal(void)
{
  /*
   * Not needed for this module
   */
}

static void _ui_init_module(void)
{
  chThdCreateStatic(_ui_stack, sizeof(_ui_stack), UI_THREAD_PRIO, _ui_thread, NULL);
}

/*
 * Callback functions
 */

/*
 * Shell functions
 */

/*
 * API functions
 */

void ui_init(void)
{
  _ui_init_hal();
  _ui_init_module();
}
