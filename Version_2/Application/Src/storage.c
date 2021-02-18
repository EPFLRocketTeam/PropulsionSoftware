/*  Title		: Storage
 *  Filename	: storage.c
 *	Author		: iacopo sprenger
 *	Date		: 07.02.2021
 *	Version		: 0.1
 *	Description	: storage on the onboard flash memory
 */

/**********************
 *	INCLUDES
 **********************/

#include <storage.h>
#include <sensor.h>
#include <control.h>
#include <flash.h>

/**********************
 *	CONSTANTS
 **********************/

#define DATA_SIZE		(32)

#define MAGIC_NUMBER	0xCBE0C5E6
#define MAGIC_ADDR		0x00000000
#define USED_SS_ADDR	0x00000004
#define STATE_ADDR		0x00000008

#define SUBSECTOR_SIZE	4096
#define SAMPLES_PER_SS	(SUBSECTOR_SIZE/DATA_SIZE)

#define NEXT_SUBSECTOR (SUBSECTOR_SIZE*used_subsectors)
#define DATA_START		SUBSECTOR_SIZE
#define NB_SUBSECTOR	4096

#define LONG_TIME		0xffff




/**********************
 *	MACROS
 **********************/

#define ADDRESS(i)		(DATA_START + (i)*DATA_SIZE)
#define SUBSECTOR(i)	(ADDRESS(i)>>12)

/**********************
 *	TYPEDEFS
 **********************/


typedef struct {
	uint16_t sample_id;
	int16_t temp_1;
	int16_t temp_2;
	int16_t temp_3;
	int32_t pres_1;
	int32_t pres_2;
	int16_t motor_pos;
	uint16_t system_status;
	uint32_t sensor_time;
	uint32_t padding_1;
	uint32_t padding_2;
}STORAGE_DATA_t;  //MUST BE AN INTEGER DIVISOR OF 4096


/**********************
 *	VARIABLES
 **********************/

static STORAGE_INST_t storage;


/**********************
 *	PROTOTYPES
 **********************/

static STORAGE_DATA_t read_data(STORAGE_INST_t * store, uint32_t address);
static void write_header(STORAGE_INST_t * store, uint32_t nb_ss);
static void write_data(STORAGE_INST_t * store, STORAGE_DATA_t data);

/**********************
 *	DECLARATIONS
 **********************/

void storage_init(STORAGE_INST_t * store) {
	static uint32_t tmp_data;
	flash_init();
	flash_read(MAGIC_ADDR, (uint8_t *) &tmp_data, sizeof(uint32_t));
	if(tmp_data == MAGIC_NUMBER) {
		flash_read(USED_SS_ADDR, (uint8_t *) &store->used_subsectors, sizeof(uint32_t));
		if(store->used_subsectors > 1) {
			STORAGE_DATA_t data;
			uint32_t count = (store->used_subsectors-2)*SAMPLES_PER_SS;
			data = read_data(store, count);
			while(data.sample_id == count){
				data = read_data(store, ++count);
			}
			store->data_counter = count;
		} else {
			store->data_counter = 0;
		}
	} else {
		write_header(store, 1);
		store->data_counter = 0;
	}
	store->ready_sem = xSemaphoreCreateBinaryStatic(&store->ready_sem_buffer);
	store->record_active = 0;
}


void storage_record_sample(STORAGE_INST_t * store) {
	STORAGE_DATA_t data = {0};
	SENSOR_DATA_t sensor_data = sensor_get_last();
	data.temp_1 = sensor_data.temperature[1];
	data.temp_2 = sensor_data.temperature[2];
	data.temp_3 = sensor_data.temperature[3];
	data.pres_1 = sensor_data.pressure_1;
	data.pres_2 = sensor_data.pressure_2;
	data.sensor_time = sensor_data.time;

	CONTROL_STATUS_t control_data = control_get_status();
	data.motor_pos = control_data.pp_position;
	data.system_status = control_data.state;

	write_data(store, data);

}

static void write_header(STORAGE_INST_t * store, uint32_t nb_ss) {
	static uint32_t tmp_data;
	flash_erase_subsector(MAGIC_ADDR);
	tmp_data = MAGIC_NUMBER;
	flash_write(MAGIC_ADDR, (uint8_t *) &tmp_data, sizeof(uint32_t));
	tmp_data = nb_ss;
	flash_write(USED_SS_ADDR, (uint8_t *) &tmp_data, sizeof(uint32_t));
	store->used_subsectors = nb_ss;
}

static STORAGE_DATA_t read_data(STORAGE_INST_t * store, uint32_t id) {
	static STORAGE_DATA_t data;
	flash_read(ADDRESS(id), (uint8_t *) &data, sizeof(STORAGE_DATA_t));
	return data;
}

static void write_data(STORAGE_INST_t * store, STORAGE_DATA_t data) {
	data.sample_id = store->data_counter;
	uint32_t addr = ADDRESS(store->data_counter++);
	if(addr % SUBSECTOR_SIZE == 0) {
		write_header(store, store->used_subsectors + 1);
		flash_erase_subsector(addr);
	}
	flash_write(addr, (uint8_t *) &data, sizeof(STORAGE_DATA_t));
}

uint32_t storage_get_used() {
	return storage.data_counter;
}

void storage_get_sample(STORAGE_INST_t * store, uint32_t id, void * dest) {
	*((STORAGE_DATA_t *)dest) = read_data(store, id);
}

SemaphoreHandle_t storage_get_sem(STORAGE_INST_t * store) {
	return store->ready_sem;
}

STORAGE_INST_t * storage_get_inst() {
	return &storage;
}

void storage_enable() {
	storage.record_active = 1;
}

void storage_disable() {
	storage.record_active = 0;
}


void storage_thread(void * arg) {

	storage_init(&storage);


	for(;;) {
		if(xSemaphoreTake(storage.ready_sem, LONG_TIME) == pdTRUE) {
			if(storage.record_active) {
				storage_record_sample(&storage);
			}
		}
	}
}



/* END */


