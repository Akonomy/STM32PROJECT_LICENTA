/*
 * init.c
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#include "init.h"
#include "main.h"
// Include și alte librării hardware necesare

void SystemClock_Config(void)

{
	RCC->CR |= RCC_CR_HSION;                      // Enable HSI
	while (!(RCC->CR & RCC_CR_HSIRDY))
		;           // Wait until HSI is ready

	RCC->CFGR = RCC_CFGR_SW_HSI;                  // Select HSI as system clock
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1 |             // AHB prescaler
			RCC_CFGR_PPRE_DIV1;              // APB1 prescaler
}
/**
 * @brief ADC Initialization Function
 * @param None
 * @retval None
 */

void MX_GPIO_Init(void) {
	LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	/**/

	/**/
	LL_GPIO_ResetOutputPin(C_GPIO_Port, C_Pin);

	/**/
	LL_GPIO_ResetOutputPin(B_GPIO_Port, B_Pin);

	/**/
	LL_GPIO_ResetOutputPin(A_GPIO_Port, A_Pin);

	/**/
	LL_GPIO_ResetOutputPin(signal_right_GPIO_Port, signal_right_Pin);

	/**/
	LL_GPIO_ResetOutputPin(signal_left_GPIO_Port, signal_left_Pin);

	/**/

	/**/
	GPIO_InitStruct.Pin = C_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	LL_GPIO_Init(C_GPIO_Port, &GPIO_InitStruct);

	/**/
	GPIO_InitStruct.Pin = B_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	LL_GPIO_Init(B_GPIO_Port, &GPIO_InitStruct);

	/**/
	GPIO_InitStruct.Pin = A_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	LL_GPIO_Init(A_GPIO_Port, &GPIO_InitStruct);

	/**/

	GPIO_InitStruct.Pin = signal_right_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(signal_right_GPIO_Port, &GPIO_InitStruct);

	/**/

	GPIO_InitStruct.Pin = signal_left_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(signal_left_GPIO_Port, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

void MX_ADC_Init(void)

{
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;           // Enable ADC1 clock
	ADC1->CR = 0;                                 // Reset control register
	ADC1->CFGR1 = 0;                             // Reset configuration register

	// Set resolution to 12-bit (reset state, no action needed as 12-bit is default)
	// Configure sampling time to 1.5 cycles
	ADC1->SMPR = 0x0;     // Set sampling time to 1.5 ADC clock cycles (default)

	ADC1->CHSELR = ADC_CHSELR_CHSEL0;             // Select channel 0
	ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN;           // Right data alignment (default)

	ADC1->CR |= ADC_CR_ADEN;                      // Enable ADC
	while (!(ADC1->ISR & ADC_ISR_ADRDY))
		;         // Wait for ADC to be ready
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */

void MX_I2C1_Init(void) {
	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* Enable clock for GPIOA and I2C1 peripherals */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  // Enable GPIOA clock
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // Enable I2C1 clock

	/* Configure GPIOA for I2C1_SCL (PA9) and I2C1_SDA (PA10) */
	GPIOA->MODER &= ~((3U << (2 * 9)) | (3U << (2 * 10))); // Clear mode bits for PA9 and PA10
	GPIOA->MODER |= ((2U << (2 * 9)) | (2U << (2 * 10))); // Set mode to Alternate Function

	GPIOA->OTYPER |= (1U << 9) | (1U << 10); // Set output type to Open-Drain for PA9 and PA10

	GPIOA->OSPEEDR |= ((3U << (2 * 9)) | (3U << (2 * 10))); // Set speed to High for PA9 and PA10

	GPIOA->PUPDR &= ~((3U << (2 * 9)) | (3U << (2 * 10))); // No pull-up, no pull-down for PA9 and PA10

	GPIOA->AFR[1] &= ~((0xFU << (4 * (9 - 8))) | (0xFU << (4 * (10 - 8)))); // Clear alternate function bits
	GPIOA->AFR[1] |= ((4U << (4 * (9 - 8))) | (4U << (4 * (10 - 8)))); // Set AF4 (I2C1) for PA9 and PA10

	/* Configure I2C1 */
	I2C1->CR1 &= ~I2C_CR1_PE;  // Disable I2C1 to configure it

	I2C1->TIMINGR = 0x2000090E; // Configure timing for 100 kHz (assuming HSI clock at 8 MHz)

	I2C1->CR1 &= ~I2C_CR1_NOSTRETCH; // Enable clock stretching
	I2C1->CR1 &= ~I2C_CR1_GCEN;      // Disable general call mode

	I2C1->OAR1 &= ~I2C_OAR1_OA1EN;   // Disable own address 1
	I2C1->OAR1 = (0 << 1) | I2C_OAR1_OA1MODE; // Configure own address 1 to 7-bit mode and set address to 0x00
	I2C1->OAR1 |= I2C_OAR1_OA1EN;    // Enable own address 1

	I2C1->OAR2 &= ~I2C_OAR2_OA2EN;   // Disable own address 2

	I2C1->CR2 &= ~I2C_CR2_ADD10;  // Use 7-bit addressing mode for slave address
	I2C1->CR2 |= I2C_CR2_AUTOEND;   // Enable AUTOEND (automatic STOP condition)

	I2C1->CR1 |= I2C_CR1_PE;         // Enable I2C1 peripheral

	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */
}

void MX_TIM14_Init(void)

{
	// Enable the clock for TIM14
	RCC->APB1ENR |= RCC_APB1ENR_TIM14EN; // TIM14 clock enable

	// Reset TIM14 configuration
	TIM14->CR1 = 0; // Clear control register
	TIM14->CNT = 0; // Reset counter

	// Configure Prescaler
	TIM14->PSC = 7999; // Set prescaler for 1 kHz timer clock (assuming 8 MHz system clock)

	// Configure Counter Mode (Up)
	TIM14->CR1 &= ~TIM_CR1_DIR; // Ensure up-counting mode (DIR = 0)

	// Configure Period (Auto-Reload Register)
	TIM14->ARR = 999; // Set period for 1 ms increment (1 kHz timer clock)

	// Set Clock Division (DIV1)
	TIM14->CR1 &= ~TIM_CR1_CKD; // Set clock division to 1 (CKD = 00)

	// Enable the Counter
	TIM14->CR1 |= TIM_CR1_CEN; // Start the counter (CEN = 1)
}

void MX_TIM16_Init(void) {
	// Enable the TIM16 clock
	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;

	// Reset TIM16 configuration
	TIM16->CR1 = 0x0000;       // Clear control register
	TIM16->CNT = 0x0000;       // Reset counter
	TIM16->PSC = 7999; // Prescaler for 1 ms time base (8 MHz clock / (PSC + 1))
	TIM16->ARR = 1000;         // Auto-reload value for 5000 ms
	TIM16->CR1 |= TIM_CR1_DIR; // Down-counting mode
	TIM16->CR1 |= TIM_CR1_OPM; // One-pulse mode (counter stops at 0)
	TIM16->CR1 &= ~TIM_CR1_CKD; // No clock division

	// Enable the timer (initially stopped)
	TIM16->CR1 &= ~TIM_CR1_CEN;
}

void MX_USART1_UART_Init(void)

{
	// Enable clock for USART1 and GPIOA
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure GPIOA: PA2 (TX) as Alternate Function
	GPIOA->MODER &= ~(3U << (2 * 2));  // Clear MODER for PA2
	GPIOA->MODER |= (2U << (2 * 2));   // Set MODER to Alternate Function
	GPIOA->AFR[0] |= (1U << (4 * 2));  // Set AF1 (USART1_TX) for PA2

	// Configure GPIOA: PA3 (RX) as Alternate Function
	GPIOA->MODER &= ~(3U << (3 * 2));  // Clear MODER for PA3
	GPIOA->MODER |= (2U << (3 * 2));   // Set MODER to Alternate Function
	GPIOA->AFR[0] |= (1U << (4 * 3));  // Set AF1 (USART1_RX) for PA3

	// Configure USART1: 9600 baud, 8N1 (1 stop bit, no parity)
	USART1->BRR = 8000000 / 38400; // Assuming 48 MHz clock
	USART1->CR1 = USART_CR1_TE | USART_CR1_RE; // Enable Transmitter and Receiver
	USART1->CR1 |= USART_CR1_UE;               // Enable USART
}
