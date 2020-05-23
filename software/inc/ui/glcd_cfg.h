/*
 * glcd_cfg.h
 *
 *  Created on: 02.05.2020
 *      Author: matti
 */

#ifndef INC_UI_GLCD_CFG_H_
#define INC_UI_GLCD_CFG_H_

#define GLCD_BLPWM_LINE          PAL_LINE(GPIOB, 6U)
#define GLCD_RESET_LINE	         PAL_LINE(GPIOA, 6U)
#define GLCD_SS_LINE             PAL_LINE(GPIOA, 4U)
#define GLCD_SCK_LINE            PAL_LINE(GPIOA, 5U)
#define GLCD_MOSI_LINE	         PAL_LINE(GPIOA, 7U)

#define GLCD_PWM_TIMER_DRIVER    (&PWMD4)
#define GLCD_PWM_TIMER_CH        1
#define GLCD_PWM_TIMER_FREQ      1000000               	 // 1MHz timer speed
#define GLCD_PWM_TIMER_P_US      100                     // 100us period
#define GLCD_PWM_DEFAULT         25


#define GLCD_SPI_DRIVER          (&SPID1)
#define GLCD_SPI_CR1_MODE        (SPI_CR1_CPHA | SPI_CR1_CPOL) // Mode 3
#define GLCD_SPI_CR1_BR          (SPI_CR1_BR_0 | SPI_CR1_BR_2) //~1MHz

#define GLCD_UPDATE_THREAD_PRIO  128
#define GLCD_UPDATE_THREAD_STACK 1024
#define GLCD_UPDATE_THREAD_P_MS  200


#define GLCD_UI_KEY_LENGTH       20
#define GLCD_UI_VAL_LENGTH       20

#endif /* INC_UI_GLCD_CFG_H_ */
