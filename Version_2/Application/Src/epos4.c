/*  Title		: Maxon EPOS4 driver
 *  Filename	: epos4.c
 *	Author		: iacopo sprenger
 *	Date		: 25.01.2021
 *	Version		: 0.1
 *	Description	: maxon epos4 driver all the epos4 boards must be attached to the same serial port.
 *				  Access to multiple boards can only be done by gateway.
 */

/**********************
 *	INCLUDES
 **********************/

#include <epos4.h>
#include <msv2.h>
#include <cmsis_os.h>
#include <usart.h>

/**********************
 *	CONFIGURATION
 **********************/

#define UART_TX	HAL_UART_Transmit
#define EPOS4_UART	huart6

#define UART_RX


/**********************
 *	CONSTANTS
 **********************/

//CONTROL SEQ PARAMS
#define DLE	0x90
#define STX	0x02
#define READ_OBJECT	0x60
#define READ_OBJECT_LEN 2
#define WRITE_OBJECT 0x68
#define WRITE_OBJECT_LEN 4
#define NODE_ID 0x01
#define DATA_SIZE 4

#define COMM_TIMEOUT pdMS_TO_TICKS(1000)
#define LONG_TIME 0xffff

#define MAX_FRAME_LEN	64

//#define MOTOR_UART	huart6

//MOTOR_SPECIFIC SETTINGS
#define MAX_POS			0	//checked
#define MIN_POS			0 	//checked
#define MAX_PROFILE_VEL	8000 //check

#define QUICK_STOP_DEC	50000 //check
#define MAX_ACC			100000 //check

//motor csts
#define MOTOR_TYPE	10			//sin com bldc motor

#define MOTOR_MAX_SPEED	10000	//check
#define MOTOR_NOM_CUR	7320	//checked
#define MOTOR_MAX_CURRENT	10000	//checked
#define MOTOR_THERMAL	327 //checked
#define MOTOR_TORQUE    12419 //checked
#define EL_RESISTANCE	139 //checked
#define EL_INDUCTANCE	93	//checked

#define GEAR_MAX_SPEED	8000 //check
#define GEAR_NUM		66  //check
#define GEAR_DEN		1 	//check

#define NUM_POLE_PAIRS	4 //checked

#define ENC_NB_PULSES	1024 //check
#define ENC_TYPE		0x0001 //check

#define HALL_TYPE		0x0000 //check
#define HALL_PATTERN	0x0005 //check

//controllers
#define CURRENT_P		140106 //check
#define CURRENT_I		203253 //check

#define POSITION_P		1703746 //check
#define POSITION_I		8397288 //check
#define POSITION_D		25239 //check
#define POSITION_FFV	3568 //check
#define POSITION_FFA	162 //check


/**********************
 *	MACROS
 **********************/


/**********************
 *	TYPEDEFS
 **********************/



struct EPOS4_INST{
	uint32_t id;
	uint8_t can_id; //CAN ID for communication and gateway to other boards
	EPOS4_STATE_t state;
	MSV2_INST_t * msv2;
};


/**********************
 *	VARIABLES
 **********************/

//semaphore (allows )
static SemaphoreHandle_t epos4_busy_sem = NULL;
static StaticSemaphore_t epos4_busy_sem_buffer;

static SemaphoreHandle_t epos4_rx_sem = NULL;
static StaticSemaphore_t epos4_rx_sem_buffer;




/**********************
 *	PROTOTYPES
 **********************/

static inline void store_uint8(uint8_t value, uint8_t * data);
static inline void store_uint16(uint16_t value, uint8_t * data);
static inline void store_uint32(uint32_t value, uint8_t * data);
static inline void store_int8(int8_t value, uint8_t * data);
static inline void store_int16(int16_t value, uint8_t * data);
static inline void store_int32(int32_t value, uint8_t * data);

//What is needed:
/*
 * EPOS4 state machine interaction:
 * 	startup, enable, recover from fault, change operating mode, etc...
 * 	Send motion orders absolute, relative
 * 	Perform homing
 *
 */

/**********************
 *	DECLARATIONS
 **********************/
