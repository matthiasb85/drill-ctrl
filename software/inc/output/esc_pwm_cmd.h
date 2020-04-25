/*
 * esc_pwm_cmd.h
 *
 *  Created on: 08.04.2020
 *      Author: matti
 */

#ifndef INC_OUTPUT_ESC_PWM_CMD_H_
#define INC_OUTPUT_ESC_PWM_CMD_H_

/*
 * Global definition of shell commands
 * for module esc_pwm
 */
extern void esc_pwm_read_output(BaseSequentialStream *chp, int argc, char *argv[]);
extern void esc_pwm_write_output(BaseSequentialStream *chp, int argc, char *argv[]);

/*
 * Shell command list
 * for module esc_pwm
 */
#define ESC_PWM_CMD_LIST \
    {"esc_read", esc_pwm_read_output}, \
    {"esc_write", esc_pwm_write_output}, \

#endif /* INC_OUTPUT_ESC_PWM_CMD_H_ */
