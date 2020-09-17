/*
 * storage.h
 *
 * author: Iacopo Sprenger
 *
 */


#ifndef STORAGE_H
#define STORAGE_H

#include <maxon_comm.h>

typedef struct {
	MOTOR_PPM_PARAMS_t persistent_ppm_params;
}PERSISTENT_DATA_t;

typedef struct {
	int16_t temp_1;
	int16_t temp_2;
	int16_t temp_3;
	uint16_t press_1;
	uint16_t press_2;
	uint32_t time;
	uint32_t sample;
}DATA_SAMPLE_t;



#endif
