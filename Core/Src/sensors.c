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
			DelayWithTimer(2);
		}
	}
}




// Logica de corecție:
// 1. Se pornește de la viteza generală (base_speed = 1800).
// 2. Dacă senzorii laterali (left_line și right_line) sunt activi, se aplică ajustări diferențiate:
//    - Dacă senzorul mijlociu (mid_line) este activ, ajustarea este redusă;
//      altfel, se cere o corecție mai agresivă.
// 3. Senzorii far (far_left_line și far_right_line) au o pondere semnificativă (±300).
// 4. Dacă ambii senzori far sunt activi, se oprește robotul (toate vitezele = 0).
// 5. Valorile finale sunt rotunjite la cel mai apropiat multiplu de 100 și limitate între 1500 și 3000.

// Funcția de calcul a comenzii de deplasare
DriveCommand line_process(void) {
    DriveCommand cmd;
    // Inițializare viteze cu baza
    uint16_t speeds[4] = {BASE_SPEED, BASE_SPEED, BASE_SPEED, BASE_SPEED};
    // Variabile pentru direcția fiecărei roți (1 = înainte, 0 = înapoi)
    uint8_t fr_dir = 1, fl_dir = 1, bl_dir = 1, br_dir = 1;
    uint16_t mask = 0;

    // Variabile statice pentru mecanismul de „recuperare” / PID
    static uint8_t last_error = 0;
    static uint8_t lost_count = 0;

    // Actualizează vectorul global de senzori
    read_sensors();

    // Obținem valorile booleene pentru senzorii de linie
    // (senzor activ = detectează linia, deci valoare 1, deoarece se folosește operatorul '!')
    uint8_t far_left_line  = !sensor_data[4];  // Senzor stânga-extrem
    uint8_t left_line      = !sensor_data[0];  // Senzor stânga
    uint8_t mid_line       = !sensor_data[1];  // Senzor mijloc
    uint8_t right_line     = !sensor_data[3];  // Senzor dreapta
    uint8_t far_right_line = !sensor_data[6];  // Senzor dreapta-extrem

    // Dacă toți senzorii sunt activi → CROSS (situație de tip "all active")
    if(far_left_line && left_line && mid_line && right_line && far_right_line) {
         CROSS = 1;
         // Toți senzorii activi: situație ambiguă → STOP
         cmd.mask = 0x0000;
         for (int i = 0; i < 4; i++) {
             cmd.speeds[i] = 0;
         }
         return cmd;
    } else {
         CROSS = 0;
    }

    // Caz special: dacă doar senzorii extreme (far left și far right) sunt activi (fără alți senzori) → STOP
    if(far_left_line && far_right_line && !left_line && !mid_line && !right_line) {


         cmd.mask = 0x0000;
         for (int i = 0; i < 4; i++) {
             cmd.speeds[i] = 0;
         }
         return cmd;
    }

    // Dacă niciun senzor nu este activ
    if(!far_left_line && !left_line && !mid_line && !right_line && !far_right_line) {
         // Dacă nu avem istoric (s-a pierdut linia de prea multe ori), oprim robotul
         if(lost_count >= 10) {

        	 SetSensorRight(1);
        	 DelayWithTimer(200);
        	 SetSensorRight(0);
             cmd.mask = 0x0000;
             for (int i = 0; i < 4; i++) {
                 cmd.speeds[i] = 0;
             }
             return cmd;
         } else {
             lost_count++;
             // Folosim ultimul error calculat pentru a "snifa" direcția unde a fost linia ultima dată
             if(last_error > 0) {
                 // Ultima linie detectată era pe partea dreaptă → virare spre dreapta
                 // Setăm: roata față dreapta merge înainte cu viteza BASE_SPEED + mid_adjust,
                 // iar roata față stânga se inversează
                 fr_dir = 1;      // înainte
                 fl_dir = 0;      // înapoi
                 speeds[0] = BASE_SPEED + mid_adjust;
                 speeds[1] = BASE_SPEED + mid_adjust;
             } else if(last_error < 0) {
                 // Ultima linie detectată era pe partea stângă → virare spre stânga
                 fl_dir = 1;      // înainte
                 fr_dir = 0;      // înapoi
                 speeds[1] = BASE_SPEED + mid_adjust;
                 speeds[0] = BASE_SPEED + mid_adjust;
             }
             // Rotile din spate rămân înainte la viteza de bază
         }
    } else {
         // Dacă cel puțin un senzor este activ, resetăm contorul de pierdere
         lost_count = 0;
         // Calculăm o eroare bazată pe senzorii activi (ponderi: far_left=-2, left=-1, mid=0, right=+1, far_right=+2)
         int sum = 0, count = 0;
         if(far_left_line) { sum += -2; count++; }
         if(left_line)     { sum += -1; count++; }
         if(mid_line)      { sum += 0;  count++; }
         if(right_line)    { sum += 1;  count++; }
         if(far_right_line){ sum += 2;  count++; }
         int error = (count > 0) ? (sum / count) : 0;
         last_error = error;

         // Se evaluează cazurile discrete:

         // 1. Doar mid activ: merge înainte
         if(mid_line && !left_line && !right_line && !far_left_line && !far_right_line) {
             // Fiecare roată: direcție înainte la viteza BASE_SPEED
         }
         // 2. mid + right: linie la mijloc și dreapta → ajustare ușoară spre dreapta
         else if(mid_line && right_line && !left_line && !far_left_line && !far_right_line) {
             speeds[0] = BASE_SPEED + mid_adjust; // roata față dreapta crește
             speeds[1] = BASE_SPEED - mid_adjust; // roata față stânga scade
         }
         // 3. mid + left: linie la mijloc și stânga → ajustare ușoară spre stânga
         else if(mid_line && left_line && !right_line && !far_left_line && !far_right_line) {
             speeds[1] = BASE_SPEED + mid_adjust; // roata față stânga crește
             speeds[0] = BASE_SPEED - mid_adjust; // roata față dreapta scade
         }
         // 4. Doar right: linie doar la dreapta → virare spre dreapta
         //    (roata față dreapta se accelerează, roata față stânga se inversează la viteza de bază)
         else if(right_line && !mid_line && !left_line && !far_left_line && !far_right_line) {
             speeds[0] = BASE_SPEED + high_adjust;
             fl_dir = 0;
         }
         // 5. Doar left: linie doar la stânga → virare spre stânga
         //    (roata față stânga se accelerează, roata față dreapta se inversează la viteza de bază)
         else if(left_line && !mid_line && !right_line && !far_left_line && !far_right_line) {
             speeds[1] = BASE_SPEED + high_adjust;
             fr_dir = 0;
         }
         // 6. right + far right: linie la dreapta și dreapta-extrem → rotire puternică spre dreapta
         //    (roțile din dreapta cresc cu high_adjust, roata față stânga se inversează)
         else if(right_line && far_right_line && !mid_line && !left_line && !far_left_line) {
             speeds[0] = BASE_SPEED + max_adjust; // față dreapta
             speeds[3] = BASE_SPEED + high_adjust; // spate dreapta
             fl_dir = 0;
         }
         // 7. left + far left: linie la stânga și stânga-extrem → rotire puternică spre stânga
         //    (roțile din stânga cresc cu high_adjust, roata față dreapta se inversează)
         else if(left_line && far_left_line && !mid_line && !right_line && !far_right_line) {
             speeds[1] = BASE_SPEED + max_adjust; // față stânga
             speeds[2] = BASE_SPEED + high_adjust; // spate stânga
             fr_dir = 0;
         }
         // 8. Doar far right: linie doar la dreapta-extrem → ajustare spre dreapta
         //    (față și spate dreapta cresc cu mid_adjust; față și spate stânga se inversează la BASE_SPEED + mid_adjust)
         else if(far_right_line && !right_line && !mid_line && !left_line && !far_left_line) {
             speeds[0] = BASE_SPEED + max_adjust; // față dreapta
             speeds[3] = BASE_SPEED + max_adjust; // spate dreapta
             fl_dir = 0;  // față stânga invers
             // Setăm roata spate stânga ca și inversată (dacă e cazul)
             // (aici o alegere; se poate ajusta după nevoie)
             // Exemplu: dacă dorești să inversezi și spatele stânga:
             // bl_dir = 0;
             speeds[2] = BASE_SPEED + mid_adjust;
         }
         // 9. Doar far left: linie doar la stânga-extrem → ajustare spre stânga
         //    (față și spate stânga cresc cu mid_adjust; față și spate dreapta se inversează la BASE_SPEED + mid_adjust)
         else if(far_left_line && !left_line && !mid_line && !right_line && !far_right_line) {
             speeds[1] = BASE_SPEED + max_adjust; // față stânga
             speeds[2] = BASE_SPEED + max_adjust; // spate stânga
             fr_dir = 0;  // față dreapta invers
             speeds[3] = BASE_SPEED + mid_adjust; // spate dreapta invers
         }
         // Dacă nicio condiție discretă nu se potrivește, aplicăm o corecție PID simplă:
         else {
             int correction = error * 200; // Factorul Kp=100 (ajustabil)
             speeds[0] = BASE_SPEED + correction;  // față dreapta primește corecția pozitivă
             speeds[1] = BASE_SPEED - correction;  // față stânga primește corecția negativă
         }
    }



    // Verificare: vitezele trebuie să fie fie 0, fie în intervalul 901-4000
    for (int i = 0; i < 4; i++) {
        if (speeds[i] != 0) {
            if (speeds[i] < 301) {
                speeds[i] = 301;
            } else if (speeds[i] > 4000) {
                speeds[i] = 4000;
            }
        }
    }


    // Construim masca finală folosind direcția fiecărei roți:
    mask |= (fr_dir ? directie_roata[0] : directie_roata[1]);
    mask |= (fl_dir ? directie_roata[2] : directie_roata[3]);
    mask |= (bl_dir ? directie_roata[4] : directie_roata[5]);
    mask |= (br_dir ? directie_roata[6] : directie_roata[7]);

    cmd.mask = mask;
    for (int i = 0; i < 4; i++) {
         cmd.speeds[i] = speeds[i];
    }
    return cmd;
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
