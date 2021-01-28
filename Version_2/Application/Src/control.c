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



/**********************
 *	PROTOTYPES
 **********************/

static void control_init(CONTROL_t * control);
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
static void emergency(CONTROL_t * control);
static void error(CONTROL_t * control);

/**********************
 *	DECLARATIONS
 **********************/

void control_thread(void * arg) {
	static CONTROL_t control;
	static TickType_t last_wake_time;
	static const TickType_t period = pdMS_TO_TICKS(CONTROL_HEART_BEAT);

	last_wake_time = xTaskGetTickCount();

	control_init(&control);

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
			emergency(&control);
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

static void control_init(CONTROL_t * control) {
	control->state = CS_IDLE;
}

static void control_update(CONTROL_t * control) {
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
static void ignition(CONTROL_t * control) {

}
static void thrust(CONTROL_t * control) {

}
static void shutdown(CONTROL_t * control) {

}
static void glide(CONTROL_t * control) {

}
static void emergency(CONTROL_t * control) {

}
static void error(CONTROL_t * control) {

}

/* END */


