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
#include <main.h>
#include <cmsis_os.h>
#include <control.h>

/**********************
 *	CONFIGURATION
 **********************/

#define CONTROL_HEART_BEAT	20 /* ms */

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

static CONTROL_t control;


/**********************
 *	PROTOTYPES
 **********************/

static void control_update(CONTROL_t * control);

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
static void ignition(CONTROL_t * control);
static void thrust(CONTROL_t * control);
static void shutdown(CONTROL_t * control);
static void glide(CONTROL_t * control);
static void _abort(CONTROL_t * control);
static void error(CONTROL_t * control);

/**********************
 *	DECLARATIONS
 **********************/

void control_thread(void * arg) {

	static TickType_t last_wake_time;
	static const TickType_t period = pdMS_TO_TICKS(CONTROL_HEART_BEAT);

	last_wake_time = xTaskGetTickCount();

	init_idle(&control);

	for(;;) {

		control_update(&control);

		switch(control.state) {
		case CS_IDLE:
			idle(&control);
			break;
		case CS_CALIBRATION:
			calibration(&control);
			break;
		case CS_ARMED:
			armed(&control);
			break;
		case CS_COUNTDOWN:
			countdown(&control);
			break;
		case CS_IGNITION:
			ignition(&control);
			break;
		case CS_THRUST:
			thrust(&control);
			break;
		case CS_SHUTDOWN:
			shutdown(&control);
			break;
		case CS_GLIDE:
			glide(&control);
			break;
		case CS_ABORT:
			_abort(&control);
			break;
		case CS_ERROR:
			error(&control);
			break;
		default:
			control.state = CS_ERROR;
			break;
		}

		vTaskDelayUntil( &last_wake_time, period );
	}
}

CONTROL_STATE_t control_get_state() {
	return control.state;
}


static void control_update(CONTROL_t * control) {
	control->time = HAL_GetTick();
	control->iter++;

	//read motors parameters
	//init error if there is an issue with a motor
}

static void init_idle(CONTROL_t * control) {
	control->state = CS_IDLE;
}

static void idle(CONTROL_t * control) {
	//React to external commands:
	//launch calib
	//arm
	//motor movements for manual homing

	//if recv calibration command -> calib init
	//if recv arm command -> arm

}

static void init_calibration(CONTROL_t * control) {
	control->state = CS_CALIBRATION;
	//send calibration command to sensors
}

static void calibration(CONTROL_t * control) {
	//Wait for the calibration ack to come from the sensors
}

static void init_armed(CONTROL_t * control) {
	control->state = CS_ARMED;
}

static void armed(CONTROL_t * control) {
	//react to external commands:
	//disarm -> back to idle
	//launch -> countdown
	//no motor movements allowed
}

static void init_countdown(CONTROL_t * control) {
	control->state = CS_COUNTDOWN;
}

static void countdown(CONTROL_t * control) {
	//Wait for the right time to ellapse
	//number of "loop iteration"
	//osDelay for the last remaining time
}

static void init_ignition(CONTROL_t * control) {
	control->state = CS_IGNITION;
}

static void ignition(CONTROL_t * control) {
	//send first motor movement command

	//wait for the half time in nb of cycles
	//and osdelay for the remaining time

	//send second movement command
	//wait for the target reached
	//init thrust

}

static void init_thrust(CONTROL_t * control) {
	control->state = CS_THRUST;
}

static void thrust(CONTROL_t * control) {
	//thrust control algorithm drives the motor
	//successive motor ppm moves with the immediate flag set!

	//detect flameout (pressure)
	//-> init shutdown
}

static void init_shutdown(CONTROL_t * control) {
	control->state = CS_SHUTDOWN;
	//start motor movement to close valve
}

static void shutdown(CONTROL_t * control) {
	//wait for the ack
}

static void init_glide(CONTROL_t * control) {
	control->state = CS_GLIDE;
}

static void glide(CONTROL_t * control) {
	//AB algorithm controls the airbrakes motor

	//expect a stop signal to go to idle
	//back to idle
}

static void init_abort(CONTROL_t * control) {
	control->state = CS_ABORT;
}

static void _abort(CONTROL_t * control) {
	//close main valve
	//close airbrakes
	//wait for user release
	//if user release -> IDLE
}

static void init_error(CONTROL_t * control) {
	control->state = CS_ERROR;
}

static void error(CONTROL_t * control) {
	//wait for user release
	//if user release -> IDLE
}

/* END */


