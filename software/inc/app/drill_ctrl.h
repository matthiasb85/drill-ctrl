/*
 * dril_ctrl.h
 *
 *  Created on: 08.04.2020
 *      Author: matti
 */

#ifndef INC_APP_DRILL_CTRL_H_
#define INC_APP_DRILL_CTRL_H_

#include "app/drill_ctrl_cfg.h"
#include "app/drill_ctrl_types.h"

extern float            drill_ctrl_current;
extern uint32_t         drill_ctrl_set_point;
extern uint32_t         drill_ctrl_rpm;

extern void drill_ctrl_init(void);
extern void drill_ctrl_change_set_point(uint32_t new_set_point);

#endif /* INC_APP_DRILL_CTRL_H_ */
