/*
 * globals.h
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#ifndef INC_GLOBALS_H_
#define INC_GLOBALS_H_

#include <stdint.h>

extern uint8_t sensor_data[8];   // Array pentru citirile senzorilor
extern uint8_t CROSS;            // Contor pentru intersecții
extern uint8_t lost_car;
extern uint8_t headTo;           // Direcția pentru viraj (default back)
extern uint8_t rasp_control;     // Flag pentru controlul de la Raspberry

#define TIMEOUT_DURATION 100
#define RX_BUFFER_SIZE 256

#define MIN_PROCESS_INTERVAL 90  // 90 ms


#define EMERGENCY_CODE 56

#define SYSTICK_CTRL   (*(volatile uint32_t *)0xE000E010)
#define SYSTICK_LOAD   (*(volatile uint32_t *)0xE000E014)
#define SYSTICK_VAL    (*(volatile uint32_t *)0xE000E018)
#define SYSTICK_CALIB  (*(volatile uint32_t *)0xE000E01C)

// Definim frecvența de lucru a nucleului (de exemplu, 8 MHz; ajustați conform configurației sistemului)
#define SYSTEM_CORE_CLOCK 8000000UL






extern uint8_t last_state_left;
extern uint8_t last_state_right;
extern uint8_t last_direction;
extern uint8_t history[8];
extern uint8_t DTIndex;
extern uint8_t phase;
extern uint8_t count_up;
extern uint8_t enough_data;
extern uint8_t CK_set;
extern uint8_t speed;

extern uint8_t *CK;
extern uint8_t *DT;

extern uint8_t helper[2];
extern uint8_t lastStateCK;
extern uint8_t lastStateDT;
extern uint8_t clockChanged;



//variables to memorize where to turn after meet a cross
extern uint8_t direction_cross;



extern volatile uint8_t rxBuffer[RX_BUFFER_SIZE];
extern volatile uint16_t rxWriteIndex;
extern volatile uint16_t rxReadIndex;


extern volatile uint8_t newDataFlag;  // Flag set by ISR when new data arrives
extern volatile uint8_t emergency_flag; // Set if an emergency code is received



extern volatile uint32_t msTicks;



#endif /* INC_GLOBALS_H_ */
