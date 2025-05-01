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


//#include <string.h>
#include <stdint.h>





/* Private defines -----------------------------------------------------------*/
#define FORWARD 0x2
#define RIGHT   0x3
#define LEFT    0x4


#define TIMEOUT_DURATION 100 // Durata timeout-ului (ms)
#define BUFFER_SIZE 256

/* Private variables ---------------------------------------------------------*/
        // Exemplu de variabilă pentru USART


void test_send_packets(void);








/* Main ----------------------------------------------------------------------*/
int main(void)
{



    //DON"T RUN WITH MODE = 1 IN PRODUCTION , this is for debug only, mode should be set by raspberry py

    mode=0;
    uint16_t values[4]={2000,2000,2000,2000};





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
        if (CROSS >= 1 && mode != 0)
        {
            //makeTurn(headTo);
        	follow_next_direction();
            CROSS = 0;
            SetSensorRight(0);
            SetSensorLeft(0);
        }





        if (mode==7){




        blink_direction_vector_debug();

        mode=0;

    }






        /* Citirea senzorilor și procesarea datelor */

        if (mode == 1) {
            // read_sensors();   /*uncomment this to read data from sensors*/

            DriveCommand cmd = line_process();

            // Mecanism de protecție pentru comenzi repetate
            static DriveCommand prev_cmd = {0};       // stochează ultima comandă trimisă
            static uint16_t same_cmd_count = 0;         // numără de câte ori a fost primit același cmd consecutiv
            static uint8_t protection_stage = 0;        // 0: nimic, 1: SetSensorLeft(1) a fost apelată, 2: SetSensorRight(1) a fost apelată

            bool same_cmd = true;
            // Comparăm cmd.mask
            if (cmd.mask != prev_cmd.mask) {
                same_cmd = false;
            } else {
                // Comparăm vectorul de viteze pentru toate cele 4 roți
                for (uint8_t i = 0; i < 4; i++) {
                    if (cmd.speeds[i] != prev_cmd.speeds[i]) {
                        same_cmd = false;
                        break;
                    }
                }
            }

            if (same_cmd) {
                same_cmd_count++;
            } else {
                same_cmd_count = 0;
                protection_stage = 0;
                prev_cmd = cmd;
            }

            // Dacă aceeași comandă a fost primită de 50 de ori, apelăm SetSensorLeft(1)
            if (same_cmd_count >= 50 && protection_stage == 0) {

            	I2C_Send_Packet(i2c_slave_address,0x0009, values, 4);
            	DelayWithTimer(100);

                protection_stage = 1;
            }
            // Dacă aceeași comandă a fost primită de 100 de ori, apelăm SetSensorRight(1)
            // și blocăm trimiterea comenzii (resetând cmd)
            else if (same_cmd_count >= 100 && protection_stage == 1) {
            	I2C_Send_Packet(i2c_slave_address,0x0006, values, 4);
            	DelayWithTimer(100);
                protection_stage = 2;
                same_cmd_count = 0;
            }

            /* ARDUINO COMMUNICATION ZONE - Nu modifica */
            I2C_Send_Packet(i2c_slave_address, cmd.mask, cmd.speeds, 4);

            DelayWithTimer(50);
        }













        DelayWithTimer(5); // Întârziere înainte de următorul ciclu
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
