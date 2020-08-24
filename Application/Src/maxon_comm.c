#include <maxon_comm.h>
#include "cmsis_os.h"
#include <semphr.h>
#include "usart.h"
#include <led.h>


static uint16_t crcDataArray[MAX_FRAME_LEN];

static uint8_t recieved_data[MAX_FRAME_LEN*2];
static uint8_t recieved_opcode = 0, recieved_length = 0;
static uint16_t recieved_crc = 0;


static uint8_t send_frame[MAX_FRAME_LEN*2];


//semaphores here because those functions could be executed
//from anywhere!
static SemaphoreHandle_t recep_end_sem = NULL;
static StaticSemaphore_t recep_end_semBuffer;
static SemaphoreHandle_t driver_busy_sem = NULL;
static StaticSemaphore_t driver_busy_semBuffer;



//DEBUG

uint8_t get_busy_state(void) {
	return uxSemaphoreGetCount(driver_busy_sem);
}

//COMMUNICATION UTILITIES


uint16_t CalcFieldCRC(uint16_t *pDataArray, uint16_t ArrayLength) {
	uint16_t shifter, c;
	uint16_t carry;
	uint16_t crc = 0;
	while (ArrayLength--) {
		shifter = 0x8000;
		c = *pDataArray++;
		do {
			carry = crc & 0x8000;
			crc <<= 1;
			if (c & shifter) crc++;
			if (carry) crc ^= 0x1021;
			shifter >>= 1;

		} while (shifter);

	}
	return crc;
}




//returns length in BYTES for transmission!!
//CRC calculated before sync and before data stuffing!!
//framebuffer size must be MAX_FRAME_LEN
//data_len in WORDS

//bytes are invertes (LSB first) in crc calculation, (which is correct)
//but I STILL HAVE TO INVERT BYTES IN FRAME!!!!!!

uint16_t Create_frame(uint8_t * frameBuffer, uint8_t opcode, uint8_t data_len, uint8_t * data) {
	uint16_t array_len = data_len+2; //we add 1 for the opcode and len fields and 1 for the crc
	frameBuffer[0] = DLE;
	frameBuffer[1] = STX;
	frameBuffer[2] = opcode;
	frameBuffer[3] = data_len;
	crcDataArray[0] = (data_len<<8) | opcode;  //header bytes inverted
	uint16_t counter=4;
	for(uint16_t i = 0; i < data_len; i++) {
		frameBuffer[counter++] = data[2*i]; //bytes in data need to be inverted before
		if(frameBuffer[counter-1] == DLE) {
			frameBuffer[counter++] = DLE;
		}
		frameBuffer[counter++] = data[2*i+1];
		if(frameBuffer[counter-1] == DLE) {
			frameBuffer[counter++] = DLE;
		}
		crcDataArray[i+1] = (data[2*i+1]<<8) |  data[2*i];
	}
	crcDataArray[array_len-1] = 0x0000;
	uint16_t crc = CalcFieldCRC(crcDataArray, array_len);
	frameBuffer[counter++] = crc&0xff; //crc bytes are inverted (LSB first) !!
	if(frameBuffer[counter-1] == DLE) {
		frameBuffer[counter++] = DLE;
	}
	frameBuffer[counter++] = crc>>8;
	if(frameBuffer[counter-1] == DLE) {
		frameBuffer[counter++] = DLE;
	}
	return counter;
}



//returns data length

typedef enum {
	WAITING_DLE,
	WAITING_STX,
	WAITING_OPCODE,
	WAITING_LEN,
	WAITING_DATA,
	WAITING_CRC1,
	WAITING_CRC2
}DECODE_STATE_t;


//returns length in WORDS

