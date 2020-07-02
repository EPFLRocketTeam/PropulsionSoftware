/*	threads.c
 *
 * 	author: Iacopo Sprenger
 *	threads initialization
 *
 *
 */


#include <threads.h>
#include <control.h>
#include <sensor.h>
#include <main.h>
#include <led.h>


typedef StaticTask_t osStaticThreadDef_t;



//control thread

osThreadId_t PP_controlHandle;
uint32_t PP_controlBuffer[ 128 ];
osStaticThreadDef_t PP_controlControlBlock;
const osThreadAttr_t PP_control_attributes = {
  .name = "PP_control",
  .stack_mem = &PP_controlBuffer[0],
  .stack_size = sizeof(PP_controlBuffer),
  .cb_mem = &PP_controlControlBlock,
  .cb_size = sizeof(PP_controlControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};

//sensor thread
/*
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
*/








void PP_initThreads(void) {


	//blinker init
	PP_controlHandle = osThreadNew(PP_controlFunc, NULL, &PP_control_attributes);
	//sensor init
	PP_sensorInit();
	//plus besoin d'un thread pour les capteurs vu que tout est géré par tim2 et adc+dma
	//PP_sensorHandle = osThreadNew(PP_sensorFunc, NULL, &PP_sensor_attributes);


}


