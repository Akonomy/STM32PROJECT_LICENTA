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
		go(4, 11); //make a first turn
		read_sensors();
		while (lineNotFound) {

			go(1, 11); // continue rotating until line found
			read_sensors();  //update sensors
			if (sensor_data[4] || sensor_data[0] || sensor_data[6]) {
				lineNotFound = 0;
				go(1, 0);
				return;
			}
		}

	case 3: //left turn
		go(2, 1);
		go(4, 10); //make a first turn
		read_sensors();
		while (lineNotFound) {

			go(1, 10); // continue rotating until line found
			read_sensors();  //update sensors
			if (sensor_data[4] || sensor_data[0] || sensor_data[6]) {
				lineNotFound = 0;
				go(1, 0);
				return;
			}
		}

	case 4: //left turn
		go(3, 12);
		go(6, 11); //make a first turn
		read_sensors();
		while (lineNotFound) {

			go(1, 11); // continue rotating until line found
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

void go(uint8_t max_x, uint8_t direction_x) {

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