int32_t Decode_frame(uint8_t d, uint8_t * opcode, uint8_t * data, uint16_t * crc) {
    static uint8_t escape = 0;
    static uint16_t length = 0;
    static uint16_t counter = 0;
    static DECODE_STATE_t state = WAITING_DLE;
    //if a DLE in data is followed by STX, we start again
    if (escape == 1 && d == STX) {
        state = WAITING_OPCODE;
        escape = 0;
        return -1;
    }

    if (state == WAITING_DLE && d == DLE) {
        state = WAITING_STX;
        return -1;
    }
    //escape in case a DLE is in the data
    if (d == DLE && escape == 0) {
        escape = 1;
        return -1;
    }
    //if it is doubled, it counts as data
    if (d == DLE && escape == 1) {
        escape = 0;

    }

    if (state == WAITING_STX && d == STX) {
        state = WAITING_OPCODE;
        return -1;
    }

    if (state == WAITING_OPCODE) {
        *opcode = d;
        state = WAITING_LEN;
        return -1;
    }

    if (state == WAITING_LEN) {
        length = d;
        counter = 0;
        state = WAITING_DATA;
        return -1;
    }

    if (state == WAITING_DATA) {
        data[counter] = d;
        counter += 1;
        //the length  is in WORDS, but we read BYTES
        if (counter==2*length) {
            state = WAITING_CRC1;
        }
        return -1;
    }

    if (state == WAITING_CRC1) {
        *crc = d;
        state = WAITING_CRC2;
        return -1;

    }

    if (state == WAITING_CRC2) {
        *crc += d<<8;
        state = WAITING_DLE;
        return length;
    }
    state=WAITING_DLE;
    return -1;
}


void maxon_comm_init(void) {
	//this semaphore is only avaiable when data is recieved -> starts taken
	recep_end_sem = xSemaphoreCreateBinaryStatic(&recep_end_semBuffer);
	driver_busy_sem = xSemaphoreCreateMutexStatic(&driver_busy_semBuffer);
	//this semaphore is avaiable at start
}


void maxon_comm_receive(uint8_t recvBuffer) {
	int32_t res = Decode_frame(recvBuffer, &recieved_opcode, recieved_data, &recieved_crc);
	if(res != -1) {
		recieved_length = res;
		xSemaphoreGive(recep_end_sem);
	}
}

//returns the comm error code

uint32_t Write_object(uint16_t index, uint8_t subindex, uint8_t * data) {
	if (xSemaphoreTake(driver_busy_sem, COMM_TIMEOUT) == pdTRUE) { //only one transmission at a time
		static uint8_t send_data[WRITE_OBJECT_LEN*2];
		static uint16_t length = 0;
		send_data[0] = NODE_ID; //node ID
		send_data[1] = index & 0xff;
		send_data[2] = index >> 8;
		send_data[3] = subindex;
		for(uint8_t i = 0; i < DATA_SIZE; i++){
			send_data[4+i] = data[i];
		}
		length = Create_frame(send_frame, WRITE_OBJECT, WRITE_OBJECT_LEN, send_data);
		HAL_UART_Transmit(&MOTOR_UART, send_frame, length, 500);
		if(xSemaphoreTake(recep_end_sem, COMM_TIMEOUT) == pdTRUE) {
			uint32_t tmp = recieved_data[0] | (recieved_data[1]<<8) | (recieved_data[2]<<16) | (recieved_data[3]<<24);
			xSemaphoreGive(driver_busy_sem); //release the sem to allow another transmission
			return tmp;
		} else {
			xSemaphoreGive(driver_busy_sem); //release the sem to allow another transmission
			return -1;
		}
	} else {

		return -1;
	}
}

uint32_t Read_object(uint16_t index, uint8_t subindex, uint8_t * data) {
	if (xSemaphoreTake(driver_busy_sem, COMM_TIMEOUT) == pdTRUE) { //only one transmission at a time
		static uint8_t send_data[READ_OBJECT_LEN*2];
		static uint16_t length = 0;
		send_data[0] = NODE_ID; //node ID
		send_data[1] = index & 0xff;  //LSB first
		send_data[2] = index >> 8;
		send_data[3] = subindex;
		length = Create_frame(send_frame, READ_OBJECT, READ_OBJECT_LEN, send_data);
		HAL_UART_Transmit(&MOTOR_UART, send_frame, length, 500);

		if(xSemaphoreTake(recep_end_sem, COMM_TIMEOUT) == pdTRUE) {
			for(uint8_t i = 0; i < DATA_SIZE; i++){
				data[i] = recieved_data[4+i];
			}
			uint32_t tmp = recieved_data[0] | (recieved_data[1]<<8) | (recieved_data[2]<<16) | (recieved_data[3]<<24);
			xSemaphoreGive(driver_busy_sem); //release the sem to allow another transmission
			return tmp;
		} else {
			xSemaphoreGive(driver_busy_sem); //release the sem to allow another transmission
			return -1;
		}
	} else {
		return -1;
	}
}




