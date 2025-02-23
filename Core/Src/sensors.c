/*
 * sensors.c
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#include "sensors.h"
#include "timer.h"
#include "globals.h"
#include "main.h" // Pentru definițiile pinilor, registre ADC etc.
#include "motion.h"

bool SetControlPins(uint8_t code) {
	GPIOA->BSRR = (code & 0x01 ? A_Pin : (uint32_t) A_Pin << 16U)
			| (code & 0x02 ? B_Pin : (uint32_t) B_Pin << 16U)
			| (code & 0x04 ? C_Pin : (uint32_t) C_Pin << 16U);
	return true;
}

void read_sensors() {
	for (uint8_t code = 0; code < 8; code++) {
		if (SetControlPins(code)) {
			// Start ADC conversion
			ADC1->CR |= ADC_CR_ADSTART;

			// Wait for conversion to complete
			while (!(ADC1->ISR & ADC_ISR_EOC))
				;

			// Read raw ADC value (12-bit)
			//uint16_t adcValue = ADC1->DR;

			// Store the raw ADC value in the sensor_data array

			//convertadc to bin

			uint8_t adcValue = (ADC1->DR > 1000) ? 1 : 0; // Threshold ADC value to 1 or 0

			sensor_data[code] = adcValue;

			// Small delay for stability
			DelayWithTimer(5);
		}
	}
}

uint8_t line_process(void) {
	// Mapăm valorile senzorilor folosind nume semnificative.
	// Ordinea senzorilor: far_left_line, left_line, mid_line, right_line, far_right_line.
	uint8_t far_left_line = sensor_data[4];
	uint8_t left_line = sensor_data[0];
	uint8_t mid_line = sensor_data[1];
	uint8_t right_line = sensor_data[3];
	uint8_t far_right_line = sensor_data[6];

	uint8_t direction = 0;  // Direcție implicită: STOP (0)



	//verificam daca linia din mijloc e activa

	if (mid_line) {

		//verificam daca toate liniile sunt active

		if (far_right_line && far_left_line && right_line && left_line) {

			direction = STOP;
			CROSS = 1;
			return direction;

		}

		else {

			direction = INAINTE;
			return direction;
		}

	}


	//DACA AI AJUNS AICI ATUNCI MID LINE NU E SET



	//SE VERIFICA INTAI SENZORII IMEDIAT DREAPTA / IMEDIUAT STANGA
	if (right_line && !left_line){

		direction=DREAPTA_FATA;

		return direction;

	}


	if (left_line && !right_line){

		direction=STANGA_FATA;

		return direction;

	}



	if (far_left_line && !far_right_line){

		direction=HARD_TURN_STANGA;

		return direction;

	}


	if (far_right_line && !far_left_line){

		direction=HARD_TURN_DREAPTA;

		return direction;

	}







}












void SetSensorRight(uint8_t state) {
	if (state) {
		signal_right_GPIO_Port->BSRR = signal_right_Pin; // Set pin high
	} else {
		signal_right_GPIO_Port->BSRR = (uint32_t) signal_right_Pin << 16U; // Reset pin low
	}
}

void SetSensorLeft(uint8_t state) {
	if (state) {
		signal_left_GPIO_Port->BSRR = signal_left_Pin; // Set pin high
	} else {
		signal_left_GPIO_Port->BSRR = (uint32_t) signal_left_Pin << 16U; // Reset pin low
	}
}
