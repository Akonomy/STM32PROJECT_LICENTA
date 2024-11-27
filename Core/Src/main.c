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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define FORWARD 0x2
#define RIGHT   0x3
#define LEFT    0x4
#define STOP    0x1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t usart_data = 0x9;           // Single byte to send via USART

uint8_t i2c_slave_address = 0x08;    // I2C slave address

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM14_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */

/* Function Prototypes */
void USART_Send_Byte(uint8_t data);
void I2C_Send_Buffer(uint8_t slave_address, uint8_t *data, uint16_t size);
void DelayWithTimer(uint32_t ms);
bool SetControlPins(uint8_t code);
void binaryToHex(const char *binary, char *hex);


void toBinaryString(uint8_t value, char *binary_string);

//timeout functions
void Timeout(uint16_t timeout_ms);
void endTimeout(void) ;
bool checkTimeout(void);


//utility functions
void makeTurn(uint8_t direction_x);
void go(uint8_t max_x, uint8_t direction_x);
void read_sensors();

uint8_t data_processor(uint8_t state_clk, uint8_t state_data);
uint8_t line_process() ;


//comunication functions
void SendSingleValue(uint8_t slave_address, uint8_t index, uint16_t value);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Variables to store data and directions
//for raw data

//uint16_t sensor_data[8]={0};  // Array to store data for states 000 to 111
uint8_t sensor_data[8]={0};

// Variables for line_process

uint8_t last_direction = 0;
uint8_t try=0;




// Variables for data_processor
bool timeout_active = false;
uint8_t first_byte = 0;
uint8_t data_read = 0;
uint8_t last_clk_state = 0;
bool _read_data = false;
uint8_t PROCESSED_DATA[8] = {0};
uint8_t tagged_clk = 0;  // Variable to identify tagged CLK
uint8_t tagged_data = 0; // Variable to identify tagged DATA




uint8_t CROSS=0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t direction=0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, 3);

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_I2C1_Init();
  MX_TIM14_Init();
 MX_TIM16_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */


  TIM14->CR1 |= TIM_CR1_CEN; // Start timer
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    /* USER CODE END WHILE */

      while (1)
      {
          // Step 1: Read sensors
          if (CROSS>=1){
         	 makeTurn(4);
         	 CROSS=0;
          }

          read_sensors();

          direction=line_process() ;
           // Array of bytes to send via I2C
          //go( times , direction, duration)
          SendSingleValue(0x08, 128, direction);



          DelayWithTimer(15); // Delay before the next cycle

      }
}








  /*

	  for(uint8_t code =0; code<8 ; code++){


		  if (SetControlPins(code)) {

			  //do nothing

			  // Start ADC conversion
			  ADC1->CR |= ADC_CR_ADSTART;

			  // Wait for conversion to complete
			  while (!(ADC1->ISR & ADC_ISR_EOC));

			  // Read ADC value

			  // Create binarry string: control pins + ADC input
			  uint8_t adcValue = (ADC1->DR > 800) ? 1 : 0; // Read and process ADC value
			  DelayWithTimer(300);


		  }
	  }


	  USART_Send_Byte(usart_data);


	  DelayWithTimer(500);


	     I2C_Send_Buffer(i2c_slave_address, i2c_data, sizeof(i2c_data));





	     DelayWithTimer(500);

*/

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)

{
    RCC->CR |= RCC_CR_HSION;                      // Enable HSI
    while (!(RCC->CR & RCC_CR_HSIRDY));           // Wait until HSI is ready

    RCC->CFGR = RCC_CFGR_SW_HSI;                  // Select HSI as system clock
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1 |             // AHB prescaler
                 RCC_CFGR_PPRE_DIV1;              // APB1 prescaler
}
/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)

