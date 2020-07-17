#include <maxon_comm.h>
#include "cmsis_os.h"
#include <semphr.h>
#include "usart.h"


static uint16_t crcDataArray[MAX_FRAME_LEN];

static uint8_t recieved_data[MAX_FRAME_LEN*2];
static uint8_t recieved_opcode = 0, recieved_length = 0;


static uint8_t send_frame[MAX_FRAME_LEN*2];

static SemaphoreHandle_t recep_end_sem = NULL;
static StaticSemaphore_t recep_end_semBuffer;



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
uint16_t Create_frame(uint8_t * frameBuffer, uint8_t opcode, uint8_t data_len, uint8_t * data) {
	uint16_t array_len = data_len+2; //we add 1 for the opcode and len fields
	frameBuffer[0] = DLE;
	frameBuffer[1] = STX;
	frameBuffer[2] = opcode;
	frameBuffer[3] = data_len;
	crcDataArray[0] = (data_len<<8) | opcode;
	uint16_t counter=4;
	for(uint16_t i = 0; i < data_len*2; i++) {
		frameBuffer[counter++] = data[i];
		if(frameBuffer[counter-1] == DLE) {
			frameBuffer[counter++] = DLE;
		}
		if(i%2==0) {
			crcDataArray[i/2+1] = (data[i+1]<<8) |  data[i];
		}
	}
	crcDataArray[array_len-1] = 0x0000;
	uint16_t crc = CalcFieldCRC(crcDataArray, array_len);
	frameBuffer[counter++] = crc&0xff;
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



int32_t Decode_frame(uint8_t recvBuffer, uint8_t * opcode, uint8_t * data) {
	//recieve order DLE STX OPCODE LEN DATA[0]LOW DATA[0]HIGH ....
	static uint16_t crc = 0;
	static DECODE_STATE_t d_state = WAITING_DLE;
	static uint8_t stuffing = 0;
	static uint16_t len = 0;
	if(d_state == WAITING_DLE && recvBuffer == DLE) {
		d_state = WAITING_STX;
		return -1;
	}
	if(recvBuffer == DLE) {
		stuffing = 1;
		return -1;
	}
	if(stuffing == 1 && recvBuffer == STX) {
		d_state = WAITING_OPCODE;
		stuffing = 0;
		return -1;
	} else {
		stuffing = 0;
	}
	if(d_state == WAITING_STX && recvBuffer == STX) {
		d_state = WAITING_OPCODE;
		return -1;
	}
	if(d_state == WAITING_OPCODE) {
		*opcode = recvBuffer;
		d_state = WAITING_LEN;
		return -1;
	}
	if(d_state == WAITING_LEN) {
		len = recvBuffer*2;  //mult by 2 to have the length in bytes!
		crcDataArray[0] = 0;
		crcDataArray[0] = ((*opcode)<<8) | recvBuffer;
		d_state = WAITING_DATA;
		return -1;
	}

	if(d_state == WAITING_DATA) {
		if(len==0) {
			d_state = WAITING_CRC1;
		}
		data[--len] = recvBuffer;

		return -1;
	}
	if(d_state == WAITING_CRC1) {
		crc = 0;
		crc = recvBuffer;
		d_state = WAITING_CRC2;
		return -1;
	}
	if(d_state == WAITING_CRC2) {
		crc |= recvBuffer<<8;
		d_state = WAITING_DLE;
		uint16_t array_len = len+1; //we add 1 for the opcode and len fields
		if(crc == CalcFieldCRC(crcDataArray, array_len)) {
			return len;
		}else {
			return 0;
		}
	}

	return -1;

}


void maxon_comm_init(void) {
	recep_end_sem = xSemaphoreCreateBinaryStatic(&recep_end_semBuffer);
}


void Reception(uint8_t recvBuffer) {
	int32_t res = Decode_frame(recvBuffer, &recieved_opcode, recieved_data);
	if(res != -1) {
		xSemaphoreGive(recep_end_sem);
	}
}

//returns the comm error code
uint32_t Write_object(uint16_t index, uint8_t subindex, uint8_t * data) {
	static uint8_t send_data[WRITE_OBJECT_LEN*2];
	static uint16_t length = 0;
	send_data[0] = NODE_ID; //node ID
	send_data[1] = index & 0xff;
	send_data[2] = index >> 8;
	send_data[3] = subindex;
	for(uint8_t i = 0; i < DATA_SIZE; i++){
		send_data[3+i] = data[i];
	}
	length = Create_frame(send_frame, WRITE_OBJECT, WRITE_OBJECT_LEN, send_data);
	HAL_UART_Transmit(&huart1, send_frame, length, 500);
//	if(xSemaphoreTake(recep_end_sem, COMM_TIMEOUT) == pdTRUE) {
//		return recieved_data[0] | (recieved_data[1]<<8) | (recieved_data[2]<<16) | (recieved_data[3]<<24);
//	} else {
//		return -1;
//	}
}

uint32_t Read_object(uint16_t index, uint8_t subindex, uint8_t * data) {
	static uint8_t send_data[READ_OBJECT_LEN*2];
	static uint16_t length = 0;
	send_data[0] = NODE_ID; //node ID
	send_data[1] = index & 0xff;
	send_data[2] = index >> 8;
	send_data[3] = subindex;
	length = Create_frame(send_frame, READ_OBJECT, READ_OBJECT_LEN, send_data);
	HAL_UART_Transmit(&huart1, send_frame, length, 500);
//	if(xSemaphoreTake(recep_end_sem, COMM_TIMEOUT) == pdTRUE) {
//		for(uint8_t i = 0; i < DATA_SIZE; i++){
//				data[i] = recieved_data[3+i];
//		}
//		return recieved_data[0] | (recieved_data[1]<<8) | (recieved_data[2]<<16) | (recieved_data[3]<<24);
//	} else {
//		return -1;
//	}
}



