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

#define MAX_WRITE_LEN	(64)
#define WRITE_INST		(0x03)

#define MAX_READ_LEN	(4)
#define READ_INST		(0x02)

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
		if(dsv2->rx.dev_id < servo_count) {
			xSemaphoreGive(servo_list[dsv2->rx.dev_id]->rx_sem);
		}
	}
	return tmp;
}



/*
 * address of the object
 * length to read
 * data to read into
 */
SERVO_ERROR_t servo_read_object(SERVO_INST_t * servo, uint16_t address, uint16_t length, uint8_t * data) {
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
			uint8_t err = recieved_data[0];
			for(uint8_t i = 0; i < length; i++){
				data[i] = recieved_data[i+1];
			}
			if(err == 0) {
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
SERVO_ERROR_t servo_write_object(SERVO_INST_t * servo, uint16_t address, uint16_t length, uint8_t * data) {
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
			uint8_t err = recieved_data[0];
			if(err == 0) {
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







/* END */


