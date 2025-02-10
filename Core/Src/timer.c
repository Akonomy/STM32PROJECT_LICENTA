/*
 * timer.c
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#include "timer.h"
#include "main.h"  // Pentru accesul la registrele TIM14, TIM16 etc.

void DelayWithTimer(uint32_t ms) {
	TIM14->CNT = 0;                               // Reset counter
	while (TIM14->CNT < ms)
		;                      // Wait for counter to reach ms
}

void Timeout(uint16_t timeout_ms) {
	TIM16->CNT = 0x0000;
	TIM16->CR1 &= ~TIM_CR1_CEN; // Stop the counter
	TIM16->CNT = timeout_ms;    // Set the counter to the desired value
	TIM16->CR1 |= TIM_CR1_CEN;  // Start the counter
}

void endTimeout(void) {
	TIM16->CR1 &= ~TIM_CR1_CEN; // Stop the counter
}

bool checkTimeout(void) {
	//return false if timmer still active

	return (TIM16->CNT == 0 && !(TIM16->CR1 & TIM_CR1_CEN)); // CNT == 0 and timer is stopped

	//return false;
}


// Funcție pentru obținerea timpului de sistem în ms.
uint32_t get_system_time_ms(void)
{
    return msTicks;
}










