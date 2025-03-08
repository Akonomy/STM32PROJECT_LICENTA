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


// Adresa I2C a SSD1306; ajustează după necesitate
#define SSD1306_ADDR 0x3C

void I2C_Send_Buffer(uint8_t slave_address, uint8_t *data, uint16_t size);
void SendSingleValue(uint8_t slave_address, uint8_t index, uint16_t value);
void I2C_Send_Packet(uint8_t slave_address, uint16_t mask, uint16_t *values, uint8_t numValues);



#endif /* INC_COMMUNICATION_H_ */
