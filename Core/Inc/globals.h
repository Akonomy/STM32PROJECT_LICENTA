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

#define MIN_PROCESS_INTERVAL 256  // 90 ms


#define EMERGENCY_CODE 56

#define SYSTICK_CTRL   (*(volatile uint32_t *)0xE000E010)
#define SYSTICK_LOAD   (*(volatile uint32_t *)0xE000E014)
#define SYSTICK_VAL    (*(volatile uint32_t *)0xE000E018)
#define SYSTICK_CALIB  (*(volatile uint32_t *)0xE000E01C)

// Definim frecvența de lucru a nucleului (de exemplu, 8 MHz; ajustați conform configurației sistemului)
#define SYSTEM_CORE_CLOCK 8000000UL


//-------------------< DIRECTII >-----------------------//


// Motion definitions using #define
#define STOP                            0   // STOP
#define INAINTE                         1   // INAINTE (FORWARD)
#define INAPOI                          2   // INAPOI (BACKWARD)
#define STANGA_FATA                    3   // DREAPTA-FATA (FRONT-RIGHT)
#define DREAPTA_FATA                     4   // STANGA-FATA (FRONT-LEFT)
#define SPATE_DREAPTA                   5   // spatele se misca in dreapta (BACK-RIGHT)
#define SPATE_STANGA                    6   // spatele se misca in stanga (BACK-LEFT)
#define ROTIRE_DREAPTA                  7   // ROTIRE DREAPTA (ROTATE RIGHT)
#define ROTIRE_STANGA                   8   // ROTIRE STANGA (ROTATE LEFT)
#define LATERALA_DREAPTA                9   // LATERALA DREAPTA (SIDE-RIGHT)
#define LATERALA_STANGA                 10  // LATERALA STANGA (SIDE-LEFT)
#define HARD_TURN_STANGA               11  // hard turn dreapta (HARD RIGHT TURN)
#define HARD_TURN_DREAPTA                12  // hard turn stanga (HARD LEFT TURN)
#define HARD_TURN_SPATE_INAPOI_STANGA    13  // hard turn stanga miscare spate inapoi
#define HARD_TURN_SPATE_INAPOI_DREAPTA   14  // hard turn dreapta miscare spate inapoi
#define DIAGONALA_STANGA_FATA           15  // diagonala stanga fata (DIAGONAL FRONT-LEFT)
#define DIAGONALA_SPATE_DREAPTA          16  // diagonala spate dreapta (DIAGONAL BACK-RIGHT)
#define DIAGONALA_DREAPTA_FATA           17  // diagonala dreapta fata (DIAGONAL FRONT-RIGHT)
#define DIAGONALA_SPATE_STANGA           18  // diagonala spate stanga (DIAGONAL BACK-LEFT)



#define LITTLE_LEFT 19 // little to left (o singura roata)
#define LITTLE_RIGHT 20 // little to right (o singura roata)


//---------------------------< END DIRECTII >-----------------------------------------//



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
