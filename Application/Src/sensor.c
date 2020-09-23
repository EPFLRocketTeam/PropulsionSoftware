/*
 * sensor.c
 *
 * author: Iacopo Sprenger
 */


#include <main.h>
#include <sensor.h>
#include "adc.h"
#include "tim.h"
#include "dma.h"
#include <threads.h>
#include "gpio.h"
#include <control.h>
#include <lut.h>

static uint16_t PP_sensorData[PP_NB_SENSOR];

static uint16_t adcBuffer[PP_NB_SENSOR];
static uint32_t time;
static SENSOR_DATA_t current_data;
//static uint32_t last_measure_time;




void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	time += SAMPLING_TIME;
	for(int i = 0; i < PP_NB_SENSOR; i++) {
		PP_sensorData[i] = adcBuffer[i];
	}
	current_data.press_1 = adcBuffer[0];
	current_data.press_2 = adcBuffer[1];
	if(adcBuffer[2] >= TEMP_MIN && adcBuffer[2] < TEMP_MAX) {
		current_data.temp_1 = temp_LUT[adcBuffer[2]-TEMP_MIN];
	} else {
		current_data.temp_1 = 0xffff;
	}
	if(adcBuffer[3] >= TEMP_MIN && adcBuffer[3] < TEMP_MAX) {
		current_data.temp_2 = temp_LUT[adcBuffer[3]-TEMP_MIN];
	} else {
		current_data.temp_2 = 0xffff;
	}
	if(adcBuffer[4] >= TEMP_MIN && adcBuffer[4] < TEMP_MAX) {
		current_data.temp_3 = temp_LUT[adcBuffer[4]-TEMP_MIN];
	} else {
		current_data.temp_3 = 0xffff;
	}
	current_data.time = time;
}


//SETUP THE SENSOR VALUES IN CUBE_MX  -> they need to be on the S2 because s1 is used for maxon_comm.
void PP_sensorInit(void) {

	//the sampling rate is 100Hz to be setup in  prop_soft.ioc
	HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcBuffer, PP_NB_SENSOR);
	time = 0;
}



void PP_sensorFunc(void *argument) {

	 TickType_t lastWakeTime;
	 const TickType_t period = pdMS_TO_TICKS(50);


	 lastWakeTime = xTaskGetTickCount();


	for(;;) {




	    vTaskDelayUntil( &lastWakeTime, period );

	}

}


uint32_t sensor_get_time(void) {
	return time;
}


uint16_t sensor_get_data(PP_SENSOR_t sensor) {
	if (sensor < PP_NB_SENSOR) {
		return PP_sensorData[sensor];
	} else {
		return -1;
	}
}

SENSOR_DATA_t sensor_get_data_struct(void) {
	return current_data;
}
