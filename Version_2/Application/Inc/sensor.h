/*  Title       : Sensor
 *  Filename    : sensor.h
 *  Author      : iacopo sprenger
 *  Date        : 28.01.2021
 *  Version     : 0.1
 *  Description : Sensor acquisition and processing
 */

#ifndef SENSOR_H
#define SENSOR_H



/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>

/**********************
 *  CONSTANTS
 **********************/


/**********************
 *  MACROS
 **********************/


/**********************
 *  TYPEDEFS
 **********************/

typedef struct SENSOR_DATA{
	uint32_t pressure_1;
	uint32_t pressure_2;
	int32_t temperature[3];
	uint8_t temperature_valid[3];
	uint32_t time;
}SENSOR_DATA_t;


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif

void sensor_calib(void);

SENSOR_DATA_t sensor_get_last(void);
SENSOR_DATA_t sensor_get_last_bfr(uint8_t n);

void sensor_thread(void * arg);


#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* SENSOR_H */

/* END */
