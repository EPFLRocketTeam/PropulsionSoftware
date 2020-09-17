/*
 * control.h
 *
 * author: Iacopo Sprenger
 */


#ifndef CONTROL_H
#define CONTROL_H

#include <main.h>
#include <error.h>


#define SOLENOID	GPIOB, GPIO_PIN_9
//SOLENOID: PB9 | D1 S2
//returns the pin state after toggle
uint8_t toggle_solenoid();


PP_STATUS_t start_setup(void);
PP_STATUS_t start_operation(void);

void save_persistent(void);


void PP_controlFunc(void *argument);




#endif
