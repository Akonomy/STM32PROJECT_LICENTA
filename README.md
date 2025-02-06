# _sensor_com Project

This project is a firmware application for the STM32F030 microcontroller. It reads sensor data, processes that data for line following and intersection detection, controls motion via I2C (e.g., to an Arduino), and communicates via USART (for commands and debugging). The code has been refactored into separate modules for easier maintenance, debugging, and expansion.

---

## Project Structure

The source code is organized into several modules:

_sensor_com/ ├── Core/ │ ├── Inc/ │ │ ├── main.h │ │ ├── globals.h │ │ ├── init.h │ │ ├── timer.h │ │ ├── communication.h │ │ ├── sensors.h │ │ ├── motion.h │ │ └── utils.h │ └── Src/ │ ├── main.c │ ├── globals.c │ ├── init.c │ ├── timer.c │ ├── communication.c │ ├── sensors.c │ ├── motion.c │ └── utils.c └── (Other folders: Drivers, CMSIS, etc.)


> **Note:** The original `sensor.c` file has been renamed to `sensors.c` for consistency.

---

## Module Overview & Documentation

### 1. Globals Module

- **Files:** `globals.h` and `globals.c`
- **Purpose:**  
  Contains global variables that are shared among multiple modules.

- **Key Variables:**
  - `uint8_t sensor_data[8]`  
    *Holds sensor readings (0 or 1).*
  - `uint8_t CROSS`  
    *Counter used when an intersection is detected.*
  - `uint8_t headTo`  
    *Specifies the turning direction. (Default is 4 = back)*
  - `uint8_t rasp_control`  
    *Flag to indicate whether Raspberry Pi control mode is active.*
  - Additional variables (e.g., `last_direction`, `history`, `DTIndex`, `phase`, `count_up`, `enough_data`, `CK_set`, `speed`, `CK`, `DT`, `helper`, `lastStateCK`, `lastStateDT`, `clockChanged`) are also declared here.

- **Usage:**  
  Include `globals.h` in any module that needs access to these shared variables.

---

### 2. Initialization Module

- **Files:** `init.h` and `init.c`
- **Purpose:**  
  Configures the system clock and initializes all peripherals.

- **Key Functions:**
  - **Syntax:**  
    ```c
    void SystemClock_Config(void) { /* ... */ }
    void MX_GPIO_Init(void) { /* ... */ }
    void MX_ADC_Init(void) { /* ... */ }
    void MX_I2C1_Init(void) { /* ... */ }
    void MX_TIM14_Init(void) { /* ... */ }
    void MX_TIM16_Init(void) { /* ... */ }
    void MX_USART1_UART_Init(void) { /* ... */ }
    ```
  - **Usage:**  
    Call these functions in `main.c` during startup.
    ```c
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC_Init();
    MX_I2C1_Init();
    MX_TIM14_Init();
    MX_TIM16_Init();
    MX_USART1_UART_Init();
    ```

---

### 3. Timer Module

- **Files:** `timer.h` and `timer.c`
- **Purpose:**  
  Provides functions for creating delays and handling timeouts.

- **Key Functions:**
  - **Syntax:**
    ```c
    void DelayWithTimer(uint32_t ms) { /* ... */ }
    void Timeout(uint16_t timeout_ms) { /* ... */ }
    void endTimeout(void) { /* ... */ }
    bool checkTimeout(void) { /* ... */ }
    ```
  - **Usage Example:**
    ```c
    DelayWithTimer(15);          // Delay for 15 ms
    Timeout(100);                // Start a 100 ms timeout
    if (checkTimeout()) { /* ... */ } // Check if timeout has elapsed
    ```

---

### 4. Communication Module

- **Files:** `communication.h` and `communication.c`
- **Purpose:**  
  Handles I2C and USART communication. It sends commands to an I2C slave (e.g., Arduino) and exchanges commands with a Raspberry Pi.

