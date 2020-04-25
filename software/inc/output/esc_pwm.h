/*
 * esc_pwm.h
 *
 *  Created on: 08.04.2020
 *      Author: matti
 */

#ifndef INC_OUTPUT_ESC_PWM_H_
#define INC_OUTPUT_ESC_PWM_H_

#include "output/esc_pwm_cfg.h"
#include "output/esc_pwm_types.h"

extern void esc_pwm_init(void);
extern void esc_pwm_set_output(uint32_t);

#endif /* INC_OUTPUT_ESC_PWM_H_ */
