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
    uint8_t sensor16_index = 0;  // index pentru sensor_data16[]

    for (uint8_t code = 0; code < 8; code++) {
        if (SetControlPins(code)) {
            // Start ADC conversion
            ADC1->CR |= ADC_CR_ADSTART;

            // Wait for conversion to complete
            while (!(ADC1->ISR & ADC_ISR_EOC))
                ;

            // Citește valoarea raw (12-bit) de la ADC
            uint16_t adc_raw = ADC1->DR;

            // Pentru indicii 0,1,3,4 și 6 folosim un prag pentru a obține valoare binară
            if (code == 0 || code == 1 || code == 3 || code == 4 || code == 6) {
                sensor_data[code] = (adc_raw > 1000) ? 1 : 0;
            }
            // Pentru indicii 2,5 și 7 salvăm valoarea raw în sensor_data16[]
            else if (code == 2 || code == 5 || code == 7) {
                sensor_data16[sensor16_index++] = adc_raw;
            }

            // Delay mic pentru stabilitate
            DelayWithTimer(1);
        }
    }
}


// Funcția de procesare a liniei
DriveCommand line_process(void) {
    DriveCommand cmd;

    // Inițializare viteze pentru cele 4 roți:
    // speed[0] - roata față dreapta
    // speed[1] - roata față stânga
    // speed[2] - roata spate stânga
    // speed[3] - roata spate dreapta
    uint16_t speed[4] = {
        BASE_SPEED, // față dreapta: viteză de bază
        BASE_SPEED, // față stânga: viteză de bază
        BASE_SPEED, // spate stânga: viteză de bază
        BASE_SPEED  // spate dreapta: viteză de bază
    };

    // Inițializare direcții pentru cele 4 roți (implicit FWD)
    uint8_t dir[4] = {
        FWD, // față dreapta: directie înainte
        FWD, // față stânga: directie înainte
        FWD, // spate stânga: directie înainte
        FWD  // spate dreapta: directie înainte
    };

    uint16_t mask = 0;

    // Variabile statice pentru mecanismul de "recuperare" / PID
    static int8_t last_error  = 0;
    static int8_t last_active = 0;
    static uint8_t lost_count = 0;  // numărul de cicluri fără detecție a liniei

    // Actualizează vectorul global de senzori
    read_sensors();
    //verificare senzori_siguranta

    if(sensor_data16[0]<1000 || sensor_data16[2]<1000){


            DANGER = 1;
            cmd.mask = 0x0000;
            for (uint8_t i = 0; i < 4; i++) {
                speed[i] = 0;
            }
            return cmd;
        } else {
            DANGER = 0;






    }

    // Citim senzorii de linie (senzor activ = detectează linia → valoare 1)
    uint8_t far_left_line  = !sensor_data[4];  // senzor stânga-extrem
    uint8_t left_line      = !sensor_data[0];  // senzor stânga
    uint8_t mid_line       = !sensor_data[1];  // senzor mijloc
    uint8_t right_line     = !sensor_data[3];  // senzor dreapta
    uint8_t far_right_line = !sensor_data[6];  // senzor dreapta-extrem

    // ------------------------------
    // Cazuri speciale de oprire
    // 1. Toți senzorii activi (CROSS): STOP
    if (far_left_line && left_line && mid_line && right_line && far_right_line) {
        CROSS = 1;
        cmd.mask = 0x0000;
        for (uint8_t i = 0; i < 4; i++) {
            speed[i] = 0;
        }
        return cmd;
    } else {
        CROSS = 0;
    }

    // 2. Doar senzorii extreme (far left și far right) activi → STOP
    if (far_left_line && far_right_line && !left_line && !mid_line && !right_line) {
        cmd.mask = 0x0000;
        for (uint8_t i = 0; i < 4; i++) {
            speed[i] = 0;
        }
        return cmd;
    }
    // ------------------------------

    // Dacă niciun senzor nu este activ
    if (!far_left_line && !left_line && !mid_line && !right_line && !far_right_line) {
        if (lost_count >= 10) {
            SetSensorRight(1);
            DelayWithTimer(200);
            SetSensorRight(0);

            cmd.mask = 0x0000;
            for (uint8_t i = 0; i < 4; i++) {
                speed[i] = 0;
            }
            return cmd;
        } else {
            lost_count++;
            // Folosim ultimul senzor activ pentru a decide direcția de recuperare:
            // Dacă last_active > 0 → căutare spre dreapta, altfel spre stânga.
            if (last_active > 0) {
                // Căutare spre dreapta:
                speed[0] = BASE_SPEED + mid_adjust;  // față dreapta: + mid_adjust
                speed[1] = BASE_SPEED + mid_adjust;  // față stânga: + mid_adjust
                speed[2] = BASE_SPEED - small_adjust;           // spate stânga: +0
                speed[3] = BASE_SPEED - small_adjust;           // spate dreapta: +0

                dir[0] = FWD;  // față dreapta: directie înainte
                dir[1] = BACK; // față stânga: inversare (întoarce)
                dir[2] = BACK;  // spate stânga: directie înainte
                dir[3] = FWD;  // spate dreapta: directie înainte
            } else if (last_active < 0) {
                // Căutare spre stânga:
                speed[0] = BASE_SPEED + mid_adjust;  // față dreapta: + mid_adjust
                speed[1] = BASE_SPEED + mid_adjust;  // față stânga: + mid_adjust
                speed[2] = BASE_SPEED - small_adjust;           // spate stânga: +0
                speed[3] = BASE_SPEED - small_adjust;           // spate dreapta: +0

                dir[0] = BACK; // față dreapta: inversare (întoarce)
                dir[1] = FWD;  // față stânga: directie înainte
                dir[2] = FWD;  // spate stânga: directie înainte
                dir[3] = BACK;  // spate dreapta: directie înainte
            }
            // Ieșim din blocul "lost" fără a continua cu celelalte condiții
        }
    } else {
        // Resetăm contorul de pierdere
        lost_count = 0;

        // Calculăm eroarea bazată pe senzorii activi (ponderi: far_left=-2, left=-1, mid=0, right=+1, far_right=+2)
        int8_t sum = 0;
        uint8_t count = 0;
        if (far_left_line)  { sum += -2; count++; }
        if (left_line)      { sum += -1; count++; }
        if (mid_line)       { sum +=  0; count++; }
        if (right_line)     { sum +=  1; count++; }
        if (far_right_line) { sum +=  2; count++; }
        int8_t error = (count > 0) ? (sum / count) : 0;
        last_error  = error;
        last_active = error;

        // ------------------------------
        // Structurarea cazurilor de deplasare:
        // 1. Doar senzorul mijloc activ: merge înainte, fără ajustări
        if (mid_line && !left_line && !right_line && !far_left_line && !far_right_line) {
            speed[0] = BASE_SPEED + 0;  // față dreapta: +0
            speed[1] = BASE_SPEED + 0;  // față stânga: +0
            speed[2] = BASE_SPEED + 0;  // spate stânga: +0
            speed[3] = BASE_SPEED + 0;  // spate dreapta: +0

            dir[0] = FWD; // față dreapta: directie înainte
            dir[1] = FWD; // față stânga: directie înainte
            dir[2] = FWD; // spate stânga: directie înainte
            dir[3] = FWD; // spate dreapta: directie înainte
        }
        // 2. Mijloc + dreapta: ajustare ușoară spre dreapta
        else if (mid_line && right_line && !left_line && !far_left_line && !far_right_line) {
            speed[0] = BASE_SPEED + small_adjust;  // față dreapta: +mid_adjust
            speed[1] = BASE_SPEED - small_adjust;  // față stânga: -mid_adjust
            speed[2] = BASE_SPEED + 0;           // spate stânga: +0
            speed[3] = BASE_SPEED + 0;           // spate dreapta: +0

            dir[0] = FWD; // față dreapta: directie înainte
            dir[1] = FWD; // față stânga: directie înainte
            dir[2] = FWD; // spate stânga: directie înainte
            dir[3] = FWD; // spate dreapta: directie înainte
        }
        // 3. Mijloc + stânga: ajustare ușoară spre stânga
        else if (mid_line && left_line && !right_line && !far_left_line && !far_right_line) {
            speed[0] = BASE_SPEED - small_adjust;  // față dreapta: -mid_adjust
            speed[1] = BASE_SPEED + small_adjust;  // față stânga: +mid_adjust
            speed[2] = BASE_SPEED + 0;           // spate stânga: +0
            speed[3] = BASE_SPEED + 0;           // spate dreapta: +0

            dir[0] = FWD; // față dreapta: directie înainte
            dir[1] = FWD; // față stânga: directie înainte
            dir[2] = FWD; // spate stânga: directie înainte
            dir[3] = FWD; // spate dreapta: directie înainte
        }
        // 4. Doar dreapta: virare spre dreapta
        else if (right_line && !mid_line && !left_line && !far_left_line && !far_right_line) {
            speed[0] = BASE_SPEED + mid_adjust; // față dreapta: +high_adjust
            speed[1] = BASE_SPEED + mid_adjust;           // față stânga: +0
            speed[2] = BASE_SPEED - small_adjust;           // spate stânga: +0
            speed[3] = BASE_SPEED - small_adjust;           // spate dreapta: +0

            // Inversăm roata față stânga
            dir[0] = FWD;  // față dreapta: directie înainte
            dir[1] = BACK; // față stânga: inversare (întoarce)
            dir[2] = FWD;  // spate stânga: directie înainte
            dir[3] = FWD;  // spate dreapta: directie înainte
        }
        // 5. Doar stânga: virare spre stânga
        else if (left_line && !mid_line && !right_line && !far_left_line && !far_right_line) {
            speed[0] = BASE_SPEED + mid_adjust;           // față dreapta: +0
            speed[1] = BASE_SPEED + mid_adjust; // față stânga: +high_adjust
            speed[2] = BASE_SPEED - small_adjust;           // spate stânga: +0
            speed[3] = BASE_SPEED - small_adjust;           // spate dreapta: +0

            // Inversăm roata față dreapta
            dir[0] = BACK; // față dreapta: inversare (întoarce)
            dir[1] = FWD;  // față stânga: directie înainte
            dir[2] = FWD;  // spate stânga: directie înainte
            dir[3] = FWD;  // spate dreapta: directie înainte
        }
        // 6. Dreapta + dreapta-extrem: virare puternică spre dreapta
        else if (right_line && far_right_line && !mid_line && !left_line && !far_left_line) {
            speed[0] = BASE_SPEED ;  // față dreapta: +max_adjust
            speed[1] = BASE_SPEED ;           // față stânga: +0
            speed[2] = BASE_SPEED + mid_adjust;           // spate stânga: +0
            speed[3] = BASE_SPEED + mid_adjust; // spate dreapta: +high_adjust

            // Inversăm roata față stânga
            dir[0] = FWD;  // față dreapta: directie înainte
            dir[1] = BACK; // față stânga: inversare (întoarce)
            dir[2] = FWD;  // spate stânga: directie înainte
            dir[3] = BACK;  // spate dreapta: directie înainte
        }
        // 7. Stânga + stânga-extrem: virare puternică spre stânga
        else if (left_line && far_left_line && !mid_line && !right_line && !far_right_line) {
            speed[0] = BASE_SPEED ;         // față dreapta: +0
            speed[1] = BASE_SPEED ;  // față stânga: +max_adjust
            speed[2] = BASE_SPEED + mid_adjust; // spate stânga: +high_adjust
            speed[3] = BASE_SPEED + mid_adjust;           // spate dreapta: +0

            // Inversăm roata față dreapta
            dir[0] = BACK; // față stanga: inversare (întoarce)
            dir[1] = FWD;  // față dreapta: directie înainte
            dir[2] = BACK;  // spate stânga: directie înainte
            dir[3] = FWD;  // spate dreapta: directie înainte
        }
        // 8. Doar dreapta-extrem: ajustare spre dreapta
        else if (far_right_line && !right_line && !mid_line && !left_line && !far_left_line) {
            speed[0] = BASE_SPEED - mid_adjust;  // față dreapta: +max_adjust
            speed[1] = BASE_SPEED - mid_adjust;           // față stânga: +0 (se inversează)
            speed[2] = BASE_SPEED + mid_adjust;  // spate stânga: +mid_adjust
            speed[3] = BASE_SPEED + mid_adjust;  // spate dreapta: +max_adjust

            dir[0] = FWD;  // față dreapta: directie înainte
            dir[1] = BACK; // față stânga: inversare (întoarce)
            dir[2] = FWD;  // spate stânga: directie înainte
            dir[3] = BACK;  // spate dreapta: directie înainte
        }
        // 9. Doar stânga-extrem: ajustare spre stânga
        else if (far_left_line && !left_line && !mid_line && !right_line && !far_right_line) {
            speed[0] = BASE_SPEED - mid_adjust;           // față dreapta: +0 (se inversează)
            speed[1] = BASE_SPEED - mid_adjust;  // față stânga: +max_adjust
            speed[2] = BASE_SPEED + mid_adjust;  // spate stânga: +max_adjust
            speed[3] = BASE_SPEED + mid_adjust;  // spate dreapta: +mid_adjust

            dir[0] = BACK; // față dreapta: inversare (întoarce)
            dir[1] = FWD;  // față stânga: directie înainte
            dir[2] = BACK;  // spate stânga: directie înainte
            dir[3] = FWD; // spate dreapta: inversare (întoarce)
        }
        // 10. Alte situații – folosim corecția PID
        else {
            int8_t correction = error * 200; // Kp ajustabil
            speed[0] = BASE_SPEED + correction; // față dreapta: corecție
            speed[1] = BASE_SPEED - correction; // față stânga: corecție
            speed[2] = BASE_SPEED + 0;          // spate stânga: +0
            speed[3] = BASE_SPEED + 0;          // spate dreapta: +0

            dir[0] = FWD; // față dreapta: directie înainte
            dir[1] = FWD; // față stânga: directie înainte
            dir[2] = FWD; // spate stânga: directie înainte
            dir[3] = FWD; // spate dreapta: directie înainte
        }
        // ------------------------------
    }

    // Verificare: vitezele nenule trebuie să fie în intervalul 301-4000
    for (uint8_t i = 0; i < 4; i++) {
        if (speed[i] != 0) {
            if (speed[i] < 101) {
                speed[i] = 101;
            } else if (speed[i] > 4000) {
                speed[i] = 4000;
            }
        }
    }

    // Construim masca finală pe baza direcțiilor pentru cele 4 roți.
    // directie_roata[] conține bitii pentru fiecare roată:
    // [0] - față dreapta FWD, [1] - față dreapta BACK,
    // [2] - față stânga FWD, [3] - față stânga BACK,
    // [4] - spate stânga FWD, [5] - spate stânga BACK,
    // [6] - spate dreapta FWD, [7] - spate dreapta BACK.
    mask |= (dir[0] ? directie_roata[0] : directie_roata[1]);
    mask |= (dir[1] ? directie_roata[2] : directie_roata[3]);
    mask |= (dir[2] ? directie_roata[4] : directie_roata[5]);
    mask |= (dir[3] ? directie_roata[6] : directie_roata[7]);

    cmd.mask = mask;

    // Asignăm vitezele pentru fiecare roată (folosind speed[0] ... speed[3])
    for (uint8_t i = 0; i < 4; i++) {
        cmd.speeds[i] = speed[i];
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