{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;           // Enable ADC1 clock
    ADC1->CR = 0;                                 // Reset control register
    ADC1->CFGR1 = 0;                              // Reset configuration register

    // Set resolution to 12-bit (reset state, no action needed as 12-bit is default)
    // Configure sampling time to 1.5 cycles
    ADC1->SMPR = 0x0;                             // Set sampling time to 1.5 ADC clock cycles (default)

    ADC1->CHSELR = ADC_CHSELR_CHSEL0;             // Select channel 0
    ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN;              // Right data alignment (default)

    ADC1->CR |= ADC_CR_ADEN;                      // Enable ADC
    while (!(ADC1->ISR & ADC_ISR_ADRDY));         // Wait for ADC to be ready
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{
    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* Enable clock for GPIOA and I2C1 peripherals */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;  // Enable GPIOA clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // Enable I2C1 clock

    /* Configure GPIOA for I2C1_SCL (PA9) and I2C1_SDA (PA10) */
    GPIOA->MODER &= ~((3U << (2 * 9)) | (3U << (2 * 10)));  // Clear mode bits for PA9 and PA10
    GPIOA->MODER |= ((2U << (2 * 9)) | (2U << (2 * 10)));   // Set mode to Alternate Function

    GPIOA->OTYPER |= (1U << 9) | (1U << 10);  // Set output type to Open-Drain for PA9 and PA10

    GPIOA->OSPEEDR |= ((3U << (2 * 9)) | (3U << (2 * 10))); // Set speed to High for PA9 and PA10

    GPIOA->PUPDR &= ~((3U << (2 * 9)) | (3U << (2 * 10)));  // No pull-up, no pull-down for PA9 and PA10

    GPIOA->AFR[1] &= ~((0xFU << (4 * (9 - 8))) | (0xFU << (4 * (10 - 8))));  // Clear alternate function bits
    GPIOA->AFR[1] |= ((4U << (4 * (9 - 8))) | (4U << (4 * (10 - 8))));       // Set AF4 (I2C1) for PA9 and PA10

    /* Configure I2C1 */
    I2C1->CR1 &= ~I2C_CR1_PE;  // Disable I2C1 to configure it

    I2C1->TIMINGR = 0x2000090E; // Configure timing for 100 kHz (assuming HSI clock at 8 MHz)

    I2C1->CR1 &= ~I2C_CR1_NOSTRETCH; // Enable clock stretching
    I2C1->CR1 &= ~I2C_CR1_GCEN;      // Disable general call mode

    I2C1->OAR1 &= ~I2C_OAR1_OA1EN;   // Disable own address 1
    I2C1->OAR1 = (0 << 1) | I2C_OAR1_OA1MODE; // Configure own address 1 to 7-bit mode and set address to 0x00
    I2C1->OAR1 |= I2C_OAR1_OA1EN;    // Enable own address 1

    I2C1->OAR2 &= ~I2C_OAR2_OA2EN;   // Disable own address 2

    I2C1->CR2 &= ~I2C_CR2_ADD10;     // Use 7-bit addressing mode for slave address
    I2C1->CR2 |= I2C_CR2_AUTOEND;    // Enable AUTOEND (automatic STOP condition)

    I2C1->CR1 |= I2C_CR1_PE;         // Enable I2C1 peripheral

    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */
}


/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)

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



//TIMMER16

static void MX_TIM16_Init(void) {
    // Enable the TIM16 clock
    RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;

    // Reset TIM16 configuration
    TIM16->CR1 = 0x0000;       // Clear control register
    TIM16->CNT = 0x0000;       // Reset counter
    TIM16->PSC = 7999;         // Prescaler for 1 ms time base (8 MHz clock / (PSC + 1))
    TIM16->ARR = 5000;         // Auto-reload value for 5000 ms
    TIM16->CR1 |= TIM_CR1_DIR; // Down-counting mode
    TIM16->CR1 |= TIM_CR1_OPM; // One-pulse mode (counter stops at 0)
    TIM16->CR1 &= ~TIM_CR1_CKD; // No clock division

    // Enable the timer (initially stopped)
    TIM16->CR1 &= ~TIM_CR1_CEN;
}

// Function to start the timer



