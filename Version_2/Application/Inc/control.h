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

/**********************
 *  CONSTANTS
 **********************/



/**********************
 *  MACROS
 **********************/



/**********************
 *  TYPEDEFS
 **********************/

typedef enum CONTROL_STATE{
	CS_IDLE,
	CS_CALIBRATION,
	CS_ARMED,
	CS_COUNTDOWN,
	CS_IGNITION,
	CS_THRUST,
	CS_SHUTDOWN,
	CS_GLIDE,
	CS_ABORT,
	CS_ERROR
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


typedef struct CONTROL_INST{
	CONTROL_STATE_t state;
	uint32_t time;
	uint32_t iter;
	CONTROL_PP_PARAMS_t pp_params;
}CONTROL_INST_t;



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

#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* CONTROL_H */

/* END */












