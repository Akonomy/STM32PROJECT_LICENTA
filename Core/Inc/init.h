/*
 * init.h
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#ifndef INC_INIT_H_
#define INC_INIT_H_

#include "globals.h"

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_ADC_Init(void);
void MX_I2C1_Init(void);
void MX_TIM14_Init(void);
void MX_TIM16_Init(void);
void MX_USART1_UART_Init(void);

void SysTick_Init(uint32_t ticks);

#endif /* INC_INIT_H_ */
