/*  Title		: Threads
 *  Filename	: threads.c
 *	Author		: iacopo sprenger
 *	Date		: 28.01.2021
 *	Version		: 0.1
 *	Description	: threads declaration
 */

/**********************
 *	INCLUDES
 **********************/


#include <cmsis_os.h>

#include <threads.h>
#include <control.h>
#include <sensor.h>

/**********************
 *	CONSTANTS
 **********************/


#define DEFAULT_SZ	(256)

#define CONTROL_SZ	DEFAULT_SZ
#define CONTROL_PRIO	(6)

#define SENSOR_SZ	DEFAULT_SZ
#define SENSOR_PRIO	(5)


/**********************
 *	MACROS
 **********************/

#define CREATE_THREAD(handle, name, func, sz, prio) \
	static StaticTask_t name##_buffer; \
	static StackType_t name##_stack[ sz ]; \
	handle = xTaskCreateStatic( \
			func, \
	        #name, \
			sz, \
			( void * ) 0, \
			prio, \
			name##_stack, \
			&name##_buffer)


/**********************
 *	TYPEDEFS
 **********************/


/**********************
 *	VARIABLES
 **********************/

static TaskHandle_t sensor_handle = NULL;
static TaskHandle_t control_handle = NULL;


/**********************
 *	PROTOTYPES
 **********************/


/**********************
 *	DECLARATIONS
 **********************/

/*
 * Create all the threads needed by the software
 */
void threads_init(void) {

	/*
	 *  Main control thread
	 *  Highest priority
	 */
	CREATE_THREAD(control_handle, control, control_thread, CONTROL_SZ, CONTROL_PRIO);

	/*
	 *  Sensor processing thread
	 *  2nd highest priority
	 */
	CREATE_THREAD(sensor_handle, sensor, sensor_thread, SENSOR_SZ, SENSOR_PRIO);




}


/* END */


