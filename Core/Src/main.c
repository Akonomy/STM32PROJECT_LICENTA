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

void test_send_packets(void);



void test_send_packets(void) {
    uint16_t mask;
    uint16_t values[16]={4000,900,900,900};

    // -------- Test 1: Activează pe rând câte un pin de la 0 la 7 la PWM 2048 --------


    for (uint8_t ch = 1; ch < 19; ch++) {


        mask=directii_implicite[ch];

        I2C_Send_Packet(i2c_slave_address, mask, values, 4);
        DelayWithTimer(270);
        I2C_Send_Packet(i2c_slave_address, mask, values, 4);
        DelayWithTimer(500);
        DelayWithTimer(500);
        DelayWithTimer(500);
    }




}




/* Main ----------------------------------------------------------------------*/
int main(void)
{



    //DON"T RUN WITH MODE = 1 IN PRODUCTION , this is for debug only, mode should be set by raspberry py

    mode=1;




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


        if (mode == 1 ){

       // read_sensors();   /*uncomment this to read data from sensors*/



        DriveCommand cmd = line_process();



        /* ARDUINO COMMUNICATION ZONE - Nu modifica */
        I2C_Send_Packet(i2c_slave_address, cmd.mask, cmd.speeds, 4);


        DelayWithTimer(100);

        }














        DelayWithTimer(10); // Întârziere înainte de următorul ciclu
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
