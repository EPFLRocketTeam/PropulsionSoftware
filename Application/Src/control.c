/*
 * control.c
 *
 * author: Iacopo Sprenger
 */


#include <control.h>
#include <main.h>
#include <led.h>
#include <sensor.h>
#include "cmsis_os.h"
#include <stdlib.h>




void PP_controlFunc(void *argument) {

	static uint16_t r, g, b;
	for(;;) {
		r = PP_getData(PP_PRESSURE_1);
		g = PP_getData(PP_PRESSURE_2);
		b = PP_getData(PP_TEMPERATURE_1);
		//PP_setLed((r>>4)/5, (g>>4)/5, (b>>4)/5);
		//PP_setLed(0, 0, 5);
		osDelay(100);
	  }
}








