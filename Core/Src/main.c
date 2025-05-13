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



    //DON"T RUN WITH MODE = 1  or other modes IN PRODUCTION , this is for debug only, mode should be set by raspberry py
	//MODE 0 is default, is safe and nice
    mode=0;
    uint16_t values[4]={2200,2200,2000,2000};
    uint8_t danger_count;





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

    mode=8;
    danger_count=0;








    while (1)
    {
        if (newDataFlag) {
            parse_and_process_data();
        }

        if (CROSS >= 1 && mode != 0) {
            follow_next_direction();
            CROSS = 0;
            SetSensorRight(0);
            SetSensorLeft(0);
        }


        if (DANGER>=1){
        	while(DANGER){


        	read_sensors();

        	if (sensor_data16[0] <1000 || sensor_data16[2] <1000){

        		DelayWithTimer(100);
        		danger_count++;

        	}
        	else{
        		danger_count=0;
        		DANGER=0;

        	}



        	if(danger_count>=100){
        		mode=2;
        		danger_count=0;
        	}

        	}
        }



        switch (mode) {
            case 1: {
                DriveCommand cmd = line_process();

                static DriveCommand prev_cmd = {0};
                static uint16_t same_cmd_count = 0;
                static uint8_t protection_stage = 0;

                bool same_cmd = (cmd.mask == prev_cmd.mask);
                for (uint8_t i = 0; i < 4 && same_cmd; i++) {
                    if (cmd.speeds[i] != prev_cmd.speeds[i]) {
                        same_cmd = false;
                    }
                }

                if (same_cmd) {
                    same_cmd_count++;
                } else {
                    same_cmd_count = 0;
                    protection_stage = 0;
                    prev_cmd = cmd;
                }

                if (same_cmd_count >= 50 && protection_stage == 0) {
                    I2C_Send_Packet(i2c_slave_address, 0x0009, values, 4);
                    DelayWithTimer(100);
                    protection_stage = 1;
                } else if (same_cmd_count >= 100 && protection_stage == 1) {
                    I2C_Send_Packet(i2c_slave_address, 0x0006, values, 4);
                    DelayWithTimer(100);
                    protection_stage = 2;
                    same_cmd_count = 0;

                }
                else if (same_cmd_count >= 25 && protection_stage == 2) {

                    mode = 3; // intră în fallback
                    protection_stage = 3; // nu repeta de mai multe ori
                }



                I2C_Send_Packet(i2c_slave_address, cmd.mask, cmd.speeds, 4);
                DelayWithTimer(30);
                break;
            }
            case 2:
            	USART_Send_Byte(0xDA);
            	mode=0;
            	break;

            case 3:

            	USART_Send_Byte(0xDE);
            	USART_Send_Byte(0xAD);
            	mode=0;

            	break;

            case 5:

            	USART_Send_Byte(0xFA); //confirmare parcare DOCK si pa pa
            	mode=0;

            	break;


            case 6:

            	USART_Send_Byte(0xC8); //confirmare ok amarat ca o ajuns la zona
            	mode=0;

            	break;



            case 7:
                blink_direction_vector_debug();
                mode = 0;
                break;

            case 8:
            	read_sensors();

            	break;



            // Add more modes here when you inevitably decide your robot needs to make coffee
            default:
                break;
        }


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
