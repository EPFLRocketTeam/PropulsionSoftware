/*
 * blinker.c
 *
 * author: Iacopo Sprenger
 */


#include <blinker.h>
#include <main.h>
#include "cmsis_os.h"

void PP_blinkerFunc(void *argument) {
	  for(;;)
	  {
		 HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
		 osDelay(500);
	  }
}



