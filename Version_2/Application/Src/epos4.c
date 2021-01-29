/*  Title		: Maxon EPOS4 driver
 *  Filename	: epos4.c
 *	Author		: iacopo sprenger
 *	Date		: 25.01.2021
 *	Version		: 0.1
 *	Description	: maxon epos4 driver
 */

/**********************
 *	INCLUDES
 **********************/

#include <epos4.h>
#include <msv2.h>

/**********************
 *	CONFIGURATION
 **********************/

#define UART_SEND

#define UART_RECV


/**********************
 *	CONSTANTS
 **********************/



/**********************
 *	MACROS
 **********************/


/**********************
 *	TYPEDEFS
 **********************/



struct EPOS4_INST {
	uint8_t id;	//CAN ID for communication and gateway to other boards
	//UART device
	EPOS4_STATE_t state;


};


/**********************
 *	VARIABLES
 **********************/


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



/* END */


