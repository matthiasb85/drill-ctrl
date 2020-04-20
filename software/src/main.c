/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
 * ChibiOS specific includes
 */
#include <stdio.h>
#include <string.h>
#include "ch.h"
#include "hal.h"
#include "shell.h"
#include "chprintf.h"
#include "ch/usb_cfg.h"

/*
 * Project specific API includes
 */
#include "app/drill_ctrl.h"
#include "output/esc_pwm.h"
#include "sensors/pwr_sup.h"
#include "sensors/rev_cnt.h"
#include "ui/glcd.h"
#include "ui/inc_enc.h"

/*
 * Project specific shell command includes
 */
#include "app/drill_ctrl_cmd.h"
#include "output/esc_pwm_cmd.h"
#include "sensors/pwr_sup_cmd.h"
#include "sensors/rev_cnt_cmd.h"
#include "ui/glcd_cmd.h"
#include "ui/inc_enc_cmd.h"

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

static const ShellCommand commands[] = {
  DRILL_CTRL_CMD_LIST
  ESC_PWM_CMD_LIST
  PWR_SUP_CMD_LIST
  REV_CNT_CMD_LIST
  GLCD_CMD_LIST
  INC_ENC_CMD_LIST
  {NULL, NULL}
};

static const ShellConfig shell_cfg = {
  (BaseSequentialStream *)&SDU1,
  commands
};

///*===========================================================================*/
///* Generic code.                                                             */
///*===========================================================================*/
//
///*
// * Blinker thread, times are in milliseconds.
// */
//static THD_WORKING_AREA(waThread1, 128);
//static __attribute__((noreturn)) THD_FUNCTION(Thread1, arg) {
//
//  (void)arg;
//  chRegSetThreadName("blinker");
//  while (true) {
//    systime_t time = serusbcfg.usbp->state == USB_ACTIVE ? 250 : 500;
//    palClearPad(GPIOB, GPIOB_LED);
//    chThdSleepMilliseconds(time);
//    palSetPad(GPIOB, GPIOB_LED);
//    chThdSleepMilliseconds(time);
//  }
//}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Initializes a serial-over-USB CDC driver.
   */
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  /*
   * Activates the USB driver and then the USB bus pull-up on D+.
   * Note, a delay is inserted in order to not have to disconnect the cable
   * after a reset.
   */
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  /*
   * Shell manager initialization.
   */
  shellInit();

//  /*
//   * Creates the blinker thread.
//   */
//  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /*
   * Project specific driver initialization
   */
//  esc_pwm_init();
//  pwr_sup_init();
  rev_cnt_init();
//  glcd_init();
  inc_enc_init();

  /*
   * Project specific application initialization
   */
//  drill_ctrl_init();

  /*
   * Normal main() thread activity, spawning shells.
   */
  while (true) {
    if (SDU1.config->usbp->state == USB_ACTIVE) {
      thread_t *shelltp = chThdCreateFromHeap(NULL, SHELL_WA_SIZE,
                                              "shell", NORMALPRIO + 1,
                                              shellThread, (void *)&shell_cfg);
      chThdWait(shelltp);               /* Waiting termination.             */
    }
    chThdSleepMilliseconds(1000);
  }
  /*
   * HIC SVNT DRACONES
   */
}