//BINARY UTILITIES





void store_uint8(uint8_t value, uint8_t * data) {
	data[0] = value;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
}



void store_uint16(uint16_t value, uint8_t * data) {
	data[0] = value;
	data[1] = value>>8;
	data[2] = 0x00;
	data[3] = 0x00;
}

void store_uint32(uint32_t value, uint8_t * data) {
	data[0] = value;
	data[1] = value>>8;
	data[2] = value>>16;
	data[3] = value>>24;

}

void store_int8(int8_t value, uint8_t * data) {
	data[0] = value;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
}

void store_int16(int16_t value, uint8_t * data) {
	data[0] = value;
	data[1] = value>>8;
	data[2] = 0x00;
	data[3] = 0x00;
}

void store_int32(int32_t value, uint8_t * data) {
	data[0] = value;
	data[1] = value>>8;
	data[2] = value>>16;
	data[3] = value>>24;
}



//MAXON MOTOR SETUP COMMANDS

#include <maxon_def.h>


static uint8_t tmp_data[DATA_SIZE];
static uint8_t tmp_data2[DATA_SIZE];

#define MAX_POS			0
#define MIN_POS			0
#define MAX_PROFILE_VEL	50000
#define MAX_MOTOR_SPEED	50000
#define QUICK_STOP_DEC	10000
#define MAX_ACC			5000000



//motor csts
#define MOTOR_MAX_SPEED	10000
#define MOTOR_NOM_CUR	7320
#define MOTOR_MAX_CURRENT	10000
#define MOTOR_THERMAL	32
#define MOTOR_TORQUE    14 //CHECK UNITS

#define GEAR_MAX_SPEED	8000
#define GEAR_NUM		66
#define GEAR_DEN		1

#define NUM_POLE_PAIRS	4


#define PROFILE_ACC 	20000
#define PROFILE_DEC 	20000
#define PROFILE_VEL 	8000
#define PROFILE_TYPE	0




void motor_setup_ppm(uint32_t profile_acc, uint32_t profile_dec, uint32_t profile_vel) {
	store_uint32(profile_acc, tmp_data);
	Write_object(MAXON_PROFILE_ACCELERATION, tmp_data);

	store_uint32(profile_dec, tmp_data);
	Write_object(MAXON_PROFILE_DECELERATION, tmp_data);

	store_uint32(profile_vel, tmp_data);
	Write_object(MAXON_PROFILE_VELOCITY, tmp_data);

	store_uint32(PROFILE_TYPE, tmp_data);
	Write_object(MAXON_MOTION_PROFILE_TYPE, tmp_data);
}

int8_t motor_get_mode(int8_t * mode) {
	if(Read_object(MAXON_MODE_OF_OPERATION, tmp_data) == -1) {
			return -1;
		}
		*mode = tmp_data[0];
		return 0;
}

void motor_set_target_abs(int32_t pos) {
	store_int32(pos, tmp_data);
	Write_object(MAXON_TARGET_POSITION, tmp_data);
	motor_set_abs();
	motor_new_pos();

}

void motor_set_target_rel(int32_t pos) {
	store_int32(pos, tmp_data);
	Write_object(MAXON_TARGET_POSITION, tmp_data);
	motor_set_rel();
	motor_new_pos();

}



int8_t read_status_word(uint16_t * data) {
	if(Read_object(MAXON_STATUS_WORD, tmp_data) == -1) {
		return -1;
	}
	*data = tmp_data[0] | (tmp_data[1]<<8);
	return 0;
}






//motor control

