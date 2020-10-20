#ifndef MAXON_COMM_H
#define MAXON_COMM_H

#include <main.h>
#include "usart.h"

//control sequences
#define DLE	0x90
#define STX	0x02
#define READ_OBJECT	0x60
#define READ_OBJECT_LEN 2
#define WRITE_OBJECT 0x68
#define WRITE_OBJECT_LEN 4
#define NODE_ID 0x01
#define DATA_SIZE 4

#define COMM_TIMEOUT pdMS_TO_TICKS(1000)
#define LONG_TIME 0xffff

#define MAX_FRAME_LEN	64

#define MOTOR_UART	huart6


//MOTOR_SPECIFIC SETTINGS
#define MAX_POS			0	//checked
#define MIN_POS			0 	//checked
#define MAX_PROFILE_VEL	8000 //check

#define QUICK_STOP_DEC	50000 //check
#define MAX_ACC			100000 //check

//motor csts
#define MOTOR_TYPE	10			//sin com bldc motor

#define MOTOR_MAX_SPEED	10000	//check
#define MOTOR_NOM_CUR	7320	//checked
#define MOTOR_MAX_CURRENT	10000	//checked
#define MOTOR_THERMAL	327 //checked
#define MOTOR_TORQUE    12419 //checked
#define EL_RESISTANCE	139 //checked
#define EL_INDUCTANCE	93	//checked

#define GEAR_MAX_SPEED	8000 //check
#define GEAR_NUM		66  //check
#define GEAR_DEN		1 	//check

#define NUM_POLE_PAIRS	4 //checked

#define ENC_NB_PULSES	1024 //check
#define ENC_TYPE		0x0001 //check

#define HALL_TYPE		0x0000 //check
#define HALL_PATTERN	0x0005 //check

//controllers
#define CURRENT_P		140106 //check
#define CURRENT_I		203253 //check
#define POSITION_P		1703746 //check
#define POSITION_I		8397288 //check
#define POSITION_D		25239 //check
#define POSITION_FFV	3568 //check
#define POSITION_FFA	162 //check

#define FOLLOW_WINDOW	1000000

#define PROFILE_ACC 	50000
#define PROFILE_DEC 	50000
#define PROFILE_VEL 	8000
#define PROFILE_TYPE	0



#define DEG2INC(deg)	(int32_t) ((int64_t) (deg)*4*ENC_NB_PULSES*GEAR_NUM/GEAR_DEN/360)
#define DDEG2INC(deg)	(int32_t) ((int64_t) (deg)*4*ENC_NB_PULSES*GEAR_NUM/GEAR_DEN/3600)
#define INC2DDEG(inc)	(int32_t) ((int64_t) (inc)*3600*GEAR_DEN/GEAR_NUM/4/ENC_NB_PULSES)

typedef struct {
	uint32_t acceleration;
	uint32_t deceleration;
	uint32_t speed;
	int32_t tmp_target;
	int32_t target;
	int32_t half_target;
	uint32_t half_wait; //ms
	uint32_t end_wait; //ms
	uint32_t pre_wait; //ms
	uint8_t absolute;
}MOTOR_PPM_PARAMS_t;


//MISC INTERNAL FUNCTIONS
uint16_t CalcFieldCRC(uint16_t* pDataArray, uint16_t ArrayLength);
uint16_t Create_frame(uint8_t * frameBuffer, uint8_t opcode, uint8_t data_len, uint8_t * data);
int32_t Decode_frame(uint8_t d, uint8_t * opcode, uint8_t * data, uint16_t * crc);
uint32_t Write_object(uint16_t index, uint8_t subindex, uint8_t * data);
uint32_t Read_object(uint16_t index, uint8_t subindex, uint8_t * data);

//MAIN FUNCTIONS
void maxon_comm_init(void);
void maxon_comm_receive(uint8_t recvBuffer);

//DIRECT EXEC FUNCTIONS
void motor_config_gen();
void motor_config_ppm();
void motor_setup_ppm(uint32_t profile_acc, uint32_t profile_dec, uint32_t profile_vel);
void motor_set_target_abs(int32_t pos);
void motor_set_target_rel(int32_t pos);
void motor_enable();
void motor_disable();
void motor_quickstop();
void motor_disable_voltage();
void motor_shutdown();
void motor_switch_on();
void motor_fault_rst();
void motor_set_rel();
void motor_set_abs();
void motor_start();
void motor_imm();
int8_t read_status_word(uint16_t * data);
void read_max_prof_vel(uint8_t * data);
//END OF DIRECT EXEC FUNCTIONS



//DEFFERED EXEC FUNCTIONS
void motor_mainloop(void * argument);

