/*
 * inc_enc.c
 *
 *  Created on: 01.04.2020
 *      Author: matti
 */

/*
 * Include ChibiOS & HAL
 */
#include "ch.h"
#include "hal.h"
#include "shell.h"
#include "chprintf.h"

/*
 * Includes module API, types & config
 */
#include "ui/inc_enc.h"
#include "ui/inc_enc_cmd.h"

/*
 * Include dependencies
 */

/*
 * Static variables
 */
static inc_enc_rot_state_t      _inc_enc_rot_state = INC_ENC_FSM_INIT;
static virtual_timer_t          _inc_enc_debounce_vtp;
static virtual_timer_t          _inc_enc_timeout_vtp;

/*
 * Global variables
 */
event_source_t                  inc_enc_events_src[INC_ENC_EVENT_MAX];

/*
 * Forward declarations of static functions
 */
static void             _inc_enc_init_hal       (void);
static void             _inc_enc_init_module    (void);
static inline void      _inc_enc_send_event     (inc_enc_rot_event_t event);
static void             _inc_enc_reset_fsm      (void);
static void             _inc_enc_timeout        (void *arg);
static void             _inc_enc_debounce       (void *arg);
static void             _inc_enc_cb_btn         (void *arg);
static void             _inc_enc_cb_rot         (void *arg);

extern SerialUSBDriver SDU1;


/*
 * Static helper functions
 */
static void _inc_enc_init_hal(void)
{
  AFIO->MAPR |= (2 << 24); // disable TDO pin, ugly hack

  palSetLineMode(INC_ENC_BTN_LINE, INC_ENC_IO_MODE);
  palEnableLineEvent(INC_ENC_BTN_LINE, INC_ENC_IMPL_E_START);
  palSetLineCallback(INC_ENC_BTN_LINE, _inc_enc_cb_btn, NULL);

  palSetLineMode(INC_ENC_CW_LINE, INC_ENC_IO_MODE);
  palSetLineCallback(INC_ENC_CW_LINE, _inc_enc_cb_rot, (void *)INC_ENC_IMPL_CW);

  palSetLineMode(INC_ENC_CCW_LINE, INC_ENC_IO_MODE);
  palSetLineCallback(INC_ENC_CCW_LINE, _inc_enc_cb_rot, (void *)INC_ENC_IMPL_CCW);
}

static void _inc_enc_init_module(void)
{
  chEvtObjectInit(&inc_enc_events_src[INC_ENC_EVENT_BTN]);
  chEvtObjectInit(&inc_enc_events_src[INC_ENC_EVENT_CW]);
  chEvtObjectInit(&inc_enc_events_src[INC_ENC_EVENT_CCW]);

  chVTObjectInit(&_inc_enc_debounce_vtp);
  chVTObjectInit(&_inc_enc_timeout_vtp);

  _inc_enc_reset_fsm();
}

static inline void _inc_enc_send_event(inc_enc_rot_event_t event)
{
  chEvtBroadcastI(&inc_enc_events_src[event]);
}

static void _inc_enc_reset_fsm(void)
{
  palEnableLineEventI(INC_ENC_CW_LINE, INC_ENC_IMPL_E_PAUSED);
  palEnableLineEventI(INC_ENC_CCW_LINE, INC_ENC_IMPL_E_PAUSED);

  _inc_enc_rot_state = INC_ENC_FSM_WF_RESET;

  chVTSetI(&_inc_enc_debounce_vtp,
           TIME_MS2I(INC_ENC_DEB_T_ROT),
           _inc_enc_debounce,
           (void *)((1 << INC_ENC_IMPL_CW) | (1 << INC_ENC_IMPL_CCW)));
}

/*
 * Callback functions
 */
static void _inc_enc_timeout(void *arg)
{
  inc_enc_rot_state_t old_state = (inc_enc_rot_state_t)arg;
  if(_inc_enc_rot_state == old_state)
    {
      _inc_enc_reset_fsm();
    }
}

static void _inc_enc_debounce(void *arg)
{
  uint32_t mask = (uint32_t)arg;
  if (mask & (1 << INC_ENC_IMPL_BTN))  palEnableLineEventI(INC_ENC_BTN_LINE, INC_ENC_IMPL_E_START);
  if (mask & (1 << INC_ENC_IMPL_CW))   palEnableLineEventI(INC_ENC_CW_LINE,  INC_ENC_IMPL_E_START);
  if (mask & (1 << INC_ENC_IMPL_CCW))  palEnableLineEventI(INC_ENC_CCW_LINE, INC_ENC_IMPL_E_START);
  if (mask & ((1 << INC_ENC_IMPL_CW) | (1 << INC_ENC_IMPL_CCW))) _inc_enc_rot_state = INC_ENC_FSM_INIT;
}

