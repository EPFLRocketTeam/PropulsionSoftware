/*  Title		: Dynamixel servo control
 *  Filename	: servo.c
 *	Author		: iacopo sprenger
 *	Date		: 22.03.2021
 *	Version		: 0.1
 *	Description	: Dynamixel servo control driver
 */

/**********************
 *	INCLUDES
 **********************/

#include <servo.h>
#include <cmsis_os.h>
#include <servo_def.h>

/**********************
 *	CONFIGURATION
 **********************/


/**********************
 *	CONSTANTS
 **********************/

#define SERVO_MAX_INST	16


#define COMM_TIMEOUT pdMS_TO_TICKS(10)
#define DRIV_TIMEOUT pdMS_TO_TICKS(200)
#define LONG_TIME 0xffff









#define DATA_SIZE 4

/**********************
 *	MACROS
 **********************/


/**********************
 *	TYPEDEFS
 **********************/


/**********************
 *	VARIABLES
 **********************/

static SERVO_INST_t * servo_list[SERVO_MAX_INST];
static uint8_t servo_count = 0;

static SERIAL_INST_t servo_serial;
static DSV2_INST_t servo_dsv2;

static SemaphoreHandle_t servo_busy_sem = NULL;
static StaticSemaphore_t servo_busy_sem_buffer;




/**********************
 *	PROTOTYPES
 **********************/



/**********************
 *	DECLARATIONS
 **********************/


void servo_init(SERVO_INST_t * servo, uint8_t dev_id) {
	static uint32_t id_counter = 0;
	servo->id = id_counter++;

	servo->dev_id = dev_id;
	servo_list[servo_count++] = servo;

	servo->rx_sem = xSemaphoreCreateBinaryStatic(&servo->rx_sem_buffer);
}

void servo_global_init(void) {
	dsv2_init(&servo_dsv2);
	serial_init(&servo_serial, &DYNAMIXEL_UART, &servo_dsv2, servo_decode_fcn);
	servo_busy_sem = xSemaphoreCreateMutexStatic(&servo_busy_sem_buffer);
}

SERIAL_RET_t servo_decode_fcn(void * inst, uint8_t data) {
	DSV2_INST_t * dsv2 = (DSV2_INST_t * ) inst;
	DSV2_ERROR_t tmp = dsv2_decode_fragment(dsv2, data);
	//this should release the semaphore corresponding the the right epos board if bridged
	if(tmp == DSV2_SUCCESS || tmp == DSV2_WRONG_CRC) {
		if(dsv2->rx.inst == 0x55) { // ONLY HANDLE STATUS PACKETS
			for(uint8_t i = 0; i < servo_count; i++) {
				if(servo_list[i]->dev_id == dsv2->rx.dev_id) {
					xSemaphoreGive(servo_list[i]->rx_sem);
					break;
				}
			}
		}
	}
	return tmp;
}



/*
 * address of the object
 * length to read
 * data to read into
 */
SERVO_ERROR_t servo_read(SERVO_INST_t * servo, uint16_t address, uint16_t length, uint8_t * data, uint8_t * err) {
	if (xSemaphoreTake(servo_busy_sem, DRIV_TIMEOUT) == pdTRUE) {
		static uint8_t send_data[MAX_READ_LEN];
		send_data[0] = address & 0xff;
		send_data[1] = address>>8;
		send_data[2] = length & 0xff;
		send_data[3] = length>>8;
		uint16_t len = dsv2_create_frame(&servo_dsv2, servo->dev_id, MAX_READ_LEN, READ_INST, send_data);
		serial_send(&servo_serial, dsv2_tx_data(&servo_dsv2), len);
		if(xSemaphoreTake(servo->rx_sem, COMM_TIMEOUT) == pdTRUE) {
			uint8_t * recieved_data = dsv2_rx_data(&servo_dsv2);
			if(err != NULL) {
				*err = recieved_data[0];
			}
			for(uint8_t i = 0; i < length; i++){
				data[i] = recieved_data[i+1];
			}
			if(*err == 0) {
				xSemaphoreGive(servo_busy_sem);
				return SERVO_SUCCESS;
			} else {
				xSemaphoreGive(servo_busy_sem);
				return SERVO_REMOTE_ERROR;
			}
		} else {
			xSemaphoreGive(servo_busy_sem);
			return SERVO_TIMEOUT;
		}
	} else {
		return SERVO_BUSY;
	}
}

