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
	SENSOR_DATA_t sensor_data;
	uint32_t sample;
}SAMPLE_DATA_t;



#define MAX_SAMPLES	8192

#define SAMPLE_BASE_ADDRESS	0
#define SAMPLE_ADDRESS(i) 	(SAMPLE_BASE_ADDRESS + i*sizeof(SAMPLE_DATA_t))



void write_sample(SAMPLE_DATA_t * sample);

void read_sample(SAMPLE_DATA_t * sample, uint32_t id);

int32_t test_read(void);
void test_write(int32_t data);


#endif
