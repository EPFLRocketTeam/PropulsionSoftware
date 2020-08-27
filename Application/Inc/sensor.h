/*
 * sensor.h
 *
 * author: Iacopo Sprenger
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <main.h>

typedef enum {
	PP_PRESSURE_1, 		// PC0 | A1
	PP_PRESSURE_2, 		// PA2 | A0
	PP_TEMPERATURE_1, 	// PC2 | MISO
	PP_TEMPERATURE_2, 	// PC3 | MOSI
	PP_TEMPERATURE_3,	// PA15| D0
	PP_NB_SENSOR
}PP_SENSOR_t;


void PP_sensorInit(void);

void PP_sensorFunc(void *argument);

uint16_t sensor_get_data(PP_SENSOR_t sensor);



#endif
