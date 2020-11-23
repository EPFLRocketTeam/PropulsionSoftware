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

#define ABS(x)   ((x)>=0?(x):-(x))

PP_STATUS_t start_operation(void) {
	NOMINAL_TRAJ_t traj_def;
	int32_t speed = motor_get_ppm_speed();
	traj_def.time_step = 50;
	traj_def.pos_1 = motor_get_half_target();
	traj_def.pos_2 = motor_get_sec_half_target();
	traj_def.pos_3 = motor_get_target();
	traj_def.time_1 = motor_get_pre_wait();
	traj_def.time_3 = motor_get_half_wait();
	traj_def.time_5 = motor_get_end_wait();
	int32_t speed_incsec = speed*1024/60;
	traj_def.time_2 = ABS(traj_def.pos_1)*1000/speed_incsec;
	traj_def.time_4 = ABS(traj_def.pos_3-traj_def.pos_2)*1000/speed_incsec;
	traj_def.time_6 = ABS(traj_def.pos_3)*1000/speed_incsec;
	traj_buffer_init(get_traj_bfr());
	generate_trajectory(traj_def, get_traj_bfr());
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


void generate_trajectory(NOMINAL_TRAJ_t traj, TRAJ_BUFFER_t * traj_bfr) {
	//first flat
	uint16_t time = 0;
	while (time < traj.time_1) {
		time += traj.time_step;
		traj_buffer_add(traj_bfr, 0);
	}
	//first slope
	time = 0;
	while (time < traj.time_2) {
		time += traj.time_step;
		int32_t pos = time*traj.pos_1/traj.time_2;
		traj_buffer_add(traj_bfr, pos);
	}
	//second slope
	time = 0;
	while (time < traj.time_3) {
		time += traj.time_step;
		int32_t pos = time*(traj.pos_2 - traj.pos_1)/traj.time_3 + traj.pos_1;
		traj_buffer_add(traj_bfr, pos);
	}
	//third slope
	time = 0;
	while (time < traj.time_4) {
		time += traj.time_step;
		int32_t pos = time*(traj.pos_3 - traj.pos_2)/traj.time_4 + traj.pos_2;
		traj_buffer_add(traj_bfr, pos);
	}
	//second flat
	time = 0;
	while (time < traj.time_5) {
		time += traj.time_step;
		traj_buffer_add(traj_bfr, traj.pos_3);
	}
	//fourth flat
	time = 0;
	while (time < traj.time_6) {
		time += traj.time_step;
		int32_t pos = traj.pos_3 - time*(traj.pos_3)/traj.time_6;
		traj_buffer_add(traj_bfr, pos);
	}

}



static CAN_msg control_msg;

//trajectory generator somewhere here


void PP_controlFunc(void *argument) {

	 TickType_t lastWakeTime;
	 const TickType_t period = pdMS_TO_TICKS(HEART_BEAT);

	 lastWakeTime = xTaskGetTickCount();

	 PP_setLed(0, 0, 5);

	for(;;) {

		//poll CAN communication
		while(can_msgPending()) {
			control_msg = can_readBuffer();
			if(control_msg.id_CAN != CAN_ID_PROPULSION_BOARD) {

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
					PP_setLed(2, 5, 3);

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
	SOLENOID_PORT->BSRR |= SOLENOID_PIN;
	return HAL_GPIO_ReadPin(SOLENOID);
}

uint8_t close_solenoid() {
	SOLENOID_PORT->BSRR |= SOLENOID_PIN << 16;
	return HAL_GPIO_ReadPin(SOLENOID);
}








