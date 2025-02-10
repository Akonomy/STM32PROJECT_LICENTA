/*
 * timer.h
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include <stdint.h>
#include <stdbool.h>
#include "globals.h"





uint32_t get_system_time_ms(void);





void DelayWithTimer(uint32_t ms);
void Timeout(uint16_t timeout_ms);
void endTimeout(void);
bool checkTimeout(void);

#endif /* INC_TIMER_H_ */
