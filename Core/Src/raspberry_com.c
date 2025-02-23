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
void control_car(uint8_t direction, uint8_t tick, uint8_t speed) {

	   SetSensorRight(1);
	    SetSensorLeft(1);
    // Verificare parametri
    if (direction > 20) {
        // Eroare: direcția este în afara intervalului permis (0-12)
        return;
    }
    if (tick < 1 || tick > 20) {
        // Eroare: tick-ul este în afara intervalului permis (1-10)
        return;
    }
    if (speed < 30 || speed > 200) {
        // Eroare: viteza este în afara intervalului permis (30-150)
        return;
    }



	/*  max_x-times  direction_x in direction x
	 *  ARDUINO DIRECTIONS MAP , FOR GO() FUNCTION
    case 0:  return 0b00000000; // STOP
    case 1:  return 0b01010101; // INAINTE (FORWARD)
    case 2:  return 0b10101010; // INAPOI (BACKWARD)
    case 3:  return 0b00001001; // DREAPTA-FATA (FRONT-RIGHT)
    case 4:  return 0b00000110; // STANGA-FATA (FRONT-LEFT)
    case 5:  return 0b10010000; // spatele se misca in dreapta (BACK-RIGHT)
    case 6:  return 0b01100000; // spatele se misca in stanga (BACK-LEFT)
    case 7:  return 0b01101001; // ROTIRE DREAPTA (ROTATE RIGHT)
    case 8:  return 0b10010110; // ROTIRE STANGA (ROTATE LEFT)
    case 9:  return 0b10011001; // LATERALA DREAPTA (SIDE-RIGHT)
    case 10: return 0b01100110; // LATERALA STANGA (SIDE-LEFT)
    case 11: return 0b01000001; // hard turn dreapta (HARD RIGHT TURN)
    case 12: return 0b00010100; // hard turn stanga (HARD LEFT TURN)
    case 13: return 0b10000010; // hard turn stanga miscare spate inapoi
    case 14: return 0b00101000; // hard turn dreapta miscare spate inapoi
    case 15: return 0b00010001; // diagonala stanga fata (DIAGONAL FRONT-LEFT)
    case 16: return 0b00100010; // diagonala spate dreapta (DIAGONAL BACK-RIGHT)
    case 17: return 0b01000100; // diagonala dreapta fata (DIAGONAL FRONT-RIGHT)
    case 18: return 0b10001000; // diagonala spate stanga (DIAGONAL BACK-LEFT)

      //SPECIAL CASE

    case 19: return 0b00000001; // little to left (o singura roata)
    case 20: return 0b00000010; // little to right (o singura roata)


	 *
	 */

    uint8_t delay_movement =105;

    if (tick==1){
    	delay_movement=150;
    }
	for (uint8_t x = 0; x < tick; x++) {

		SendSingleValue(0x08, speed, direction);
		DelayWithTimer(delay_movement);
		SendSingleValue(0x08, speed, 0);

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



void debug_mode(uint8_t code, uint8_t ticks, uint8_t speed) {
    // Verifică condiția specială
    if (code == 4 && ticks == 1 && speed == 3) {
    	SendSingleValue(0x08, 7, (53 << 8) | 54);  // Trimite valori speciale

        SetSensorRight(1);
           SetSensorLeft(1);
           DelayWithTimer(50);
           SetSensorRight(0);
           SetSensorLeft(0);

        return;  // Oprește execuția funcției după acest caz
    }

    // Execută comportamentul normal dacă nu se îndeplinește condiția specială
    for (uint8_t x = 0; x < ticks; x++) {
        SendSingleValue(0x08, speed, code);
        DelayWithTimer(100);
        SendSingleValue(0x08, speed, 0);
    }
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
void process_rasp_data(uint8_t type, uint8_t data1, uint8_t data2, uint8_t data3) {


	    // Trimiterea pachetului prin USART

/* DEBUG ONLY *//*

	    USART_Send_Byte(type);
	    USART_Send_Byte(data1);
	    USART_Send_Byte(data2);
	    USART_Send_Byte(data3);

*/


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


        case 5:
        	debug_mode(data1, data2, data3);
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

    char message[50] = {0};         // Buffer local pentru mesaj
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

    // Dacă nu s-a găsit markerul de început, se consideră mesaj invalid.
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
    message[msgOffset] = '\0';  // Asigură terminarea cu null a șirului

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

    // Elimină caracterele de delimitare: transformă '>' în '\0' și setează un pointer la interiorul mesajului.
    message[msgOffset - 1] = '\0';
    char *innerMessage = message + 1;  // Sărim peste caracterul '<'

    // Tokenizează șirul interior; se permit până la 5 token-uri, dar se folosesc primele 4.
    uint8_t values[5] = {0};
    int count = 0;
    char *token = strtok(innerMessage, " ");
    while (token != NULL && count < 5)
    {
        values[count++] = (uint8_t)atoi(token);
        token = strtok(NULL, " ");
    }

    // Asigură că avem cel puțin 4 token-uri; completează cu zero dacă e necesar.
    while (count < 4)
    {
        values[count++] = 0;
    }

    // Curățăm ring buffer-ul pentru a evita recitirea datelor vechi.
    RingBuffer_Clear();

    // Apelăm funcția de procesare a datelor cu primele 4 token-uri:
    //   values[0] -> tip, values[1] -> data1, values[2] -> data2, values[3] -> data3.
    process_rasp_data(values[0], values[1], values[2], values[3]);
}




