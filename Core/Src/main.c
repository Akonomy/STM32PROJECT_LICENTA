/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"         // Fișierul pentru comunicații USART
#include "init.h"          // Funcții de inițializare hardware
#include "timer.h"         // Funcții de temporizare
#include "communication.h" // Funcții de comunicație (I2C, Raspberry control, etc.)
#include "sensors.h"       // Funcții pentru citirea și procesarea senzorilor
#include "motion.h"        // Funcții de mișcare (go, makeTurn, go_servo)
#include "utils.h"         // Funcții utilitare (convertire șiruri, debug etc.)
#include "globals.h"       // Variabile globale


#include "raspberry_com.h" //process mesaje de la raspberry pi


#include <string.h>
#include <stdint.h>




/* Private defines -----------------------------------------------------------*/
#define FORWARD 0x2
#define RIGHT   0x3
#define LEFT    0x4


#define TIMEOUT_DURATION 100 // Durata timeout-ului (ms)
#define BUFFER_SIZE 256

/* Private variables ---------------------------------------------------------*/
        // Exemplu de variabilă pentru USART
uint8_t i2c_slave_address = 0x08;    // Adresa I2C a sclavului



/* Main ----------------------------------------------------------------------*/
int main(void)
{
    uint8_t direction = 0;
    uint8_t temp_variable[8] = {0};



    /* MCU Configuration */
    /* Activează ceasurile pentru SYSCFG și PWR */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* Configurează prioritatea pentru SysTick */
    NVIC_SetPriority(SysTick_IRQn, 3);

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_ADC_Init();
    MX_I2C1_Init();
    MX_TIM14_Init();
    MX_TIM16_Init();
    MX_USART1_UART_Init();

    SysTick_Init(SYSTEM_CORE_CLOCK / 1000); // Dacă SYSTEM_CORE_CLOCK e 8MHz, atunci ticks = 8000


    /* Start base timer (ex: TIM14) */
    TIM14->CR1 |= TIM_CR1_CEN;

    /* Infinite loop */






    while (1)
    {









/*
    	for (uint8_t x = 0; x < 21; x++) {

    		for (uint8_t ticks=0; ticks<20; ticks++){
    			SendSingleValue(0x08, 130, x);
    			DelayWithTimer(50);
    		}
    			DelayWithTimer(500);
    			SendSingleValue(0x08, 130, 0);
    			DelayWithTimer(500);
    			DelayWithTimer(500);
    			DelayWithTimer(500);
    			DelayWithTimer(500);

    	}

*/



    	if (newDataFlag){
    		parse_and_process_data();
    	}



        /* Dacă s-a detectat o intersecție (CROSS >= 1) se efectuează o virare */
        if (CROSS >= 1)
        {
            //makeTurn(headTo);
            headTo = 4;      // Resetează la direcția default (back)
            CROSS = 0;
            SetSensorRight(0);
            SetSensorLeft(0);
        }

        /* Citirea senzorilor și procesarea datelor */



        read_sensors();   /*uncomment this to read data from sensors*/


        DelayWithTimer(15);




        /* ARDUINO COMMUNICATION ZONE - Nu modifica */


        direction = line_process();  /*uncomment this to process data from line sensors*/



        /* Trimite comanda către Arduino prin I2C */

        SendSingleValue(0x08, speed, direction);   /*uncomment this to send data to arduino*/

        DelayWithTimer(30); // Întârziere înainte de următorul ciclu
    }
}

/* USER CODE BEGIN 3 */
/* USER CODE END 3 */

/* Error Handler */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* Raportează eroarea (ex: se poate folosi printf sau log) */
}
#endif /* USE_FULL_ASSERT */
