/*
 * inc_enc_cfg.h
 *
 *  Created on: 10.04.2020
 *      Author: matti
 */

#ifndef INC_CFG_INC_ENC_CFG_H_
#define INC_CFG_INC_ENC_CFG_H_

#define INC_ENC_CCW_LINE   PAL_LINE(GPIOB, 5U)
#define INC_ENC_CW_LINE    PAL_LINE(GPIOB, 4U)
#define INC_ENC_BTN_LINE   PAL_LINE(GPIOB, 3U)
#define INC_ENC_IO_MODE    PAL_MODE_INPUT_PULLUP        // use PAL_MODE_INPUT, PAL_MODE_INPUT_PULLUP or PAL_MODE_INPUT_PULLDOWN
#define INC_ENC_DEB_T_BTN  500                          // debounce timeout for button in [ms]
#define INC_ENC_DEB_T_ROT  20                           // debounce timeout for rotary encoder impulse in [ms]
#define INC_ENC_TIMEOUT_T  50                           // state change timeout in [ms]


#endif /* INC_CFG_INC_ENC_CFG_H_ */