uint8_t write_to_controlword(uint16_t set, uint16_t clr) {
	if(Read_object(MAXON_CONTROL_WORD, tmp_data2) == -1) {
		return -1;
	}
	uint16_t tmp_cw = tmp_data2[0] + ( tmp_data2[1] << 8);
	tmp_cw = tmp_cw | set;
	tmp_cw = tmp_cw & (~clr);
	store_uint16(tmp_cw, tmp_data);
	if(Write_object(MAXON_CONTROL_WORD, tmp_data) == -1) {
		return -1;
	}

	return 0;
}


void motor_enable() {
	write_to_controlword(MAXON_SET_ENABLE, MAXON_CLR_ENABLE);
}

void motor_disable() {
	write_to_controlword(MAXON_SET_DISABLE, MAXON_CLR_DISABLE);
}

void motor_quickstop() {
	write_to_controlword(MAXON_SET_QUICKSTOP, MAXON_CLR_QUICKSTOP);
}

void motor_disable_voltage() {
	write_to_controlword(MAXON_SET_DISABLE_V, MAXON_CLR_DISABLE_V);
}

void motor_shutdown() {
	write_to_controlword(MAXON_SET_SHUTDOWN, MAXON_CLR_SHUTDOWN);
}

void motor_switch_on() {
	write_to_controlword(MAXON_SET_SWITCH_ON, MAXON_CLR_SWITCH_ON);
}

void motor_fault_rst() {
	write_to_controlword(MAXON_SET_FAULT_RST, MAXON_CLR_FAULT_RST);
}

void motor_set_rel() {
	write_to_controlword(MAXON_SET_REL, MAXON_CLR_REL);
}

void motor_set_abs() {
	write_to_controlword(MAXON_SET_ABS, MAXON_CLR_ABS);
}

void motor_new_pos() {
	write_to_controlword(MAXON_SET_NEW_POS, MAXON_CLR_NEW_POS);
}



//START OF DEFFERED CONTROL FUNCTIONS

#define HEART_BEAT	200

typedef struct {
	uint8_t enable;
	uint8_t disable;
	uint8_t config;
	uint8_t startup;
	uint8_t shutdown;
	uint8_t set_ppm;
	uint8_t set_homing;
	uint8_t start_ppm_operation;
	uint8_t start_homing_operation;
	uint8_t start_operation;
}MOTOR_TODO_t;

typedef struct {
	uint32_t acceleration;
	uint32_t deceleration;
	uint32_t speed;
	int32_t target;
	int32_t half_target;
	uint32_t half_wait; //ms
	uint32_t end_wait; //ms
	uint8_t absolute;
}MOTOR_PPM_PARAMS_t;


//to be completed
typedef struct {
	//motor properties
	uint32_t nominal_current;
	uint32_t current_limit;
	uint8_t nb_pole_pairs;
	uint16_t thermal_time_cste;
	uint32_t torque_constant;


	//gear properties
	uint32_t gear_reduction_numerator;
	uint32_t gear_reduction_denominator;
	uint32_t gear_max_input_speed;

	//limits
	uint32_t max_motor_speed;
	int32_t min_position_limit;
	int32_t max_position_limit;


}MOTOR_GEN_PARAMS_t;


static MOTOR_TODO_t motor_todo;
static MOTOR_TODO_t motor_todo_buf;
static MOTOR_GEN_PARAMS_t motor_gen_params;
static MOTOR_PPM_PARAMS_t motor_ppm_params;
static uint16_t motor_status;
static uint32_t operation_counter;

void motor_def_init(void) {
	motor_todo.enable = 0;
	motor_todo.disable = 0;
	motor_todo.config = 0;
	motor_todo.startup = 0;
	motor_todo.shutdown = 0;
	motor_todo.set_ppm = 0;
	motor_todo.set_homing = 0;
	motor_todo.start_ppm_operation = 0;
	motor_todo.start_homing_operation = 0;
	operation_counter = 0;

	motor_ppm_params.acceleration = PROFILE_ACC;
	motor_ppm_params.deceleration = PROFILE_DEC;
	motor_ppm_params.speed = PROFILE_VEL;
	motor_ppm_params.absolute = 1;

}

