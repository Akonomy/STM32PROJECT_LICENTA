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



volatile uint8_t rxBuffer[RX_BUFFER_SIZE] = {0};
volatile uint16_t rxWriteIndex = 0;
volatile uint16_t rxReadIndex = 0;



volatile uint8_t newDataFlag = 0;    // Set if any new data is received
volatile uint8_t emergency_flag = 0; // Set if an emergency code is received





//pentru systick
volatile uint32_t msTicks = 0;





