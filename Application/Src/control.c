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


#define UNSTABLE_THRESH 5
static PP_SYSTEM_REPORT_t report;

//implement this in a more complex fashion so it can be asked and then run!
PP_SYSTEM_REPORT_t PP_selfDiagnose(void) {
	return report;
}

void self_diagnose(void) {
	report.s1_plugged = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
	report.s2_plugged = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
	static uint16_t pres1, pres2, temp1, temp2, temp3;
	static uint16_t ppres1, ppres2, ptemp1, ptemp2, ptemp3;
	ppres1 = pres1;
	ppres2 = pres2;
	ptemp1 = temp1;
	ptemp2 = temp2;
	ptemp3 = temp3;
	pres1 = PP_getData(PP_PRESSURE_1);
	pres2 = PP_getData(PP_PRESSURE_2);
	temp1 = PP_getData(PP_TEMPERATURE_1);
	temp2 = PP_getData(PP_TEMPERATURE_2);
	temp3 = PP_getData(PP_TEMPERATURE_3);
	if(abs(pres1-ppres1) > UNSTABLE_THRESH) {
		report.pres1_stable = 0;
	} else {
		report.pres1_stable = 1;
	}
	if(abs(pres2-ppres2) > UNSTABLE_THRESH) {
		report.pres2_stable = 0;
	} else {
		report.pres2_stable = 1;
	}
	if(abs(temp1-ptemp1) > UNSTABLE_THRESH) {
		report.temp1_stable = 0;
	} else {
		report.temp1_stable = 1;
	}
	if(abs(temp2-ptemp2) > UNSTABLE_THRESH) {
		report.temp2_stable = 0;
	} else {
		report.temp2_stable = 1;
	}
	if(abs(temp3-ptemp3) > UNSTABLE_THRESH) {
		report.temp3_stable = 0;
	} else {
		report.temp3_stable = 1;
	}
}

void PP_controlFunc(void *argument) {

	static uint16_t r, g, b;
	for(;;) {
		r = PP_getData(PP_PRESSURE_1);
		g = PP_getData(PP_PRESSURE_2);
		b = PP_getData(PP_TEMPERATURE_1);
		//PP_setLed((r>>4)/5, (g>>4)/5, (b>>4)/5);
		PP_setLed(0, 0, 5);
		osDelay(100);
	  }
}








