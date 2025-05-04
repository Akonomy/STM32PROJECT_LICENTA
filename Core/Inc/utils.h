/*
 * utils.h
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

#include "globals.h"
#include "timer.h"
#include "sensors.h"

void blink_direction_vector_debug(void);


void convert_array_to_string(uint8_t *array, uint8_t size, char *result,
		int result_size);
void toBinaryString(uint8_t value, char *binary_string);

void setGlobaldirections(uint8_t v1, uint8_t v2,uint8_t fill_value);


#endif /* INC_UTILS_H_ */
