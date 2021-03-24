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
#include <servo_def.h>
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
	uint16_t psu_voltage;
	int8_t temperature;
	int32_t position;
	uint8_t error;

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

SERVO_ERROR_t servo_read(SERVO_INST_t * servo, uint16_t address, uint16_t length, uint8_t * data, uint8_t * err);

SERVO_ERROR_t servo_write(SERVO_INST_t * servo, uint16_t address, uint16_t length, uint8_t * data, uint8_t * err);

SERVO_ERROR_t servo_ping(uint8_t id);

SERVO_ERROR_t servo_write_u8 (SERVO_INST_t * servo, uint16_t address, uint8_t  data, uint8_t * err);
SERVO_ERROR_t servo_write_u16(SERVO_INST_t * servo, uint16_t address, uint16_t data, uint8_t * err);
SERVO_ERROR_t servo_write_u32(SERVO_INST_t * servo, uint16_t address, uint32_t data, uint8_t * err);
SERVO_ERROR_t servo_write_i8 (SERVO_INST_t * servo, uint16_t address, int8_t   data, uint8_t * err);
SERVO_ERROR_t servo_write_i16(SERVO_INST_t * servo, uint16_t address, int16_t  data, uint8_t * err);
SERVO_ERROR_t servo_write_i32(SERVO_INST_t * servo, uint16_t address, int32_t  data, uint8_t * err);

SERVO_ERROR_t servo_read_u8 (SERVO_INST_t * servo, uint16_t address, uint8_t *  data, uint8_t * err);
SERVO_ERROR_t servo_read_u16(SERVO_INST_t * servo, uint16_t address, uint16_t * data, uint8_t * err);
SERVO_ERROR_t servo_read_u32(SERVO_INST_t * servo, uint16_t address, uint32_t * data, uint8_t * err);
SERVO_ERROR_t servo_read_i8 (SERVO_INST_t * servo, uint16_t address, int8_t *   data, uint8_t * err);
SERVO_ERROR_t servo_read_i16(SERVO_INST_t * servo, uint16_t address, int16_t *  data, uint8_t * err);
SERVO_ERROR_t servo_read_i32(SERVO_INST_t * servo, uint16_t address, int32_t *  data, uint8_t * err);



#define servo_enable_torque(servo, err)		servo_write_u8(servo, SERVO_TORQUE_ENABLE, 1, err)
#define servo_disable_torque(servo, err)	servo_write_u8(servo, SERVO_TORQUE_ENABLE, 0, err)

#define servo_enable_led(servo, err)		servo_write_u8(servo, SERVO_LED, 1, err)
#define servo_disable_led(servo, err)		servo_write_u8(servo, SERVO_LED, 0, err)

SERVO_ERROR_t servo_sync(SERVO_INST_t * servo);

SERVO_ERROR_t servo_config(SERVO_INST_t * servo);

SERVO_ERROR_t servo_move(SERVO_INST_t * servo, int32_t target);



#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* SERVO_H */

/* END */
