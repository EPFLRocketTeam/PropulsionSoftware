/*
 * sensor.h
 *
 * author: Iacopo Sprenger
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <main.h>

typedef enum {
	PP_PRESSURE_1, 		// PA0 | D0 S1
	PP_PRESSURE_2, 		// PA1 | D1 S1
	PP_TEMPERATURE_1, 	// PA2 | D1 S2
	PP_TEMPERATURE_2, 	// PC2 | MISO
	PP_TEMPERATURE_3,	// PC3 | MOSI
	PP_NB_SENSOR
}PP_SENSOR_t;


void PP_sensorInit(void);

void PP_sensorFunc(void *argument);

uint16_t sensor_get_data(PP_SENSOR_t sensor);



#endif
