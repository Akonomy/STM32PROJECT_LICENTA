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
uint16_t viteza[4] = {1990, 1990, 1990, 1990};
uint16_t vitezaFR[4] = {2290, 2290, 1690, 1690};

uint16_t vitezaMICA[4] = {1590, 1590, 1790, 1790};








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



	// detectăm linia cu o verificare de siguranță
	uint8_t found_line = 0;
	uint8_t retry_count = 0;



	    // Ne pregătim pentru detectare linie
	    read_sensors();




	    switch (direction_x) {
	        case 0:
	        	 //move_car(2, 1, vitezaMICA);
	        	 move_car(0, 1, STOPIE); // Oprire
	        	 mode=0;
	        	 break;

	        case 1:
	            // Stop sau față — nu facem nimic
	        	move_car(1, 1, viteza); // "1" = în față
	        	read_sensors();
	            break;

	        case 2: // Right turn
	        	//mutam si centram masina cu intersectia
	        	move_car(1, 3, viteza); // "1" = în față
	        	//ne rotim 45 de grade
	            move_car(8, 5, viteza); // Rotim spre dreapta

	            retry_count = 0;


	            read_sensors();
	            //cautam linia
	            while ( !SEE_LINE() && retry_count<50 ) {
	                move_car(8, 1, vitezaFR);
	                read_sensors();
	                retry_count++;
	            }
	            found_line=confirm_line_detection_or_continue(2);
	            if (found_line){
	            	break;
	            }
	            else{
	            	retry_count=0;
	            	  while ( !SEE_LINE() && retry_count<50 ) {
	            		                move_car(8, 1, vitezaFR);
	            		                read_sensors();
	            		                retry_count++;
	            		            }
	            	  if(retry_count>40 && !SEE_LINE()){
	            		  mode=3; //ceva nu a mers bine , linia s-a despawnat
	            	  }
	            }

	            break;




	        case 3: // Left turn
	        	//mutam si centram masina cu intersectia
	        	move_car(1, 3, viteza); // "1" = în față
	        	//ne rotim 45 de grade
	            move_car(7, 5, viteza); // Rotim spre stanga

	            retry_count = 0;


	            read_sensors();
	            //cautam linia
	            while ( !SEE_LINE() && retry_count<50 ) {
	                move_car(7, 1, vitezaFR);
	                read_sensors();
	                retry_count++;
	            }
	            found_line=confirm_line_detection_or_continue(3);
	            if (found_line){
	            	break;
	            }
	            else{
	            	retry_count=0;
	            	  while ( !SEE_LINE() && retry_count<50 ) {
	            		                move_car(7, 1, vitezaFR);
	            		                read_sensors();
	            		                retry_count++;
	            		            }
	            	  if(retry_count>40 && !SEE_LINE()){
	            		  mode=3; //ceva nu a mers bine , linia s-a despawnat
	            	  }
	            }

	            break;



	        case 4: // Back turn


	        	//spate si fugim de intersectia
	        	move_car(2, 2, viteza);
	        	//ne rotim 90 de grade


	            move_car(8, 8, viteza); // Rotim spre dreapta

	            retry_count = 0;

	            read_sensors();



	            //cautam linia
	            while ( !SEE_LINE() && retry_count<50 ) {
	                move_car(8, 1, vitezaFR);
	                read_sensors();
	                retry_count++;
	            }
	            found_line=confirm_line_detection_or_continue(2);
	            if (found_line){
	            	break;
	            }
	            else{
	            	retry_count=0;
	            	  while ( !SEE_LINE() && retry_count<50 ) {
	            		                move_car(8, 1, vitezaFR);
	            		                read_sensors();
	            		                retry_count++;
	            		            }
	            	  if(retry_count>40 && !SEE_LINE()){
	            		  mode=3; //ceva nu a mers bine , linia s-a despawnat
	            	  }
	            }

				//move_car(3,1,vitezaMICA);

	            break;

	        case 5: // Back turn


	        	//spate si fugim de intersectia
	        	move_car(2, 2, viteza);
	        	//ne rotim 90 de grade

	            move_car(8, 8, viteza); // Rotim spre dreapta

	            retry_count = 0;



	            //cautam linia
	            read_sensors();
	            while ( !SEE_LINE() && retry_count<50 ) {
	                move_car(8, 1, vitezaFR);
	                read_sensors();
	                retry_count++;
	            }
	            found_line=confirm_line_detection_or_continue(2);
	            if (found_line){
	            	mode=5;
	            	break;
	            }
	            else{
	            	retry_count=0;
	            	  while ( !SEE_LINE() && retry_count<50 ) {
	            		                move_car(8, 1, vitezaFR);
	            		                read_sensors();
	            		                retry_count++;
	            		            }
	            	  if(retry_count>40 && !SEE_LINE()){
	            		  mode=3; //ceva nu a mers bine , linia s-a despawnat
	            	  }
	            }

				//move_car(3,1,vitezaMICA);


				mode=5; //parcare
	            break;



	        case 6: // PARCARE cu stop si zice un singur mesaj de speranta
	    	        	 move_car(2, 1, vitezaMICA);
	    	        	 move_car(0, 1, STOPIE); // Oprire


	       				mode=6; //parcare idle asteapta comenzi de la rasp
	       	            break;




	        default:
	            move_car(0,0, STOPIE); // Oprire
	            mode=3; //ceva o mers prost rau de tot
	            break;
	    }


	}

