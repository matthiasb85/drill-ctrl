/*
 * glcd.h
 *
 *  Created on: 08.04.2020
 *      Author: matti
 */

#ifndef INC_UI_GLCD_H_
#define INC_UI_GLCD_H_

#include "ui/glcd_cfg.h"
#include "ui/glcd_types.h"
#include "ui/ui_types.h"

extern void glcd_init(void);
extern void glcd_set_object_list(ui_object_t * head);

#endif /* INC_UI_GLCD_H_ */