- **Key Functions:**
  - **I2C Communication:**
    - **Syntax:**  
      ```c
      void I2C_Send_Buffer(uint8_t slave_address, uint8_t *data, uint16_t size) { /* ... */ }
      void SendSingleValue(uint8_t slave_address, uint8_t index, uint16_t value) { /* ... */ }
      ```
    - **Usage Example:**  
      ```c
      // Send a movement command with speed 128 and direction (calculated by sensors)
      SendSingleValue(0x08, 128, direction);
      ```
  - **USART Communication:**
    - **Syntax:**  
      ```c
      bool USART_Receive_Two_Values(uint8_t *value1, uint8_t *value2) { /* ... */ }
      void USART_Send_String(const char *str) { /* ... */ }
      ```
    - **Usage Example:**  
      ```c
      uint8_t v1, v2;
      if (USART_Receive_Two_Values(&v1, &v2)) {
          // Process the received values.
      }
      USART_Send_String("Hello, world!\n");
      ```
  - **Raspberry Pi Control:**
    - **Syntax:**
      ```c
      void rasp_process(void) { /* ... */ }
      void raspberry_control(void) { /* ... */ }
      ```
    - **Usage Example:**  
      The code checks for a command such as `"254 1\n"`, which might set `headTo` to 1 (turn forward).  
      *Command Codes:*  
      - `254`: When received with a parameter (1, 2, 3, or 4), it sets `headTo` accordingly.
      - `200`: Activates Raspberry Pi control mode.
      - In `raspberry_control()`, commands like values between 299–307 trigger `go_servo()`; a value of 400 triggers a sensor reading dump.

---

### 5. Sensors Module

- **Files:** `sensors.h` and `sensors.c`
- **Purpose:**  
  Reads sensor data via ADC, processes the data to determine line position, and computes commands for movement.

- **Key Functions:**
  - **Syntax:**
    ```c
    bool SetControlPins(uint8_t code) { /* ... */ }
    void read_sensors(void) { /* ... */ }
    uint8_t line_process(void) { /* ... */ }
    void data_process(void) { /* ... */ }
    void SetSensorRight(uint8_t state) { /* ... */ }
    void SetSensorLeft(uint8_t state) { /* ... */ }
    ```
  - **Usage Example:**  
    ```c
    read_sensors();
    uint8_t direction = line_process();  // Returns 0 (STOP), 1 (FORWARD), 2 (RIGHT), etc.
    ```
  - **Detailed Behavior:**  
    - `line_process()` checks the sensor states:
      - If the middle sensor is active and both side sensors are active, it stops (increases `CROSS`).
      - If the middle sensor is active and one side is active, it slightly adjusts.
      - If only a side sensor is active, it turns left or right.
    - `data_process()` collects additional sensor timing data (using CK and DT signals).

---

### 6. Motion Module

- **Files:** `motion.h` and `motion.c`
- **Purpose:**  
  Controls robot movement by sending commands via I2C.

- **Key Functions:**
  - **Syntax:**
    ```c
    void makeTurn(uint8_t direction_x) { /* ... */ }
    void go(uint8_t max_x, uint8_t direction_x) { /* ... */ }
    void go_servo(uint8_t servo, uint8_t position) { /* ... */ }
    ```
  - **Usage Examples:**
    - **Turning:**  
      ```c
      // If an intersection is detected:
      makeTurn(headTo);
      headTo = 4;  // Reset to default (back)
      ```
    - **Movement Command:**  
      ```c
      // Move forward (direction 0 for STOP is used as a reset command):
      go(1, 0);
      ```
    - **Servo Control:**  
      ```c
      // Move servo number 302 (e.g., claw) to position 90:
      go_servo(302, 90);
      ```
    - **Command Codes (example):**
      - `0`: STOP  
      - `1`: FORWARD  
      - `2`: RIGHT  
      - `3`: LEFT  
      - `4`: SLIGHTLY RIGHT  
      - `5`: SLIGHTLY LEFT  
      - `12`: BACKWARD  
      - And others as defined in the code comments.

---

### 7. Utilities Module

- **Files:** `utils.h` and `utils.c`
- **Purpose:**  
  Provides helper functions for formatting and debugging.

