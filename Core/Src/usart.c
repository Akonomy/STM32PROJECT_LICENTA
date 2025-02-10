/*
 * usart.c
 *
 *  Created on: Dec 4, 2024
 *      Author: andre
 */

/**
 * @file usart.c
 * @brief Contains functions for USART communication.
 */
#include <stdio.h>
#include "usart.h"
#include <string.h> // For memset, strncpy, etc.
#include <stdlib.h> // For atoi
#include "globals.h"


// Global variables
uint8_t receive_buffer[BUFFER_SIZE]; // Buffer for received data
uint16_t bytes_received;            // Number of bytes received

/**
 * @brief Sends a single byte via USART.
 * @param data The byte to send.
 */
void USART_Send_Byte(uint8_t data) {
	while (!(USART1->ISR & USART_ISR_TXE))
		; // Wait until TXE flag is set
	USART1->TDR = data;          // Write the data to the Transmit Data Register
	while (!(USART1->ISR & USART_ISR_TC))
		; // Wait until TC flag is set
}

/**
 * @brief Sends an array of data via USART.
 * @param data Pointer to the data array.
 * @param length Length of the data array.
 */
void USART_Send_Array(uint8_t *data, uint16_t length) {
	for (uint16_t i = 0; i < length; i++) {
		USART_Send_Byte(data[i]); // Send each byte
	}
}

/**
 * @brief Receives data from USART and parses it into two uint8_t values.
 * @param value1 Pointer to store the first uint8_t value.
 * @param value2 Pointer to store the second uint8_t value.
 * @return True if successful, false otherwise.
 */
bool USART_Receive_Two_Values(uint8_t *value1, uint8_t *value2) {
	char buffer[BUFFER_SIZE] = { 0 }; // Temporary buffer for received string
	uint16_t count = 0;

	// Receive the string
	while (count < BUFFER_SIZE) {
		if (USART1->ISR & USART_ISR_RXNE) { // Check if RXNE flag is set
			char received_char = (char) USART1->RDR; // Read the received character
			if (received_char == '\n') { // Stop on newline
				buffer[count] = '\0'; // Null-terminate the string
				break;
			}
			buffer[count++] = received_char;
		}
	}

	if (count == 0) {
		return false; // No data received
	}

	// Parse the string for two uint8_t values
	char *token = strtok(buffer, " "); // Split string at the first space
	if (token == NULL) {
		return false; // Parsing failed
	}

	*value1 = (uint8_t) atoi(token); // Convert the first token to uint8_t

	token = strtok(NULL, " "); // Get the next token
	if (token == NULL) {
		return false; // Parsing failed
	}

	*value2 = (uint8_t) atoi(token); // Convert the second token to uint8_t

	return true; // Parsing succeeded
}

/**
 * @brief Sends a null-terminated string via USART.
 * @param str Pointer to the null-terminated string.
 */
void USART_Send_String(const char *str) {
	while (*str) {
		USART_Send_Byte((uint8_t) *str); // Send each character
		str++;
	}
}

/**
 * @brief Checks for received data and processes it.
 */

/**
 * @brief Receives and parses two integer values from USART.
 * @param value1 Pointer to store the first integer.
 * @param value2 Pointer to store the second integer.
 * @return True if successful, false otherwise.
 */

/**
 * @brief Processes received data (Placeholder for user-defined logic).
 * @param buffer Pointer to the received data buffer.
 * @param length Length of the received data.
 */






/**
 * @brief Receives data from USART and parses up to max_values uint8_t values.
 *
 * This function reads characters from USART until a newline ('\n') is encountered
 * or until the buffer is full. It then tokenizes the received string by spaces and
 * converts each token into a uint8_t value. The number of tokens parsed may be less
 * than max_values (e.g., 2, 3, or 4 values), depending on the input.
 *
 * @param values Pointer to an array where parsed values will be stored.
 * @param max_values Maximum number of values to parse (for example, 5).
 * @return The number of values successfully parsed, or -1 if no data was received.
 */
int USART_Receive_Values(uint16_t *values, uint8_t max_values) {
    char buffer[BUFFER_SIZE] = {0}; // Temporary buffer for received string
    uint16_t count = 0;

    // Receive characters until newline is encountered or the buffer is full.
    while (count < BUFFER_SIZE) {
        if (USART1->ISR & USART_ISR_RXNE) { // Check if RXNE flag is set
            char received_char = (char)USART1->RDR; // Read the received character
            if (received_char == '\n') { // End of message detected
                buffer[count] = '\0'; // Null-terminate the string
                break;
            }
            buffer[count++] = received_char;
        }
    }

    if (count == 0) {
        return -1; // No data received
    }

    // Tokenize the string using space as the delimiter.
    int token_count = 0;
    char *token = strtok(buffer, " ");
    while (token != NULL && token_count < max_values) {
        values[token_count] = (uint16_t)atoi(token);  // Convert to uint16_t instead of uint8_t
        token_count++;
        token = strtok(NULL, " ");
    }

    return token_count;
}



void RingBuffer_Put(uint8_t data)
{
    rxBuffer[rxWriteIndex] = data;
    rxWriteIndex = (rxWriteIndex + 1) % RX_BUFFER_SIZE;
    // Optionally handle overflow: if writeIndex catches up to readIndex.
}

uint16_t RingBuffer_Available(void)
{
    if (rxWriteIndex >= rxReadIndex)
        return rxWriteIndex - rxReadIndex;
    else
        return RX_BUFFER_SIZE - rxReadIndex + rxWriteIndex;
}


void RingBuffer_Clear(void)
{
    // Resetăm indicii pentru a marca buffer-ul ca fiind gol.
    rxWriteIndex = 0;
    rxReadIndex = 0;

    // Ștergem complet conținutul buffer-ului.
    memset((void *)rxBuffer, 0, RX_BUFFER_SIZE);

}






