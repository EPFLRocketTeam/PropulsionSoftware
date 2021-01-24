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
	CS_THURST,
	CS_SHUTDOWN,
	CS_ABORT,
	CS_ERROR
}CONTROL_STATE_t;


typedef struct CONTROL_INST{
	CONTROL_STATE_t state;
	uint32_t time;
}CONTROL_t;

/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif

void control_mainloop(void);

#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* CONTROL_H */

/* END */












