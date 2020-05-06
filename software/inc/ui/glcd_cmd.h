/*
 * glcd_cmd.h
 *
 *  Created on: 08.04.2020
 *      Author: matti
 */

#ifndef INC_UI_GLCD_CMD_H_
#define INC_UI_GLCD_CMD_H_

/*
 * Global definition of shell commands
 * for module glcd
 */
extern void glcd_set_backlight(BaseSequentialStream *chp, int argc, char *argv[]);

/*
 * Shell command list
 * for module glcd
 */
#define GLCD_CMD_LIST \
	    {"glcd_backlight", glcd_set_backlight}, \

#endif /* INC_UI_GLCD_CMD_H_ */
