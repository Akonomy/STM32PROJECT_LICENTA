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

bool SetControlPins(uint8_t code);
void read_sensors(void);
uint8_t line_process(void);
void data_process(void);
void SetSensorRight(uint8_t state);  // BLUE LIGHT
void SetSensorLeft(uint8_t state); //RED LIGHT

#endif /* INC_SENSORS_H_ */
