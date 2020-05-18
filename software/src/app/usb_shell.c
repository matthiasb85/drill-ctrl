/*
 * usb_shell.c
 *
 *  Created on: 10.05.2020
 *      Author: matti
 */

/*
 * Include ChibiOS & HAL
 */
#include "ch.h"
#include "hal.h"
#include "shell.h"
#include "chprintf.h"
#include "ch/usb_cfg.h"

/*
 * Includes module API, types & config
 */
#include "app/usb_shell.h"

/*
 * Include dependencies
 */
#include "app/drill_ctrl_cmd.h"
#include "app/ctrl_loop_cmd.h"
#include "output/esc_pwm_cmd.h"
#include "sensors/pwr_sup_cmd.h"
#include "sensors/rev_cnt_cmd.h"
#include "ui/glcd_cmd.h"
#include "ui/inc_enc_cmd.h"

/*
 * Global variables
 */

/*
 * Forward declarations of static functions
 */
static void     _usb_shell_init_hal     (void);
static void     _usb_shell_init_module  (void);

/*
 * Static variables
 */
static const ShellCommand _usb_shell_cmds[] = {
  DRILL_CTRL_CMD_LIST
  ESC_PWM_CMD_LIST
  PWR_SUP_CMD_LIST
  REV_CNT_CMD_LIST
  GLCD_CMD_LIST
  INC_ENC_CMD_LIST
  {NULL, NULL}
};

static const ShellConfig _usb_shell_cfg = {
  (BaseSequentialStream *)&SDU1,
  _usb_shell_cmds
};


/*
 * Static helper functions
 */
static void _usb_shell_init_hal(void)
{
  /*
   * Initializes a serial-over-USB CDC driver.
   */
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);
}

static void _usb_shell_init_module(void)
{
  /*
   * Shell manager initialization.
   */
  shellInit();
}

/*
 * API functions
 */

void usb_shell_init(void)
{
  _usb_shell_init_hal();
  _usb_shell_init_module();
}

void usb_shell_loop(void)
{
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
   * Use main() thread to spawn shells.
   */
  while (true) {
    if (SDU1.config->usbp->state == USB_ACTIVE)
    {
      thread_t *shelltp = chThdCreateFromHeap(NULL,
          THD_WORKING_AREA_SIZE(USB_SHELL_WA_SIZE),
          "shell", USB_SHELL_PRIO,
          shellThread, (void *)&_usb_shell_cfg);

      chThdWait(shelltp);               /* Waiting termination.             */
    }
    chThdSleepMilliseconds(1000);
  }
}
