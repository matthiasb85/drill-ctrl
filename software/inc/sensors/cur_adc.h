/*
 * cur_adc.h
 *
 *  Created on: 08.04.2020
 *      Author: matti
 */

#ifndef INC_SENSORS_CUR_ADC_H_
#define INC_SENSORS_CUR_ADC_H_

#include "sensors/cur_adc_cfg.h"

extern void  cur_adc_init(void);
extern float cur_adc_get_amp(void);

#endif /* INC_SENSORS_CUR_ADC_H_ */
