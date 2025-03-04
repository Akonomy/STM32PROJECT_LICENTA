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




// Funcția I2C_Send_Buffer este deja definită și trimite un buffer de date de mărimea specificată către un slave

// Funcția I2C_Send_Packet construiește un pachet cu următoarea structură:
//  - Primii 2 bytes: mask-ul (16-bit, de exemplu, big-endian: MSB, apoi LSB)
//  - Următorii N*2 bytes: valorile PWM pentru canalele active, în ordine de la canalul cu număr cel mai mic la cel cu număr cel mai mare,
//    unde N este numărul de biți setați în mask.
void I2C_Send_Packet(uint8_t slave_address, uint16_t mask, uint16_t *values, uint8_t numValues) {
    // Dimensiunea pachetului: 2 bytes pentru mask + 2 bytes pentru fiecare valoare
    uint8_t packetSize = 2 + (numValues * 2);
    uint8_t packet[2 + (numValues * 2)];

    // Codificăm mask-ul: MSB primul, apoi LSB
    packet[0] = (mask >> 8) & 0xFF; // High byte
    packet[1] = mask & 0xFF;        // Low byte

    // Pentru fiecare valoare, codificăm pe 2 bytes (big-endian)
    for (uint8_t i = 0; i < numValues; i++) {
        packet[2 + (i * 2)]     = (values[i] >> 8) & 0xFF; // High byte
        packet[2 + (i * 2) + 1] = values[i] & 0xFF;          // Low byte
    }

    // Transmiterea pachetului prin I2C
    I2C_Send_Buffer(slave_address, packet, packetSize);
}








