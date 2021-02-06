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
#include <epos4.h>
#include <led.h>

/**********************
 *	CONFIGURATION
 **********************/

#define CONTROL_HEART_BEAT	20 /* ms */

/**********************
 *	CONSTANTS
 **********************/




#define SCHED_ALLOWED_WIDTH	(5)

/**********************
 *	MACROS
 **********************/

#define DEG2INC(deg)	((int32_t)(((float)-(deg))*4*1024*66/1/360))

/**********************
 *	TYPEDEFS
 **********************/



/**********************
 *	VARIABLES
 **********************/

static CONTROL_INST_t control;

static CONTROL_SCHED_t sched_allowed[][SCHED_ALLOWED_WIDTH] = {
		{CONTROL_SCHED_ABORT, CONTROL_SCHED_MOVE, CONTROL_SCHED_CALIBRATE, CONTROL_SCHED_ARM, CONTROL_SCHED_NOTHING}, 		//IDLE
		{CONTROL_SCHED_ABORT, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING}, 	//CALIBRATION
		{CONTROL_SCHED_ABORT, CONTROL_SCHED_IGNITE, CONTROL_SCHED_DISARM, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING}, 	//ARMED
		{CONTROL_SCHED_ABORT, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING}, 	//COUNTDOWN
		{CONTROL_SCHED_ABORT, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING}, 	//IGNITION
		{CONTROL_SCHED_ABORT, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING}, 	//THRUST
		{CONTROL_SCHED_ABORT, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING}, 	//SHUTDOWN
		{CONTROL_SCHED_ABORT, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING}, 	//GLIDE
		{CONTROL_SCHED_ABORT, CONTROL_SCHED_RECOVER, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING}, 	//ABORT
		{CONTROL_SCHED_ABORT, CONTROL_SCHED_RECOVER, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING, CONTROL_SCHED_NOTHING} 	//ERROR
};


/**********************
 *	PROTOTYPES
 **********************/
static void init_control(CONTROL_INST_t * control);
static void control_update(CONTROL_INST_t * control);

// Enter state functions
static void init_idle(CONTROL_INST_t * control);
static void init_calibration(CONTROL_INST_t * control);
static void init_armed(CONTROL_INST_t * control);
static void init_countdown(CONTROL_INST_t * control);
static void init_ignition(CONTROL_INST_t * control);
static void init_thrust(CONTROL_INST_t * control);
static void init_shutdown(CONTROL_INST_t * control);
static void init_abort(CONTROL_INST_t * control);
static void init_error(CONTROL_INST_t * control);

// Main state functions
static void idle(CONTROL_INST_t * control);
static void calibration(CONTROL_INST_t * control);
static void armed(CONTROL_INST_t * control);
static void countdown(CONTROL_INST_t * control);
static void ignition(CONTROL_INST_t * control);
static void thrust(CONTROL_INST_t * control);
static void shutdown(CONTROL_INST_t * control);
static void glide(CONTROL_INST_t * control);
static void _abort(CONTROL_INST_t * control);
static void error(CONTROL_INST_t * control);

//scheduling

static uint8_t control_sched_should_run(CONTROL_INST_t * control, CONTROL_SCHED_t num);
static void control_sched_done(CONTROL_INST_t * control, CONTROL_SCHED_t num);
static void control_sched_set(CONTROL_INST_t * control, CONTROL_SCHED_t num);

/**********************
 *	DECLARATIONS
 **********************/

