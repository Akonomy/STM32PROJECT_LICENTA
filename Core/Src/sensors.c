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




float calculeaza_PID(PID_Control *pid, float eroare, float dt) {
    pid->suma_erori += eroare * dt;
    float derivata = (eroare - pid->eroare_anterioara) / dt;
    float iesire = pid->KP * eroare + pid->KI * pid->suma_erori + pid->KD * derivata;
    pid->eroare_anterioara = eroare;
    return iesire;
}








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

// Funcția line_process:
// Procesează senzorii și calculează vitezele pentru cele 4 grupuri de roți
// Ordinea rezultată: [0] FATA_DREAPTA, [1] FATA_STANGA, [2] SPATE_STANGA, [3] SPATE_DREAPTA
// Dacă nu se detectează linie timp de câteva cicluri, se returnează toate vitezele 0.
uint16_t* line_process(void) {
    // Vector static de 4 elemente care va fi returnat
    static uint16_t viteze[4];

    // Variabilă statică pentru contorizarea ciclurilor fără detectare
    static int noDetectionCount = 0;

    // Variabilă statică pentru controlul PID – se inițializează o singură dată
    static PID_Control pid = { .KP = 50.0f, .KI = 0.0f, .KD = 0.0f, .eroare_anterioara = 0.0f, .suma_erori = 0.0f };

    // dt – intervalul de timp între cicluri (exemplu: 20ms)
    float dt = 0.02f;

    // Citim stările senzorilor (operatorul '!' pentru a inversa, conform cerințelor)
    // Se presupune că senzorii sunt poziționați astfel:
    // sensor_data[4] => FAR_LEFT_LINE
    // sensor_data[0] => LEFT_LINE
    // sensor_data[1] => MID_LINE
    // sensor_data[3] => RIGHT_LINE
    // sensor_data[6] => FAR_RIGHT_LINE
    uint8_t FAR_LEFT_LINE  = !sensor_data[4];
    uint8_t LEFT_LINE      = !sensor_data[0];
    uint8_t MID_LINE       = !sensor_data[1];
    uint8_t RIGHT_LINE     = !sensor_data[3];
    uint8_t FAR_RIGHT_LINE = !sensor_data[6];

    // Dacă niciun senzor nu detectează linia, incrementăm contorul
    if (!FAR_LEFT_LINE && !LEFT_LINE && !MID_LINE && !RIGHT_LINE && !FAR_RIGHT_LINE) {
        noDetectionCount++;
        // Dacă s-a depășit pragul, oprește mașina (viteza 0)
        if (noDetectionCount >= NO_DETECTION_THRESHOLD) {
            viteze[0] = viteze[1] = viteze[2] = viteze[3] = 0;
            return viteze;
        }
    } else {
        // Resetăm contorul dacă se detectează cel puțin un senzor
        noDetectionCount = 0;
    }

    // Viteză de bază recomandată pentru deplasare normală
    uint16_t baza = 2000;

    // Inițial, toate grupurile primesc viteza de bază
    viteze[0] = baza; // FATA_DREAPTA
    viteze[1] = baza; // FATA_STANGA
    viteze[2] = baza; // SPATE_STANGA
    viteze[3] = baza; // SPATE_DREAPTA

    // Corecții imediate bazate pe senzorii laterali:
    // Dacă se detectează RIGHT_LINE, se crește viteza pentru roata FATA_DREAPTA (gr1)
    if (RIGHT_LINE) {
        if (viteze[0] + PAS_VITEZA <= VITEZA_MAX)
            viteze[0] += PAS_VITEZA;
    }
    // Dacă se detectează FAR_RIGHT_LINE, se crește viteza pentru roțile din partea dreaptă (gr1 și gr4)
    if (FAR_RIGHT_LINE) {
        if (viteze[0] + PAS_VITEZA <= VITEZA_MAX)
            viteze[0] += PAS_VITEZA;
        if (viteze[3] + PAS_VITEZA <= VITEZA_MAX)
            viteze[3] += PAS_VITEZA;
    }
    // Dacă se detectează LEFT_LINE, se crește viteza pentru roata FATA_STANGA (gr2)
    if (LEFT_LINE) {
        if (viteze[1] + PAS_VITEZA <= VITEZA_MAX)
            viteze[1] += PAS_VITEZA;
    }
    // Dacă se detectează FAR_LEFT_LINE, se crește viteza pentru roțile din partea stângă (gr2 și gr3)
    if (FAR_LEFT_LINE) {
        if (viteze[1] + PAS_VITEZA <= VITEZA_MAX)
            viteze[1] += PAS_VITEZA;
        if (viteze[2] + PAS_VITEZA <= VITEZA_MAX)
            viteze[2] += PAS_VITEZA;
    }

    // Calcularea erorii pe baza senzorilor:
    // Se folosește următoarea formulă:
    // eroare = (RIGHT_LINE + 2*FAR_RIGHT_LINE) - (LEFT_LINE + 2*FAR_LEFT_LINE)
    int eroare = (RIGHT_LINE + 2 * FAR_RIGHT_LINE) - (LEFT_LINE + 2 * FAR_LEFT_LINE);

    // Calculăm corecția PID
    float corectiePID = calculeaza_PID(&pid, (float)eroare, dt);

    // Mapăm rezultatul PID la un offset în trepte de 250 (PAS_VITEZA)
    // Alegem un factor de scalare experimental (de exemplu, împărțim la 100)
    int offset = (int)(corectiePID / 100.0f) * PAS_VITEZA;

    // Aplicăm corecția PID:
    // Dacă offset-ul este pozitiv, creștem viteza pe roțile din partea dreaptă (grupurile 1 și 4)
    // Dacă offset-ul este negativ, creștem viteza pe roțile din partea stângă (grupurile 2 și 3)
    if (offset > 0) {
        if (viteze[0] + offset <= VITEZA_MAX)
            viteze[0] += offset;
        if (viteze[3] + offset <= VITEZA_MAX)
            viteze[3] += offset;
    } else if (offset < 0) {
        int off_abs = -offset;
        if (viteze[1] + off_abs <= VITEZA_MAX)
            viteze[1] += off_abs;
        if (viteze[2] + off_abs <= VITEZA_MAX)
            viteze[2] += off_abs;
    }

    // Pentru funcționare obișnuită, limităm vitezele la un interval restrâns
    for (int i = 0; i < 4; i++) {
        if (viteze[i] < VITEZA_NORMAL_MIN)
            viteze[i] = VITEZA_NORMAL_MIN;
        if (viteze[i] > VITEZA_NORMAL_MAX)
            viteze[i] = VITEZA_NORMAL_MAX;
    }

    return viteze;
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
