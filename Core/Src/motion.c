/*
 * motion.c
 *
 *  Created on: Feb 6, 2025
 *      Author: andre
 */

#include "motion.h"
#include "communication.h"
#include "timer.h"
#include "sensors.h"   // Pentru prototipul read_sensors()
#include "globals.h"   // Pentru sensor_data (declarată ca extern în globals.h)

void makeTurn(uint8_t direction_x) {
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

	uint8_t lineNotFound = 1;

	switch (direction_x) {

	case 0:
		go(1, 0);
	case 1: //front
		//do nothing
		go(2, 1);
		return;
	case 2: //right
		go(2, 1);
		go(4, 7); //make a first turn
		read_sensors();
		while (lineNotFound) {

			go(1, 7); // continue rotating until line found
			read_sensors();  //update sensors
			if (sensor_data[4] || sensor_data[0] || sensor_data[6]) {
				lineNotFound = 0;
				go(1, 0);
				return;
			}
		}

	case 3: //left turn
		go(2, 1);
		go(4, 8); //make a first turn
		read_sensors();
		while (lineNotFound) {

			go(1, 8); // continue rotating until line found
			read_sensors();  //update sensors
			if (sensor_data[4] || sensor_data[0] || sensor_data[6]) {
				lineNotFound = 0;
				go(1, 0);
				return;
			}
		}

	case 4: //left turn
		go(3, 3);
		go(6, 7); //make a first turn
		read_sensors();
		while (lineNotFound) {

			go(1, 7); // continue rotating until line found
			read_sensors();  //update sensors
			if (sensor_data[4] || sensor_data[0] || sensor_data[6]) {
				lineNotFound = 0;
				go(1, 0);
				return;
			}
		}

	default:
		go(1, 0);
		return;

	} //end of switch

} //end of makeTurn()





/*OLD
 *
 *
 * 	 case 1: return "FORWARD";
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
 *
 *
 * */







void go(uint8_t max_x, uint8_t direction_x) {

	/*  max_x-times  direction_x in direction x
	 *  ARDUINO DIRECTIONS MAP , FOR GO() FUNCTION
	 *  case 0: return "STOP";
v    case 0:  return 0b00000000; // STOP
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

	for (uint8_t x = 0; x < max_x; x++) {

		SendSingleValue(0x08, 128, direction_x);
		DelayWithTimer(100);
		SendSingleValue(0x08, 128, 0);

	}

}

void go_servo(uint8_t servo, uint8_t position) {
	/*
	 case 300: return "SERVO BAZA";
	 case 301: return "SERVO BRAT";
	 case 302: return "SERVO CLAW";

	 */

	SendSingleValue(0x08, servo, position);
	DelayWithTimer(10);

}