void motor_config_gen(void) {
	store_uint32(motor_gen_params.max_motor_speed, tmp_data);
	Write_object(MAXON_MOTOR_MAX_SPEED, tmp_data);

	store_uint32(motor_gen_params.nominal_current, tmp_data);
	Write_object(MAXON_MOTOR_NOMINAL_CURRENT, tmp_data);

	store_uint32(motor_gen_params.current_limit, tmp_data);
	Write_object(MAXON_MOTOR_CURRENT_LIMIT, tmp_data);

	store_uint16(motor_gen_params.thermal_time_cste, tmp_data);
	Write_object(MAXON_MOTOR_THERMAL_CST, tmp_data);


	store_uint32(motor_gen_params.gear_max_input_speed, tmp_data);
	Write_object(MAXON_GEAR_MAX_INPUT_SPEED, tmp_data);

	store_uint32(motor_gen_params.gear_reduction_numerator, tmp_data);
	Write_object(MAXON_GEAR_NUMERATOR, tmp_data);

	store_uint32(motor_gen_params.gear_reduction_denominator, tmp_data);
	Write_object(MAXON_GEAR_DENOMINATOR, tmp_data);

	store_uint8(motor_gen_params.nb_pole_pairs, tmp_data);
	Write_object(MAXON_MOTOR_POLE_PAIRS, tmp_data);

	store_int32(motor_gen_params.min_position_limit, tmp_data);
	Write_object(MAXON_SOFTWARE_MIN_POSITION, tmp_data);

	store_int32(motor_gen_params.max_position_limit, tmp_data);
	Write_object(MAXON_SOFTWARE_MAX_POSITION, tmp_data);

}

void motor_config_ppm(void) {
	store_int8(MAXON_MODE_PPM, tmp_data);
	Write_object(MAXON_MODE_OF_OPERATION, tmp_data);

	store_uint32(motor_ppm_params.acceleration, tmp_data);
	Write_object(MAXON_PROFILE_ACCELERATION, tmp_data);

	store_uint32(motor_ppm_params.deceleration, tmp_data);
	Write_object(MAXON_PROFILE_DECELERATION, tmp_data);

	store_uint32(motor_ppm_params.speed, tmp_data);
	Write_object(MAXON_PROFILE_VELOCITY, tmp_data);

	store_int16(PROFILE_TYPE, tmp_data);
	Write_object(MAXON_MOTION_PROFILE_TYPE, tmp_data);
}


void motor_config_homing(void) {
	store_int8(MAXON_MODE_HOMING, tmp_data);
	Write_object(MAXON_MODE_OF_OPERATION, tmp_data);
}

void update_todo(void) {
	if(motor_todo_buf.enable) {
		motor_todo.enable = 1;
	}
	if(motor_todo_buf.disable) {
		motor_todo.disable = 1;
	}
	if(motor_todo_buf.config) {
		motor_todo.config = 1;
	}
	if(motor_todo_buf.startup) {
		motor_todo.startup = 1;
	}
	if(motor_todo_buf.shutdown) {
		motor_todo.shutdown = 1;
	}
	if(motor_todo_buf.set_ppm) {
		motor_todo.set_ppm = 1;
	}
	if(motor_todo_buf.set_homing) {
		motor_todo.set_homing = 1;
	}
	if(motor_todo_buf.start_ppm_operation && motor_todo.start_ppm_operation == 0) {
		motor_todo.start_ppm_operation = 1;
	}
	if(motor_todo_buf.start_homing_operation && motor_todo.start_homing_operation == 0) {
		motor_todo.start_homing_operation = 1;
	}
	if(motor_todo_buf.start_operation && motor_todo.start_operation == 0) {
		motor_todo.start_operation = 1;
	}
}

