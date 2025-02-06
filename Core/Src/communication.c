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


void rasp_process(){

	uint8_t value1 = 0, value2 = 0;
	char response[20]; // Buffer for the response string

	while (value1==0 ) {
		if (USART_Receive_Two_Values(&value1, &value2)) {

			/*Confirmation, on rasp side if not sent in 2 sec another cmd will arrive*/
			// Format the values into a string as "value1_value2"
			//snprintf(response, sizeof(response), "%d_%d\n", value1, value2);
			// Send the response string back to the Raspberry Pi
			//USART_Send_String(response);


			if(value1==195){

				switch(value2){

				case 1:
					headTo=1;
					break;
				case 2:
					headTo=2;
					break;
				case 3:
					headTo=3;
					break;
				case 4:
					headTo=4;
					break;

				default:
					headTo=0;
					break;
				}


			}else if(value1==200){

				rasp_control=1;  //pentru a intra in modul de a fi controlat de raspberry py ignorand celelalte comenzi
				break;

			}

			else{
				value1=0;  //in case of wrong received at first try reset value 1
				SetSensorLeft(0);
			}




	}else {
		   // Optional: Add a small delay to avoid busy-waiting
			DelayWithTimer(50); // Adjust delay as needed
		 }



}//end while

}//end function

void raspberry_control() {

	uint8_t value1 = 0, value2 = 0;
	char response[20]; // Buffer for the response string

	snprintf(response, sizeof(response), "413\n");
	// Send the response string back to the Raspberry Pi
	USART_Send_String(response);

	while (rasp_control) {
		if (USART_Receive_Two_Values(&value1, &value2)) {

			if (value1 == 200) {
				rasp_control = 0;
				SetSensorRight(0);
				SetSensorLeft(0);
				break;
			}

			else if (value1 > 179 && value1<186) {
				go_servo(value1, value2);

			}

			else if (value1 < 10 && value2 < 20) {
				go(value1, value2);

			}

			else if (value1 == 190) {
				read_sensors();

				char result[50]; // Ensure this buffer is large enough
				memset(result, 0, sizeof(result)); // Initialize buffer to 0
				// Convert array to space-separated string
				convert_array_to_string(sensor_data, 8, result, sizeof(result));

				USART_Send_String(result);

			}

			else {
				snprintf(response, sizeof(response), "207\n");
				// Send the response string back to the Raspberry Pi
				USART_Send_String(response);
				SetSensorRight(0);

			}
		} else {
			DelayWithTimer(5);
		}

	}

}


