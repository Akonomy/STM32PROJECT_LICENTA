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

/* Private defines -----------------------------------------------------------*/
#define FORWARD 0x2
#define RIGHT   0x3
#define LEFT    0x4
#define STOP    0x1

#define TIMEOUT_DURATION 100 // Durata timeout-ului (ms)
#define BUFFER_SIZE 256

/* Private variables ---------------------------------------------------------*/
        // Exemplu de variabilă pentru USART
uint8_t i2c_slave_address = 0x08;    // Adresa I2C a sclavului

/* Main ----------------------------------------------------------------------*/
int main(void)
{
    uint8_t direction = 0;

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

    /* Start base timer (ex: TIM14) */
    TIM14->CR1 |= TIM_CR1_CEN;

    /* Infinite loop */
    while (1)
    {



    	if (emergency_flag)
    	        {
    	            // Send back "413\n" immediately.
    	            USART_Send_String("413\n");
    	            SetSensorLeft(0);
    	            SetSensorRight(0);
    	            emergency_flag = 0;
    	 }

        /* ---------------------------------- -----------------
         * Example USART processing:
         * Try to receive up to 5 values. If received, add 1 to each value
         * and send them back.
         * ------------------------------------------------------ */
    	/* ----------------------------------------------------- */

    	if (newDataFlag)
    	{
    	    // Clear the flag so we process new data only once.
    	    newDataFlag = 0;
    	    SetSensorRight(1);

    	    // Extract a complete message from the ring buffer.
    	    // (Assume messages are terminated by '\n'.)
    	    char message[50] = {0};
    	    int msgOffset = 0;

    	    // Continue extracting while data is available and we haven't filled the buffer.
    	    while (RingBuffer_Available() > 0 && msgOffset < (sizeof(message) - 1))
    	    {
    	        // Get one byte from the ring buffer.
    	        uint8_t ch = rxBuffer[rxReadIndex];
    	        rxReadIndex = (rxReadIndex + 1) % RX_BUFFER_SIZE;
    	        message[msgOffset++] = ch;
    	        // If we encounter a newline, assume the message is complete.
    	        if (ch == '\n')
    	        {
    	            break;
    	        }
    	    }
    	    message[msgOffset] = '\0';  // Null-terminate the message

    	    // Now, tokenize the message using spaces as delimiters.
    	    // Assume the message contains up to 5 numeric values.
    	    uint16_t receivedValues[5] = {0};
    	    int numValues = 0;
    	    char *token = strtok(message, " ");
    	    while (token != NULL && numValues < 5)
    	    {
    	        receivedValues[numValues++] = (uint16_t)atoi(token);
    	        token = strtok(NULL, " ");
    	    }

    	    // Add 1 to each received value.
    	    for (int i = 0; i < numValues; i++)
    	    {
    	        receivedValues[i] += 1;
    	    }

    	    // Build a response string from the updated values.
    	    char response[50] = {0};
    	    int offset = 0;
    	    for (int i = 0; i < numValues; i++)
    	    {
    	        offset += sprintf(response + offset, "%d ", receivedValues[i]);
    	    }
    	    strcat(response, "\n");

    	    // Send the response back over USART.
    	    USART_Send_String(response);
    	}
        /* ------------------------------------------------------
         * Rest of your application code (sensor processing, motion, etc.)
         * ------------------------------------------------------ */





        /* Dacă s-a detectat o intersecție (CROSS >= 1) se efectuează o virare */
        if (CROSS >= 1)
        {
            makeTurn(headTo);
            headTo = 4;      // Resetează la direcția default (back)
            CROSS = 0;
            SetSensorRight(0);
            SetSensorLeft(0);
        }

        /* Citirea senzorilor și procesarea datelor */
        read_sensors();
        data_process();

        /* Dacă s-a colectat suficientă informație (flag enough_data este activ) */
        if (enough_data)
        {
            /* Execută o comandă (ex: stop) */
            go(1, 0);
            SetSensorRight(1);
            SetSensorLeft(1);

            char result[50] = {0};
            /* Convertește datele colectate (ex. din array-ul history sau sensor_data) într-un șir */
            convert_array_to_string(history, DTIndex, result, sizeof(result));
            USART_Send_String(result);

            /* Resetează flag-ul pentru date suficiente */
            enough_data = 0;

            /* Procesează comenzi primite de la Raspberry Pi */
            rasp_process();
        }

        /* Dacă modul de control de la Raspberry Pi este activ */
        if (rasp_control)
        {
            raspberry_control();
        }

        /* ARDUINO COMMUNICATION ZONE - Nu modifica */
        direction = line_process();

        /* Ajustează viteza în funcție de starea CK_set */
        if (CK_set)
        {
            speed = 145;
        }
        else
        {
            speed = 128;
        }
        /* Trimite comanda către Arduino prin I2C */
        SendSingleValue(0x08, speed, direction);

        DelayWithTimer(15); // Întârziere înainte de următorul ciclu
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
