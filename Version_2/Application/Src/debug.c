/*  Title		: Debug
 *  Filename	: debug.c
 *	Author		: iacopo sprenger
 *	Date		: 20.01.2021
 *	Version		: 0.1
 *	Description	: debug interface code
 *	What needs to be available:
 *	Access the internal state
 *	Trigger ignition and flight sequence
 *	Trigger calibration
 *	Read sensor data
 *	Read stored data
 *	Configure ignition sequence parameters
 */

/**********************
 *	INCLUDES
 **********************/

#include <debug.h>


/**********************
 *	CONSTANTS
 **********************/


/**********************
 *	MACROS
 **********************/


/**********************
 *	TYPEDEFS
 **********************/


/**********************
 *	VARIABLES
 **********************/


/**********************
 *	PROTOTYPES
 **********************/

static void debug_action(DEBUG_INST_t * debug);

/**********************
 *	DECLARATIONS
 **********************/

SERIAL_RET_t debug_decode_fcn(void * inst, uint8_t data) {
	MSV2_ERROR_t tmp = msv2_decode_fragment((MSV2_INST_t *) inst, data);

	return tmp;
}

void debug_init(DEBUG_INST_t * debug) {
	//??
}

/* END */


