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



uint16_t CalcFieldCRC(uint16_t* pDataArray, uint16_t ArrayLength);
uint16_t Create_frame(uint8_t * frameBuffer, uint8_t opcode, uint8_t data_len, uint8_t * data);
int32_t Decode_frame(uint8_t d, uint8_t * opcode, uint8_t * data, uint16_t * crc);
uint32_t Write_object(uint16_t index, uint8_t subindex, uint8_t * data);
uint32_t Read_object(uint16_t index, uint8_t subindex, uint8_t * data);

void maxon_comm_init(void);
void maxon_comm_receive(uint8_t recvBuffer);

//functions
void motor_config_gen();
void motor_config_ppm();
void motor_setup_ppm(uint32_t profile_acc, uint32_t profile_dec, uint32_t profile_vel);
void motor_set_target(int32_t pos);
void motor_enable();
void motor_disable();
void motor_quickstop();
void motor_disable_voltage();
void motor_shutdown();
void motor_switch_on();
void motor_fault_rst();
void motor_set_rel();
void motor_set_abs();
void motor_new_pos();
uint8_t read_status_word(uint16_t * data);
void read_max_prof_vel(uint8_t * data);


uint8_t get_busy_state(void);


#endif
