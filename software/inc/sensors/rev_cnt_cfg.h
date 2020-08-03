/*
 * rev_cnt_cfg.h
 *
 *  Created on: 20.04.2020
 *      Author: matti
 */

#ifndef INC_SENSORS_REV_CNT_CFG_H_
#define INC_SENSORS_REV_CNT_CFG_H_

#define REV_CNT_TIMER_FREQ      100000
#define REV_CNT_IMPL_P_REV      20
#define REV_CNT_INPUT_LINE      PAL_LINE(GPIOA, 8U)
#define REV_CNT_INPUT_MODE      PAL_MODE_INPUT_PULLDOWN
#define REV_CNT_ICUD            (&ICUD1)
#define REV_CNT_ICUD_CH         ICU_CHANNEL_1
#define REV_CNT_OLD_VAL_EMPH    4
#define REV_CNT_NEW_VAL_EMPH    1


#endif /* INC_SENSORS_REV_CNT_CFG_H_ */
