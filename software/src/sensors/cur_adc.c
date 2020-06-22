/*
 * cur_adc.c
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
#include "sensors/cur_adc.h"
#include "sensors/cur_adc_cmd.h"

/*
 * Include dependencies
 */

/*
 * Global variables
 */

/*
 * Forward declarations of static functions
 */
static void     _cur_adc_init_hal               (void);
static void     _cur_adc_init_module            (void);
static void     _cur_adc_start_measurement      (void);
static void     _cur_adc_start_measurement_cb   (void *arg);
static void     _cur_adc_finish_measurement_cb  (ADCDriver *adcp);

/*
 * Static variables
 */
static const ADCConversionGroup _cur_adc_cfg = {
  FALSE,
  1,
  _cur_adc_finish_measurement_cb,
  NULL,
  0, 0,                         /* CR1, CR2 */
  ADC_SMPR1_SMP_AN10(ADC_SAMPLE_13P5),
  0,                            /* SMPR2 */
  ADC_SQR1_NUM_CH(1),
  0,                            /* SQR2 */
  ADC_SQR3_SQ1_N(CUR_ADC_CHANNEL)
};
static virtual_timer_t          _cur_adc_start_measurement_vtp;
static adcsample_t              _cur_adc_sample_shadow;
static uint32_t                 _cur_adc_sample;

/*
 * Static helper functions
 */
static void _cur_adc_init_hal(void)
{
  palSetLineMode(CUR_ADC_INPUT_LINE, CUR_ADC_INPUT_MODE);
  adcStart(CUR_ACD_DRIVER, NULL);
}

static void _cur_adc_init_module(void)
{
  chVTObjectInit(&_cur_adc_start_measurement_vtp);
  _cur_adc_start_measurement();
}

static uint32_t _cur_adc_get_raw(void)
{
  chSysLockFromISR();
  uint32_t ret = _cur_adc_sample;
  chSysUnlockFromISR();
  return ret;
}

static void _cur_adc_start_measurement (void)
{
  adcStartConversionI(CUR_ACD_DRIVER, &_cur_adc_cfg, &_cur_adc_sample_shadow, 1);

  chVTSetI(&_cur_adc_start_measurement_vtp,
           TIME_MS2I(CUR_ADC_PERIOD_MS),
           _cur_adc_start_measurement_cb,
           (void *)(NULL));
}

/*
 * Callback functions
 */

void _cur_adc_start_measurement_cb(void *arg)
{
  (void)arg;
  _cur_adc_start_measurement();
}

static void _cur_adc_finish_measurement_cb(ADCDriver *adcp)
{
  (void)adcp;
  chSysLockFromISR();
  _cur_adc_sample = (_cur_adc_sample_shadow/2) + (_cur_adc_sample/2);
  chSysUnlockFromISR();
}


/*
 * Shell functions
 */

void cur_adc_read_input(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: current_read\r\n");
    return;
  }

  chprintf(chp, "      RAW |      AMP\r\n");
  chprintf(chp, "--------------------\r\n");
  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT)
  {
      uint32_t raw = _cur_adc_get_raw();
      float amp = cur_adc_get_amp();
      chprintf(chp, " %8d | %8f.2\r", raw, amp);
      chThdSleepMilliseconds(100);
  }
  chprintf(chp, "\r\n\nstopped\r\n");
}

/*
 * API functions
 */

void cur_adc_init(void)
{
  _cur_adc_init_hal();
  _cur_adc_init_module();
}

float cur_adc_get_amp(void)
{
  uint32_t raw = _cur_adc_get_raw();
  return (float)(raw)/CUR_ADC_TICKS_PER_MV*CUR_ADC_A_PER_MV;
}
