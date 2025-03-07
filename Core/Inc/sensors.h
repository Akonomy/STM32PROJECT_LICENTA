/*
 * sensors.h
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#ifndef INC_SENSORS_H_
#define INC_SENSORS_H_

#include <stdint.h>
#include <stdbool.h>

// Intervalele de viteză și pasul de modificare (în trepte de 250)
#define VITEZA_MIN           1250
#define VITEZA_MAX           3500
#define PAS_VITEZA           250

// Interval recomandat pentru funcționare normală
#define VITEZA_NORMAL_MIN    1500
#define VITEZA_NORMAL_MAX    2500

// Prag pentru cicluri fără detectare (ex: 5 cicluri consecutive)
#define NO_DETECTION_THRESHOLD 5




// Structura pentru controlul PID
typedef struct {
    float KP;
    float KI;
    float KD;
    float eroare_anterioara;
    float suma_erori;
} PID_Control;

// Prototipul funcției PID
float calculeaza_PID(PID_Control *pid, float eroare, float dt);

// Prototipul funcției line_process care calculează vitezele pentru cele 4 grupuri de roți
// Rezultatul este un vector static de 4 valori:
// [0] Fata dreapta, [1] Fata stânga, [2] Spate stânga, [3] Spate dreapta.
uint16_t* line_process(void);






bool SetControlPins(uint8_t code);
void read_sensors(void);

void data_process(void);
void SetSensorRight(uint8_t state);  // BLUE LIGHT
void SetSensorLeft(uint8_t state); //RED LIGHT

#endif /* INC_SENSORS_H_ */