static inline void store_uint8(uint8_t value, uint8_t * data) {
	data[0] = value;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
}
static inline void store_uint16(uint16_t value, uint8_t * data) {
	data[0] = value;
	data[1] = value>>8;
	data[2] = 0x00;
	data[3] = 0x00;
}
static inline void store_uint32(uint32_t value, uint8_t * data) {
	data[0] = value;
	data[1] = value>>8;
	data[2] = value>>16;
	data[3] = value>>24;
}
static inline void store_int8(int8_t value, uint8_t * data) {
	data[0] = value;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
}
static inline void store_int16(int16_t value, uint8_t * data) {
	data[0] = value;
	data[1] = value>>8;
	data[2] = 0x00;
	data[3] = 0x00;
}
static inline void store_int32(int32_t value, uint8_t * data) {
	data[0] = value;
	data[1] = value>>8;
	data[2] = value>>16;
	data[3] = value>>24;
}


void epos4_global_init() {
	//create global epos4 mutex to avoid simultaneous access.
	epos4_busy_sem = xSemaphoreCreateMutexStatic(&epos4_busy_sem_buffer);

	//create rx mutex
	epos4_rx_sem = xSemaphoreCreateMutexStatic(&epos4_rx_sem_buffer);
}

void epos4_init(EPOS4_INST_t * epos4, uint8_t id) {

}



EPOS4_ERROR_t epos4_readobject(EPOS4_INST_t * epos4, uint16_t index, uint8_t subindex, uint8_t * data, uint32_t * err) {
	if (xSemaphoreTake(epos4_busy_sem, COMM_TIMEOUT) == pdTRUE) { //only one transmission at the same time
		static uint8_t send_data[READ_OBJECT_LEN*2];
		static uint16_t length = 0;
		send_data[0] = epos4->id; //node ID
		send_data[1] = index & 0xff;
		send_data[2] = index >> 8;
		send_data[3] = subindex;
		for(uint8_t i = 0; i < DATA_SIZE; i++){
			send_data[4+i] = data[i];
		}
		length = msv2_create_frame(epos4->msv2, READ_OBJECT, READ_OBJECT_LEN, send_data);
		UART_TX(&EPOS4_UART, msv2_tx_data(epos4->msv2), length, 500);
		if(xSemaphoreTake(epos4_rx_sem, COMM_TIMEOUT) == pdTRUE) {
			uint8_t * recieved_data = msv2_rx_data(epos4->msv2);
			*err = recieved_data[0] | (recieved_data[1]<<8) | (recieved_data[2]<<16) | (recieved_data[3]<<24);
			for(uint8_t i = 0; i < DATA_SIZE; i++){
				data[i] = recieved_data[4+i];
			}
			xSemaphoreGive(epos4_busy_sem); //release the sem to allow another transmission
			if(*err == 0) {
				return EPOS4_SUCCESS;
			} else {
				return EPOS4_REMOTE_ERROR;
			}
		} else {
			xSemaphoreGive(epos4_busy_sem); //release the sem to allow another transmission
			return EPOS4_TIMEOUT;
		}
	} else {
		return EPOS4_ERROR;
	}
}

EPOS4_ERROR_t epos4_writeobject(EPOS4_INST_t * epos4, uint16_t index, uint8_t subindex, uint8_t * data, uint32_t * err) {
	if (xSemaphoreTake(epos4_busy_sem, COMM_TIMEOUT) == pdTRUE) { //only one transmission at the same time
		static uint8_t send_data[WRITE_OBJECT_LEN*2];
		static uint16_t length = 0;
		send_data[0] = epos4->id; //node ID
		send_data[1] = index & 0xff;
		send_data[2] = index >> 8;
		send_data[3] = subindex;
		for(uint8_t i = 0; i < DATA_SIZE; i++){
			send_data[4+i] = data[i];
		}
		length = msv2_create_frame(epos4->msv2, WRITE_OBJECT, WRITE_OBJECT_LEN, send_data);
		UART_TX(&EPOS4_UART, msv2_tx_data(epos4->msv2), length, 500);
		if(xSemaphoreTake(epos4_rx_sem, COMM_TIMEOUT) == pdTRUE) {
			uint8_t * recieved_data = msv2_rx_data(epos4->msv2);
			*err = recieved_data[0] | (recieved_data[1]<<8) | (recieved_data[2]<<16) | (recieved_data[3]<<24);
			xSemaphoreGive(epos4_busy_sem); //release the sem to allow another transmission
			if(*err == 0) {
				return EPOS4_SUCCESS;
			} else {
				return EPOS4_REMOTE_ERROR;
			}
		} else {
			xSemaphoreGive(epos4_busy_sem); //release the sem to allow another transmission
			return EPOS4_TIMEOUT;
		}
	} else {
		return EPOS4_ERROR;
	}
}



/* END */


