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
static uint32_t         _drill_ctrl_loop_step    (uint32_t set_point, uint32_t current_rpm);
/*
 * Static variables
 */

static THD_WORKING_AREA(_drill_ctrl_main_stack, DRILL_CTRL_MAIN_THREAD_STACK);
static float            _drill_ctrl_kP = 1.0;
static float            _drill_ctrl_kI = 1.0;
static float            _drill_ctrl_kD = 1.0;

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

    uint32_t output = _drill_ctrl_loop_step(
        (menu_get_sys_state() == MENU_SSTATE_RUNNING) ? set_point : 0,
        drill_ctrl_rpm);
    esc_pwm_set_output(output);

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

static uint32_t  _drill_ctrl_loop_step(uint32_t set_point, uint32_t current_rpm)
{
  static DRILL_CTRL_L_STATE_T loop_state = DRILL_CTRL_L_INIT;
  static float prev_err_p = 0.0;
  float        err_p = 0.0;
  static float err_i = 0.0;
  float        err_d = 0.0;

  uint32_t output = 0;

  if(set_point == 0 && loop_state != DRILL_CTRL_L_INIT)
  {
    loop_state = DRILL_CTRL_L_SDOWN;
  }

  switch(loop_state)
  {
    case DRILL_CTRL_L_INIT:
      output = DRILL_CTRL_MIN_ESC_OUTPUT;
      loop_state = DRILL_CTRL_L_SUP;
      break;
    case DRILL_CTRL_L_SUP:
      {
        err_p = (float)(set_point - current_rpm);
        uint32_t new_output = (uint32_t)(err_p*_drill_ctrl_kP);
        output = (new_output < DRILL_CTRL_MIN_ESC_OUTPUT) ?
            DRILL_CTRL_MIN_ESC_OUTPUT :
            new_output;
        loop_state = DRILL_CTRL_L_RUN;
      }
      break;
    case DRILL_CTRL_L_RUN:
      {
        err_p = (float)(set_point - current_rpm);
        err_i = err_i + err_p;
        err_d = (err_p - prev_err_p)/((float)(DRILL_CTRL_MAIN_THREAD_P_MS));

        uint32_t new_output = (uint32_t)(err_p*_drill_ctrl_kP + err_i*_drill_ctrl_kI + err_d*_drill_ctrl_kD);

        output = (new_output < DRILL_CTRL_MIN_ESC_OUTPUT) ?
            DRILL_CTRL_MIN_ESC_OUTPUT :
            new_output;
      }
      break;
    case DRILL_CTRL_L_SDOWN:
    default:
      loop_state = DRILL_CTRL_L_INIT;
      err_i = 0.0;
      output = 0;
      break;
  }

  prev_err_p = err_p;

  return output;
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
