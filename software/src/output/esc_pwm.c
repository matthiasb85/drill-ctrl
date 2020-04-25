/*
 * esc_pwm.c
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
#include "output/esc_pwm.h"
#include "output/esc_pwm_cmd.h"

/*
 * Include dependencies
 */
#include <stdlib.h>

/*
 * Global variables
 */

/*
 * Forward declarations of static functions
 */
static void             _esc_pwm_init_hal       (void);
static void             _esc_pwm_init_module    (void);
static void             _esc_pwm_set_duty_cycle (uint32_t);

/*
 * Static variables
 */
static PWMConfig _esc_pwm_pwmd_cfg = {
    ESC_PWM_TIMER_FREQ,
    ESC_PWM_PERIOD_TICKS,
	NULL,
    {
        ESC_PWM_CHANNEL_CFG
    },
    0,
    0,
#if STM32_PWM_USE_ADVANCED
    0
#endif
};
static uint32_t _esc_pwm_duty_cycle = 0;
/*
 * Static helper functions
 */
static void _esc_pwm_init_hal(void)
{
  pwmStart(ESC_PWM_TIMER_DRIVER, &_esc_pwm_pwmd_cfg);
  palSetLineMode(ESC_PWM_OUTPUT_LINE, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  _esc_pwm_set_duty_cycle(0);
}

static void _esc_pwm_init_module(void)
{
  /*
   * Not needed for this module
   */
}

static void _esc_pwm_set_duty_cycle(uint32_t duty_cycle)
{
  pwmEnableChannel(ESC_PWM_TIMER_DRIVER, (ESC_PWM_TIMER_CH-1), ESC_PWM_MIN_DUTY_TICKS + ESC_PWM_INT_TO_TICKS(duty_cycle));
  _esc_pwm_duty_cycle = duty_cycle;
}


/*
 * Callback functions
 */

/*
 * Shell functions
 */

void esc_pwm_read_output(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: esc_read\r\n");
    return;
  }

  chprintf(chp, "        %% |      Raw\r\n");
  chprintf(chp, "--------------------\r\n");
  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT)
  {
      uint32_t percent = ESC_PWM_TICKS_TO_PER(_esc_pwm_duty_cycle);
      chprintf(chp, " %8d | %8d\r", percent, _esc_pwm_duty_cycle);
      chThdSleepMilliseconds(100);
  }
  chprintf(chp, "\r\n\nstopped\r\n");
}

void esc_pwm_write_output(BaseSequentialStream *chp, int argc, char *argv[])
{
  (void)argv;
  if (argc != 1) {
    chprintf(chp, "Usage: esc_write value (0...100)\r\n");
    return;
  }
  uint32_t value = atoi(argv[0]);
  esc_pwm_set_output(value*(ESC_PWM_MAX_VALUE/100));
}

/*
 * API functions
 */

void esc_pwm_init(void)
{
  _esc_pwm_init_hal();
  _esc_pwm_init_module();
}

void esc_pwm_set_output(uint32_t value)
{
  value = (value > ESC_PWM_MAX_VALUE) ? ESC_PWM_MAX_VALUE : value;
  _esc_pwm_set_duty_cycle(value);
}

