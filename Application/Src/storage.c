/*
 * storage.c
 *
 * author: Iacopo Sprenger
 */



#include <main.h>
#include <storage.h>
#include <flash.h>
#include <maxon_comm.h>



static uint32_t sample_count = 0;


void write_persistent(PERSISTENT_DATA_t * data) {
	flash_write(PERSISTENT_ADDRESS, (uint8_t *) data, sizeof(PERSISTENT_DATA_t));
}

void read_persistent(PERSISTENT_DATA_t * data) {
	flash_read(PERSISTENT_ADDRESS, (uint8_t *) data, sizeof(PERSISTENT_DATA_t));
}

void write_sample(SAMPLE_DATA_t * sample) {
	sample->sample = sample_count;
	flash_write(SAMPLE_ADDRESS(sample_count), (uint8_t *) sample, sizeof(SAMPLE_DATA_t));
	sample_count++;
}

void read_sample(SAMPLE_DATA_t * sample, uint32_t id) {
	flash_read(SAMPLE_ADDRESS(id), (uint8_t *) sample, sizeof(SAMPLE_DATA_t));
}


void save_persistent(void) {
	PERSISTENT_DATA_t data;
	data.persistent_ppm_params = motor_get_ppm_params();
	write_persistent(&data);
}

void recover_persistent(void) {
	PERSISTENT_DATA_t data;
	read_persistent(&data);
	//motor_set_ppm_params(data.persistent_ppm_params);
}


