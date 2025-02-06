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

void convert_array_to_string(uint8_t *array, uint8_t size, char *result,
		int result_size);
void toBinaryString(uint8_t value, char *binary_string);
void ProcessReceivedData(uint8_t *buffer, uint16_t length);


bool isFirstValueEqual(uint16_t *values, uint8_t count, uint16_t expected);

#endif /* INC_UTILS_H_ */