void motor_mainloop(void * argument) {
	 TickType_t lastWakeTime;
		 const TickType_t period = pdMS_TO_TICKS(HEART_BEAT);

		 lastWakeTime = xTaskGetTickCount();

		for(;;) {
			update_todo();
			read_status_word(&motor_status);
			if(motor_todo.enable) {
				motor_enable();
				motor_todo.enable = 0;
			}
			if(motor_todo.disable) {
				motor_disable();
				motor_todo.disable = 0;
			}
			if(motor_todo.config) {
				motor_config_gen();
				motor_todo.config = 0;
			}
			if(motor_todo.startup) {
				motor_fault_rst();
				motor_shutdown();
				motor_switch_on();
				motor_todo.startup = 0;
			}
			if(motor_todo.shutdown) {
				motor_disable_voltage();
				motor_shutdown();
				motor_todo.shutdown = 0;
			}
			if(motor_todo.set_ppm) {
				motor_config_ppm();
				motor_todo.set_ppm = 0;
			}
			if(motor_todo.set_homing) {
				motor_config_homing();
				motor_todo.set_homing = 0;
			}
			if(motor_todo.start_ppm_operation) {
				//check that the motor is in PPM mode
				int8_t mode;
				motor_get_mode(&mode);
				if(mode == MAXON_MODE_PPM) {
					if(motor_ppm_params.absolute) {
						motor_set_target_abs(motor_ppm_params.target);
					} else {
						motor_set_target_rel(motor_ppm_params.target);
					}

				}else{
					//wrong mode error
				}

				if(SW_TARGET_REACHED(motor_status)) {
					motor_todo.start_operation = 0;
				}
			}
			if(motor_todo.start_homing_operation) {

				//if homing op finished
				motor_todo.start_homing_operation = 0;
			}
			if(motor_todo.start_operation) {
				//check that the motor is in PPM mode
				if(motor_todo.start_operation == 1) {
					int8_t mode;
					motor_get_mode(&mode);
					if(mode == MAXON_MODE_PPM) {
						motor_set_target_abs(motor_ppm_params.half_target);
						motor_todo.start_operation = 2;
					}else{
						motor_todo.start_operation = 0;
					}
				}
				if(motor_todo.start_operation == 2 && SW_TARGET_REACHED(motor_status)) {
					operation_counter += HEART_BEAT;
					if(operation_counter >= motor_ppm_params.half_wait) {
						motor_todo.start_operation = 3;
						operation_counter = 0;
					}
				}
				if(motor_todo.start_operation == 3) {
					motor_set_target_abs(motor_ppm_params.target);
					motor_todo.start_operation = 4;
				}
				if(motor_todo.start_operation == 4 && SW_TARGET_REACHED(motor_status)) {
					operation_counter += HEART_BEAT;
					if(operation_counter >= motor_ppm_params.end_wait) {
						motor_todo.start_operation = 5;
						operation_counter = 0;
					}
				}
				if(motor_todo.start_operation == 5) {
					motor_set_target_abs(0);
					motor_todo.start_operation = 6;
				}
				if(motor_todo.start_operation == 6 && SW_TARGET_REACHED(motor_status)) {
					motor_todo.start_operation = 0;
				}
			}

			//do something with the status

		    vTaskDelayUntil( &lastWakeTime, period );

		}
}

uint16_t motor_get_status(void) {
	return motor_status;
}


void motor_def_enable() {
	motor_todo_buf.enable = 1;
}
void motor_def_disable() {
	motor_todo_buf.disable = 1;
}
void motor_def_config() {
	motor_todo_buf.config = 1;
}
void motor_def_startup() {
	motor_todo_buf.startup = 1;
}
void motor_def_shutdown() {
	motor_todo_buf.shutdown = 1;
}
void motor_def_set_ppm() {
	motor_todo_buf.set_ppm = 1;
}
void motor_def_set_homing() {
	motor_todo_buf.set_homing = 1;
}
void motor_def_start_ppm_operation() {
	motor_todo_buf.start_ppm_operation = 1;
}
void motor_def_start_homing_operation() {
	motor_todo_buf.start_homing_operation = 1;
}
void motor_def_start_operation() {
	motor_todo_buf.start_operation = 1;
}

void motor_register_speed(uint32_t speed) {
	motor_ppm_params.speed = speed;
}
void motor_register_acceleration(uint32_t acc) {
	motor_ppm_params.acceleration = acc;
}
void motor_register_deceleration(uint32_t dec) {
	motor_ppm_params.deceleration = dec;
}
void motor_register_target(int32_t target) {
	motor_ppm_params.target = target;
}

void motor_register_absolute() {
	motor_ppm_params.absolute = 1;
}
void motor_register_relative() {
	motor_ppm_params.absolute = 0;
}












