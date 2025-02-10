/*
 * communication.h
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#ifndef INC_COMMUNICATION_H_
#define INC_COMMUNICATION_H_

#include <stdint.h>
#include <stdbool.h>

void I2C_Send_Buffer(uint8_t slave_address, uint8_t *data, uint16_t size);
void SendSingleValue(uint8_t slave_address, uint8_t index, uint16_t value);




#endif /* INC_COMMUNICATION_H_ */
