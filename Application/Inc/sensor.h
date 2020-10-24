/*
 * sensor.h
 *
 * author: Iacopo Sprenger
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <main.h>

#define SAMPLING_TIME 	20 //ms
#define ADC_FREQ		3200 //Hz
#define NB_SAMPLES		32


#define SEND_RATE	20

typedef enum { //MODIFY IN IOC
	PP_PRESSURE_1, 		// PC0 | A0 S2
	PP_PRESSURE_2, 		// PA2 | A1 S2
	PP_TEMPERATURE_1, 	// PA0 | D0 S1
	PP_TEMPERATURE_2, 	// PA1 | D1 S1
	PP_TEMPERATURE_3,	// PA3 | A0 S1
	PP_NB_SENSOR
}PP_SENSOR_t;

typedef struct {
	int16_t temp_1;
	int16_t temp_2;
	int16_t temp_3;
	uint16_t press_1;
	uint16_t press_2;
	uint32_t time;
}SENSOR_DATA_t;

typedef struct {
	int32_t temp_1;
	int32_t temp_2;
	int32_t temp_3;
	uint32_t press_1;
	uint32_t press_2;
}SAMPLING_DATA_t;


void PP_sensorInit(void);

void PP_sensorFunc(void *argument);

uint16_t sensor_get_data(PP_SENSOR_t sensor);
SENSOR_DATA_t sensor_get_data_struct(void);
uint32_t sensor_get_time(void);



#endif
