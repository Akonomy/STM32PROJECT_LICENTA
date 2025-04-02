/*
 * raspberry_com.h
 *
 *  Created on: Feb 8, 2025
 *      Author: andre
 */

#ifndef INC_RASPBERRY_COM_H_
#define INC_RASPBERRY_COM_H_

#include <stdint.h>
#include "usart.h"




#define MESSAGE_SIZE 9       // 9 octeți: 1 start, 3 valori, 4 vector, 1 end
#define START_MARKER 0x02
#define END_MARKER   0x03



/**
 * @brief Controlează mașina.
 *
 * Această funcție procesează comanda de control pentru mașină.
 *
 * @param direction Reprezintă direcția de deplasare a mașinii (DATA1) - interval valid: 0 până la 12.
 * @param tick      Reprezintă numărul de "ticks" (DATA2) - interval valid: 1 până la 10.
 * @param speed     Reprezintă viteza mașinii (DATA3) - interval valid: 30 până la 150.
 */
void control_car(uint8_t direction, uint8_t tick, uint16_t speed[4]);

/**
 * @brief Controlează un servo.
 *
 * Această funcție procesează comanda de control pentru un servo.
 *
 * @param servo_id  Reprezintă ID-ul servo-ului (DATA1) - interval valid: 180 până la 190.
 * @param angle     Reprezintă unghiul la care se setează servo-ul (DATA2) - interval valid: 0 până la 180.
 */
void control_servo(uint8_t servo_id, uint8_t angle);

/**
 * @brief Solicită date.
 *
 * Această funcție procesează comanda de request pentru date.
 *
 * @param sensor_type Reprezintă tipul senzorului (DATA1):
 *                    - 1 pentru senzori de linie,
 *                    - 2 pentru detectare obstacol la roți.
 */
void request_data(uint8_t sensor_type);

/**
 * @brief Salvează direcția următoarei intersecții.
 *
 * Această funcție procesează comanda de informare și salvează direcția următoarei intersecții.
 *
 * @param direction Reprezintă direcția de întoarcere la intersecție (DATA1) -
 *                  valorile valide sunt:
 *                    1 - front,
 *                    2 - right,
 *                    3 - left,
 *                    4 - back,
 *                    0 - stop.
 */
void save_next_cross_direction(uint8_t direction);





void decode_and_save_directions(uint8_t data1, uint8_t data2, uint8_t vector[4]);



void set_mode(uint8_t data1);


/**
 * @brief Procesează datele primite.
 *
 * Această funcție primește toți cei 4 parametri (TYPE, DATA1, DATA2, DATA3) și, în funcție
 * de valoarea lui TYPE, apelează funcția corespunzătoare:
 *   - TYPE 1: apelează control_car(DATA1, DATA2, DATA3)
 *   - TYPE 2: apelează control_servo(DATA1, DATA2)
 *   - TYPE 3: apelează request_data(DATA1)
 *   - TYPE 4: apelează save_next_cross_direction(DATA1)
 *
 * @param type   Tipul de comandă (TYPE)
 * @param data1  Primul parametru (DATA1)
 * @param data2  Al doilea parametru (DATA2)
 * @param data3  Al treilea parametru (DATA3)
 */
void process_rasp_data(uint8_t type, uint8_t data1, uint8_t data2,uint8_t vector[4]);



void parse_and_process_data(void);

#endif /* INC_RASPBERRY_COM_H_ */
