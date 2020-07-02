/*
 * cur_adc_cfg.h
 *
 *  Created on: 22.06.2020
 *      Author: matti
 */

#ifndef INC_SENSORS_CUR_ADC_CFG_H_
#define INC_SENSORS_CUR_ADC_CFG_H_

#define CUR_ADC_INPUT_LINE      PAL_LINE(GPIOA, 0U)
#define CUR_ADC_INPUT_MODE      PAL_MODE_INPUT_ANALOG
#define CUR_ADC_CHANNEL         ADC_CHANNEL_IN0
#define CUR_ADC_DRIVER          (&ADCD1)
#define CUR_ADC_PERIOD_MS       100                           // ADC sampling period in [ms]

#define CUR_ADC_VREF_IN_MV      ((float)(3300))
#define CUR_ADC_MV_PER_TICKS    CUR_ADC_VREF_IN_MV/((float)(4096))
#define CUR_ADC_OFFSET_IN_MV    ((float)(0.12))*CUR_ADC_VREF_IN_MV
#define CUR_ADC_A_PER_MV        ((float)(0.025))

#endif /* INC_SENSORS_CUR_ADC_CFG_H_ */
