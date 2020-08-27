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


PP_STATUS_t PP_start_setup(void) {
	return PP_SUCCESS;
}


PP_STATUS_t PP_start_operation(void) {
	return PP_SUCCESS;
}




void PP_controlFunc(void *argument) {

	 TickType_t lastWakeTime;
	 const TickType_t period = pdMS_TO_TICKS(HEART_BEAT);


	 lastWakeTime = xTaskGetTickCount();


	for(;;) {

		//do stuff


		if(get_busy_state()) {
			PP_setLed(0, 5, 0);
		} else {
			PP_setLed(0, 5, 5);
		}

	    vTaskDelayUntil( &lastWakeTime, period );

	}
}

uint8_t toggle_solenoid() {
	HAL_GPIO_TogglePin(SOLENOID);
	return HAL_GPIO_ReadPin(SOLENOID);
}








