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

/**********************
 *	CONFIGURATION
 **********************/


/**********************
 *	CONSTANTS
 **********************/

#define SERVO_MAX_INST	16


/**********************
 *	MACROS
 **********************/


/**********************
 *	TYPEDEFS
 **********************/


/**********************
 *	VARIABLES
 **********************/

static SERVO_INST_t * servos[SERVO_MAX_INST];
static uint8_t servo_count = 0;


/**********************
 *	PROTOTYPES
 **********************/



/**********************
 *	DECLARATIONS
 **********************/


void servo_init(SERVO_INST_t * servo, uint8_t dev_id){
	static uint32_t id_counter = 0;
	servo->id = id_counter++;

	dsv2_init(&servo->dsv2);

	servo->dev_id = dev_id;
	servos[servo_count++] = &servo;
}

 void servo_global_init(void) {
	 serial_init(&servo->ser, &DYNAMIXEL_UART, servo, servo_decode_fcn);

 }

 SERIAL_RET_t servo_decode_fcn(void * inst, uint8_t data) {
 	SERVO_INST_t * servo = (SERVO_INST_t * ) inst;
 	DSV2_ERROR_t tmp = dsv2_decode_fragment(&servo->dsv2, data);
 	//this should release the semaphore corresponding the the right epos board if bridged
 	if(tmp == DSV2_SUCCESS || tmp == DSV2_WRONG_CRC) {
 		 // one frame has been received!
 	}
 	return tmp;
 }




/* END */


