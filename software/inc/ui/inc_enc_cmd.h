/*
 * inc_enc_cmd.h
 *
 *  Created on: 01.04.2020
 *      Author: matti
 */

#ifndef INC_UI_INC_ENC_CMD_H_
#define INC_UI_INC_ENC_CMD_H_

/*
 * Global definition of shell commands
 * for module inc_enc
 */
extern void inc_enc_read_input(BaseSequentialStream *chp, int argc, char *argv[]);

/*
 * Shell command list
 * for module inc_enc
 */
#define INC_ENC_CMD_LIST \
		  {"encoder_read", inc_enc_read_input}, \

#endif /* INC_UI_INC_ENC_CMD_H_ */
