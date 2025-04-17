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
#include "communication.h"
#include "timer.h"





    uint32_t lastProcessTime = 0;              // Timpul ultimei procesări
    uint32_t currentTime = 0;        // Obține timpul curent (în ms)







/**
 * @brief Controlează mașina.
 *
 * Parametri:
 *  - direction: reprezintă direcția de deplasare a mașinii (0-12)
 *  - tick: reprezintă numărul de "ticks" (1-10)
 *  - speed: reprezintă viteza mașinii (30-150)
 */
void control_car(uint8_t direction, uint8_t tick, uint16_t speed[4]) {

	   SetSensorRight(1);

    // Verificare parametri

	if (tick == 0 || direction==0){
		SetSensorLeft(1);
		 I2C_Send_Packet(i2c_slave_address,0x0000 ,0, 4);
		 return;
	}
    if (direction > 20) {
        // Eroare: direcția este în afara intervalului permis (0-12)
    	SetSensorLeft(1);
        return;
    }
    if (tick>30) {
    	SetSensorLeft(1);
        // Eroare: tick-ul este în afara intervalului permis (1-10)
        return;
    }






    if (tick==1){


    	for (uint8_t x = 0; x < tick; x++) {
    			SetSensorRight(0);

    			 I2C_Send_Packet(i2c_slave_address, directii_implicite[direction] , speed, 4);
    			 DelayWithTimer(155);
    			 SetSensorRight(1);
    }
    }


    else {

    		for (uint8_t x = 0; x < tick; x++) {
    				SetSensorRight(0);

    				 I2C_Send_Packet(i2c_slave_address, directii_implicite[direction] , speed, 4);
    				 DelayWithTimer(100);
    				 SetSensorRight(1);


    			}

    	}



	 I2C_Send_Packet(i2c_slave_address, 0x0000,0, 4);


    SetSensorRight(0);
    SetSensorLeft(0);
}

/**
 * @brief Controlează un servo.
 *
 * Parametri:
 *  - servo_id: reprezintă ID-ul servo-ului (180-190)
 *  - angle: reprezintă unghiul la care se setează servo-ul (0-180)
 */
void control_servo(uint8_t servo_id, uint8_t state) {
    uint16_t mask = 0;

    // Selectăm masca pe baza ID-ului servo-ului
    if (servo_id == 181) {
        mask = 0x0200; // Servo 9
    } else if (servo_id == 182) {
        mask = 0x0400; // Servo 10
    } else {
        // Servo ID necunoscut – ignoră comanda
        return;
    }

    // Trimitere pachet cu o singură valoare (state = comanda 0,1,2,3)
    uint16_t value = (uint16_t)state;
    I2C_Send_Packet(i2c_slave_address, mask, &value, 1);

    // Feedback simbolic, pentru moralul roboților
    SetSensorLeft(1); // Maybe blink this in Morse code to spell "why?"
}


/**
 * @brief Solicită date.
 *
 * Parametru:
 *  - sensor_type: reprezintă tipul senzorului (DATA1):
 *                  - 1 pentru senzori de linie,
 *                  - 2 pentru detectare obstacol la roți.
 */