/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)

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

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

  /**/
  LL_GPIO_ResetOutputPin(R_GPIO_Port, R_Pin);

  /**/
  LL_GPIO_ResetOutputPin(L_GPIO_Port, L_Pin);

  /**/
  LL_GPIO_ResetOutputPin(C_GPIO_Port, C_Pin);

  /**/
  LL_GPIO_ResetOutputPin(B_GPIO_Port, B_Pin);

  /**/
  LL_GPIO_ResetOutputPin(A_GPIO_Port, A_Pin);

  /**/
  GPIO_InitStruct.Pin = R_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(R_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = L_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(L_GPIO_Port, &GPIO_InitStruct);

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

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void USART_Send_Byte(uint8_t data)
{
    /* Wait until TXE (Transmit Data Register Empty) is set */
    while (!(USART1->ISR & USART_ISR_TXE));  // Check if TXE flag is set

    /* Send the data */
    USART1->TDR = data;  // Write the data to the Transmit Data Register

    /* Wait until TC (Transmission Complete) is set */
    while (!(USART1->ISR & USART_ISR_TC));  // Check if TC flag is set
}
/**
  * @brief Sends a buffer of data via I2C.
  * @param slave_address I2C slave address
  * @param data Pointer to the data buffer
  * @param size Size of the data buffer
  */
void I2C_Send_Buffer(uint8_t slave_address, uint8_t *data, uint16_t size)
{
    /* Wait until I2C is not busy */
    while (I2C1->ISR & I2C_ISR_BUSY);

    /* Configure the slave address and the number of bytes to transfer */
    I2C1->CR2 = (slave_address << 1) |    // Set slave address (7-bit left aligned)
                (size << 16) |            // Set number of bytes to transfer
                I2C_CR2_START |           // Generate START condition
                I2C_CR2_AUTOEND;          // Enable AUTOEND (STOP condition automatically after N bytes)

    /* Transmit the data */
    for (uint16_t i = 0; i < size; i++)
    {
        /* Wait until TXIS (Transmit Interrupt Status) is set */

    	while (!(I2C1->ISR & I2C_ISR_TXIS));

        /* Write the data to the transmit data register */
        I2C1->TXDR = data[i];
    }

    /* Wait until the STOP condition is generated */
    while (!(I2C1->ISR & I2C_ISR_STOPF));

    /* Clear the STOP flag */
    I2C1->ICR = I2C_ICR_STOPCF;
}



bool SetControlPins(uint8_t code)
{
    GPIOA->BSRR = (code & 0x01 ? A_Pin : (uint32_t) A_Pin << 16U) |
                  (code & 0x02 ? B_Pin : (uint32_t) B_Pin << 16U) |
                  (code & 0x04 ? C_Pin : (uint32_t) C_Pin << 16U);
    return true;
}

void DelayWithTimer(uint32_t ms)
{
    TIM14->CNT = 0;                               // Reset counter
    while (TIM14->CNT < ms);                      // Wait for counter to reach ms
}

void binaryToHex(const char *binary, char *hex) {
    const char hexMap[] = "0123456789ABCDEF";
    for (int i = 0; binary[i * 4] != '\0'; i++) {
        int value = 0;
        for (int j = 0; j < 4; j++) {
            value = (value << 1) + (binary[i * 4 + j] - '0');
        }
        hex[i] = hexMap[value];
    }
    hex[strlen(binary) / 4] = '\0'; // Null-terminate
}



void Timeout(uint16_t timeout_ms) {
    TIM16->CNT = timeout_ms;    // Set the counter to the desired value
    TIM16->CR1 |= TIM_CR1_CEN;  // Start the counter
}

void endTimeout(void) {
    TIM16->CR1 &= ~TIM_CR1_CEN; // Stop the counter
}

bool checkTimeout(void) {
    return (TIM16->CNT == 0 && !(TIM16->CR1 & TIM_CR1_CEN)); // CNT == 0 and timer is stopped
}




//UTILITY FUNCTIONS

void makeTurn(uint8_t direction_x){
	//aliniaza cu centrul intersectiei
	/*
	 *INPUT  DIRECTIONS MAP
	 * 1-front
	 * 2-right
	 * 3-left
	 * 4-back
	 * 0-stop
	 *
	uint8_t left_line = sensor_data[0];
    uint8_t mid_line = sensor_data[4];
    uint8_t right_line = sensor_data[6];


	 */

	uint8_t lineNotFound=1;

	switch(direction_x){

	case 0:
		go(1,0);
	case 1: //front
		//do nothing
		go(1,1);
	case 2: //right
		go(2,1);
		go(4,11); //make a first turn
		read_sensors();
		while(lineNotFound ){

			go(1,11); // continue rotating until line found
			read_sensors();  //update sensors
			if (sensor_data[4] || sensor_data[0] || sensor_data[6]){
				lineNotFound=0;
				go(1,0);
				return;
			}
		}

	case 3: //left turn
		go(2,1);
		go(4,10); //make a first turn
		read_sensors();
		while( lineNotFound){

			go(1,10); // continue rotating until line found
			read_sensors();  //update sensors
			if (sensor_data[4] || sensor_data[0] || sensor_data[6]){
				lineNotFound=0;
				go(1,0);
			}
		}


	case 4: //left turn
		go(3,12);
		go(6,11); //make a first turn
		read_sensors();
		while( lineNotFound){

			go(1,11); // continue rotating until line found
			read_sensors();  //update sensors
			if (sensor_data[4] || sensor_data[0] || sensor_data[6]){
				lineNotFound=0;
				go(1,0);
			}
		}

	default:
		go(1,0);



	} //end of switch




}//end of makeTurn()



void go(uint8_t max_x, uint8_t direction_x){

	/*  max_x-times  direction_x in direction x
	 *  ARDUINO DIRECTIONS MAP , FOR GO() FUNCTION
	 *  case 0: return "STOP";
        case 1: return "FORWARD";
        case 2: return "RIGHT";
        case 3: return "LEFT";
        case 4: return "SLIGHTLY RIGHT";
        case 5: return "SLIGHTLY LEFT";
        case 6: return "DIAGONAL RIGHT";
        case 7: return "DIAGONAL LEFT";
        case 8: return "HARD TURN LEFT";
        case 9: return "HARD TURN RIGHT";
        case 10: return "LEFT ROTATE";
        case 11: return "RIGHT ROTATE";
        case 12: return "BACKWARD";

	 *
	 */


	for(uint8_t x=0 ;x< max_x; x++ ){

	 SendSingleValue(0x08, 128, direction_x);
	 DelayWithTimer(100);
	 SendSingleValue(0x08, 128, 0);

	}

}

void read_sensors() {
    for (uint8_t code = 0; code < 8; code++) {
        if (SetControlPins(code)) {
            // Start ADC conversion
             ADC1->CR |= ADC_CR_ADSTART;

            // Wait for conversion to complete
            while (!(ADC1->ISR & ADC_ISR_EOC));

            // Read raw ADC value (12-bit)
            //uint16_t adcValue = ADC1->DR;

            // Store the raw ADC value in the sensor_data array

            //convertadc to bin
            uint8_t adcValue = (ADC1->DR > 1000) ? 1 : 0; // Threshold ADC value to 1 or 0


            sensor_data[code] = adcValue;

            // Small delay for stability
            DelayWithTimer(5);
        }
    }
}
uint8_t helper[2] = {0, 0};  // helper[0] = RIGHT used, helper[1] = LEFT used

uint8_t line_process() {
    // Assign sensor data to logical variables
    uint8_t left_line = sensor_data[0];
    uint8_t mid_line = sensor_data[4];
    uint8_t right_line = sensor_data[6];

    // Default direction is STOP
    uint8_t direction = 0;


    if (mid_line) {
    	try=0;// Middle sensor is on the line
        if (left_line && right_line) {
            // All sensors are active -> Stop
        	CROSS++;
            direction = 0;  // STOP
            helper[0] = 0;  // Reset guesses
            helper[1] = 0;  // Reset guesses
        } else if (left_line && !right_line) {
            // Middle and Left sensors active -> Adjust slightly left
            direction = 5;  // A LITTLE TO LEFT
            helper[0] = 0;  // Reset guesses
            helper[1] = 0;  // Reset guesses
        } else if (right_line && !left_line) {
            // Middle and Right sensors active -> Adjust slightly right
            direction = 4;  // A LITTLE TO RIGHT
            helper[0] = 0;  // Reset guesses
            helper[1] = 0;  // Reset guesses
        } else {
            // Only Middle sensor active -> Go Forward
            direction = 1;  // FORWARD
            helper[0] = 0;  // Reset guesses
            helper[1] = 0;  // Reset guesses
        }
    } else {  // Middle sensor is off the line
        if (left_line && !right_line) {
            // Line is on the left -> Turn left
            direction = 3;  // LEFT
            helper[0] = 0;  // Reset guesses
            helper[1] = 0;  // Reset guesses
        } else if (right_line && !left_line) {
            // Line is on the right -> Turn right
            direction = 2;  // RIGHT
            helper[0] = 0;  // Reset guesses
            helper[1] = 0;  // Reset guesses
        } else if (!left_line && !right_line) {



            // All sensors are off -> Alternate based on helper
            if (last_direction == 2 && helper[1] == 0) {
                direction = 2;  // Guess LEFT
                go(1,2);
                helper[1] = 1;  // Mark RIGHT as used
            }

             else if (last_direction == 3 && helper[0] == 0) {
                direction = 3;  // Guess RIGHT
                go(1,3);
                helper[0] = 1;  // Mark LEFT as used
             }


             else if (helper[0] == 1 && helper[1] == 1) {
                // Both directions used -> Stop
                direction = 0;  // STOP
                helper[0] = 0;  // Reset guesses
                helper[1] = 0;  // Reset guesses
                go(1,4);
                go(1,1);
            }




        } else {
            // Unexpected case: Default to STOP
            direction = 0;  // STOP
            helper[0] = 0;  // Reset guesses
            helper[1] = 0;  // Reset guesses
        }
    }

    // Update last_direction for tracking
    if (direction != 6 && direction != 0) {  // Exclude STOP and BACK commands from direction memory
        last_direction = direction;
    }

    if(helper[1] && !helper[0]){
    	last_direction=3;
    } if( helper[0] && !helper[1]){
    	last_direction=2;
    } if(direction==0){
    	last_direction=0;
    }





    return direction;
}



uint8_t data_processor(uint8_t state_clk, uint8_t state_data) {
    if (!timeout_active && (state_clk || state_data)) {
        // Reset all previous states
        first_byte = 1;
        timeout_active = true;
        data_read = 0;
        _read_data = false;

        if (state_clk == 2) {
            tagged_data = 0x2; // Tag left as DATA
            tagged_clk = 0x4;  // Tag right as CLK
        } else if (state_clk == 4) {
            tagged_data = 0x4; // Tag right as DATA
            tagged_clk = 0x2;  // Tag left as CLK
        }

        last_clk_state = tagged_clk;
        Timeout(3000); // Ensure timeout starts properly
        return 0; // Still processing
    }

    if (timeout_active && first_byte <= 3) {
        if (state_clk != last_clk_state) {
            first_byte++;
            last_clk_state = state_clk;
            Timeout(3000); // Timeout reset for every CLK change
            return 0; // Still processing
        }
    }

    if (timeout_active && first_byte > 3) {
        if (data_read <= 7) {
            if (state_clk && !_read_data) {
                _read_data = true;
                PROCESSED_DATA[data_read] = tagged_data; // Append data
                data_read++;
                Timeout(3000); // Reset timeout after saving data
                return 0; // Still processing
            }

            if (!state_clk) {
                _read_data = false;
                Timeout(3000); // Reset timeout after processing CLK low
                return 0; // Still processing
            }
        }

        if (data_read > 7) {
            timeout_active = false;
            first_byte = 0;
            Timeout(3000); // Final reset before exiting
            return 1; // Processing complete
        }
    }

    return 0; // Default: still processing
}








void toBinaryString(uint8_t value, char *binary_string)
{
    for (int i = 7; i >= 0; i--)
    {
        binary_string[7 - i] = (value & (1 << i)) ? '1' : '0';
    }
    binary_string[8] = '\0'; // Null-terminate the string
}


//COMUNICATION FUNCTIONS

void SendSingleValue(uint8_t slave_address, uint8_t index, uint16_t value) {
    uint8_t buffer[3];
    buffer[0] = index;               // Index
    buffer[1] = (value >> 8) & 0xFF; // High byte of value
    buffer[2] = value & 0xFF;        // Low byte of value

    I2C_Send_Buffer(slave_address, buffer, sizeof(buffer)); // Send single value
}






/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
