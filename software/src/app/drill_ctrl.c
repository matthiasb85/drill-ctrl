/*
 * drill_ctrl.c
 *
 *  Created on: 08.04.2020
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
#include "app/drill_ctrl.h"
#include "app/drill_ctrl_cmd.h"

/*
 * Include dependencies
 */
#include "app/ctrl_loop.h"
#include "app/usb_shell.h"
#include "app/menu.h"
#include "output/esc_pwm.h"
#include "sensors/pwr_sup.h"
#include "sensors/rev_cnt.h"
#include "ui/glcd.h"
#include "ui/inc_enc.h"
#include "ui/ui.h"

/*
 * Global variables
 */
float            drill_ctrl_current     = 0.0;
uint32_t         drill_ctrl_set_point   = 200;
uint32_t         drill_ctrl_rpm         = 150;

/*
 * Forward declarations of static functions
 */
static void             _drill_ctrl_init_module  (void);

/*
 * Static variables
 */

static THD_WORKING_AREA(_drill_ctrl_main_stack, DRILL_CTRL_MAIN_THREAD_STACK);

/*
 * Tasks
 */

static __attribute__((noreturn)) THD_FUNCTION(_drill_ctrl_main_thread, arg)
{
  (void)arg;
  float     current_old   = drill_ctrl_current;
  uint32_t  rpm_old       = drill_ctrl_rpm;
  uint32_t  set_point_old = drill_ctrl_set_point;

  chRegSetThreadName("drill_ctrl_main");

  while(true)
  {
    systime_t time = chVTGetSystemTimeX();

    chSysLock();
    uint32_t  set_point = drill_ctrl_set_point;
    drill_ctrl_rpm      = rev_get_rpm();
    drill_ctrl_current  = 0.0; //pwr_sup_get_current();
    chSysUnlock();

//    uint32_t output = ctrl_loop_step(set_point, drill_ctrl_rpm);
//    esc_pwm_set_output(output);

    if(current_old   != drill_ctrl_current) menu_update_current();
    if(rpm_old       != drill_ctrl_rpm)     menu_update_rpm();
    if(set_point_old != set_point)          menu_update_setpoint();

    chSysLock();
    current_old   = drill_ctrl_current;
    rpm_old       = drill_ctrl_rpm;
    set_point_old = set_point;
    chSysUnlock();

    chThdSleepUntilWindowed(time, time + TIME_MS2I(DRILL_CTRL_MAIN_THREAD_P_MS));
  }
}

/*
 * Static helper functions
 */

static void _drill_ctrl_init_module(void)
{
  chThdCreateStatic(_drill_ctrl_main_stack, sizeof(_drill_ctrl_main_stack),
                    DRILL_CTRL_MAIN_THREAD_PRIO, _drill_ctrl_main_thread, NULL);
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

void drill_ctrl_init(void)
{
  /*
   * Project specific driver initialization
   */
  esc_pwm_init();
//  pwr_sup_init();drill_ctrl_init
  rev_cnt_init();
  glcd_init();
  inc_enc_init();
  menu_init();
  ui_init();
//  loop_ctrl_init();
  usb_shell_init();

  /*
   * Initialize main module
   */
  _drill_ctrl_init_module();

  /*
   * Start USB shell handling,
   * should never return
   */
  usb_shell_loop();

  /*
   * If we reach this point,
   * something went horribly wrong
   */
}

void drill_ctrl_change_set_point(uint32_t new_set_point)
{
  chSysLockFromISR();
  drill_ctrl_set_point = new_set_point;
  chSysUnlockFromISR();
}
