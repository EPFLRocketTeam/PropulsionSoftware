#ifndef MAXON_COMM_H
#define MAXON_COMM_H

#include <main.h>

//control sequences
#define DLE	0x90
#define STX	0x02
#define READ_OBJECT	0x60
#define READ_OBJECT_LEN 2
#define WRITE_OBJECT 0x68
#define WRITE_OBJECT_LEN 4
#define NODE_ID 0x01
#define DATA_SIZE 4

#define COMM_TIMEOUT 500

#define MAX_FRAME_LEN	64




uint16_t CalcFieldCRC(uint16_t* pDataArray, uint16_t ArrayLength);
uint16_t Create_frame(uint8_t * frameBuffer, uint8_t opcode, uint8_t data_len, uint8_t * data);
int32_t Decode_frame(uint8_t recvBuffer, uint8_t * opcode, uint8_t * data);

uint32_t Write_object(uint16_t index, uint8_t subindex, uint8_t * data);
uint32_t Read_object(uint16_t index, uint8_t subindex, uint8_t * data);

void maxon_comm_init(void);

void Reception(uint8_t recvBuffer);

#endif
