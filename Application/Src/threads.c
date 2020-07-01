/*	threads.c
 *
 * 	author: Iacopo Sprenger
 *	threads initialization
 *
 *
 */


#include <threads.h>
#include <blinker.h>
#include <sensor.h>
#include <main.h>
#include <led.h>


typedef StaticTask_t osStaticThreadDef_t;



//blinker thread

osThreadId_t PP_blinkerHandle;
uint32_t PP_blinkerBuffer[ 128 ];
osStaticThreadDef_t PP_blinkerControlBlock;
const osThreadAttr_t PP_blinker_attributes = {
  .name = "PP_blinker",
  .stack_mem = &PP_blinkerBuffer[0],
  .stack_size = sizeof(PP_blinkerBuffer),
  .cb_mem = &PP_blinkerControlBlock,
  .cb_size = sizeof(PP_blinkerControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};

//sensor thread

osThreadId_t PP_sensorHandle;
uint32_t PP_sensorBuffer[ 128 ];
osStaticThreadDef_t PP_sensorControlBlock;
const osThreadAttr_t PP_sensor_attributes = {
  .name = "PP_sensor",
  .stack_mem = &PP_sensorBuffer[0],
  .stack_size = sizeof(PP_sensorBuffer),
  .cb_mem = &PP_sensorControlBlock,
  .cb_size = sizeof(PP_sensorControlBlock),
  .priority = (osPriority_t) osPriorityRealtime,
};









void PP_initThreads(void) {


	//blinker init
	PP_blinkerHandle = osThreadNew(PP_blinkerFunc, NULL, &PP_blinker_attributes);
	//sensor init
	PP_sensorHandle = osThreadNew(PP_sensorFunc, NULL, &PP_sensor_attributes);


}


