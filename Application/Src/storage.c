/*
 * storage.c
 *
 * author: Iacopo Sprenger
 */



#include <main.h>
#include <storage.h>
#include <flash.h>
#include <maxon_comm.h>
#include <control.h>
#include <string.h>

//data storage
/*
 * The data will be stored in cells, each cell containing the data to store.
 * 4KB will be a multiple of each data so that there are no data overlapping between subsectors.
 * the first subsector will contain only the peristent config data
 *
 *
 */

/*
 * data in the first subsector:
 * 0h	MAGIC_NUMBER
 * 4h	USED_SUBSECTORS
 *
 */

typedef struct {
	uint16_t sample_id;
	SENSOR_DATA_t sensor_data;
	int16_t motor_pos;
	uint16_t system_status;
	uint16_t reserved[4];
}DATA_t;

#define MAGIC_NUMBER	0xCBE0C5E5
#define MAGIC_ADDR		0x00000000
#define USED_SS_ADDR	0x00000004

#define SUBSECTOR_SIZE	4096
#define SAMPLES_PER_SS	(SUBSECTOR_SIZE/sizeof(DATA_t))

#define NEXT_SUBSECTOR (SUBSECTOR_SIZE*used_subsectors)
#define DATA_START		SUBSECTOR_SIZE
#define NB_SUBSECTOR	4096

#define ADDRESS(i)		(DATA_START + (i)*sizeof(DATA_t))
#define SUBSECTOR(i)	(ADDRESS(i)>>12)

static SemaphoreHandle_t ready2store_sem = NULL;
static StaticSemaphore_t ready2store_semBuffer;

SemaphoreHandle_t get_storage_sem(void) {
	return ready2store_sem;
}

static uint32_t tmp_data;
static uint32_t used_subsectors;
static uint32_t data_counter;
static uint8_t should_store = 1;


void write_used_subsectors(uint32_t nb) {
	flash_erase_subsector(MAGIC_ADDR);
	tmp_data = MAGIC_NUMBER;
	flash_write(MAGIC_ADDR, (uint8_t *) &tmp_data, sizeof(uint32_t));
	tmp_data = nb;
	flash_write(USED_SS_ADDR, (uint8_t *) &tmp_data, sizeof(uint32_t));
	used_subsectors = nb;
}

DATA_t read_data(uint32_t address) {
	static DATA_t data;
	flash_read(address, (uint8_t *) &data, sizeof(DATA_t));
	return data;
}

void storage_start(void) {
	write_used_subsectors(1);
	data_counter = 0;
	should_store = 1;
}

void storage_stop(void) {
	should_store = 0;
}

void storage_resume(void) {
	should_store = 1;
}

uint32_t get_data_count(void) {
	return data_counter;
}

void get_32_samples(uint16_t sample_id, uint8_t * out) {
	static DATA_t data;
	for(uint16_t i = 0; i < 32; i++) {
		data = read_data(ADDRESS(i+sample_id));
		memcpy(out+(i*sizeof(DATA_t)), (uint8_t *) &data, sizeof(DATA_t));
	}
}

void storage_init() {
	flash_init();
	flash_read(MAGIC_ADDR, (uint8_t *) &tmp_data, sizeof(uint32_t));
	if(tmp_data == MAGIC_NUMBER) {
		flash_read(USED_SS_ADDR, (uint8_t *) &used_subsectors, sizeof(uint32_t));
		//find the data count in last subsector
		DATA_t data;
		data.sample_id = 0;
		uint16_t count = 0;
		while(data.sample_id != 0xffff) {
			data = read_data(SUBSECTOR(used_subsectors)+count);
			count++;
		}
		data_counter = count + SAMPLES_PER_SS*(used_subsectors-1);
	} else {
		write_used_subsectors(1);
		data_counter = 0;
	}
	ready2store_sem = xSemaphoreCreateBinaryStatic( &ready2store_semBuffer );
}

uint32_t get_used_subsectors(void) {
	return used_subsectors;
}

uint32_t read_mem(uint32_t address) {
	flash_read(address, (uint8_t *) &tmp_data, sizeof(uint32_t));
	return tmp_data;
}


void PP_storageFunc(void *argument) {
	storage_init();
	for(;;) {
		if( xSemaphoreTake( ready2store_sem, LONG_TIME ) == pdTRUE ) {
			if(should_store) {
				//fetch data
				static DATA_t data;
				uint32_t lol = sizeof(data);
				data.sample_id = data_counter;
				data.sensor_data = sensor_get_data_struct();
				data.motor_pos = motor_get_pos();
				data.system_status = get_global_status();

				//compute address
				uint32_t address = ADDRESS(data_counter++);
				if(address % SUBSECTOR_SIZE == 0) { // new subsector
					write_used_subsectors(used_subsectors + 1);
					flash_erase_subsector(address);
				}
				flash_write(address, (uint8_t *) &data, sizeof(DATA_t));
			}
		}
	}
}