/*
 * address of the object
 * length of the data to write
 * data to write
 */
SERVO_ERROR_t servo_write(SERVO_INST_t * servo, uint16_t address, uint16_t length, uint8_t * data, uint8_t * err) {
	if (xSemaphoreTake(servo_busy_sem, DRIV_TIMEOUT) == pdTRUE) {
		static uint8_t send_data[MAX_WRITE_LEN];
		send_data[0] = address & 0xff;
		send_data[1] = address>>8;
		for(uint16_t i = 0; i < length; i++) {
			send_data[2 + i] = data[i];
		}
		uint16_t len = dsv2_create_frame(&servo_dsv2, servo->dev_id, length+2, WRITE_INST, send_data);
		serial_send(&servo_serial, dsv2_tx_data(&servo_dsv2), len);
		if(xSemaphoreTake(servo->rx_sem, COMM_TIMEOUT) == pdTRUE) {
			uint8_t * recieved_data = dsv2_rx_data(&servo_dsv2);
			if(err != NULL) {
				*err = recieved_data[0];
			}
			if(*err == 0) {
				xSemaphoreGive(servo_busy_sem);
				return SERVO_SUCCESS;
			} else {
				xSemaphoreGive(servo_busy_sem);
				return SERVO_REMOTE_ERROR;
			}
		} else {
			xSemaphoreGive(servo_busy_sem);
			return SERVO_TIMEOUT;
		}
	} else {
		return SERVO_BUSY;
	}
}

/*
 * USELESS FOR NOW
 */
SERVO_ERROR_t servo_ping(uint8_t id) {
	if (xSemaphoreTake(servo_busy_sem, DRIV_TIMEOUT) == pdTRUE) {
		uint16_t len = dsv2_create_frame(&servo_dsv2, id, 0, PING_INST, NULL);
		serial_send(&servo_serial, dsv2_tx_data(&servo_dsv2), len);
		xSemaphoreGive(servo_busy_sem);
		return SERVO_SUCCESS;
	} else {
		return SERVO_BUSY;
	}
}


SERVO_ERROR_t servo_write_u8(SERVO_INST_t * servo, uint16_t address, uint8_t data, uint8_t * err) {
	uint8_t bin_data[DATA_SIZE];
	util_encode_u8(bin_data, data);
	return servo_write(servo, address, 1, bin_data, err);
}

SERVO_ERROR_t servo_write_u16(SERVO_INST_t * servo, uint16_t address, uint16_t data, uint8_t * err) {
	uint8_t bin_data[DATA_SIZE];
	util_encode_u16(bin_data, data);
	return servo_write(servo, address, 2, bin_data, err);
}

SERVO_ERROR_t servo_write_u32(SERVO_INST_t * servo, uint16_t address, uint32_t data, uint8_t * err) {
	uint8_t bin_data[DATA_SIZE];
	util_encode_u32(bin_data, data);
	return servo_write(servo, address, 4, bin_data, err);
}

SERVO_ERROR_t servo_write_i8(SERVO_INST_t * servo, uint16_t address, int8_t data, uint8_t * err) {
	uint8_t bin_data[DATA_SIZE];
	util_encode_i8(bin_data, data);
	return servo_write(servo, address, 1, bin_data, err);
}

SERVO_ERROR_t servo_write_i16(SERVO_INST_t * servo, uint16_t address, int16_t data, uint8_t * err) {
	uint8_t bin_data[DATA_SIZE];
	util_encode_i16(bin_data, data);
	return servo_write(servo, address, 2, bin_data, err);
}

