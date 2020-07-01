/*
 * sensor.c
 *
 * author: Iacopo Sprenger
 */


#include <main.h>
#include <sensor.h>
#include "adc.h"
#include <threads.h>
#include "gpio.h"

static uint16_t PP_sensorData[PP_NB_SENSOR];


void PP_sensorFunc(void *argument) {

	 TickType_t lastWakeTime;
	 const TickType_t period = pdMS_TO_TICKS(50);
	 lastWakeTime = xTaskGetTickCount();

	for(;;) {
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
	    HAL_ADC_Start(&hadc1);
	    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	    PP_sensorData[PP_PRESSURE_1] = HAL_ADC_GetValue(&hadc1);
	    HAL_ADC_Start(&hadc2);
	    HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
	    PP_sensorData[PP_PRESSURE_2] = HAL_ADC_GetValue(&hadc2);
	    HAL_ADC_Start(&hadc3);
	    HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
	    PP_sensorData[PP_TEMPERATURE_1] = HAL_ADC_GetValue(&hadc3);
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
