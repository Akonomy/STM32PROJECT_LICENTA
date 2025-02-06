/*
 * motion.h
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#ifndef INC_MOTION_H_
#define INC_MOTION_H_

#include <stdint.h>

void makeTurn(uint8_t direction_x);
void go(uint8_t max_x, uint8_t direction_x);
void go_servo(uint8_t servo, uint8_t position);

#endif /* INC_MOTION_H_ */
