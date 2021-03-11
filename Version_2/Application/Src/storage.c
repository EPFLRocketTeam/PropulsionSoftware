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
#include <led.h>

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
#define SS_TO_PREP		500

#define STORAGE_AFTER_SAVE 3000


#define STORAGE_HEART_BEAT 1




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


static uint32_t used_subsectors;
static uint32_t data_counter;
static uint8_t record_active;
static uint8_t restart_required;

static int32_t record_should_stop;


/**********************
 *	PROTOTYPES
 **********************/

static STORAGE_DATA_t read_data(uint32_t address);
static void write_header(uint32_t nb_ss);
static void write_data(STORAGE_DATA_t data);

/**********************
 *	DECLARATIONS
 **********************/

void storage_init() {
	static uint32_t tmp_data;
	flash_init();
	flash_read(MAGIC_ADDR, (uint8_t *) &tmp_data, sizeof(uint32_t));
	if(tmp_data == MAGIC_NUMBER) {
		flash_read(USED_SS_ADDR, (uint8_t *) &used_subsectors, sizeof(uint32_t));
		if(used_subsectors > 1) {
			STORAGE_DATA_t data;
			uint32_t count = (used_subsectors-2)*SAMPLES_PER_SS;
			data = read_data(count);
			while(data.sample_id == count){
				data = read_data(++count);
			}
			data_counter = count;
		} else {
			data_counter = 0;
		}
	} else {
		write_header(1);
		data_counter = 0;
	}
	record_active = 0;
	restart_required = 0;
}


void storage_record_sample() {
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

	write_data(data);

}

static void write_header(uint32_t nb_ss) {
	static uint32_t tmp_data;
	flash_erase_subsector(MAGIC_ADDR);
	tmp_data = MAGIC_NUMBER;
	flash_write(MAGIC_ADDR, (uint8_t *) &tmp_data, sizeof(uint32_t));
	tmp_data = nb_ss;
	flash_write(USED_SS_ADDR, (uint8_t *) &tmp_data, sizeof(uint32_t));
	used_subsectors = nb_ss;
}

static STORAGE_DATA_t read_data(uint32_t id) {
	static STORAGE_DATA_t data;
	flash_read(ADDRESS(id), (uint8_t *) &data, sizeof(STORAGE_DATA_t));
	return data;
}

static void write_data(STORAGE_DATA_t data) {
	data.sample_id = data_counter;
	uint32_t addr = ADDRESS(data_counter++);
	if(addr % SUBSECTOR_SIZE == 0) {
		write_header(used_subsectors + 1);
		flash_erase_subsector(addr);
	}
	flash_write(addr, (uint8_t *) &data, sizeof(STORAGE_DATA_t));
}

uint32_t storage_get_used() {
	return data_counter;
}

void storage_get_sample(uint32_t id, void * dest) {
	*((STORAGE_DATA_t *)dest) = read_data(id);
}

void storage_enable() {
	record_active = 1;
}

void storage_disable() {
	record_should_stop = STORAGE_AFTER_SAVE;
}

void storage_restart() {
	restart_required = 1;
}


void storage_thread(void * arg) {

	static TickType_t last_wake_time;
	static const TickType_t period = pdMS_TO_TICKS(STORAGE_HEART_BEAT);

	storage_init();

	last_wake_time = xTaskGetTickCount();




	for(;;) {
		//TIMING TEST
		HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
		if(restart_required) {
			write_header(1);
			data_counter = 0;
			restart_required = 0;
		}
		if(record_should_stop) {
			record_should_stop -= STORAGE_HEART_BEAT;
			if(record_should_stop<=0){
				record_active=0;
				record_should_stop=0;
			}
		}
		if(record_active && sensor_new_data_storage()) {
			storage_record_sample();
		}
		vTaskDelayUntil( &last_wake_time, period );
	}
}



/* END */


