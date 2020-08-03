/*
 * rev_cnt.c
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
#include "sensors/rev_cnt.h"
#include "sensors/rev_cnt_cmd.h"

/*
 * Include dependencies
 */

/*
 * Global variables
 */

/*
 * Forward declarations of static functions
 */
static void             _rev_cnt_init_hal       (void);
static void             _rev_cnt_init_module    (void);
static uint32_t         _rev_cnt_get_dT         (void);
static void             _rev_cnt_period_cb      (ICUDriver *icup);
static void             _rev_cnt_overflow_cb    (ICUDriver *icup);

/*
 * Static variables
 */
static ICUConfig _rev_cnt_icu_cfg = {
    ICU_INPUT_ACTIVE_HIGH,
    REV_CNT_TIMER_FREQ,
    NULL,
    _rev_cnt_period_cb,
    _rev_cnt_overflow_cb,
    REV_CNT_ICUD_CH,
    0
  };
static icucnt_t _rev_cnt_last_dT = 0;

/*
 * Static helper functions
 */
static void _rev_cnt_init_hal(void)
{
  palSetLineMode(REV_CNT_INPUT_LINE, REV_CNT_INPUT_MODE);
  icuStart(REV_CNT_ICUD, &_rev_cnt_icu_cfg);
  icuStartCapture(REV_CNT_ICUD);
  icuEnableNotifications(REV_CNT_ICUD);
}

static void _rev_cnt_init_module(void)
{
  /*
   * Not needed for this module
   */
}

static uint32_t _rev_cnt_get_dT(void)
{
  chSysLockFromISR();
  uint32_t ret = (uint32_t)_rev_cnt_last_dT;
  chSysUnlockFromISR();
  return ret;
}

/*
 * Callback functions
 */
static void _rev_cnt_period_cb(ICUDriver *icup)
{
  chSysLockFromISR();
  _rev_cnt_last_dT = (_rev_cnt_last_dT*REV_CNT_OLD_VAL_EMPH + icuGetPeriodX(icup)*REV_CNT_NEW_VAL_EMPH)/(REV_CNT_OLD_VAL_EMPH + REV_CNT_NEW_VAL_EMPH);
  chSysUnlockFromISR();
}

static void _rev_cnt_overflow_cb(ICUDriver *icup)
{
  (void)icup;
  chSysLockFromISR();
  _rev_cnt_last_dT = 0;
  chSysUnlockFromISR();
}

/*
 * Shell functions
 */

void rev_cnt_read_input(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: revolution_read\r\n");
    return;
  }

  chprintf(chp, "   dT[ms] |      RPM\r\n");
  chprintf(chp, "--------------------\r\n");
  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT)
  {
      uint32_t dT = _rev_cnt_get_dT();
      uint32_t rpm = rev_get_rpm();
      chprintf(chp, " %8d | %8d\r", dT/(REV_CNT_TIMER_FREQ/1000), rpm);
      chThdSleepMilliseconds(100);
  }
  chprintf(chp, "\r\n\nstopped\r\n");
}

/*
 * API functions
 */

void rev_cnt_init(void)
{
  _rev_cnt_init_hal();
  _rev_cnt_init_module();
}

uint32_t rev_get_rpm(void)
{
  uint32_t dT = _rev_cnt_get_dT();
  uint32_t rpm = (dT > 0) ?
      (uint32_t)((60/REV_CNT_IMPL_P_REV)*((float)REV_CNT_TIMER_FREQ)/((float)dT)) :
      0 ;
  return rpm;
}
