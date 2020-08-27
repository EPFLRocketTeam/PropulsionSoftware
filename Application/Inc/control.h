/*
 * control.h
 *
 * author: Iacopo Sprenger
 */


#ifndef CONTROL_H
#define CONTROL_H

#include <main.h>
#include <error.h>


//SOLENOID: PB9 | D1
void toggle_solenoid();


PP_STATUS_t PP_start_setup(void);
PP_STATUS_t PP_start_operation(void);


void PP_controlFunc(void *argument);




#endif