uint16_t motor_get_status(void);
uint16_t motor_get_error(void);
int32_t motor_get_pos(void);
uint16_t motor_get_psu_voltage(void) ;
uint16_t motor_get_torque(void);
int32_t motor_get_pos_cmd(void);
int32_t motor_get_vel(void);
int32_t motor_get_curr(void);
int32_t motor_get_curr_cmd(void);
uint32_t motor_get_time(void);

/*
 * @brief	enable motor (deffered version)
 */
void motor_def_enable();

/*
 * @brief	disable motor (deffered version)
 */
void motor_def_disable();

/*
 * @brief	configure motor (deffered version)
 */
void motor_def_config();

/*
 * @brief	startup motor (deffered version)
 */
void motor_def_startup();

/*
 * @brief	shutdown motor (deffered version)
 */
void motor_def_shutdown();

/*
 * @brief	set and setup ppm mode (deffered version)
 */
void motor_def_set_ppm();

/*
 * @brief	set and setup homing mode (deffered version)
 */
void motor_def_set_homing();

/*
 * @brief	start ppm operation (deffered version)
 */
void motor_def_start_ppm_operation();

/*
 * @brief	start motor homing (deffered version)
 */
void motor_def_start_homing_operation();

/*
 * @brief	start motor operation (deffered version)
 */
void motor_def_start_operation();

/*
 * @brief	start abort procedure (deffered version)
 */
void motor_def_abort(void);

void motor_def_custom_write();

/*
 * @biref			register ppm speed
 *
 * @param speed 	the new speed
 */
void motor_register_speed(uint32_t speed);

/*
 * @biref			register ppm acceleration
 *
 * @param acc 		the new acceleration
 */
void motor_register_acceleration(uint32_t acc);

/*
 * @biref			register ppm deceleration
 *
 * @param dec 		the new deceleration
 */
void motor_register_deceleration(uint32_t dec);

/*
 * @biref			register ppm target
 *
 * @param target 	the new target
 */
void motor_register_tmp_target(int32_t target);

/*
 * @biref			register operation target
 *
 * @param target 	the new target
 */
void motor_register_target(int32_t target);

/*
 * @biref			register operation half target
 *
 * @param half_target 	the new half target
 */
void motor_register_half_target(int32_t half_target);

/*
 * @biref			register operation end wait
 *
 * @param end_wait 	the new end wait
 */
void motor_register_end_wait(uint32_t end_wait);

/*
 * @biref			register operation half wait
 *
 * @param half_wait 	the new half wait
 */
void motor_register_half_wait(uint32_t half_wait);

/*
 * @biref			register operation pre wait
 *
 * @param pre_wait 	the new pre wait
 */
void motor_register_pre_wait(uint32_t pre_wait);

/*
 * @biref			set ppm mode absolute
 */
void motor_register_absolute();

/*
 * @biref			set ppm mode relative
 */
void motor_register_relative();

/*
 * @brief			get ppm speed
 *
 * @return			current ppm speed
 */
uint32_t motor_get_ppm_speed();

/*
 * @brief			get ppm acceleration
 *
 * @return			current ppm acceleration
 */
uint32_t motor_get_ppm_acceleration();

/*
 * @brief			get ppm deceleration
 *
 * @return			current ppm deceleration
 */
uint32_t motor_get_ppm_deceleration();

/*
 * @brief			get operation target
 *
 * @return			current operation target
 */
int32_t motor_get_target();

/*
 * @brief			get operation half target
 *
 * @return			current operation half target
 */
int32_t motor_get_half_target();

/*
 * @brief			get operation end wait
 *
 * @return			current operation end wait
 */
uint32_t motor_get_end_wait();

/*
 * @brief			get operation half wait
 *
 * @return			current operation half wait
 */
uint32_t motor_get_half_wait();

/*
 * @brief			get operation pre wait
 *
 * @return			current operation pre wait
 */
uint32_t motor_get_pre_wait();

/*
 * @brief			get custom object value
 *
 * @return			current custom object value
 */
uint32_t motor_get_custom_object();

/*
 * @brief			register custom object index
 *
 * @param index		custom object index
 */
void motor_register_custom_index(uint32_t index);

/*
 * @brief			register custom object subindex
 *
 * @param subindex		custom object subindex
 */
void motor_register_custom_subindex(uint8_t subindex);

/*
 * @brief			register custom object data
 *
 * @param data		custom object data
 */
void motor_register_custom_write(uint8_t data);

MOTOR_PPM_PARAMS_t motor_get_ppm_params(void);
void motor_set_ppm_params(MOTOR_PPM_PARAMS_t params);



//END OF DEFFERED EXEC FUNCTIONS

//
uint8_t get_busy_state(void);


#endif
