/*
 * control.h
 *
 * author: Iacopo Sprenger
 */


#ifndef CONTROL_H
#define CONTROL_H

#include <main.h>


typedef struct {
	uint8_t s1_plugged;
	uint8_t s2_plugged;
	uint8_t pres1_stable;
	uint8_t pres2_stable;
	uint8_t temp1_stable;
	uint8_t temp2_stable;
	uint8_t temp3_stable;
}PP_SYSTEM_REPORT_t;



void PP_blinkerFunc(void *argument);
void PP_controlFunc(void *argument);

PP_SYSTEM_REPORT_t PP_selfDiagnose(void);


#endif
