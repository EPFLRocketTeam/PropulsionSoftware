/*
 * control.c
 *
 * author: Iacopo Sprenger
 */


#include <control.h>
#include <main.h>
#include <led.h>
#include <sensor.h>
#include "cmsis_os.h"
#include <stdlib.h>
#include <maxon_comm.h>
#include <can_comm.h>
#include <storage.h>

#define HEART_BEAT	100 //ms


typedef enum {
	S_IDLE,
	S_SETUP,
	S_HOMING,
	S_ARMED,
	S_OPENING,
	S_CLOSING,
	S_FAULT

}GLOBAL_STATE_t;

static GLOBAL_STATE_t control_state;

void PP_init_control(void) {
	control_state = S_IDLE;


}


PP_STATUS_t start_setup(void) {
	motor_def_config();
	return PP_SUCCESS;
}


PP_STATUS_t start_operation(void) {
	motor_def_start_operation();
	return PP_SUCCESS;
}

PP_STATUS_t start_homing(void) {
	motor_def_start_homing_operation();
	return PP_SUCCESS;
}

PP_STATUS_t start_abort(void) {
	motor_def_abort();
	return PP_SUCCESS;
}

uint16_t get_global_status() {
	return 0x00;
}




static CAN_msg control_msg;


void PP_controlFunc(void *argument) {

	 TickType_t lastWakeTime;
	 const TickType_t period = pdMS_TO_TICKS(HEART_BEAT);

	 lastWakeTime = xTaskGetTickCount();

	 PP_setLed(0, 0, 5);

	for(;;) {

		//poll CAN communication
		while(can_msgPending()) {
			control_msg = can_readBuffer();
			if(control_msg.id_CAN == CAN_ID_PROPULSION_BOARD) {

				if(control_msg.id == DATA_ID_START_OPERATION && control_msg.data == DATA_COMMAND_CHECK_VALUE) {
					PP_setLed(0, 5, 0);
					start_operation();
				}
				if(control_msg.id == DATA_ID_OPEN_SOLENOID && control_msg.data == DATA_COMMAND_CHECK_VALUE) {
					open_solenoid();
					PP_setLed(5, 5, 0);

				}
				if(control_msg.id == DATA_ID_CLOSE_SOLENOID && control_msg.data == DATA_COMMAND_CHECK_VALUE) {
					close_solenoid();
					PP_setLed(0, 5, 0);

				}
				if(control_msg.id == DATA_ID_START_HOMING && control_msg.data == DATA_COMMAND_CHECK_VALUE) {
					start_homing();
					PP_setLed(0, 5, 5);

				}
				if(control_msg.id == DATA_ID_ABORT && control_msg.data == DATA_COMMAND_CHECK_VALUE) {
					start_abort();
					PP_setLed(5, 0, 3);

				}
			}
		}



//		if(get_busy_state()) {
//			PP_setLed(0, 5, 0);
//		} else {
//			PP_setLed(0, 5, 5);
//		}

		//sensor data will be recorded from the fueling start to some time after the valve closure.

		//RECORD DATA ON FLASH
		//when connected to the python the data can be downloaded via RX-TX


	    vTaskDelayUntil( &lastWakeTime, period );

	}
}

uint8_t toggle_solenoid() {
	HAL_GPIO_TogglePin(SOLENOID);
	return HAL_GPIO_ReadPin(SOLENOID);
}

uint8_t open_solenoid() {
	HAL_GPIO_WritePin(SOLENOID, 1);
	return HAL_GPIO_ReadPin(SOLENOID);
}

uint8_t close_solenoid() {
	HAL_GPIO_WritePin(SOLENOID, 0);
	return HAL_GPIO_ReadPin(SOLENOID);
}








