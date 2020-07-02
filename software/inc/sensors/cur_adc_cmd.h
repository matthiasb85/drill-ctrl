/*
 * cur_adc_cmd.h
 *
 *  Created on: 08.04.2020
 *      Author: matti
 */

#ifndef INC_SENSORS_CUR_ADC_CMD_H_
#define INC_SENSORS_CUR_ADC_CMD_H_

/*
 * Global definition of shell commands
 * for module cur_adc
 */
extern void cur_adc_read_input(BaseSequentialStream *chp, int argc, char *argv[]);

/*
 * Shell command list
 * for module cur_adc
 */
#define CUR_ADC_CMD_LIST \
    {"current_read", cur_adc_read_input}, \

#endif /* INC_SENSORS_CUR_ADC_CMD_H_ */
