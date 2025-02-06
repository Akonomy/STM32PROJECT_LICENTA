/*
 * globals.h
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#ifndef INC_GLOBALS_H_
#define INC_GLOBALS_H_

#include <stdint.h>

extern uint8_t sensor_data[8];   // Array pentru citirile senzorilor
extern uint8_t CROSS;            // Contor pentru intersecții
extern uint8_t headTo;           // Direcția pentru viraj (default back)
extern uint8_t rasp_control;     // Flag pentru controlul de la Raspberry

#define TIMEOUT_DURATION 100

extern uint8_t last_state_left;
extern uint8_t last_state_right;
extern uint8_t last_direction;
extern uint8_t history[8];
extern uint8_t DTIndex;
extern uint8_t phase;
extern uint8_t count_up;
extern uint8_t enough_data;
extern uint8_t CK_set;
extern uint8_t speed;

extern uint8_t *CK;
extern uint8_t *DT;

extern uint8_t helper[2];
extern uint8_t lastStateCK;
extern uint8_t lastStateDT;
extern uint8_t clockChanged;

#endif /* INC_GLOBALS_H_ */
