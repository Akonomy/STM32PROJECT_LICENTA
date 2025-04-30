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

void move_car(uint8_t direction, uint8_t tick, uint16_t speed[4]);

void follow_next_direction();

#endif /* INC_MOTION_H_ */
