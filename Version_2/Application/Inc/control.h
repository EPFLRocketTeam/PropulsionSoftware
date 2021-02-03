/*  Title       : Control
 *  Filename    : control.h
 *  Author      : iacopo sprenger
 *  Date        : 20.01.2021
 *  Version     : 0.1
 *  Description : main program control
 */

#ifndef CONTROL_H
#define CONTROL_H

/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>
#include <epos4.h>

/**********************
 *  CONSTANTS
 **********************/

#define CONTROL_SCHED_LEN sizeof(CONTROL_SCHED_t)



/**********************
 *  MACROS
 **********************/



/**********************
 *  TYPEDEFS
 **********************/

typedef enum CONTROL_STATE{
	CS_IDLE = 0x00,
	CS_CALIBRATION = 0x01,
	CS_ARMED = 0x02,
	CS_COUNTDOWN  = 0x03,
	CS_IGNITION  = 0x04,
	CS_THRUST  = 0x05,
	CS_SHUTDOWN  = 0x06,
	CS_GLIDE  = 0x07,
	CS_ABORT  = 0x08,
	CS_ERROR  = 0x09
}CONTROL_STATE_t;

typedef struct CONTROL_PP_PARAMS {
	uint32_t acc;
	uint32_t dec;
	uint32_t speed;
	uint32_t half_speed;
	uint32_t countdown_wait;
	uint32_t half_wait;
	uint32_t full_wait;
	int32_t half_angle;
	int32_t full_angle;
}CONTROL_PP_PARAMS_t;


//schedule: higher in the list -> higher priority
typedef struct CONTROL_SCHED{
	uint8_t abort;
	uint8_t move;
}CONTROL_SCHED_t;


typedef struct CONTROL_INST{
	CONTROL_STATE_t state;
	uint32_t time;
	uint32_t iter;
	CONTROL_PP_PARAMS_t pp_params;
	EPOS4_MOV_t mov_type;
	int32_t mov_target;
	union SCHED{
		CONTROL_SCHED_t sched;
		uint8_t sched_list[CONTROL_SCHED_LEN];
	};
}CONTROL_INST_t;

//action scheduling
//



/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif

void control_thread(void * arg);

CONTROL_STATE_t control_get_state();

CONTROL_PP_PARAMS_t control_get_pp_params();

void control_set_pp_params(CONTROL_PP_PARAMS_t params);

void control_move(EPOS4_MOV_t mov_type, int32_t target);


#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* CONTROL_H */

/* END */












