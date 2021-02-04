/*  Title       : Maxon EPOS4 driver
 *  Filename    : epos4.h
 *  Author      : iacopo sprenger
 *  Date        : 25.01.2021
 *  Version     : 0.1
 *  Description : Maxon epos 4 board driver
 *  Careful Only one board supported for now
 *  Bridged boards support comming soon
 *  Multiboard support comming soon aswell :)
 */

#ifndef EPOS4_H
#define EPOS4_H

/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>
#include <serial.h>
#include <msv2.h>

/**********************
 *  CONSTANTS
 **********************/



/**********************
 *  MACROS
 **********************/


/**********************
 *  TYPEDEFS
 **********************/

typedef enum EPOS4_ERROR {
	EPOS4_SUCCESS = 0,
	EPOS4_TIMEOUT = 0b001,
	EPOS4_REMOTE_ERROR = 0b010,
	EPOS4_ERROR
}EPOS4_ERROR_t;

typedef enum EPOS4_MOV {
	EPOS4_ABSOLUTE = 0x00,
	EPOS4_ABSOLUTE_IMMEDIATE = 0x01,
	EPOS4_RELATIVE  = 0x02,
	EPOS4_RELATIVE_IMMEDIATE = 0x03
}EPOS4_MOV_t;

typedef struct EPOS4_INST{
	uint32_t id;
	uint8_t can_id; //CAN ID for communication and gateway to other boards
	MSV2_INST_t * msv2;
	SERIAL_INST_t * ser;
	//semaphore for sync
	//main board in case of bridged

}EPOS4_INST_t;

typedef struct EPOS4_PPM_CONFIG {
	uint32_t profile_velocity;
	uint32_t profile_acceleration;
	uint32_t profile_deceleration;
}EPOS4_PPM_CONFIG_t;

typedef struct EPOS4_CSP_CONFIG {

}EPOS4_CSP_CONFIG_t;

typedef struct EPOS4_HOM_CONFIG {

}EPOS4_HOM_CONFIG_t;


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif

//MISC
void epos4_global_init();
void epos4_init(EPOS4_INST_t * epos4, uint8_t id);
void epos4_init_bridged(EPOS4_INST_t * epos4, EPOS4_INST_t * bridge, uint8_t id);

SERIAL_RET_t epos4_decode_fcn(void * inst, uint8_t data);

//LOW LEVEL
EPOS4_ERROR_t epos4_readobject(EPOS4_INST_t * epos4, uint16_t index, uint8_t subindex, uint8_t * data, uint32_t * err);
EPOS4_ERROR_t epos4_writeobject(EPOS4_INST_t * epos4, uint16_t index, uint8_t subindex, uint8_t * data, uint32_t * err);

EPOS4_ERROR_t epos4_write_u8(EPOS4_INST_t * epos4, uint16_t index, uint8_t subindex, uint8_t data, uint32_t * err);
EPOS4_ERROR_t epos4_write_u16(EPOS4_INST_t * epos4, uint16_t index, uint8_t subindex, uint16_t data, uint32_t * err);
EPOS4_ERROR_t epos4_write_u32(EPOS4_INST_t * epos4, uint16_t index, uint8_t subindex, uint32_t data, uint32_t * err);
EPOS4_ERROR_t epos4_write_i8(EPOS4_INST_t * epos4, uint16_t index, uint8_t subindex, int8_t data, uint32_t * err);
EPOS4_ERROR_t epos4_write_i16(EPOS4_INST_t * epos4, uint16_t index, uint8_t subindex, int16_t data, uint32_t * err);
EPOS4_ERROR_t epos4_write_i32(EPOS4_INST_t * epos4, uint16_t index, uint8_t subindex, int32_t data, uint32_t * err);

//MEDIUM LEVEL


// not neccessarly useful...
#define epos4_write_controlword(epos4, data, err)	epos4_write_u16(epos4, EPOS4_CONTROL_WORD, data, err)
#define epos4_read_statusword(epos4, data, err)		epos4_read_u16(epos4, EPOS4_STATUS_WORD, data, err)

#define epos4_control_shutdown(epos4, err)			epos4_write_controlword(epos4, EPOS4_CW_SHUTDOWN, err)
#define epos4_control_soenable(epos4, err)			epos4_write_controlword(epos4, EPOS4_CW_SOENABLE, err)
#define epos4_control_disable(epos4, err)			epos4_write_controlword(epos4, EPOS4_CW_DISABLE, err)

#define epos4_control_ppm_start_abs(epos4, err)		epos4_write_controlword(epos4, EPOS4_CW_PPM_ABSOLUTE, err)
#define epos4_control_ppm_start_abs_imm(epos4, err)	epos4_write_controlword(epos4, EPOS4_CW_PPM_ABSOLUTE_I, err)
#define epos4_control_ppm_start_rel(epos4, err)		epos4_write_controlword(epos4, EPOS4_CW_PPM_RELATIVE, err)
#define epos4_control_ppm_start_rel_imm(epos4, err)	epos4_write_controlword(epos4, EPOS4_CW_PPM_RELATIVE_I, err)




//HIGH LEVEL

EPOS4_ERROR_t epos4_config(EPOS4_INST_t * epos4);


EPOS4_ERROR_t epos4_ppm_prep(EPOS4_INST_t * epos4);
EPOS4_ERROR_t epos4_ppm_move(EPOS4_INST_t * epos4, EPOS4_MOV_t type, int32_t target);
EPOS4_ERROR_t epos4_ppm_config(EPOS4_INST_t * epos4, EPOS4_PPM_CONFIG_t config);

EPOS4_ERROR_t epos4_csp_move(EPOS4_INST_t * epos4, int32_t target);
EPOS4_ERROR_t epos4_csp_config(EPOS4_INST_t * epos4, EPOS4_CSP_CONFIG_t config);

EPOS4_ERROR_t epos4_hom_move(EPOS4_INST_t * epos4);
EPOS4_ERROR_t epos4_hom_config(EPOS4_INST_t * epos4, EPOS4_HOM_CONFIG_t config);


#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* EPOS4_H */

/* END */
