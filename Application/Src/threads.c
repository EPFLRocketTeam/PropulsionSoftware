/*	threads.c
 *
 * 	author: Iacopo Sprenger
 *	threads initialization
 *
 *
 */


#include <threads.h>
#include <blinker.h>
#include <main.h>
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

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









void PP_initThreads(void) {
	//blinker init
	PP_blinkerHandle = osThreadNew(PP_blinkerFunc, NULL, &PP_blinker_attributes);


}


