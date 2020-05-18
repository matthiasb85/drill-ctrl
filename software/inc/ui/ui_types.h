/*
 * ui_types.h
 *
 *  Created on: 15.05.2020
 *      Author: matti
 */

#ifndef INC_UI_UI_TYPES_H_
#define INC_UI_UI_TYPES_H_

#include "ui/inc_enc.h"

typedef enum {
  UI_MODE_NONE = 0,
  UI_MODE_FOCUS,
  UI_MODE_ACTIVE
}ui_object_mode_t;

typedef enum {
  UI_STATE_CLEAN = 0,
  UI_STATE_DIRTY
}ui_object_state_t;

typedef enum {
  UI_OBJECT_T_BOX = 0,
  UI_OBJECT_VALUE,
  UI_OBJECT_BITMAP,
  UI_OBJECT_MAX
}ui_object_class_t;

typedef enum {
  UI_KV_TYPE_STR = 0,
  UI_KV_TYPE_UINT,
  UI_KV_TYPE_INT,
  UI_KV_TYPE_FLOAT,
  UI_KV_TYPE_MAX
}ui_value_type_t;

typedef struct {
  uint8_t           height;
  uint8_t           width;
  uint8_t           t_x_offset;
  uint8_t           t_y_offset;
  const uint8_t *   font;
  const char *      text;
}ui_object_tbox_t;

typedef struct {
  const uint8_t *   font;
  const char *      fmt;
  ui_value_type_t   type;
  void *            value;
}ui_object_value_t;

typedef struct {
  uint8_t           cnt;
  uint8_t           height;
  const uint8_t *   bitmap;
}ui_object_bitmap_t;

typedef struct _ui_object_t {
  ui_object_class_t     class;
  ui_object_state_t     state;
  ui_object_mode_t      mode;
  struct _ui_object_t * (*cb)(inc_enc_rot_event_t event);
  uint8_t               x_pos;
  uint8_t               y_pos;
  void *                content;
  struct _ui_object_t * next;
}ui_object_t;

#endif /* INC_UI_UI_TYPES_H_ */
