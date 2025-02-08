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
        uint8_t far_left_line  = sensor_data[3];
        uint8_t left_line      = sensor_data[0];
        uint8_t mid_line       = sensor_data[4];
        uint8_t right_line     = sensor_data[6];
        uint8_t far_right_line = sensor_data[1];

        uint8_t direction = 0;  // Direcție implicită: STOP (0)

        // Flaguri helper statice pentru algoritmul de căutare (când niciun senzor nu este activ).
        static uint8_t helper_left = 0;
        static uint8_t helper_right = 0;
        // Variabilă statică pentru a reține ce parte a fost activă ultima dată:
        // -1: senzorii din stânga; 1: senzorii din dreapta; 0: necunoscut.
        static int8_t last_sensor_side = 0;

        /* --- CAZ: CROSS ---
           Pattern: 1 1 1 1 1 (toți senzorii activi)
           Acțiune: Oprește mașina și înregistrează evenimentul CROSS.
        */
        if (far_left_line && left_line && mid_line && right_line && far_right_line) {
            CROSS++;         // Înregistrăm evenimentul de cross.
            direction = 0;   // Oprire.
            helper_left = 0;
            helper_right = 0;
            last_sensor_side = 0;
        }
        /* --- CAZURI: STÂNGA ---
           Tabel:
             Pattern: 1 0 0 0 0  -> direction 10
             Pattern: 1 1 0 0 0  -> direction 3
             Pattern: 1 1 1 0 0  -> direction 8
        */
        else if (far_left_line && !left_line && !mid_line && !right_line && !far_right_line) {
            direction = 10;
            last_sensor_side = -1;
            helper_left = 0;
            helper_right = 0;
        }
        else if (far_left_line && left_line && !mid_line && !right_line && !far_right_line) {
            direction = 3;
            last_sensor_side = -1;
            helper_left = 0;
            helper_right = 0;
        }
        else if (far_left_line && left_line && mid_line && !right_line && !far_right_line) {
            direction = 8;
            last_sensor_side = -1;
            helper_left = 0;
            helper_right = 0;
        }
        /* --- CAZURI: FORWARD ---
           Tabelul original:
             Pattern: 0 1 1 1 0  -> direction 1
             Pattern: 0 1 1 0 0  -> direction 1
             Pattern: 0 0 1 1 0  -> direction 1
           Am inclus și cazul suplimentar:
             Pattern: 0 0 1 0 0  -> direction 1
        */
        else if ((!far_left_line && left_line && mid_line && right_line && !far_right_line) ||
                 (!far_left_line && left_line && mid_line && !right_line && !far_right_line) ||
                 (!far_left_line && !left_line && mid_line && right_line && !far_right_line) ||
                 (!far_left_line && !left_line && mid_line && !right_line && !far_right_line)) {
            direction = 1;
            last_sensor_side = 0;  // Fără deviație laterală.
            helper_left = 0;
            helper_right = 0;
        }
        /* --- CAZURI: DREAPTA ---
           Tabel:
             Pattern: 0 0 1 1 1  -> direction 9
             Pattern: 0 0 0 1 1  -> direction 2
             Pattern: 0 0 0 0 1  -> direction 11
        */
        else if (!far_left_line && !left_line && mid_line && right_line && far_right_line) {
            direction = 9;
            last_sensor_side = 1;
            helper_left = 0;
            helper_right = 0;
        }
        else if (!far_left_line && !left_line && !mid_line && right_line && far_right_line) {
            direction = 2;
            last_sensor_side = 1;
            helper_left = 0;
            helper_right = 0;
        }
        else if (!far_left_line && !left_line && !mid_line && !right_line && far_right_line) {
            direction = 11;
            last_sensor_side = 1;
            helper_left = 0;
            helper_right = 0;
        }
        /* --- CAZ SPECIAL: Niciun Senzor Activ ---
           Pattern: 0 0 0 0 0
           Acțiune:
             - Dacă ultima detecție a fost pe partea stângă, încearcă întâi rotația la stânga (direction 10),
               apoi la dreapta (direction 11).
             - Dacă ultima detecție a fost pe partea dreaptă, încearcă întâi rotația la dreapta,
               apoi la stânga.
             - Dacă ambele opțiuni au fost deja încercate, oprește mașina și setează flagul lost_car.
             - Dacă nu avem informații anterioare, oprește mașina.
        */



        /* --- CAZ: DEFAULT ---
           Pentru orice altă combinație neașteptată, oprește mașina.
        */
        else {
            direction = 0;
            helper_left = 0;
            helper_right = 0;
            last_sensor_side = 0;
        }

        // Actualizăm last_direction pentru referințe ulterioare (ignorăm comenzile STOP (0) și BACK (6)).
        if (direction != 6 && direction != 0) {
            last_direction = direction;
        } else if (direction == 0) {
            last_direction = 0;
        }

        return direction;
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
