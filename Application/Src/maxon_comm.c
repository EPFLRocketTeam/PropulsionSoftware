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

static SemaphoreHandle_t recep_end_sem = NULL;
static StaticSemaphore_t recep_end_semBuffer;


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
}


void maxon_comm_init(void) {
	recep_end_sem = xSemaphoreCreateBinaryStatic(&recep_end_semBuffer);
}


void Reception(uint8_t recvBuffer) {
	int32_t res = Decode_frame(recvBuffer, &recieved_opcode, recieved_data, &recieved_crc);
	if(res != -1) {
		recieved_length = res;
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
	HAL_UART_Transmit(&huart6, send_frame, length, 500);
	if(xSemaphoreTake(recep_end_sem, 0xffff) == pdTRUE) {
		return recieved_data[0] | (recieved_data[1]<<8) | (recieved_data[2]<<16) | (recieved_data[3]<<24);
	} else {
		return -1;
	}
}

uint32_t Read_object(uint16_t index, uint8_t subindex, uint8_t * data) {
	static uint8_t send_data[READ_OBJECT_LEN*2];
	static uint16_t length = 0;
	send_data[0] = NODE_ID; //node ID
	send_data[1] = index & 0xff;  //LSB first
	send_data[2] = index >> 8;
	send_data[3] = subindex;
	length = Create_frame(send_frame, READ_OBJECT, READ_OBJECT_LEN, send_data);
	HAL_UART_Transmit(&huart6, send_frame, length, 500);

	if(xSemaphoreTake(recep_end_sem, 0xffff) == pdTRUE) {
		for(uint8_t i = 0; i < DATA_SIZE; i++){
				data[i] = recieved_data[3+i];
		}
		return recieved_data[0] | (recieved_data[1]<<8) | (recieved_data[2]<<16) | (recieved_data[3]<<24);
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
	data[0] = value & 0xff;
	data[1] = (value>>8) & 0xff;
	data[2] = 0x00;
	data[3] = 0x00;
}

void store_uint32(uint32_t value, uint8_t * data) {
	data[0] = value & 0xff;
	data[1] = (value>>8) & 0xff;
	data[2] = (value>>16) & 0xff;
	data[3] = (value>>24) & 0xff;

}

void store_int8(int8_t value, uint8_t * data) {
	data[0] = value;
	data[1] = 0x00;
	data[2] = 0x00;
	data[3] = 0x00;
}

void store_int16(int16_t value, uint8_t * data) {
	data[0] = value & 0xff;
	data[1] = (value>>8) & 0xff;
	data[2] = 0x00;
	data[3] = 0x00;
}

void store_int32(int32_t value, uint8_t * data) {
	data[0] = value & 0xff;
	data[1] = (value>>8) & 0xff;
	data[2] = (value>>16) & 0xff;
	data[3] = (value>>24) & 0xff;
}



//MAXON MOTOR SETUP COMMANDS

#include <maxon_def.h>


static uint8_t tmp_data[DATA_SIZE];

#define MAX_POS			1000000
#define MIN_POS			0
#define MAX_PROFILE_VEL	1000
#define MAX_MOTOR_SPEED	10000
#define QUICK_STOP_DEC	10000
#define MAX_ACC			50000

#define PROFILE_ACC 	9999
#define PROFILE_DEC 	9989
#define PROFILE_VEL 	10000
#define PROFILE_TYPE	0


void motor_config_ppm() {

	store_uint8(MAXON_MODE_PPM, tmp_data);
	Write_object(MAXON_MODE_OF_OPERATION, tmp_data);

	store_int32(MAX_POS, tmp_data);
	Write_object(MAXON_SOFTWARE_MAX_POSITION, tmp_data);

	store_int32(MIN_POS, tmp_data);
	Write_object(MAXON_SOFTWARE_MIN_POSITION, tmp_data);

	store_uint32(MAX_PROFILE_VEL, tmp_data);
	Write_object(MAXON_MAX_PROFILE_VELOCITY, tmp_data);

	store_uint32(MAX_MOTOR_SPEED, tmp_data);
	Write_object(MAXON_MAX_MOTOR_SPEED, tmp_data);

	store_uint32(QUICK_STOP_DEC, tmp_data);
	Write_object(MAXON_QUICKSTOP_DECELERATION, tmp_data);

	store_uint32(MAX_ACC, tmp_data);
	Write_object(MAXON_MAX_ACCELERATION, tmp_data);

	store_uint32(PROFILE_ACC, tmp_data);
	Write_object(MAXON_PROFILE_ACCELERATION, tmp_data);

	store_uint32(PROFILE_DEC, tmp_data);
	Write_object(MAXON_PROFILE_DECELERATION, tmp_data);

	store_uint32(PROFILE_VEL, tmp_data);
	Write_object(MAXON_PROFILE_VELOCITY, tmp_data);

	store_uint32(PROFILE_TYPE, tmp_data);
	Write_object(MAXON_MOTION_PROFILE_TYPE, tmp_data);

}

void motor_set_target(int32_t pos) {
	store_int32(pos, tmp_data);
	Write_object(MAXON_TARGET_POSITION, tmp_data);
}

void motor_enable() {

	store_uint16(0x0F, tmp_data);
	Write_object(MAXON_CONTROL_WORD, tmp_data);

}

void motor_disable() {
	store_uint16(0x07, tmp_data);
	Write_object(MAXON_CONTROL_WORD, tmp_data);

}

void motor_quickstop() {
	store_uint16(0b0010, tmp_data);
	Write_object(MAXON_CONTROL_WORD, tmp_data);

}














