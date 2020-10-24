/*
 * control.h
 *
 * author: Iacopo Sprenger
 */


#ifndef CONTROL_H
#define CONTROL_H

#include <main.h>
#include <error.h>

#define SOLENOID_PORT	GPIOB
#define SOLENOID_PIN	GPIO_PIN_7

#define SOLENOID	SOLENOID_PORT, SOLENOID_PIN


//SOLENOID: PB7 | D2 S2
//returns the pin state after toggle
uint8_t toggle_solenoid();
uint8_t open_solenoid();
uint8_t close_solenoid();


PP_STATUS_t start_setup(void);
PP_STATUS_t start_operation(void);
uint16_t get_global_status();

void save_persistent(void);


void PP_controlFunc(void *argument);




#endif
