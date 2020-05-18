/*
 * main.c
 *
 *  Created on: 10.05.2020
 *      Author: matti
 *
 *      based on ChibiOS example
 */

/*
 * ChibiOS specific includes
 */
#include <stdio.h>
#include <string.h>
#include "ch.h"
#include "hal.h"

/*
 * Project specific API includes
 */
#include "app/drill_ctrl.h"


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
   * Project specific application initialization,
   * does not return since it also starts USB shell
   * thread handling
   */
  drill_ctrl_init();

  /*
   * HIC SVNT DRACONES
   */
}
