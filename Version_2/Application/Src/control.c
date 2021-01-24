/*  Title		: Control
 *  Filename	: control.c
 *	Author		: iacopo sprenger
 *	Date		: 20.01.2021
 *	Version		: 0.1
 *	Description	: main program control
 */

/**********************
 *	INCLUDES
 **********************/

#include <control.h>
#include <main.h>

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

static void init_control(CONTROL_t * control);
static void update_control(CONTROL_t * control);

// Enter state functions
static void init_idle(CONTROL_t * control);
static void init_calibration(CONTROL_t * control);
static void init_armed(CONTROL_t * control);
static void init_countdown(CONTROL_t * control);
static void init_ignintion(CONTROL_t * control);
static void init_thrust(CONTROL_t * control);
static void init_shutdown(CONTROL_t * control);
static void init_abort(CONTROL_t * control);
static void init_error(CONTROL_t * control);

// Main state functions
static void idle(CONTROL_t * control);
static void calibration(CONTROL_t * control);
static void armed(CONTROL_t * control);
static void countdown(CONTROL_t * control);
static void ignintion(CONTROL_t * control);
static void thrust(CONTROL_t * control);
static void shutdown(CONTROL_t * control);
static void abort(CONTROL_t * control);
static void error(CONTROL_t * control);

/**********************
 *	DECLARATIONS
 **********************/

void control_mainloop(void) {
	static CONTROL_t control;

	init_control(&control);

	for(;;) {

		update_control(&control);

		switch(control.state) {
		case CS_IDLE:
			idle(&control);
			break;
		case CS_CALIBRATION:
			calibration(&control);
			break;
		default:
			control.state = CS_ERROR;
			break;
		}
	}
}

static void init_control(CONTROL_t * control) {
	control->state = CS_IDLE;
}

static void update_control(CONTROL_t * control) {
	control->time = HAL_GetTick();
}

static void idle(CONTROL_t * control) {

}
static void calibration(CONTROL_t * control) {

}
static void armed(CONTROL_t * control) {

}
static void countdown(CONTROL_t * control) {

}
static void ignintion(CONTROL_t * control) {

}
static void thrust(CONTROL_t * control) {

}
static void shutdown(CONTROL_t * control) {

}
static void abort(CONTROL_t * control) {

}
static void error(CONTROL_t * control) {

}

/* END */