static void _inc_enc_cb_btn(void *arg)
{
  (void)arg;
  chSysLockFromISR();

  _inc_enc_send_event(INC_ENC_EVENT_BTN);

  palEnableLineEventI(INC_ENC_BTN_LINE, INC_ENC_IMPL_E_PAUSED);

  chVTSetI(&_inc_enc_debounce_vtp,
           TIME_MS2I(INC_ENC_DEB_T_BTN),
           _inc_enc_debounce,
           (void *)(1 << INC_ENC_IMPL_BTN));

  chSysUnlockFromISR();
}

static void _inc_enc_cb_rot(void *arg)
{
  chSysLockFromISR();
  inc_enc_rot_impulse_t impulse = (inc_enc_rot_impulse_t)arg;

  switch(_inc_enc_rot_state)
  {
    case INC_ENC_FSM_INIT:
      switch(impulse)
      {
	case INC_ENC_IMPL_CW:
	  palEnableLineEventI(INC_ENC_CW_LINE, INC_ENC_IMPL_E_PAUSED);
	  _inc_enc_rot_state = INC_ENC_FSM_WFF_CCW;
	  break;
	case INC_ENC_IMPL_CCW:
          palEnableLineEventI(INC_ENC_CCW_LINE, INC_ENC_IMPL_E_PAUSED);
	  _inc_enc_rot_state = INC_ENC_FSM_WFF_CW;
	  break;
	default:
	  /*
	   * should never happen
	   */
	  _inc_enc_reset_fsm();
	  break;
      }
      break;
    case INC_ENC_FSM_WFF_CW:
      if(impulse == INC_ENC_IMPL_CW)
      {
          _inc_enc_send_event(INC_ENC_EVENT_CCW);
          _inc_enc_reset_fsm();
      }
      break;
    case INC_ENC_FSM_WFF_CCW:
      if(impulse == INC_ENC_IMPL_CCW)
      {
          _inc_enc_send_event(INC_ENC_EVENT_CW);
          _inc_enc_reset_fsm();
      }
      break;
    case INC_ENC_FSM_WF_RESET:
      break;
    default:
      /*
       * should never happen
       */
      _inc_enc_reset_fsm();
      break;
  }
  if(_inc_enc_rot_state == INC_ENC_FSM_WFF_CW || _inc_enc_rot_state == INC_ENC_FSM_WFF_CCW)
  {
      chVTSetI(&_inc_enc_timeout_vtp,
               TIME_MS2I(INC_ENC_TIMEOUT_T),
               _inc_enc_timeout,
               (void *)(_inc_enc_rot_state));
  }
  chSysUnlockFromISR();
}

/*
 * Shell functions
 */

void inc_enc_read_input(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: encoder_read\r\n");
    return;
  }
  event_listener_t  event_listener[INC_ENC_EVENT_MAX];

  chEvtRegister(&inc_enc_events_src[INC_ENC_EVENT_BTN], &event_listener[INC_ENC_EVENT_BTN], INC_ENC_EVENT_BTN);
  chEvtRegister(&inc_enc_events_src[INC_ENC_EVENT_CW],  &event_listener[INC_ENC_EVENT_CW],  INC_ENC_EVENT_CW);
  chEvtRegister(&inc_enc_events_src[INC_ENC_EVENT_CCW], &event_listener[INC_ENC_EVENT_CCW], INC_ENC_EVENT_CCW);

  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT)
  {
      eventmask_t events;

      events = chEvtWaitAny(EVENT_MASK(INC_ENC_EVENT_BTN)
                            | EVENT_MASK(INC_ENC_EVENT_CW)
                            | EVENT_MASK(INC_ENC_EVENT_CCW));
      if (events & EVENT_MASK(INC_ENC_EVENT_BTN)) {
          chprintf(chp, "BTN\r\n");
      }
      if (events & EVENT_MASK(INC_ENC_EVENT_CW)) {
          chprintf(chp, "CW\r\n");
      }
      if (events & EVENT_MASK(INC_ENC_EVENT_CCW)) {
          chprintf(chp, "CCW\r\n");
      }
  }
  chprintf(chp, "\r\n\nstopped\r\n");
}

/*
 * API functions
 */

void inc_enc_init(void)
{
  _inc_enc_init_hal();
  _inc_enc_init_module();
}
