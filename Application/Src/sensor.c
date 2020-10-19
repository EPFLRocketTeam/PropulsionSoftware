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
#include <comm.h>
#include <storage.h>

static uint16_t PP_sensorData[PP_NB_SENSOR];

static uint16_t adcBuffer[PP_NB_SENSOR];
static uint32_t time;
static SENSOR_DATA_t current_data;
//static uint32_t last_measure_time;

static SAMPLING_DATA_t sampling = {0};

static uint16_t counter = 0;


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){

	for(int i = 0; i < PP_NB_SENSOR; i++) {
		PP_sensorData[i] = adcBuffer[i];
	}
	sampling.press_1 += adcBuffer[0];
	sampling.press_2 += adcBuffer[1];
	if(adcBuffer[2] >= TEMP_MIN && adcBuffer[2] < TEMP_MAX) {
		sampling.temp_1 += temp_LUT[adcBuffer[2]-TEMP_MIN];
	} else {
		sampling.temp_1 += 0x8000;
	}
	if(adcBuffer[3] >= TEMP_MIN && adcBuffer[3] < TEMP_MAX) {
		sampling.temp_2 += temp_LUT[adcBuffer[3]-TEMP_MIN];
	} else {
		sampling.temp_2 += 0x8000;
	}
	if(adcBuffer[4] >= TEMP_MIN && adcBuffer[4] < TEMP_MAX) {
		sampling.temp_3 += temp_LUT[adcBuffer[4]-TEMP_MIN];
	} else {
		sampling.temp_3 += 0x8000;
	}
	counter++;
	if(counter == NB_SAMPLES) {
		time += SAMPLING_TIME;
		current_data.temp_1 = sampling.temp_1>>6;
		current_data.temp_2 = sampling.temp_2>>6;
		current_data.temp_3 = sampling.temp_3>>6;
		current_data.press_1 = sampling.press_1>>6;
		current_data.press_2 = sampling.press_2>>6;
		current_data.time = xTaskGetTickCountFromISR() * 1000 / configTICK_RATE_HZ;
		counter = 0;
		sampling.temp_1 = 0;
		sampling.temp_2 = 0;
		sampling.temp_3 = 0;
		sampling.press_1 = 0;
		sampling.press_2 = 0;
		static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR( get_can_sem(), &xHigherPriorityTaskWoken );
		xSemaphoreGiveFromISR( get_storage_sem(), &xHigherPriorityTaskWoken );
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

	}
}


//SETUP THE SENSOR VALUES IN CUBE_MX  -> they need to be on the S2 because s1 is used for maxon_comm.
void PP_sensorInit(void) {

	//the sampling rate is 100Hz to be setup in  prop_soft.ioc
	HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcBuffer, PP_NB_SENSOR);
	time = 0;

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