- **Key Functions:**
  - **Syntax:**
    ```c
    void convert_array_to_string(uint8_t *array, uint8_t size, char *result, int result_size) { /* ... */ }
    void toBinaryString(uint8_t value, char *binary_string) { /* ... */ }
    void ProcessReceivedData(uint8_t *buffer, uint16_t length) { /* ... */ }
    ```
  - **Usage Example:**  
    ```c
    char str[50];
    convert_array_to_string(sensor_data, 8, str, sizeof(str));
    USART_Send_String(str);
    ```
  - **Details:**  
    - `toBinaryString()` converts an 8-bit number into its binary string representation.
    - `ProcessReceivedData()` is a placeholder for user-defined logic to process raw received data.

---

## How `main.c` Works

The **main.c** file ties together all the modules in the following workflow:

1. **Initialization Phase:**
   - The MCU peripherals are initialized by calling:
     ```c
     SystemClock_Config();
     MX_GPIO_Init();
     MX_ADC_Init();
     MX_I2C1_Init();
     MX_TIM14_Init();
     MX_TIM16_Init();
     MX_USART1_UART_Init();
     ```
   - The base timer (TIM14) is started with:
     ```c
     TIM14->CR1 |= TIM_CR1_CEN;
     ```

2. **Main Loop:**
   - **Sensor Handling:**  
     - The sensors are read using `read_sensors()`.
     - Data is processed by `data_process()`.
     - If an intersection is detected (`CROSS >= 1`), `makeTurn(headTo)` is called to execute a turning maneuver.
   - **Data Ready:**  
     - If the flag `enough_data` is set, the code calls:
       ```c
       go(1, 0);
       SetSensorRight(1);
       SetSensorLeft(1);
       ```
     - It then converts sensor history (or sensor_data) to a string using `convert_array_to_string()` and sends it via `USART_Send_String()`.
     - Finally, it resets the flag and calls `rasp_process()` to handle any received commands.
   - **Raspberry Pi Control:**  
     - If the `rasp_control` flag is active, `raspberry_control()` is called to process direct commands from the Raspberry Pi.
   - **Movement Command:**  
     - The `line_process()` function calculates the movement direction.
     - Based on the state of `CK_set`, the `speed` is adjusted.
     - A command is sent via I2C using `SendSingleValue(0x08, speed, direction)`.
   - **Delay:**  
     - A short delay is introduced using `DelayWithTimer(15)` before the next loop iteration.

3. **Error Handling:**  
   The `Error_Handler()` function loops indefinitely if a critical error occurs.

---

## Command Examples

- **Raspberry Pi Command Parsing:**  
  - Sending `"254 1\n"` via USART might set `headTo` to 1 (turn front).
  - Sending `"200 ...\n"` activates Raspberry Pi control mode.
- **Servo Control:**  
  - In Raspberry Pi control mode, a command with a first value between 300–307 triggers `go_servo()`.  
    Example: `302 90` moves servo 302 (claw) to position 90.
- **Movement Commands:**  
  - The `go()` function uses direction codes (0: STOP, 1: FORWARD, 2: RIGHT, 3: LEFT, etc.) to control movement.

---

## How to Build and Flash

1. **Clone the Repository:**
   ```bash
   git clone <repository-url>
   cd _sensor_com

    Open in STM32CubeIDE:
    Open the project in STM32CubeIDE.
    Build the Project:
    Use the IDE’s build function.
    Flash to the Device:
    Use a programmer/debugger to flash the firmware onto the STM32F030.
    Monitor Communication:
    Use a serial terminal to view USART output for debugging.

Conclusion

This modular structure splits the code into clear sections for initialization, timing, communication, sensor processing, motion control, and utilities. Each module is documented and provides functions with clearly defined syntax and usage examples. This README.md should help new developers quickly understand the project structure and how to work with and extend the code.

For further details or modifications, refer to the respective header files (e.g., init.h, timer.h, etc.) and source files. Contributions and improvements are welcome!

Happy coding!


---

This README.md file is formatted in Markdown for GitHub. It provides an overview of the project structure, detailed module descriptions (including function syntax, usage examples, and specific command codes), and instructions for building and running the project. Feel free to customize further based on your specific code and needs.

