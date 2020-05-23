/*
 * menu.h
 *
 *  Created on: 15.05.2020
 *      Author: matti
 */

#ifndef INC_APP_MENU_H_
#define INC_APP_MENU_H_

#include "app/menu_cfg.h"
#include "app/menu_types.h"
#include "ui/ui_types.h"

extern void             menu_init(void);
extern void             menu_update_current(void);
extern void             menu_update_rpm(void);
extern void             menu_update_setpoint(void);
extern ui_object_t *    menu_get_greeting_screen(void);
extern ui_object_t *    menu_get_entry_screen(void);
extern ui_object_t *    menu_get_focus_object(void);
extern MENU_SYS_STATE_T menu_get_sys_state(void);

#endif /* INC_APP_MENU_H_ */
