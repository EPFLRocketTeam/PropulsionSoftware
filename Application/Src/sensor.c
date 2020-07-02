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

static uint16_t PP_sensorData[PP_NB_SENSOR];
static uint16_t adcBuffer[PP_NB_SENSOR];
//static uint32_t last_measure_time;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){

	for(int i = 0; i < PP_NB_SENSOR; i++) {
		PP_sensorData[i] = adcBuffer[i];
	}
}

void PP_sensorInit(void) {

	//the sampling rate is 50Hz to be setup in  prop_soft.ioc
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcBuffer, PP_NB_SENSOR);
}



void PP_sensorFunc(void *argument) {

	 TickType_t lastWakeTime;
	 const TickType_t period = pdMS_TO_TICKS(50);


	 lastWakeTime = xTaskGetTickCount();


	for(;;) {
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);

		for(int i = 0; i < PP_NB_SENSOR; i++) {
		    PP_sensorData[i] = adcBuffer[i];
		}




	    vTaskDelayUntil( &lastWakeTime, period );

	}

}


uint16_t PP_getData(PP_SENSOR_t sensor) {
	if (sensor < PP_NB_SENSOR) {
		return PP_sensorData[sensor];
	} else {
		return -1;
	}
}
