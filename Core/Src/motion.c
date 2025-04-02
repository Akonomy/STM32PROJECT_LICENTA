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




void go(uint8_t max_x, uint8_t direction_x) {



	for (uint8_t x = 0; x < max_x; x++) {


		DelayWithTimer(100);


	}

}
