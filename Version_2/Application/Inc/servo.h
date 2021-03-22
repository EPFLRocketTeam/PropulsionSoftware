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
#include <cmsis_os.h>

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


typedef enum SERVO_ERROR {
	SERVO_SUCCESS = 0,
	SERVO_TIMEOUT = 0b001,
	SERVO_REMOTE_ERROR = 0b010,
	SERVO_BUSY = 0b100,
	SERVO_ERROR
}SERVO_ERROR_t;

typedef struct SERVO_INST SERVO_INST_t;


struct SERVO_INST{
	uint32_t id;
	DSV2_INST_t dsv2;
	SERIAL_INST_t ser;
	uint8_t dev_id;
	uint8_t first;
	SERVO_INST_t * parent;
	SemaphoreHandle_t rx_sem;
	StaticSemaphore_t rx_sem_buffer;

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

SERVO_ERROR_t servo_read_object(SERVO_INST_t * servo, uint16_t address, uint16_t length, uint8_t * data);

SERVO_ERROR_t servo_write_object(SERVO_INST_t * servo, uint16_t address, uint16_t length, uint8_t * data);



#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* SERVO_H */

/* END */