SERVO_ERROR_t servo_write_i32(SERVO_INST_t * servo, uint16_t address, int32_t data, uint8_t * err) {
	uint8_t bin_data[DATA_SIZE];
	util_encode_i32(bin_data, data);
	return servo_write(servo, address, 4, bin_data, err);
}

SERVO_ERROR_t servo_read_u8(SERVO_INST_t * servo, uint16_t address, uint8_t * data, uint8_t * err) {
	uint8_t bin_data[DATA_SIZE];
	SERVO_ERROR_t tmp = servo_read(servo, address, 1, bin_data, err);
	*data = util_decode_u8(bin_data);
	return tmp;
}

SERVO_ERROR_t servo_read_u16(SERVO_INST_t * servo, uint16_t address, uint16_t * data, uint8_t * err) {
	uint8_t bin_data[DATA_SIZE];
	SERVO_ERROR_t tmp = servo_read(servo, address, 2, bin_data, err);
	*data = util_decode_u16(bin_data);
	return tmp;
}

SERVO_ERROR_t servo_read_u32(SERVO_INST_t * servo, uint16_t address, uint32_t * data, uint8_t * err) {
	uint8_t bin_data[DATA_SIZE];
	SERVO_ERROR_t tmp = servo_read(servo, address, 4, bin_data, err);
	*data = util_decode_u32(bin_data);
	return tmp;
}

SERVO_ERROR_t servo_read_i8(SERVO_INST_t * servo, uint16_t address, int8_t * data, uint8_t * err) {
	uint8_t bin_data[DATA_SIZE];
	SERVO_ERROR_t tmp = servo_read(servo, address, 1, bin_data, err);
	*data = util_decode_i8(bin_data);
	return tmp;
}

SERVO_ERROR_t servo_read_i16(SERVO_INST_t * servo, uint16_t address, int16_t * data, uint8_t * err) {
	uint8_t bin_data[DATA_SIZE];
	SERVO_ERROR_t tmp = servo_read(servo, address, 2, bin_data, err);
	*data = util_decode_i16(bin_data);
	return tmp;
}

SERVO_ERROR_t servo_read_i32(SERVO_INST_t * servo, uint16_t address, int32_t * data, uint8_t * err) {
	uint8_t bin_data[DATA_SIZE];
	SERVO_ERROR_t tmp = servo_read(servo, address, 4, bin_data, err);
	*data = util_decode_i32(bin_data);
	return tmp;
}



//HIGH LEVEL FUNCTIONS

SERVO_ERROR_t servo_sync(SERVO_INST_t * servo) {
	uint8_t err;
	SERVO_ERROR_t error = 0;

	error |= servo_read_u8(servo, SERVO_HARDWARE_ERROR_STATUS, &servo->error, &err);

	error |= servo_read_u16(servo, SERVO_PRESENT_INPUT_VOLTAGE, &servo->psu_voltage, &err);

	error |= servo_read_i8(servo, SERVO_PRESENT_TEMPERATURE, &servo->temperature, &err);

	error |= servo_read_i32(servo, SERVO_PRESENT_POSITION, &servo->position, &err);


	return error;
}

SERVO_ERROR_t servo_config(SERVO_INST_t * servo) {
	uint8_t err;
	SERVO_ERROR_t error = 0;

	error |= servo_disable_torque(servo, &err);

	error |= servo_write_i32(servo, SERVO_MAX_POSITION_LIMIT, 4095, &err);

	error |= servo_write_i32(servo, SERVO_MIN_POSITION_LIMIT, 0, &err);

	error |= servo_write_u8(servo, SERVO_OPERATING_MODE, 3, &err);

	return error;
}

SERVO_ERROR_t servo_move(SERVO_INST_t * servo, int32_t target) {
	uint8_t err;
	SERVO_ERROR_t error = 0;

	error |= servo_enable_torque(servo, &err);

	error |= servo_write_i32(servo, SERVO_GOAL_POSITION, target, &err);

	return error;
}




/* END */


