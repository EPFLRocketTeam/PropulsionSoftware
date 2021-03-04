/*  Title		: Thrust control
 *  Filename	: thrust_control.c
 *	Author		: iacopo sprenger
 *	Date		: 03.03.2021
 *	Version		: 0.1
 *	Description	: thrust control algorithm implementation
 *				  The algorithm was developped by Antoine Faltz and
 *				  Luca Rezzonico
 *
 *				  The algorithm will take as input the CC pressure and thrust command value
 *				  and output the desired valve opening angle
 *
 *				  controller: c = tf([1 28.6 484], [1 200 0])
 *				  discretization: d = c2d(c, 0.01)
 */

/**********************
 *	INCLUDES
 **********************/

#include "thrust_control.h"

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





/**********************
 *	DECLARATIONS
 **********************/

void tc_init() {

}

int32_t tc_compute(int32_t cc_pressure, int32_t desired_thrust, uint32_t time) {

}



/* END */


