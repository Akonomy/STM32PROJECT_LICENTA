/*
 * raspberry_com.c
 *
 *  Created on: Feb 8, 2025
 *      Author: andre
 */
#include <stdint.h>
#include "raspberry_com.h"
#include "usart.h"
#include "globals.h"
#include "sensors.h"

/**
 * @brief Controlează mașina.
 *
 * Parametri:
 *  - direction: reprezintă direcția de deplasare a mașinii (0-12)
 *  - tick: reprezintă numărul de "ticks" (1-10)
 *  - speed: reprezintă viteza mașinii (30-150)
 */
void control_car(uint16_t direction, uint16_t tick, uint16_t speed) {
    // Verificare parametri
    if (direction > 12) {
        // Eroare: direcția este în afara intervalului permis (0-12)
        return;
    }
    if (tick < 1 || tick > 10) {
        // Eroare: tick-ul este în afara intervalului permis (1-10)
        return;
    }
    if (speed < 30 || speed > 150) {
        // Eroare: viteza este în afara intervalului permis (30-150)
        return;
    }

    // DE COMPLETAT LOGIC FOR FUNCTION control_car
    // Aici se va implementa logica de control pentru mașină,
    // folosind parametrii direction, tick și speed.
    // ...
    // end

    SetSensorRight(1);
    SetSensorLeft(0);
}

/**
 * @brief Controlează un servo.
 *
 * Parametri:
 *  - servo_id: reprezintă ID-ul servo-ului (180-190)
 *  - angle: reprezintă unghiul la care se setează servo-ul (0-180)
 */
void control_servo(uint16_t servo_id, uint16_t angle) {
    // Verificare parametri
    if (servo_id < 180 || servo_id > 190) {
        // Eroare: ID-ul servo-ului este în afara intervalului permis (180-190)
        return;
    }
    if (angle > 180) {
        // Eroare: unghiul este în afara intervalului permis (0-180)
        return;
    }


    SetSensorRight(1);
    SetSensorLeft(1);
    // DE COMPLETAT LOGIC FOR FUNCTION control_servo
    // Aici se va implementa logica de control pentru servo,
    // folosind parametrii servo_id și angle.
    // ...
    // end
}

/**
 * @brief Solicită date.
 *
 * Parametru:
 *  - sensor_type: reprezintă tipul senzorului (DATA1):
 *                  - 1 pentru senzori de linie,
 *                  - 2 pentru detectare obstacol la roți.
 */
void request_data(uint16_t sensor_type) {
    // Verificare parametru
    if (sensor_type != 1 && sensor_type != 2) {
        // Eroare: tipul senzorului nu este valid (trebuie să fie 1 sau 2)
        return;
    }

    SetSensorRight(0);
    SetSensorLeft(0);

    // DE COMPLETAT LOGIC FOR FUNCTION request_data
    // Aici se va implementa logica pentru solicitarea datelor,
    // folosind parametrul sensor_type.
    // ...
    // end
}

/**
 * @brief Salvează direcția următoarei intersecții.
 *
 * Parametru:
 *  - direction: reprezintă direcția de întoarcere la intersecție,
 *               valorile valide sunt:
 *                 1 - front,
 *                 2 - right,
 *                 3 - left,
 *                 4 - back,
 *                 0 - stop.
 */
void save_next_cross_direction(uint16_t direction) {
    // Verificare parametru
    if (direction > 4) {
        // Eroare: direcția nu este validă (trebuie să fie între 0 și 4)
        return;
    }


    SetSensorRight(0);
    SetSensorLeft(1);

    // DE COMPLETAT LOGIC FOR FUNCTION save_next_cross_direction
    // Aici se va implementa logica de salvare a direcției pentru următoarea intersecție,
    // folosind parametrul direction.
    // ...
    // end
}

/**
 * @brief Procesează datele primite.
 *
 * Parametri:
 *  - type: tipul de comandă (TYPE)
 *  - data1: primul parametru (DATA1)
 *  - data2: al doilea parametru (DATA2)
 *  - data3: al treilea parametru (DATA3)
 *
 * În funcție de valoarea lui type, se apelează:
 *   - TYPE 1: control_car(data1, data2, data3)
 *   - TYPE 2: control_servo(data1, data2)
 *   - TYPE 3: request_data(data1)
 *   - TYPE 4: save_next_cross_direction(data1)
 */
void process_rasp_data(uint16_t type, uint16_t data1, uint16_t data2, uint16_t data3) {
    switch(type) {
        case 1:
            // TYPE 1: Control Car
            control_car(data1, data2, data3);
            break;
        case 2:
            // TYPE 2: Control Servo
            control_servo(data1, data2);
            break;
        case 3:
            // TYPE 3: Request Data
            request_data(data1);
            break;
        case 4:
            // TYPE 4: Save Next Cross Direction
            save_next_cross_direction(data1);
            break;
        default:
            // Tip de comandă necunoscut - nu se realizează nicio acțiune
            break;
    }
}



//----------------------------------------------------------------------
// This function extracts a newline-terminated message from the ring buffer,
// tokenizes it (using space as a delimiter), and passes the first four numbers
// to process_rasp_data. Up to 5 tokens are read from the message, though only
// the first 4 are used for processing.
//----------------------------------------------------------------------
void parse_and_process_data(void)
{
    char message[50] = {0};
    int msgOffset = 0;

    // Extract a message from the ring buffer until '\n' or the message buffer is full.
    while (RingBuffer_Available() > 0 && msgOffset < (sizeof(message) - 1))
    {
        uint8_t ch = rxBuffer[rxReadIndex];
        rxReadIndex = (rxReadIndex + 1) % /* RX_BUFFER_SIZE */ 256;  // Replace 256 with your RX_BUFFER_SIZE
        message[msgOffset++] = ch;
        if (ch == '\n')
        {
            break;
        }
    }
    message[msgOffset] = '\0';  // Ensure null termination

    // Tokenize the message. We allow up to 5 tokens, but only use the first 4.
    uint16_t values[5] = {0};
    int count = 0;
    char *token = strtok(message, " \n");
    while (token != NULL && count < 5)
    {
        values[count++] = (uint16_t)atoi(token);
        token = strtok(NULL, " \n");
    }

    // Make sure we have at least 4 tokens; pad with zeros if needed.
    while (count < 4)
    {
        values[count++] = 0;
    }

    // Call process_rasp_data with the first four tokens:
    //   values[0] -> type, values[1] -> data1, values[2] -> data2, values[3] -> data3.
    process_rasp_data(values[0], values[1], values[2], values[3]);
}



