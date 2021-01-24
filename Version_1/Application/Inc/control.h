/*
 * control.h
 *
 * author: Iacopo Sprenger
 */


#ifndef CONTROL_H
#define CONTROL_H

#include <main.h>
#include <error.h>
#include <maxon_comm.h>


#define SOLENOID_PORT	GPIOB
#define SOLENOID_PIN	GPIO_PIN_7

#define SOLENOID	SOLENOID_PORT, SOLENOID_PIN


typedef struct {
	int32_t pos_1;
	int32_t pos_2;
	int32_t pos_3;
	int32_t time_1;
	int32_t time_2;
	int32_t time_3;
	int32_t time_4;
	int32_t time_5;
	int32_t time_6;
	uint16_t time_step;
}NOMINAL_TRAJ_t;


//SOLENOID: PB7 | D2 S2
//returns the pin state after toggle
uint8_t toggle_solenoid();
uint8_t open_solenoid();
uint8_t close_solenoid();

void generate_trajectory(NOMINAL_TRAJ_t traj, TRAJ_BUFFER_t * traj_bfr);

PP_STATUS_t start_setup(void);
PP_STATUS_t start_operation(void);
uint16_t get_global_status();

void save_persistent(void);


void PP_controlFunc(void *argument);




#endif
