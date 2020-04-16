/*
 * inc_enc_types.h
 *
 *  Created on: 10.04.2020
 *      Author: matti
 */

#ifndef INC_UI_INC_ENC_TYPES_H_
#define INC_UI_INC_ENC_TYPES_H_

typedef enum {
  INC_ENC_IMPL_BTN = 0,
  INC_ENC_IMPL_CW,
  INC_ENC_IMPL_CCW,
  INC_ENC_IMPL_MAX
}inc_enc_rot_impulse_t;

typedef enum {
  INC_ENC_FSM_INIT,
  INC_ENC_FSM_WFF_CW,
  INC_ENC_FSM_WFF_CCW,
  INC_ENC_FSM_WF_RESET
}inc_enc_rot_state_t;

typedef enum {
  INC_ENC_EVENT_BTN = 0,
  INC_ENC_EVENT_CW,
  INC_ENC_EVENT_CCW,
  INC_ENC_EVENT_MAX
}inc_enc_rot_event_t;

#if INC_ENC_IO_MODE == PAL_MODE_INPUT_PULLUP
#define INC_ENC_IMPL_E_START  PAL_EVENT_MODE_FALLING_EDGE
#define INC_ENC_IMPL_E_PAUSED PAL_EVENT_MODE_DISABLED
#elif INC_ENC_IO_MODE == PAL_MODE_INPUT_PULLDOWN
#define INC_ENC_IMPL_E_START  PAL_EVENT_MODE_RISING_EDGE
#define INC_ENC_IMPL_E_PAUSED PAL_EVENT_MODE_DISABLED
#elif INC_ENC_IO_MODE == PAL_MODE_INPUT
#define INC_ENC_IMPL_E_PAUSED PAL_EVENT_MODE_DISABLED
#warning "Please specify INC_ENC_IMPL_E_START and INC_ENC_IMPL_E_STOP manually according to your board layout"
#else
#error "Unsupported input type"
#endif


#endif /* INC_UI_INC_ENC_TYPES_H_ */
