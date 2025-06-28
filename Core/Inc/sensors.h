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



#define FWD 1
#define BACK 0


// Definirea vitezei de bază și a ajustărilor (modificabile după nevoie)
#define BASE_SPEED 1300
static const uint16_t small_adjust = 250;
static const uint16_t mid_adjust   = 500;
static const uint16_t high_adjust  = 1000;
static const uint16_t max_adjust   = 2000;  // (nu este folosit explicit în condiții, dar poate fi integrat ulterior)

// Structura ce va fi returnată: o mască și un vector de 4 viteze
typedef struct {
    uint16_t mask;
    uint16_t speeds[4];  // [0]=Fata dreapta, [1]=Fata stânga, [2]=Spate stânga, [3]=Spate dreapta
} DriveCommand;









// Prototipul funcției line_process care calculează vitezele pentru cele 4 grupuri de roți
// Rezultatul este un vector static de 4 valori:
// [0] Fata dreapta, [1] Fata stânga, [2] Spate stânga, [3] Spate dreapta.
DriveCommand line_process(void);





bool SetControlPins(uint8_t code);
void read_sensors(void);

void data_process(void);
void SetSensorRight(uint8_t state);  // BLUE LIGHT
void SetSensorLeft(uint8_t state); //RED LIGHT

#endif /* INC_SENSORS_H_ */
