/*
 * storage.h
 *
 * author: Iacopo Sprenger
 *
 */


#ifndef STORAGE_H
#define STORAGE_H

#include <maxon_comm.h>
#include <sensor.h>

typedef struct {
	MOTOR_PPM_PARAMS_t persistent_ppm_params;
}PERSISTENT_DATA_t;

typedef struct {
	SENSOR_DATA_t sensor_data;
	uint32_t sample;
}SAMPLE_DATA_t;


#define PERSISTENT_ADDRESS	0
#define SAMPLE_ADDRESS(i) 	(sizeof(PERSISTENT_DATA_t) + PERSISTENT_ADDRESS + i*sizeof(SAMPLE_DATA_t))


void write_persistent(PERSISTENT_DATA_t * data);

void read_persistent(PERSISTENT_DATA_t * data);

void write_sample(SAMPLE_DATA_t * sample);

void read_sample(SAMPLE_DATA_t * sample, uint32_t id);

void recover_persistent(void);

void save_persistent(void);



#endif
