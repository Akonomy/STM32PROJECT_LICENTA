/*
 * globals.h
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#ifndef INC_GLOBALS_H_
#define INC_GLOBALS_H_

#include <stdint.h>




#define MAX_DIRECTIONS 11


#define TIMEOUT_DURATION 100
#define RX_BUFFER_SIZE 256

#define MIN_PROCESS_INTERVAL 1000  // 90 ms


#define EMERGENCY_CODE 56

#define SYSTICK_CTRL   (*(volatile uint32_t *)0xE000E010)
#define SYSTICK_LOAD   (*(volatile uint32_t *)0xE000E014)
#define SYSTICK_VAL    (*(volatile uint32_t *)0xE000E018)
#define SYSTICK_CALIB  (*(volatile uint32_t *)0xE000E01C)

// Definim frecvența de lucru a nucleului (de exemplu, 8 MHz; ajustați conform configurației sistemului)
#define SYSTEM_CORE_CLOCK 8000000UL


//-------------------< DIRECTII >-----------------------//


// Motion definitions using #define
#define STOP                           0   // STOP (index 0)
#define INAINTE                        1   // INAINTE (FORWARD) (index 1)
#define INAPOI                         2   // INAPOI (BACKWARD) (index 2)
#define STANGA_FATA                    3   // STANGA FATA (FRONT-RIGHT) (index 3)
#define DREAPTA_FATA                   4   // DREAPTA FATA (FRONT-LEFT) (index 4)
#define SPATE_STANGA                   5   // spatele se mișcă în stânga (BACK-RIGHT) (index 5)
#define SPATE_DREAPTA                  6   // spatele se mișcă în dreapta (BACK-LEFT) (index 6)
#define ROTIRE_STANGA                  7   // ROTIRE STÂNGA (ROTATE RIGHT) (index 7)
#define ROTIRE_DREAPTA                 8   // ROTIRE DREAPTA (ROTATE LEFT) (index 8)
#define LATERALA_STANGA                9   // LATERALĂ STÂNGA (SIDE-RIGHT) (index 9)
#define LATERALA_DREAPTA               10  // LATERALĂ DREAPTA (SIDE-LEFT) (index 10)
#define HARD_TURN_STANGA               11  // hard turn stânga (HARD RIGHT TURN) (index 11)
#define HARD_TURN_DREAPTA              12  // hard turn dreapta (HARD LEFT TURN) (index 12)
#define HARD_TURN_SPATE_INAPOI_STANGA   13  // hard turn spate înapoi cu mișcare spre stânga (index 13)
#define HARD_TURN_SPATE_INAPOI_DREAPTA  14  // hard turn spate înapoi cu mișcare spre dreapta (index 14)
#define DIAGONALA_STANGA_FATA          15  // diagonală stânga față (DIAGONAL FRONT-LEFT) (index 15)
#define DIAGONALA_SPATE_DREAPTA         16  // diagonală spate dreapta (DIAGONAL BACK-RIGHT) (index 16)
#define DIAGONALA_DREAPTA_FATA          17  // diagonală dreapta față (DIAGONAL FRONT-RIGHT) (index 17)
#define DIAGONALA_SPATE_STANGA          18  // diagonală spate stânga (DIAGONAL BACK-LEFT) (index 18)




//---------------------------< END DIRECTII >-----------------------------------------//
//return true is he see the line
#define SEE_LINE() (!sensor_data[0] || !sensor_data[1] || !sensor_data[3] || !sensor_data[4] || !sensor_data[6])






extern uint16_t sensor_data16[3];
extern uint8_t sensor_data[8];   // Array pentru citirile senzorilor
extern uint8_t CROSS;    // Contor pentru intersecții
extern uint8_t DANGER; //danger flag
extern uint8_t lost_car;

extern uint8_t headTo;           // Direcția pentru viraj (default back)

// Vector global de 16 poziții, inițializat cu 0.
extern uint8_t global_directions[MAX_DIRECTIONS];  //vectorul de directii
extern uint8_t rasp_control;     // Flag pentru controlul de la Raspberry









extern uint8_t mode;

extern uint8_t speed;

extern uint8_t i2c_slave_address;



//variables to memorize where to turn after meet a cross
extern uint8_t direction_cross;

extern const uint16_t directii_implicite[19] ;

extern const uint16_t directie_roata[8] ;
extern volatile uint8_t rxBuffer[RX_BUFFER_SIZE];
extern volatile uint16_t rxWriteIndex;
extern volatile uint16_t rxReadIndex;


extern volatile uint8_t newDataFlag;  // Flag set by ISR when new data arrives
extern volatile uint8_t emergency_flag; // Set if an emergency code is received



extern volatile uint32_t msTicks;



#endif /* INC_GLOBALS_H_ */
