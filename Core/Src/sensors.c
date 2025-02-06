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





    uint8_t line_process() {
    	// Assign sensor data to logical variables
    	uint8_t left_line = sensor_data[0];
    	uint8_t mid_line = sensor_data[4];
    	uint8_t right_line = sensor_data[6];

    	// Default direction is STOP
    	uint8_t direction = 0;

    	if (mid_line) {
    		// Middle sensor is on the line
    		if (left_line && right_line) {
    			// All sensors are active -> Stop
    			CROSS++;
    			direction = 0;  // STOP
    			helper[0] = 0;  // Reset guesses
    			helper[1] = 0;  // Reset guesses
    		} else if (left_line && !right_line) {
    			// Middle and Left sensors active -> Adjust slightly left
    			direction = 5;  // A LITTLE TO LEFT
    			helper[0] = 0;  // Reset guesses
    			helper[1] = 0;  // Reset guesses
    		} else if (right_line && !left_line) {
    			// Middle and Right sensors active -> Adjust slightly right
    			direction = 4;  // A LITTLE TO RIGHT
    			helper[0] = 0;  // Reset guesses
    			helper[1] = 0;  // Reset guesses
    		} else {
    			// Only Middle sensor active -> Go Forward
    			direction = 1;  // FORWARD
    			helper[0] = 0;  // Reset guesses
    			helper[1] = 0;  // Reset guesses
    		}
    	} else {  // Middle sensor is off the line
    		if (left_line && !right_line) {
    			// Line is on the left -> Turn left
    			direction = 3;  // LEFT
    			helper[0] = 0;  // Reset guesses
    			helper[1] = 0;  // Reset guesses
    		} else if (right_line && !left_line) {
    			// Line is on the right -> Turn right
    			direction = 2;  // RIGHT
    			helper[0] = 0;  // Reset guesses
    			helper[1] = 0;  // Reset guesses
    		} else if (!left_line && !right_line) {

    			// All sensors are off -> Alternate based on helper
    			if (last_direction == 2 && helper[1] == 0) {
    				direction = 2;  // Guess LEFT
    				go(1, 2);
    				helper[1] = 1;  // Mark RIGHT as used
    			}

    			else if (last_direction == 3 && helper[0] == 0) {
    				direction = 3;  // Guess RIGHT
    				go(1, 3);
    				helper[0] = 1;  // Mark LEFT as used
    			}

    			else if (helper[0] == 1 && helper[1] == 1) {
    				// Both directions used -> Stop
    				direction = 0;  // STOP
    				helper[0] = 0;  // Reset guesses
    				helper[1] = 0;  // Reset guesses
    				go(1, 4);
    				go(1, 1);
    			}

    		} else {
    			// Unexpected case: Default to STOP
    			direction = 0;  // STOP
    			helper[0] = 0;  // Reset guesses
    			helper[1] = 0;  // Reset guesses
    		}
    	}

    	// Update last_direction for tracking
    	if (direction != 6 && direction != 0) { // Exclude STOP and BACK commands from direction memory
    		last_direction = direction;
    	}

    	if (helper[1] && !helper[0]) {
    		last_direction = 3;
    	}
    	if (helper[0] && !helper[1]) {
    		last_direction = 2;
    	}
    	if (direction == 0) {
    		last_direction = 0;
    	}

    	return direction;
    }







    void data_process() {
    	// Attribute local names
    	uint8_t data_left = sensor_data[3];
    	uint8_t data_right = sensor_data[1];
    	clockChanged = 0;

    	// PHASE 0: INIT PHASE
    	if (phase == 0) {
    		// Check sensors

    		// Reset data array for a fresh start

    		//set a CK
    		if (!CK_set) {
    			if (data_left == 0 && data_right == 0)
    				return; // None high
    			if (data_left == 1 && data_right == 1)
    				return; // Both high

    			// Determine CK and DT

    			CK = (data_left == 1) ? &data_left : &data_right;
    			DT = (data_left == 0) ? &data_left : &data_right;

    			CK_set = 1;

    		}

    		if (CK_set) {
    			if (count_up == 0 && (*CK == 1 && *DT == 0)) {
    				count_up++;
    				Timeout(TIMEOUT_DURATION);
    				return;

    			} else if ((!checkTimeout() && count_up == 1)
    					&& (*CK == 0 && *DT == 0)) {
    				count_up++;
    				Timeout(TIMEOUT_DURATION);
    				return;

    			} else if ((!checkTimeout() && count_up == 2)
    					&& (*CK == 1 && *DT == 0)) {

    				for (int i = 0; i < 16; i++) {
    					history[i] = 0;
    				}
    				DTIndex = 0;

    				enough_data = 0;
    				count_up++;
    				lastStateCK = *CK;
    				lastStateDT = *DT;
    				phase++;
    				Timeout(TIMEOUT_DURATION);
    				return;

    			} else if (checkTimeout() || *DT == 1) {
    				count_up = 0;
    				CK_set = 0;
    				phase = 0;

    			}

    		} //end if ck_set

    	}

    	// PHASE 1: READ DATA
    	if (phase == 1) {

    		if (checkTimeout()) {



    			if (checkTimeout() && DTIndex > 4) {
    					phase = 0;
    					enough_data = 1;
    					SetSensorRight(1);
    				}

    			else {
    			phase = 0; // Timeout, reset phase
    			count_up = 0;
    			DTIndex = 0;
    			CK_set = 0;
    			}
    			return;

    		}

    		if (*CK != lastStateCK) {
    			lastStateCK = *CK;
    			clockChanged = 1;
    			Timeout(600);


    		}

    		if (clockChanged && *CK == 1) { // daca a existat o tranzitie si clock e high

    			history[DTIndex] = *DT;
    			DTIndex++;
    		}

    		else if (DTIndex > 5) {

    			phase = 0;
    			enough_data = 1;

    		}

    	}

    } //end void


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
