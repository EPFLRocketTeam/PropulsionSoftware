/*
 * sensor.h
 *
 * author: Iacopo Sprenger
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <main.h>

typedef enum {
	PP_PRESSURE_1,
	PP_PRESSURE_2,
	PP_TEMPERATURE_1,
	PP_NB_SENSOR
}PP_SENSOR_t;




void PP_sensorFunc(void *argument);

uint16_t PP_getData(PP_SENSOR_t sensor);



#endif
