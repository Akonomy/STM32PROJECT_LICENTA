/*
 * utils.c
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>




void convert_array_to_string(uint8_t *array, uint8_t size, char *result,
		int result_size) {
	int offset = 0; // Tracks the current position in the result string

	for (int i = 0; i < size; i++) {
		// Append each number to the result string
		offset += snprintf(result + offset, result_size - offset, "%d",
				array[i]);

		// Add a space after each number except the last
		if (i < size - 1) {
			offset += snprintf(result + offset, result_size - offset, " ");
		}

		// Check if we've exceeded the buffer size
		if (offset >= result_size) {
			printf("Error: Result buffer too small.\n");
			break;
		}
	}
}

void toBinaryString(uint8_t value, char *binary_string) {
	for (int i = 7; i >= 0; i--) {
		binary_string[7 - i] = (value & (1 << i)) ? '1' : '0';
	}
	binary_string[8] = '\0'; // Null-terminate the string
}






