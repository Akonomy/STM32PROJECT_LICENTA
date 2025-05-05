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

uint16_t STOPIE[4] = {0,0,0,0};
uint16_t viteza[4] = {1700, 1700, 1700, 1700};
uint16_t vitezaFR[4] = {2200, 2200, 1600, 1600};
uint16_t vitezaLR[4] = {2000, 2000, 1500, 1500};
uint16_t vitezaMICA[4] = {800, 800, 900, 900};


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



	read_sensors();

    // Citim senzorii de linie (senzor activ = detectează linia → valoare 1)
    uint8_t far_left_line  = !sensor_data[4];  // senzor stânga-extrem
    uint8_t left_line      = !sensor_data[0];  // senzor stânga
    uint8_t mid_line       = !sensor_data[1];  // senzor mijloc
    uint8_t right_line     = !sensor_data[3];  // senzor dreapta
    uint8_t far_right_line = !sensor_data[6];  // senzor dreapta-extrem

*/

    // Mergem înainte 3 ticks pentru a ne alinia în centru




	    // Ne pregătim pentru detectare linie
	    read_sensors();


	    #define SEE_LINE() (!sensor_data[0] || !sensor_data[1] || !sensor_data[3] || !sensor_data[4] || !sensor_data[6])

	    switch (direction_x) {
	        case 0:
	        	 //move_car(2, 1, vitezaMICA);
	        	 move_car(0, 1, STOPIE); // Oprire
	        	 mode=0;
	        	 break;

	        case 1:
	            // Stop sau față — nu facem nimic
	        	move_car(1, 4, viteza); // "1" = în față
	            break;

	        case 2: // Right turn
	        	//mutam si centram masina cu intersectia
	        	move_car(1, 4, viteza); // "1" = în față
	        	//ne rotim 45 de grade
	            move_car(8, 5, viteza); // Rotim spre dreapta


	            //actualizam senzorii
	            read_sensors();
	            //cautam linia
	            while(!SEE_LINE()){
	            	//mutam fata spre dreapta
	            	move_car(4,1,vitezaFR);
	            	read_sensors();
	            }

	            break;

	        case 3: // Left turn
	        	//mutam si centram masina cu intersectia
	        	move_car(1, 4, viteza); // "1" = în față
	        	//ne rotim 45 de grade
	            move_car(7, 5, viteza); // Rotim spre stanga


	            //actualizam senzorii
	            read_sensors();
	            //cautam linia
	            while(!SEE_LINE()){
	            	//mutam fata spre stanga
	            	move_car(3,1,vitezaFR);
	            	read_sensors();
	            }

	            break;

	        case 4: // Back turn
	        	//mutam si centram masina cu intersectia

	        	//ne rotim 90 de grade
	            move_car(8, 8, vitezaFR); // Rotim spre dreapta

	            read_sensors();

	            //cautam linia
				while(!SEE_LINE()){
					//mutam fata spre dreapta
					move_car(8,1,vitezaLR);
					read_sensors();
				}

				move_car(3,1,vitezaMICA);
	            break;

	        case 5: // PARCARE  cu intoarcere + un mesaj amarat de ok
	        	//mutam si centram masina cu intersectia

	        	//ne rotim 90 de grade
	            move_car(8, 8, vitezaMICA); // Rotim spre dreapta

	            read_sensors();

	            //cautam linia
				while(!SEE_LINE()){
					//mutam fata spre dreapta
					move_car(8,1,vitezaLR);
					read_sensors();
				}
				move_car(3,1,vitezaMICA);

				mode=5; //parcare
	            break;



	        case 6: // PARCARE cu stop si zice un singur mesaj de speranta
	    	        	 move_car(2, 1, vitezaMICA);
	    	        	 move_car(0, 1, STOPIE); // Oprire


	       				mode=6; //parcare idle asteapta comenzi de la rasp
	       	            break;




	        default:
	            move_car(0,0, STOPIE); // Oprire
	            mode=3; //ceva o mers prost
	            break;
	    }


	}

//end of makeTurn()





void move_car(uint8_t direction, uint8_t tick, uint16_t speed[4]) {

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




void follow_next_direction() {
    uint8_t direction = 0;

    if (headTo <= 7) { // Acum acceptăm și 0 ca valid!
        if (global_directions[0] == headTo) {
            direction = headTo;

            // Shift vectorul la stânga
            for (uint8_t i = 0; i < MAX_DIRECTIONS - 1; i++) {
                global_directions[i] = global_directions[i + 1];
            }
            global_directions[MAX_DIRECTIONS - 1] = 0;
        } else {
            // Neconcordanță — invalidăm vectorul, dar folosim headTo oricum
            for (uint8_t i = 0; i < MAX_DIRECTIONS; i++) {
                global_directions[i] = 0;
            }
            direction = headTo;
        }

        headTo = 255; // Folosim 255 ca „headTo inactiv” – nu ne băgăm iar în asta
    } else {
        // headTo e inactiv, deci încercăm să folosim vectorul
        if (global_directions[0] <= 7) {
            direction = global_directions[0];

            for (uint8_t i = 0; i < MAX_DIRECTIONS - 1; i++) {
                global_directions[i] = global_directions[i + 1];
            }
            global_directions[MAX_DIRECTIONS - 1] = 0;
        } else {
            direction = 4; // default: BACK – plângem și mergem acasă
        }
    }


    if (direction == 0) {
        mode = 0;
    }


    makeTurn(direction);


}






