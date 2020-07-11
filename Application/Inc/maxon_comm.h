#ifndef MAXON_COMM_H
#define MAXON_COMM_H

#include <main.h>

//control sequences
#define DLE	0x90
#define STX	0x02

#define MAX_FRAME_LEN	128




uint16_t CalcFieldCRC(uint16_t* pDataArray, uint16_t ArrayLength);
uint16_t Create_frame(uint8_t * frameBuffer, uint8_t opcode, uint8_t data_len, uint16_t * data);
int32_t Decode_frame(uint8_t recvBuffer, uint8_t * opcode, uint16_t * data);

#endif
