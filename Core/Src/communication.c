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




// Funcția I2C_Send_Buffer este deja definită și trimite un buffer de date de mărimea specificată către un slave

// Funcția I2C_Send_Packet construiește un pachet cu următoarea structură:
//  - Primii 2 bytes: mask-ul (16-bit, de exemplu, big-endian: MSB, apoi LSB)
//  - Următorii N*2 bytes: valorile PWM pentru canalele active, în ordine de la canalul cu număr cel mai mic la cel cu număr cel mai mare,
//    unde N este numărul de biți setați în mask.
void I2C_Send_Packet(uint8_t slave_address, uint16_t mask, uint16_t *values, uint8_t numValues) {
    uint8_t activeCount = 0;
    uint8_t i;

    // Calculează numărul de pini activi din mască (16 biti)
    for (i = 0; i < 16; i++) {
        if (mask & (1 << i)) {
            activeCount++;
        }
    }

    // Dimensiunea pachetului: 2 octeți pentru mască + 2 octeți pentru fiecare canal activ
    uint8_t packetSize = 2 + (activeCount * 2);
    uint8_t packet[2 + (activeCount * 2)];

    // Codifică masca în format big-endian
    packet[0] = (mask >> 8) & 0xFF; // High byte
    packet[1] = mask & 0xFF;        // Low byte

    // Pentru fiecare canal activ, completează cu valoarea din vector sau cu 2048 dacă nu sunt suficiente
    for (i = 0; i < activeCount; i++) {
        uint16_t val;
        if (i < numValues) {
            val = values[i];
        } else {
            val = 2048;  // Valoare implicită
        }
        packet[2 + (i * 2)]     = (val >> 8) & 0xFF; // High byte
        packet[2 + (i * 2) + 1] = val & 0xFF;          // Low byte
    }

    // Transmiterea pachetului prin I2C
    I2C_Send_Buffer(slave_address, packet, packetSize);
}




