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
	    SetSensorLeft(1);
    // Verificare parametri

	if (tick == 0 || direction==0){
		 I2C_Send_Packet(i2c_slave_address,0x0000 ,0, 4);
		 return;
	}
    if (direction > 20) {
        // Eroare: direcția este în afara intervalului permis (0-12)
        return;
    }
    if (tick>30) {
        // Eroare: tick-ul este în afara intervalului permis (1-10)
        return;
    }




    uint8_t delay_movement =105;

    if (tick==1){
    	delay_movement=150;
    }
	for (uint8_t x = 0; x < tick; x++) {

		 I2C_Send_Packet(i2c_slave_address, directii_implicite[direction] , speed, 4);
		 DelayWithTimer(delay_movement);
		 I2C_Send_Packet(i2c_slave_address, 0x0000,0, 4);

	}


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
void control_servo(uint8_t servo_id, uint8_t angle) {
    // Verificare parametri
    if (servo_id < 180 || servo_id > 190) {
        // Eroare: ID-ul servo-ului este în afara intervalului permis (180-190)
        return;
    }
    if (angle > 180) {
        // Eroare: unghiul este în afara intervalului permis (0-180)
        return;
    }

    //SendSingleValue(0x08, servo_id, angle);  /*UNCOMMENT WHEN U IMPLEMENT SERVO*/
    //DelayWithTimer(10);


    SetSensorRight(1);
    SetSensorLeft(1);

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




    if (sensor_type==2){

    	for( uint8_t x=0; x<7; x++ ) {
    	        // Stare 1: Aprinde LED-ul roșu, stinge LED-ul albastru
    	        SetSensorRight(1);  // LED roșu aprins
    	        SetSensorLeft(0);   // LED albastru stins
    	        DelayWithTimer(200);  // așteaptă 200 ms




    	        // Stare 2: Ambele LED-uri stinse (pauză scurtă)
    	        SetSensorRight(0);
    	        SetSensorLeft(0);
    	        DelayWithTimer(200);  // așteaptă 200 ms

    	        // Stare 3: Stinge LED-ul roșu, aprinde LED-ul albastru
    	        SetSensorRight(0);
    	        SetSensorLeft(1);   // LED albastru aprins
    	        DelayWithTimer(200);  // așteaptă 200 ms





    	        // Stare 4: Ambele LED-uri stinse (pauză scurtă)
    	        SetSensorRight(0);
    	        SetSensorLeft(0);
    	        DelayWithTimer(200);  // așteaptă 200 ms
    }
    }
    SetSensorRight(0);
    SetSensorLeft(0);


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
    SetSensorRight(1);
    SetSensorLeft(1);
    DelayWithTimer(50);
    SetSensorRight(0);
    SetSensorLeft(0);
    DelayWithTimer(50);
    SetSensorRight(1);
    SetSensorLeft(1);
    DelayWithTimer(50);
    SetSensorRight(0);
    SetSensorLeft(0);




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
            break;
        case 3:
            // Tipul 3: Request Data
            request_data(data1);
            break;
        case 4:
            // Tipul 4: Save Next Cross Direction
            save_next_cross_direction(data1);
            break;
        default:
            // Tip de comandă necunoscut - nu se realizează nicio acțiune.
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
    currentTime = get_system_time_ms();

    // Verifică intervalul minim între procesări pentru a evita "spam"-ul de mesaje.
    if ((currentTime - lastProcessTime) < MIN_PROCESS_INTERVAL)
    {
        RingBuffer_Clear();
        SetSensorRight(1);
        DelayWithTimer(250);
        SetSensorRight(0);
        lastProcessTime = currentTime;
        return;
    }
    // Actualizează timpul ultimei procesări

    char message[50] = {0};  // Buffer local pentru mesaj
    int msgOffset = 0;

    // Caută markerul de început '<' în ring buffer.
    while (RingBuffer_Available() > 0 && msgOffset < (sizeof(message) - 1))
    {
        uint8_t ch = rxBuffer[rxReadIndex];
        rxReadIndex = (rxReadIndex + 1) % RX_BUFFER_SIZE;  // Asigură circularitatea bufferului
        if (ch == '<')
        {
            message[msgOffset++] = ch;  // Salvează markerul de început
            break;
        }
    }

    // Dacă nu s-a găsit markerul de început, mesajul este invalid.
    if (msgOffset == 0)
    {
        RingBuffer_Clear();
        return;
    }

    // Citește caractere până se găsește markerul de sfârșit '>' sau până se umple bufferul.
    bool endMarkerFound = false;
    while (RingBuffer_Available() > 0 && msgOffset < (sizeof(message) - 1))
    {
        uint8_t ch = rxBuffer[rxReadIndex];
        rxReadIndex = (rxReadIndex + 1) % RX_BUFFER_SIZE;
        message[msgOffset++] = ch;
        if (ch == '>')
        {
            endMarkerFound = true;
            break;
        }
    }
    message[msgOffset] = '\0';  // Terminare cu null

    // Dacă nu s-a găsit markerul de sfârșit, mesajul este incomplet.
    if (!endMarkerFound)
    {
        RingBuffer_Clear();
        return;
    }

    // Verifică dacă mesajul începe cu '<' și se termină cu '>'
    if (message[0] != '<' || message[msgOffset - 1] != '>')
    {
        RingBuffer_Clear();
        return;
    }

    // Elimină markerul de sfârșit '>' și setează un pointer la interiorul mesajului.
    message[msgOffset - 1] = '\0';
    char *innerMessage = message + 1;  // Sărim peste caracterul '<'

    // Se așteaptă formatul: val1 val2 val3 [vector]
    // Tokenizează primele 3 valori folosind spațiul ca delimitator.
    char *token = strtok(innerMessage, " ");
    if (token == NULL)
    {
        RingBuffer_Clear();
        return;
    }
    uint8_t val1 = (uint8_t)atoi(token);

    token = strtok(NULL, " ");
    if (token == NULL)
    {
        RingBuffer_Clear();
        return;
    }
    uint8_t val2 = (uint8_t)atoi(token);

    token = strtok(NULL, " ");
    if (token == NULL)
    {
        RingBuffer_Clear();
        return;
    }
    uint8_t val3 = (uint8_t)atoi(token);

    // Acum se caută secvența vectorului, care trebuie să înceapă cu '['.
    token = strtok(NULL, ""); // Rămâne restul șirului, inclusiv spațiile, pentru a putea găsi '['
    if (token == NULL)
    {
        RingBuffer_Clear();
        return;
    }

    // Caută începutul secvenței de vector
    char *vecStart = strchr(token, '[');
    if (vecStart == NULL)
    {
        RingBuffer_Clear();
        return;
    }
    vecStart++;  // Sărim peste '['

    // Caută sfârșitul secvenței vector (caracterul ']')
    char *vecEnd = strchr(vecStart, ']');
    if (vecEnd == NULL)
    {
        RingBuffer_Clear();
        return;
    }
    *vecEnd = '\0';  // Înlocuiește ']' cu '\0' pentru a avea un subșir curat

    // Acum, vecString conține conținutul din interiorul parantezelor, de ex: "255 255 255 255" sau "255"
    // Folosim strtok pentru a obține token-urile din vector, folosind ca delimitatori spațiile.
    uint8_t vector[4] = {0};
    int vecCount = 0;
    token = strtok(vecStart, " ");
    while (token != NULL && vecCount < 4)
    {
        vector[vecCount++] = (uint8_t)atoi(token);
        token = strtok(NULL, " ");
    }

    // Dacă vectorul conține doar un element, replică-l pe toate cele 4 poziții.
    if (vecCount == 1)
    {
        vector[1] = vector[0];
        vector[2] = vector[0];
        vector[3] = vector[0];
    }
    else if (vecCount != 4)
    {
        // Dacă numărul de elemente nu e 1 sau 4, poți alege să:
        // - Completezi cu zerouri,
        // - Folosești doar primele 4, sau
        // - Marci mesajul ca invalid.
        // Aici optăm pentru completarea cu zerouri dacă sunt mai puține de 4.
        while (vecCount < 4)
        {
            vector[vecCount++] = 0;
        }
    }

    // Curățăm ring buffer-ul pentru a evita recitirea datelor vechi.
    RingBuffer_Clear();

    // Apelăm funcția de procesare a datelor:
    //   val1, val2, val3 sunt primele 3 valori, iar vectorul de 4 elemente este transmis ca al 4-lea parametru.
    process_rasp_data(val1, val2, val3, vector);
}




