/*
 * motion.h
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#ifndef INC_MOTION_H_
#define INC_MOTION_H_





#include <stdint.h>



void update_sensors();

void makeTurn(uint8_t direction_x);

uint8_t confirm_line_detection_or_continue(uint8_t movement);

void move_car(uint8_t direction, uint8_t tick, uint16_t speed[4]);

void move_wheel(uint8_t direction, uint8_t tick, uint16_t speed[4]);


void follow_next_direction();

#endif /* INC_MOTION_H_ */