//end of makeTurn()







///new function to go back to path





void go_back(uint8_t last_direction_x) {

	uint8_t found_line = 0;
	uint8_t retry_count = 0;

	    // Ne pregătim pentru detectare linie
	    read_sensors();






	        	//ne rotim 90 de grade

	        	if(last_direction_x==9){


	            move_car(8, 16, viteza); // Rotim spre dreapta

	            retry_count = 0;



	            //cautam linia
	            read_sensors();
	            while ( !SEE_LINE() && retry_count<16 ) {
	                move_car(8, 1, vitezaFR);
	                read_sensors();
	                retry_count++;
	            }
	            found_line=confirm_line_detection_or_continue(2);
	            if (found_line){
	            	mode=5;

	            }
	            else{
	            	retry_count=0;
	            	  while ( !SEE_LINE() && retry_count<16 ) {
	            		                move_car(8, 1, vitezaFR);
	            		                read_sensors();
	            		                retry_count++;
	            		            }
	            	  if(retry_count>16 && !SEE_LINE()){
	            		  mode=3; //ceva nu a mers bine , linia s-a despawnat
	            	  }
	            }

				//move_car(3,1,vitezaMICA);


				mode=5; //parcare


	        	}

	        	else if(last_direction_x==10){



		            move_car(7, 16, viteza); // Rotim spre dreapta

		            retry_count = 0;



		            //cautam linia
		            read_sensors();
		            while ( !SEE_LINE() && retry_count<16 ) {
		                move_car(7, 1, vitezaFR);
		                read_sensors();
		                retry_count++;
		            }
		            found_line=confirm_line_detection_or_continue(3);
		            if (found_line){
		            	mode=5;

		            }
		            else{
		            	retry_count=0;
		            	  while ( !SEE_LINE() && retry_count<16 ) {
		            		                move_car(7, 1, vitezaFR);
		            		                read_sensors();
		            		                retry_count++;
		            		            }
		            	  if(retry_count>40 && !SEE_LINE()){
		            		  mode=3; //ceva nu a mers bine , linia s-a despawnat
		            	  }
		            }

					//move_car(3,1,vitezaMICA);


					mode=5; //parcare



	        	}





	    }








uint8_t far_left_line;
uint8_t left_line;
uint8_t mid_line;
uint8_t right_line;
uint8_t far_right_line;


