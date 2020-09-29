/*
 * storage.c
 *
 * author: Iacopo Sprenger
 */



#include <main.h>
#include <storage.h>
#include <flash.h>
#include <maxon_comm.h>

//static const int32_t saved_data;
//static const SAMPLE_DATA_t samples[MAX_SAMPLES];

static uint32_t sample_count = 0;


void write_sample(SAMPLE_DATA_t * sample) {
	sample->sample = sample_count;
	flash_write(SAMPLE_ADDRESS(sample_count), (uint8_t *) sample, sizeof(SAMPLE_DATA_t));
	sample_count++;
}

void read_sample(SAMPLE_DATA_t * sample, uint32_t id) {
	flash_read(SAMPLE_ADDRESS(id), (uint8_t *) sample, sizeof(SAMPLE_DATA_t));
}


void test_write(int32_t data) {
	flash_write(0, &data, 4);
}

int32_t test_read(void) {
	int32_t data;
	flash_read(0, &data, 4);
	return 0;
}


