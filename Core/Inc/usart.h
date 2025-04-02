/**
 * @file usart.h
 * @brief Header file for USART communication functions.
 */

#ifndef USART_H
#define USART_H

#include "main.h" // Include necessary dependencies
#include "usart.h"

// Macros
#define BUFFER_SIZE 256 // Adjust buffer size as needed

// Global variables (extern declarations)
extern uint8_t receive_buffer[BUFFER_SIZE];
extern uint16_t bytes_received;

// Function prototypes
void USART_Send_Byte(uint8_t data);
void USART_Send_Array(uint8_t *data, uint16_t length);

void USART_Send_String(const char *str);





void RingBuffer_Put(uint8_t data);
uint16_t RingBuffer_Available(void);

void RingBuffer_Clear(void);
void ProcessMessage(void);



#endif // USART_H
