/*
 * rev_cnt_cmd.h
 *
 *  Created on: 08.04.2020
 *      Author: matti
 */

#ifndef INC_SENSORS_REV_CNT_CMD_H_
#define INC_SENSORS_REV_CNT_CMD_H_

/*
 * Global definition of shell commands
 * for module rev_cnt
 */
extern void esc_pwn_read_output(BaseSequentialStream *chp, int argc, char *argv[]);

/*
 * Shell command list
 * for module rev_cnt
 */
#define REV_CNT_CMD_LIST \
    {"revolution_read", rev_cnt_read_input}, \

#endif /* INC_SENSORS_REV_CNT_CMD_H_ */
