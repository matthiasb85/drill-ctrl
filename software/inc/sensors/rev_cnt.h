/*
 * rev_cnt.h
 *
 *  Created on: 08.04.2020
 *      Author: matti
 */

#ifndef INC_SENSORS_REV_CNT_H_
#define INC_SENSORS_REV_CNT_H_

#include "sensors/rev_cnt_cfg.h"
#include "sensors/rev_cnt_types.h"

extern void rev_cnt_init(void);
extern uint32_t rev_get_rpm(void);

#endif /* INC_SENSORS_REV_CNT_H_ */
