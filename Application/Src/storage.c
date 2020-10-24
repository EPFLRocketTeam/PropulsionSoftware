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
	int16_t temp_1;
	int16_t temp_2;
	int16_t temp_3;
	uint16_t press_1;
	uint16_t press_2;
	int16_t motor_pos;
	uint16_t system_status;
	int32_t motor_ipos;
	int32_t motor_icmd;
	uint32_t sensor_time;
	uint32_t motor_time;
}DATA_t;

#define MAGIC_NUMBER	0xCBE0C5E5
#define MAGIC_ADDR		0x00000000
#define USED_SS_ADDR	0x00000004
#define STATE_ADDR		0x00000008

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
static uint8_t should_stop = 0;
static uint8_t should_resume = 0;
static uint8_t should_restart = 0;
static uint32_t mem_state;


void write_header(uint32_t nb_ss, uint32_t st) {
	flash_erase_subsector(MAGIC_ADDR);
	tmp_data = MAGIC_NUMBER;
	flash_write(MAGIC_ADDR, (uint8_t *) &tmp_data, sizeof(uint32_t));
	tmp_data = nb_ss;
	flash_write(USED_SS_ADDR, (uint8_t *) &tmp_data, sizeof(uint32_t));
	tmp_data = st;
	flash_write(STATE_ADDR, (uint8_t *) &tmp_data, sizeof(uint32_t));
	used_subsectors = nb_ss;
	mem_state = st;
}

DATA_t read_data(uint32_t address) {
	static DATA_t data;
	flash_read(address, (uint8_t *) &data, sizeof(DATA_t));
	return data;
}

void storage_start(void) {
	should_restart = 1;
}

void storage_stop(void) {
	should_stop = 1;
}

void storage_resume(void) {
	should_resume = 1;
}

uint32_t get_data_count(void) {
	return data_counter;
}



void get_32_samples(uint16_t sample_id, uint8_t * out) {
	static DATA_t data;
	for(uint16_t i = 0; i < 32; i++) {
		data = read_data(ADDRESS(i+sample_id));
		memcpy((uint32_t *)(out+(i*sizeof(DATA_t))), (uint32_t *) &data, sizeof(DATA_t));
	}
}

void storage_init() {
	flash_init();
	flash_read(MAGIC_ADDR, (uint8_t *) &tmp_data, sizeof(uint32_t));
	if(tmp_data == MAGIC_NUMBER) {
		flash_read(USED_SS_ADDR, (uint8_t *) &used_subsectors, sizeof(uint32_t));
		flash_read(STATE_ADDR, (uint8_t *) &mem_state, sizeof(uint32_t));
		//find the data count in last subsector
		if(used_subsectors >=2) {
			DATA_t data;
			uint32_t count = (used_subsectors-2)*SAMPLES_PER_SS;
			data = read_data(ADDRESS(count));
			while(data.sample_id == count){
				data = read_data(ADDRESS(++count));
			}
			data_counter = count;
		} else {
			data_counter = 0;
		}
	} else {
		write_header(1, 0);
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
		if(should_restart) {
			write_header(1, 1);
			should_restart = 0;
			data_counter = 0;
		}
		if(should_stop) {
			write_header(used_subsectors, 0);
			should_stop = 0;
		}
		if(should_resume) {
			write_header(used_subsectors, 1);
			should_resume = 0;
		}
		if( xSemaphoreTake( ready2store_sem, LONG_TIME ) == pdTRUE ) {
			if(mem_state) {
				//fetch data
				static DATA_t data;
				data.sample_id = data_counter;
				SENSOR_DATA_t tmp = sensor_get_data_struct();
				data.temp_1 = tmp.temp_1;
				data.temp_2 = tmp.temp_2;
				data.temp_3 = tmp.temp_3;
				data.press_1 = tmp.press_1;
				data.press_2 = tmp.press_2;
				data.sensor_time = tmp.time;
				data.motor_pos = INC2DDEG(motor_get_pos());
				data.motor_time = motor_get_time();
				data.system_status = get_global_status();
				data.motor_ipos = motor_get_pos();
				data.motor_icmd = motor_get_pos_cmd();
				//compute address
				uint32_t address = ADDRESS(data_counter++);
				if(address % SUBSECTOR_SIZE == 0) { // new subsector
					write_header(used_subsectors + 1, mem_state);
					flash_erase_subsector(address);
				}
				flash_write(address, (uint8_t *) &data, sizeof(DATA_t));
			}
		}
	}
}




