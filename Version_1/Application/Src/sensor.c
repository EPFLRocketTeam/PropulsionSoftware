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

static uint16_t initialized = 0;

static uint32_t press_1_calib = 0;
static uint32_t press_2_calib = 0;

static uint16_t press_1_base =1000;
static uint16_t press_2_base =1000;

static uint16_t counter = 0;
static uint16_t can_counter = 0;

#define MS_2_SENSOR_TIMER(ms)	72e6/32*(ms)/1000


#define R1_VAL	1799
#define R2_VAL	4700

#define R3_VAL	1798
#define R4_VAL	4700

#define KULITE_322_CAL			64341	//uV/bar

#define KULITE_322_DECODE(val)	((uint64_t)(val) * (R3_VAL+R4_VAL) * 3300 * 1000000 / KULITE_322_CAL / 4096 / R4_VAL)

#define KULITE_323_CAL			64271	//uV/bar

#define KULITE_323_DECODE(val)	((uint64_t)(val) * (R1_VAL+R2_VAL) * 3300 * 1000000 / KULITE_323_CAL / 4096 / R2_VAL)


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){

	for(int i = 0; i < PP_NB_SENSOR; i++) {
		PP_sensorData[i] = adcBuffer[i];
	}
	sampling.press_1 += KULITE_323_DECODE(adcBuffer[0]) - (press_1_base - 1000);
	sampling.press_2 += KULITE_322_DECODE(adcBuffer[1]) - (press_2_base - 1000);
	if(adcBuffer[2] >= TEMP_MIN && adcBuffer[2] < TEMP_MAX) {
		sampling.temp_1 += temp_LUT[adcBuffer[2]-TEMP_MIN];
	} else {
		sampling.temp_1 += 0x0;
	}
	if(adcBuffer[3] >= TEMP_MIN && adcBuffer[3] < TEMP_MAX) {
		sampling.temp_2 += temp_LUT[adcBuffer[3]-TEMP_MIN];
	} else {
		sampling.temp_2 += 0x0;
	}
	if(adcBuffer[4] >= TEMP_MIN && adcBuffer[4] < TEMP_MAX) {
		sampling.temp_3 += temp_LUT[adcBuffer[4]-TEMP_MIN];
	} else {
		sampling.temp_3 += 0x0;
	}
	counter++;
	if(counter == NB_SAMPLES) {
		time += SAMPLING_TIME;
		current_data.temp_1 = sampling.temp_1>>5;
		current_data.temp_2 = sampling.temp_2>>5;
		current_data.temp_3 = sampling.temp_3>>5;
		current_data.press_1 = (sampling.press_1>>5);
		current_data.press_2 = (sampling.press_2>>5);
		current_data.time = xTaskGetTickCountFromISR() * 1000 / configTICK_RATE_HZ;
		counter = 0;
		sampling.temp_1 = 0;
		sampling.temp_2 = 0;
		sampling.temp_3 = 0;
		sampling.press_1 = 0;
		sampling.press_2 = 0;
		if(initialized < 8) {
			press_1_calib += current_data.press_1;
			press_2_calib += current_data.press_2;
			initialized += 1;
		} else if(initialized == 8) {
			press_1_base = press_1_calib >> 3;
			press_2_base = press_2_calib >> 3;
			initialized += 1;
		}
		static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		if(get_can_sem()) {
			xSemaphoreGiveFromISR( get_can_sem(), &xHigherPriorityTaskWoken );
			can_counter = 0;
		}
		can_counter++;
		if(get_storage_sem()) {
			xSemaphoreGiveFromISR( get_storage_sem(), &xHigherPriorityTaskWoken );
		}
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

	}
}


//SETUP THE SENSOR VALUES IN CUBE_MX  -> they need to be on the S2 because s1 is used for maxon_comm.
void PP_sensorInit(void) {

	//define the sampling period
	TIM3->ARR = MS_2_SENSOR_TIMER(5);
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
