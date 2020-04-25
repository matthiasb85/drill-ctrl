/*
 * esc_pwm_cfg.h
 *
 *  Created on: 22.04.2020
 *      Author: matti
 */

#ifndef INC_OUTPUT_ESC_PWM_CFG_H_
#define INC_OUTPUT_ESC_PWM_CFG_H_

#define ESC_PWM_TIMER_FREQ      200000                	// 20KHz timer speed
#define ESC_PWM_PERIOD_MS       20                      // 20ms period
#define ESC_PWM_MIN_DUTY_MS     1
#define ESC_PWM_MAX_DUTY_MS     2
#define ESC_PWM_OUTPUT_LINE     PAL_LINE(GPIOB, 0U)
#define ESC_PWM_TIMER_DRIVER    (&PWMD3)
#define ESC_PWM_TIMER_CH        3
#define ESC_PWM_MAX_VALUE       10000

#endif /* INC_OUTPUT_ESC_PWM_CFG_H_ */
