/*
 * sensor.h
 *
 * author: Iacopo Sprenger
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <main.h>

#define SAMPLING_TIME 10 //ms

typedef enum { //MODIFY IN IOC
	PP_PRESSURE_1, 		// PC0 | A0 S2
	PP_PRESSURE_2, 		// PA2 | A1 S2
	PP_TEMPERATURE_1, 	// PA0 | D0 S1
	PP_TEMPERATURE_2, 	// PA1 | D1 S1
	PP_TEMPERATURE_3,	// PA3 | A0 S1
	PP_NB_SENSOR
}PP_SENSOR_t;


void PP_sensorInit(void);

void PP_sensorFunc(void *argument);

uint16_t sensor_get_data(PP_SENSOR_t sensor);
uint32_t sensor_get_time(void);



#endif