void control_thread(void * arg) {

	static TickType_t last_wake_time;
	static const TickType_t period = pdMS_TO_TICKS(CONTROL_HEART_BEAT);

	last_wake_time = xTaskGetTickCount();

	led_init();

	init_control(&control);


	static EPOS4_INST_t pp_epos4;
	static EPOS4_INST_t ab_epos4;

	epos4_global_init();

	epos4_init(&pp_epos4, 1);
	//epos4_init_bridged(&ab_epos4, &pp_epos4, 2);
	//Bridged func not yet ready

	control.pp_epos4 = &pp_epos4;
	control.ab_epos4 = &ab_epos4;


	for(;;) {

		control_update(&control);
		//read status
		//read motor pos
		//read error register
		//??
		//For PP and AB

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


static void control_update(CONTROL_INST_t * control) {
	control->time = HAL_GetTick();
	control->iter++;

	//read motors parameters

	//Error register
	//motor position
	//psu voltage

	//init error if there is an issue with a motor

	if(control_sched_should_run(control, CONTROL_SCHED_ABORT)) {
		init_abort(control);
		control_sched_done(control, CONTROL_SCHED_ABORT);
	}
}

static void init_control(CONTROL_INST_t * control) {
	control->sched = CONTROL_SCHED_NOTHING;


	control->pp_params.acc = 50000;
	control->pp_params.dec = 50000;
	control->pp_params.speed = 8000;
	control->pp_params.countdown_wait = 2000;
	control->pp_params.half_wait = 2500;
	control->pp_params.full_wait = 20000;
	control->pp_params.half_angle = DEG2INC(27);
	control->pp_params.full_angle = DEG2INC(90);
}

static void init_idle(CONTROL_INST_t * control) {
	control->state = CS_IDLE;
	led_set_color(LED_GREEN);
}

static void idle(CONTROL_INST_t * control) {
	//React to external commands:
	//launch calib
	//arm
	//motor movements for manual homing

	//if a move is in progress, disable torque when done

	if(control->mov_started) {
		uint8_t terminated = 0;
		epos4_ppm_terminate(control->pp_epos4, &terminated);
		if(terminated) {
			control->mov_started = 0;
			control_sched_done(control, CONTROL_SCHED_MOVE);
		}
	}

	//if a move is scheduled, perform it
	if(control_sched_should_run(control, CONTROL_SCHED_MOVE) && !control->mov_started) {
		EPOS4_PPM_CONFIG_t ppm_config;
		ppm_config.profile_acceleration = control->pp_params.acc;
		ppm_config.profile_deceleration = control->pp_params.dec;
		ppm_config.profile_velocity = control->pp_params.speed;
		epos4_ppm_config(control->pp_epos4, ppm_config);
		epos4_ppm_prep(control->pp_epos4);
		epos4_ppm_move(control->pp_epos4, control->mov_type, control->mov_target);
		control->mov_started = 1;
	}

	if(control_sched_should_run(control, CONTROL_SCHED_CALIBRATE)) {
		init_calibration(control);
		control_sched_done(control, CONTROL_SCHED_CALIBRATE);
	}

	if(control_sched_should_run(control, CONTROL_SCHED_ARM)) {
		init_armed(control);
		control_sched_done(control, CONTROL_SCHED_ARM);
	}


}

static void init_calibration(CONTROL_INST_t * control) {
	control->state = CS_CALIBRATION;
	led_set_color(LED_BLUE);
	//send calibration command to sensors
}

static void calibration(CONTROL_INST_t * control) {
	//Wait for the calibration ack to come from the sensors
}

static void init_armed(CONTROL_INST_t * control) {
	control->state = CS_ARMED;
	led_set_color(LED_TEAL);
}

static void armed(CONTROL_INST_t * control) {

	if(control_sched_should_run(control, CONTROL_SCHED_IGNITE)) {
		init_countdown(control);
		control_sched_done(control, CONTROL_SCHED_IGNITE);
	}

	if(control_sched_should_run(control, CONTROL_SCHED_DISARM)) {
		init_idle(control);
		control_sched_done(control, CONTROL_SCHED_DISARM);
	}
}

static void init_countdown(CONTROL_INST_t * control) {
	led_set_color(LED_ORANGE);
	control->state = CS_COUNTDOWN;
}

static void countdown(CONTROL_INST_t * control) {
	//Wait for the right time to ellapse
	//number of "loop iteration"
	//osDelay for the last remaining time
}

static void init_ignition(CONTROL_INST_t * control) {
	control->state = CS_IGNITION;
}

static void ignition(CONTROL_INST_t * control) {
	//send first motor movement command

	//wait for the half time in nb of cycles
	//and osdelay for the remaining time

	//send second movement command
	//wait for the target reached
	//init thrust

}

static void init_thrust(CONTROL_INST_t * control) {
	control->state = CS_THRUST;
}

static void thrust(CONTROL_INST_t * control) {
	//thrust control algorithm drives the motor
	//successive motor ppm moves with the immediate flag set!

	//detect flameout (pressure)
	//-> init shutdown
}

static void init_shutdown(CONTROL_INST_t * control) {
	control->state = CS_SHUTDOWN;
	//start motor movement to close valve
}

static void shutdown(CONTROL_INST_t * control) {
	//wait for the ack
}

static void init_glide(CONTROL_INST_t * control) {
	control->state = CS_GLIDE;
}

static void glide(CONTROL_INST_t * control) {
	//AB algorithm controls the airbrakes motor

	//expect a stop signal to go to idle
	//back to idle
}

static void init_abort(CONTROL_INST_t * control) {
	led_set_color(LED_PINK);
	control->state = CS_ABORT;
}

static void _abort(CONTROL_INST_t * control) {
	//close main valve
	//close airbrakes
	//wait for user release
	if(control_sched_should_run(control, CONTROL_SCHED_RECOVER)) {
		init_idle(control);
		control_sched_done(control, CONTROL_SCHED_RECOVER);
	}
}

static void init_error(CONTROL_INST_t * control) {
	led_set_color(LED_RED);
	control->state = CS_ERROR;
}

static void error(CONTROL_INST_t * control) {
	//wait for user release
	if(control_sched_should_run(control, CONTROL_SCHED_RECOVER)) {
		init_idle(control);
		control_sched_done(control, CONTROL_SCHED_RECOVER);
	}
}

CONTROL_STATE_t control_get_state() {
	return control.state;
}

CONTROL_PP_PARAMS_t control_get_pp_params() {
	return control.pp_params;
}

void control_set_pp_params(CONTROL_PP_PARAMS_t params) {
	control.pp_params = params;
}

void control_move(EPOS4_MOV_t mov_type, int32_t target) {
	control_sched_set(&control, CONTROL_SCHED_MOVE);
	control.mov_type = mov_type;
	control.mov_target = target;
	control.mov_started = 0;
}

void control_calibrate() {
	control_sched_set(&control, CONTROL_SCHED_CALIBRATE);
}

void control_arm() {
	control_sched_set(&control, CONTROL_SCHED_ARM);
}

void control_disarm() {
	control_sched_set(&control, CONTROL_SCHED_DISARM);
}

void control_ignite() {
	control_sched_set(&control, CONTROL_SCHED_IGNITE);
}

void control_abort() {
	control_sched_set(&control, CONTROL_SCHED_ABORT);
}

void control_recover() {
	control_sched_set(&control, CONTROL_SCHED_RECOVER);
}

CONTROL_STATUS_t control_get_status() {
	CONTROL_STATUS_t status;
	status.state = control.state;
	status.pp_error = control.pp_epos4->error;
	status.pp_psu_voltage = control.pp_epos4->psu_voltage;
	status.pp_position = control.pp_epos4->position;
	status.pp_status = control.pp_epos4->status;
	return status;
}

static uint8_t control_sched_should_run(CONTROL_INST_t * control, CONTROL_SCHED_t num) {
	return control->sched == num;
}

static void control_sched_done(CONTROL_INST_t * control, CONTROL_SCHED_t num) {
	if(control->sched == num) {
		control->sched = CONTROL_SCHED_NOTHING;
	} else {
		init_error(control);
	}
}

static void control_sched_set(CONTROL_INST_t * control, CONTROL_SCHED_t num) {
	if(control->sched == CONTROL_SCHED_NOTHING) {
		for(uint8_t i = 0; i < SCHED_ALLOWED_WIDTH; i++) {
			if(sched_allowed[control->state][i] == num) {
				control->sched = num;
				return;
			}
		}
	}
}

/* END */