void update_sensors() {
    read_sensors();

    // Update flags based on new sensor data
    far_left_line  = !sensor_data[4];  // sensor 4: far left
    left_line      = !sensor_data[0];  // sensor 0: left
    mid_line       = !sensor_data[1];  // sensor 1: center
    right_line     = !sensor_data[3];  // sensor 3: right
    far_right_line = !sensor_data[6];  // sensor 6: far right
}



uint8_t confirm_line_detection_or_continue(uint8_t movement) {
	uint16_t speedy[4]={1900,1900,0,0};
	uint8_t overdose =0;
	move_car(1,1,speedy);
	update_sensors();




	if(!far_left_line && !left_line && !mid_line && !right_line && !far_right_line){ //RECOVERY PHASE , no return here!!
		//movement from move car  2 for right 3 for left
		if(movement==2){


			while (overdose<5){


						overdose++;
						move_wheel(3, 1, speedy);

						update_sensors();
						if(SEE_LINE()){
							overdose=0;
							break;
						}
					}

		}
		else if (movement==3){

			while (overdose<5){
									overdose++;
									move_wheel(3, 1, speedy);

									update_sensors();
									if(SEE_LINE()){
										overdose=0;
										break;
									}
								}



		}

	}



	if(mid_line){


		//do checks for mid line then
		while (overdose<8){
			overdose++;
			move_wheel(3, 1, speedy);

			update_sensors();
			if(right_line || far_right_line){
				overdose=0;
				break;
			}
		}

		while (overdose<16){
				overdose++;
				move_wheel(4, 1, speedy);

				update_sensors();
				if(left_line || far_left_line){
					overdose=0;
					break;
				}
			}
		while (overdose<8){
			overdose++;
			move_wheel(3, 1, speedy);

			update_sensors();
			if(mid_line || right_line || far_right_line){
				overdose=0;
				return 1;
			}
		}




		return 0;
	} //end check if start from mid line


	// Presupunem că linia poate fi în dreapta
	if (far_right_line || right_line  ) {

		// miscare dreapta
		while(!mid_line && !far_left_line && overdose<15){
		overdose++;
		move_wheel(4, 1, speedy);
		update_sensors();

		}
		if(mid_line){
		return 1;
		}

	}

	// Dacă pare că e în stânga
	if (far_left_line || left_line   ) {

		while(!mid_line && !far_right_line && overdose<15){
		overdose++;
		move_wheel(3, 1, speedy);
		update_sensors();

		}

		if(mid_line){
		return 1;
		}

	}



	if (!far_left_line && !left_line && !mid_line && !right_line && !far_right_line) {
		// Muscă confirmed. Linia nu există.
		return 0;
	}

	// Dacă totul e ok
	return 0;
}




















void move_car(uint8_t direction, uint8_t tick, uint16_t speed[4]) {

	/*ATENTIE SINCE FUNCTIA ASTA E FOLOSITA INTERN  EA NU ARE PROTECTIE LA DIRECTII GRESITE SAU TIKS INFINIT
	 * pentru a mari viteza de procesare s-a renuntat la verificari asa ca respecta :

	MAX DIRECTIONS  20 IAR TICKS NU DEPASI 30

	A


	*/


	   SetSensorRight(1);

    // Verificare parametri

	if (tick == 0 || direction==0){
		SetSensorLeft(1);
		 I2C_Send_Packet(i2c_slave_address,0x0000 ,0, 4);
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




void move_wheel(uint8_t direction, uint8_t tick, uint16_t speed[4]) {

	   SetSensorRight(1);

    // Verificare parametri   0 e roata dreapta 2 e roata stanga




    	for (uint8_t x = 0; x < tick; x++) {
    			SetSensorRight(0);

    			 I2C_Send_Packet(i2c_slave_address, directii_implicite[direction] , speed, 4);
    			 DelayWithTimer(69);
    			 SetSensorRight(1);
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






