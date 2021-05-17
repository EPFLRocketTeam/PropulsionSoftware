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
#include <storage.h>
#include <serial.h>
#include <debug.h>
#include <epos4.h>
#include <can_comm.h>

/**********************
 *	CONSTANTS
 **********************/


#define DEFAULT_SZ	(1024)

#define CONTROL_SZ	DEFAULT_SZ
#define CONTROL_PRIO	(6)

#define SENSOR_SZ	DEFAULT_SZ
#define SENSOR_PRIO		(4)

#define SERIAL_SZ	DEFAULT_SZ
#define SERIAL_PRIO		(5)

#define STORAGE_SZ	DEFAULT_SZ
#define STORAGE_PRIO	(3)

#define CAN_SZ		DEFAULT_SZ
#define CAN_PRIO		(3)


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
static TaskHandle_t serial_handle = NULL;
static TaskHandle_t storage_handle = NULL;
static TaskHandle_t can_send_handle = NULL;


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


	serial_global_init();
	static DEBUG_INST_t debug;
	debug_init(&debug);

	can_init();


	/*
	 *  Feedback thread
	 *  Lowest priority
	 *  Handle blinking led and sound effects
	 */


	/*
	 * Storage thread
	 * lowest priority
	 */

	CREATE_THREAD(storage_handle, storage, storage_thread, STORAGE_SZ, STORAGE_PRIO);

	/*
	 *  Serial RX processing thread (Bottom half)
	 *  low priority
	 */
	CREATE_THREAD(serial_handle, serial, serial_thread, SERIAL_SZ, SERIAL_PRIO);

	/*
	 *  Sensor processing thread
	 *  2nd highest priority
	 */
	CREATE_THREAD(sensor_handle, sensor, sensor_thread, SENSOR_SZ, SENSOR_PRIO);

	/*
	 *  Main control thread
	 *  Highest priority
	 */
	CREATE_THREAD(control_handle, control, control_thread, CONTROL_SZ, CONTROL_PRIO);

	/*
	 *  CAN send thread
	 *  Highest priority
	 */
	CREATE_THREAD(can_send_handle, can_send, can_send_thread, CAN_SZ, CAN_PRIO);



}


/* END */