void request_data(uint8_t sensor_type) {
    // Verificare parametru
    if (sensor_type != 1 && sensor_type != 2) {
        // Eroare: tipul senzorului nu este valid (trebuie să fie 1 sau 2)
        return;
    }

    read_sensors();
    USART_Send_Array(sensor_data,8);
    DelayWithTimer(200);
    USART_Send_Array(sensor_data,8);
    DelayWithTimer(200);
    USART_Send_Array(sensor_data,8);
    DelayWithTimer(500);





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
void save_next_cross_direction(uint8_t direction) {
    // Verificare parametru
    if (direction > 4) {
        // Eroare: direcția nu este validă (trebuie să fie între 0 și 4)
        return;
    }

    headTo=direction;


}





void decode_and_save_directions(uint8_t data1, uint8_t data2, uint8_t vector[4]) {
    // Verificare: lungimea trebuie să fie în intervalul 0..MAX_DIRECTIONS.
    if (data1 > MAX_DIRECTIONS) {
        // Date invalide (lungime prea mare), deci ignorăm complet datele.
        return;
    }

    uint8_t unxored[4];
    uint32_t packed = 0;
    uint8_t i;

    // Decriptează fiecare octet prin XOR cu cheia data2.
    for (i = 0; i < 4; i++) {
        unxored[i] = vector[i] ^ data2;
    }

    // Reasamblăm cei 32 de biți din cei 4 octeți (presupunem little-endian)
    packed = ((uint32_t)unxored[0]) |
             (((uint32_t)unxored[1]) << 8) |
             (((uint32_t)unxored[2]) << 16) |
             (((uint32_t)unxored[3]) << 24);

    // Extragem fiecare direcție (2 biți per element) și convertim din intervalul 0-3 în 1-4.
    for (i = 0; i < data1; i++) {
        global_directions[i] = ((packed >> (2 * i)) & 0x03) + 1;
    }

    // Setăm restul pozițiilor din vectorul global la 0.
    for (; i < MAX_DIRECTIONS; i++) {
        global_directions[i] = 0;
    }
}





void set_mode(uint8_t data1){

	mode=data1;


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
 *   - TYPE 2: control_servo(data1, data2)a
 *   - TYPE 3: request_data(data1)
 *   - TYPE 4: save_next_cross_direction(data1)
 */
void process_rasp_data(uint8_t type, uint8_t data1, uint8_t data2, uint8_t vector[4]) {
	 SetSensorLeft(1);
    switch(type) {
        case 1:
        {
            // Tipul 1: Control Car
            // Convertim valorile din vector la uint16_t prin înmulțire cu 16
            uint16_t convVector[4];
            for (int i = 0; i < 4; i++) {
                convVector[i] = vector[i] * 16;
            }
            // Se presupune că funcția control_car a fost modificată pentru a primi un vector de uint16_t.
            control_car(data1, data2, convVector);
            break;
        }
        case 2:
            // Tipul 2: Control Servo
            control_servo(data1, data2);
            SetSensorLeft(0); //future , if PASS
            break;
        case 3:
            // Tipul 3: Request Data

            request_data(data1);
            break;
        case 4:
            // Tipul 4: Save Next Cross Direction
        	SetSensorLeft(0);
            save_next_cross_direction(data1);
            break;

        case 5:
        	SetSensorLeft(0);
        	set_mode(data1);

        case 6:
        	SetSensorLeft(0);
        	decode_and_save_directions(data1,data2,vector);

        case 7:  //signal that i received data via usart
        	SetSensorLeft(0);
        	SetSensorRight(0);
        	DelayWithTimer(500);
           	SetSensorLeft(1);
            SetSensorRight(1);
            DelayWithTimer(500);
           	SetSensorLeft(0);
            SetSensorRight(1);
            DelayWithTimer(500);
            SetSensorRight(0);


        default:
            // Tip de comandă necunoscut - nu se realizează nicio acțiune.
            break;
    }
}




//----------------------------------------------------------------------
// This function extracts a newline-terminated message from the ring buffer,
// tokenizes it (using space as a delimiter), and passes the first four numbers4
// to process_rasp_data. Up to 5 tokens are read from the message, though only
// the first 4 are used for processing.
//----------------------------------------------------------------------

void parse_and_process_data(void)
{
    // Un pachet complet are lungimea: 1 (start) + 3 (cmd_type, data1, data2) + 1 (lungime vector) + vector_length + 1 (end)
    // Lungimea minimă este 7 (dacă vector_length == 1) și maximă 10 (dacă vector_length == 4).

    // Procesăm cât timp avem cel puțin 7 octeți în buffer
    while (RingBuffer_Available() >= 7)
    {
        // Căutăm markerul de început (0xAA)
        if (rxBuffer[rxReadIndex] != 0xAA)
        {
            rxReadIndex = (rxReadIndex + 1) % RX_BUFFER_SIZE;
            continue;
        }

        // Avem markerul de început; dar trebuie să avem cel puțin 5 octeți pentru a citi până la lungimea vectorului
        if (RingBuffer_Available() < 5){

            break;  // Nu avem suficiente date pentru a determina lungimea vectorului
        }
        // Folosim o zonă temporară pentru a citi primele 5 octeți (fără a-i scoate definitiv din buffer)
        uint8_t temp_packet[5];
        uint16_t temp_index = rxReadIndex;
        for (int i = 0; i < 5; i++)
        {
            temp_packet[i] = rxBuffer[temp_index];
            temp_index = (temp_index + 1) % RX_BUFFER_SIZE;
        }
        // temp_packet[0] = 0xAA (start)
        // temp_packet[1] = cmd_type, [2] = data1, [3] = data2, [4] = vector_length
        uint8_t vector_length = temp_packet[4];

        // Verificăm dacă vector_length este valid (doar 1 sau 4 sunt acceptate)
        if (vector_length != 1 && vector_length != 4)
        {
            // Nu este un pachet valid, deci eliminăm markerul de început și continuăm
            rxReadIndex = (rxReadIndex + 1) % RX_BUFFER_SIZE;
            RingBuffer_Clear();
            continue;
        }

        // Lungimea totală a pachetului
        uint8_t packet_length = 6 + vector_length; // 1+3+1+vector_length+1

        // Verificăm dacă avem întregul pachet în buffer
        if (RingBuffer_Available() < packet_length)
            break;  // Așteptăm mai multe date

        // Citim întregul pachet
        uint8_t packet[10];  // maxim 10 octeți
        for (int i = 0; i < packet_length; i++)
        {
            packet[i] = rxBuffer[rxReadIndex];
            rxReadIndex = (rxReadIndex + 1) % RX_BUFFER_SIZE;
        }

        // Verificăm markerul de sfârșit (ultimul octet)
        if (packet[packet_length - 1] != 0xBB)
        {
            // Pachet invalid: markerul de sfârșit nu este corect, deci ignorăm pachetul
        	 RingBuffer_Clear();
            continue;
        }

        // Extragem câmpurile din pachet:
        // packet[0] este 0xAA (start)
        // packet[1] = cmd_type, [2] = data1, [3] = data2, [4] = vector_length,
        // vectorul începe la packet[5] și markerul de sfârșit este la packet[packet_length - 1]
        uint8_t cmd_type = packet[1];
        uint8_t data1 = packet[2];
        uint8_t data2 = packet[3];

        // Pregătim un vector de 4 elemente.
        // Dacă vector_length este 4, le preluăm direct.
        // Dacă este 1, completăm toate cele 4 elemente cu aceeași valoare ca primul element primit.
        uint8_t vector_out[4];
        if (vector_length == 4)
        {
            for (int i = 0; i < 4; i++)
            {
                vector_out[i] = packet[5 + i];
            }
        }
        else // vector_length == 1
        {
            uint8_t val = packet[5];
            for (int i = 0; i < 4; i++)
            {
                vector_out[i] = val;
            }
        }
        RingBuffer_Clear();

        // Apelează funcția care procesează datele primite
        process_rasp_data(cmd_type, data1, data2, vector_out);
    }
}


