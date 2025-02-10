/*
 * communication.c
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#include "communication.h"
#include "main.h"      // Pentru acces la registrele I2C etc.
#include "timer.h"     // Pentru DelayWithTimer
#include "globals.h"

#include "usart.h"

#include "sensors.h"        // pentru read_sensors(), SetSensorLeft(), SetSensorRight()
#include "motion.h"         // pentru go_servo() și go()
#include "utils.h"          // pentru convert_array_to_string()


void I2C_Send_Buffer(uint8_t slave_address, uint8_t *data, uint16_t size) {
	/* Wait until I2C is not busy */
	while (I2C1->ISR & I2C_ISR_BUSY)
		;

	/* Configure the slave address and the number of bytes to transfer */
	I2C1->CR2 = (slave_address << 1) | // Set slave address (7-bit left aligned)
			(size << 16) |            // Set number of bytes to transfer
			I2C_CR2_START |           // Generate START condition
			I2C_CR2_AUTOEND; // Enable AUTOEND (STOP condition automatically after N bytes)

	/* Transmit the data */
	for (uint16_t i = 0; i < size; i++) {
		/* Wait until TXIS (Transmit Interrupt Status) is set */

		while (!(I2C1->ISR & I2C_ISR_TXIS))
			;

		/* Write the data to the transmit data register */
		I2C1->TXDR = data[i];
	}

	/* Wait until the STOP condition is generated */
	while (!(I2C1->ISR & I2C_ISR_STOPF))
		;

	/* Clear the STOP flag */
	I2C1->ICR = I2C_ICR_STOPCF;
}



void SendSingleValue(uint8_t slave_address, uint8_t index, uint16_t value) {
	uint8_t buffer[3];
	buffer[0] = index;               // Index
	buffer[1] = (value >> 8) & 0xFF; // High byte of value
	buffer[2] = value & 0xFF;        // Low byte of value

	I2C_Send_Buffer(slave_address, buffer, sizeof(buffer)); // Send single value
}






