/*
 * globals.c
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#include "globals.h"
#include <stddef.h>






uint8_t sensor_data[8] = {0};
uint8_t CROSS = 0;
uint8_t lost_car=0;
uint8_t headTo = 4;      // Default: back
uint8_t rasp_control = 0;





//PENTRU MODUL DE LUCRU


/*
 * pentru mode = 0 masina se afla in satare iddle si asteapta comenzi de la raspberry pi
 * pentru mode = 1 masina actioneaza ca si line follower
 * pentru mode = 2 masina ruleaza
 *
 *
 */
uint8_t mode =0;



uint8_t speed = 125;




uint8_t *CK = NULL;
uint8_t *DT = NULL;



uint8_t helper[2] = {0, 0};
uint8_t lastStateCK = 0;
uint8_t lastStateDT = 0;
uint8_t clockChanged = 0;



//variables to memorize where to turn after meet a cross
uint8_t direction_cross=0;


volatile uint8_t newDataFlag = 0;    // Set if any new data is received
volatile uint8_t emergency_flag = 0; // Set if an emergency code is received


volatile uint8_t rxBuffer[RX_BUFFER_SIZE] = {0};
volatile uint16_t rxWriteIndex = 0;
volatile uint16_t rxReadIndex = 0;

const uint16_t directii_implicite[19] = {
    0x0000, // STOP

    0x0055, // INAINTE (FORWARD)
    0x00AA, // INAPOI (BACKWARD)

    0x0009, // STANGA FATA (FRONT-RIGHT)
    0x0006, // DREAPTA-FATA (FRONT-LEFT)



    0x0090, // spatele se misca in stanga (BACK-RIGHT)
    0x0060, // spatele se misca in dreapta (BACK-LEFT)

    0x0069, // ROTIRE STANGA(ROTATE RIGHT)
    0x0096, // ROTIRE DREAPTA (ROTATE LEFT)

    0x0099, // LATERALA STANGA (SIDE-RIGHT)
    0x0066, // LATERALA DREAPTA(SIDE-LEFT)

    0x0041, // hard turn stanga (HARD RIGHT TURN)
    0x0014, // hard turn dreapta (HARD LEFT TURN)

    0x0082, // hard turn stanga miscare spate inapoi
    0x0028, // hard turn dreapta miscare spate inapoi

    0x0011, // diagonala stanga fata (DIAGONAL FRONT-LEFT)
    0x0022, // diagonala spate dreapta (DIAGONAL BACK-RIGHT)

    0x0044, // diagonala dreapta fata (DIAGONAL FRONT-RIGHT)
    0x0088  // diagonala spate stanga (DIAGONAL BACK-LEFT)
};



const uint16_t directie_roata[8] = {
    0x0004, // Fata dreapta - Înainte
    0x0008, // Fata dreapta - Înapoi
    0x0001, // Fata stânga - Înainte
    0x0002, // Fata stânga - Înapoi
    0x0010, // Spate stânga - Înainte
    0x0020, // Spate stânga - Înapoi
    0x0040, // Spate dreapta - Înainte
    0x0080  // Spate dreapta - Înapoi
};





//pentru systick
volatile uint32_t msTicks = 0;





