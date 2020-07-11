#include <maxon_comm.h>



static uint16_t crcDataArray[MAX_FRAME_LEN];



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
			if (c & shifter)
				crc++;
			if (carry)
				crc ^= 0x1021;
			shifter >>= 1;

		} while (shifter);

	}
	return crc;
}




//returns length in BYTES for transmission!!
//CRC calculated before sync and before data stuffing!!
//framebuffer size must be MAX_FRAME_LEN
//data_len in WORDS
uint16_t Create_frame(uint8_t * frameBuffer, uint8_t opcode, uint8_t data_len, uint16_t * data) {
	uint16_t array_len = data_len+1; //we add 1 for the opcode and len fields
	frameBuffer[0] = DLE;
	frameBuffer[1] = STX;
	frameBuffer[2] = opcode;
	frameBuffer[3] = data_len;
	crcDataArray[0] = frameBuffer[2];
	uint16_t counter=4;
	for(uint16_t i = 0; i < data_len; i++) {
		frameBuffer[counter++] = data[i];
		if(frameBuffer[counter-1] == DLE) {
			frameBuffer[counter++] = DLE;
		}
		frameBuffer[counter++] = data[i]>>8;
		crcDataArray[i+1] = data[i];
		if(frameBuffer[counter-1] == DLE) {
			frameBuffer[counter++] = DLE;
		}

	}
	uint16_t crc = CalcFieldCRC(crcDataArray, array_len);
	frameBuffer[counter++] = crc;
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



int32_t Decode_frame(uint8_t recvBuffer, uint8_t * opcode, uint16_t * data) {
	//recieve order DLE STX OPCODE LEN DATA[0]LOW DATA[0]HIGH ....
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
		d_state = WAITING_DATA;
		return -1;
	}

	if(d_state == WAITING_DATA) {
		if(len==0) {
			d_state == WAITING_CRC1;
		}
		if(len%2 == 1) {
			data[--len] |= recvBuffer<<8;
		} else {
			data[--len] |= recvBuffer;
		}
		return -1;
	}
	if(d_state == WAITING_CRC1) {
		d_state = WAITING_CRC2;
		return -1;
	}
	if(d_state == WAITING_CRC2) {
		d_state = WAITING_DLE;
		return 0;
	}

	return -1;




}




