/*
 * esc_pwm_types.h
 *
 *  Created on: 22.04.2020
 *      Author: matti
 */

#ifndef INC_OUTPUT_ESC_PWM_TYPES_H_
#define INC_OUTPUT_ESC_PWM_TYPES_H_

#if ESC_PWM_TIMER_CH == 1
#define ESC_PWM_CHANNEL_CFG \
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}, \
   {PWM_OUTPUT_DISABLED, NULL}, \
   {PWM_OUTPUT_DISABLED, NULL}, \
   {PWM_OUTPUT_DISABLED, NULL}
#elif ESC_PWM_TIMER_CH == 2
#define ESC_PWM_CHANNEL_CFG \
   {PWM_OUTPUT_DISABLED, NULL}, \
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}, \
   {PWM_OUTPUT_DISABLED, NULL}, \
   {PWM_OUTPUT_DISABLED, NULL}
#elif ESC_PWM_TIMER_CH == 3
#define ESC_PWM_CHANNEL_CFG \
   {PWM_OUTPUT_DISABLED, NULL}, \
   {PWM_OUTPUT_DISABLED, NULL}, \
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}, \
   {PWM_OUTPUT_DISABLED, NULL}
#elif ESC_PWM_TIMER_CH == 4
#define ESC_PWM_CHANNEL_CFG \
   {PWM_OUTPUT_DISABLED, NULL}, \
   {PWM_OUTPUT_DISABLED, NULL}, \
   {PWM_OUTPUT_DISABLED, NULL}, \
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
#else
#error "Channel number not supported"
#endif

#define ESC_PWM_MS_TO_TICKS(x)  ((ESC_PWM_TIMER_FREQ/1000)*(x))
#define ESC_PWM_PERIOD_TICKS    ESC_PWM_MS_TO_TICKS(ESC_PWM_PERIOD_MS)
#define ESC_PWM_MIN_DUTY_TICKS  ESC_PWM_MS_TO_TICKS(ESC_PWM_MIN_DUTY_MS)
#define ESC_PWM_MAX_DUTY_TICKS  ESC_PWM_MS_TO_TICKS(ESC_PWM_MAX_DUTY_MS)

#define ESC_PWM_INT_TO_TICKS(x) (((ESC_PWM_MAX_DUTY_TICKS-ESC_PWM_MIN_DUTY_TICKS)*(x))/ESC_PWM_MAX_VALUE)
#define ESC_PWM_TICKS_TO_PER(x) ((((x)-ESC_PWM_MIN_DUTY_TICKS)*100)/(ESC_PWM_MAX_DUTY_TICKS-ESC_PWM_MIN_DUTY_TICKS))


#endif /* INC_OUTPUT_ESC_PWM_TYPES_H_ */
