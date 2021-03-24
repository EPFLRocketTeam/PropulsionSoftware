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

#include <feedback.h>
#include <led.h>
#include <control.h>


/**********************
 *	CONSTANTS
 **********************/

#define FEEDBACK_HEART_BEAT	500


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

void feedback_thread(void * arg) {

	static TickType_t last_wake_time;
	static const TickType_t period = pdMS_TO_TICKS(FEEDBACK_HEART_BEAT);

	last_wake_time = xTaskGetTickCount();

	led_init();



	for(;;) {



		vTaskDelayUntil( &last_wake_time, period );
	}
}



/* END */


