/*  Title       : Dynamixel servo control
 *  Filename    : servo.h
 *  Author      : iacopo sprenger
 *  Date        : 22.03.2021
 *  Version     : 0.1
 *  Description : Dynamixel servo motor control
 */

#ifndef SERVO_H
#define SERVO_H

/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>
#include <dsv2.h>

/**********************
 *  CONSTANTS
 **********************/

#define DYNAMIXEL_UART	huart1


/**********************
 *  MACROS
 **********************/


/**********************
 *  TYPEDEFS
 **********************/

typedef struct SERVO_INST SERVO_INST_t;


struct SERVO_INST{
	uint32_t id;
	DSV2_INST_t dsv2;
	SERIAL_INST_t ser;
	uint8_t dev_id;
	uint8_t first;
	SERVO_INST_t * parent;
};


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif


void servo_global_init(void);

void servo_init(SERVO_INST_t * servo, uint8_t dev_id);


SERIAL_RET_t servo_decode_fcn(void * inst, uint8_t data);



#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* SERVO_H */

/* END */
