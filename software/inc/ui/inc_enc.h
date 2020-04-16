/*
 * inc_enc.h
 *
 *  Created on: 01.04.2020
 *      Author: matti
 */

#ifndef INC_UI_INC_ENC_H_
#define INC_UI_INC_ENC_H_

#include "ui/inc_enc_cfg.h"
#include "ui/inc_enc_types.h"

extern void inc_enc_init(void);

extern event_source_t           inc_enc_events_src[INC_ENC_EVENT_MAX];

#endif /* INC_UI_INC_ENC_H_ */
