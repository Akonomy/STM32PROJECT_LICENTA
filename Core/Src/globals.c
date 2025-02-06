/*
 * globals.c
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#include "globals.h"
#include <stddef.h>




uint8_t sensor_data[8] = {0};
uint8_t CROSS = 0;
uint8_t headTo = 4;      // Default: back
uint8_t rasp_control = 0;


uint8_t last_state_left = 0;
uint8_t last_state_right = 0;
uint8_t last_direction = 0;
uint8_t history[8] = {0};
uint8_t DTIndex = 0;
uint8_t phase = 0;
uint8_t count_up = 0;
uint8_t enough_data = 0;
uint8_t CK_set = 0;
uint8_t speed = 125;

uint8_t *CK = NULL;
uint8_t *DT = NULL;

uint8_t helper[2] = {0, 0};
uint8_t lastStateCK = 0;
uint8_t lastStateDT = 0;
uint8_t clockChanged = 0;
