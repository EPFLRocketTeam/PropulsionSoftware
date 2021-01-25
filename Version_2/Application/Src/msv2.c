/*  Title		: Maxon serial v2
 *  Filename	: msv2.c
 *	Author		: iacopo sprenger
 *	Date		: 20.01.2021
 *	Version		: 0.1
 *	Description	: Maxon serial v2 data transport layer
 */

/**********************
 *	INCLUDES
 **********************/

#include <msv2.h>

/**********************
 *	CONFIGURATION
 **********************/

//define serial write
//define serial read

/**********************
 *	CONSTANTS
 **********************/

#define DLE		(0x90)
#define STX		(0x02)



/**********************
 *	MACROS
 **********************/


/**********************
 *	TYPEDEFS
 **********************/

typedef enum MSV2_DECODE_STATE{
	WAITING_DLE,
	WAITING_STX,
	WAITING_OPCODE,
	WAITING_LEN,
	WAITING_DATA,
	WAITING_CRC1,
	WAITING_CRC2
}MSV2_DECODE_STATE_t;

typedef struct MSV2_RX_DATA{
	uint8_t opcode;
	uint8_t data_len;
	uint16_t crc;
	MSV2_DECODE_STATE_t state;
	uint8_t escape;
	uint16_t length;
	uint16_t counter;
	uint8_t data[MSV2_MAX_FRAME_LEN];
}MSV2_RX_DATA_t;

typedef struct MSV2_TX_DATA{
	uint8_t opcode;
	uint8_t data_len;
	uint16_t crc;
	uint8_t data[MSV2_MAX_FRAME_LEN];
	uint16_t crc_data[MSV2_MAX_FRAME_LEN/sizeof(uint16_t)];
}MSV2_TX_DATA_t;

struct MSV2_INST{
	uint32_t id;
	MSV2_RX_DATA_t rx;
	MSV2_TX_DATA_t tx;
};


/**********************
 *	VARIABLES
 **********************/


/**********************
 *	PROTOTYPES
 **********************/

static uint16_t calc_field_CRC(uint16_t* p_data_array, uint16_t ArrayLength);


/**********************
 *	DECLARATIONS
 **********************/

uint16_t calc_field_CRC(uint16_t *p_data_array, uint16_t ArrayLength) {
	uint16_t shifter, c;
	uint16_t carry;
	uint16_t crc = 0;
	while (ArrayLength--) {
		shifter = 0x8000;
		c = *p_data_array++;
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

uint16_t msv2_create_frame(MSV2_INST_t * msv2, uint8_t opcode, uint8_t data_len, uint8_t * data) {
	uint16_t array_len = data_len+2; //we add 1 for the opcode and len fields and 1 for the crc
	msv2->tx.data_len = data_len;
	msv2->tx.opcode = opcode;
	msv2->tx.data[0] = DLE;
	msv2->tx.data[1] = STX;
	msv2->tx.data[2] = opcode;
	msv2->tx.data[3] = data_len;
	msv2->tx.crc_data[0] = (data_len<<8) | opcode;  //header bytes inverted
	uint16_t counter=4;
	for(uint16_t i = 0; i < data_len; i++) {
		msv2->tx.data[counter++] = data[2*i]; //bytes in data need to be inverted before
		if(msv2->tx.data[counter-1] == DLE) {
			msv2->tx.data[counter++] = DLE;
		}
		msv2->tx.data[counter++] = data[2*i+1];
		if(msv2->tx.data[counter-1] == DLE) {
			msv2->tx.data[counter++] = DLE;
		}
		msv2->tx.crc_data[i+1] = (data[2*i+1]<<8) |  data[2*i];
	}
	msv2->tx.crc_data[array_len-1] = 0x0000;
	uint16_t crc = calc_field_CRC(msv2->tx.crc_data, array_len);
	msv2->tx.data[counter++] = crc&0xff; //crc bytes are inverted (LSB first) !!
	if(msv2->tx.data[counter-1] == DLE) {
		msv2->tx.data[counter++] = DLE;
	}
	msv2->tx.data[counter++] = crc>>8;
	if(msv2->tx.data[counter-1] == DLE) {
		msv2->tx.data[counter++] = DLE;
	}
	return counter;
}


/*
 * 	d: received byte
 *
 */
int32_t msv2_decode_fragment(MSV2_INST_t * msv2, uint8_t d) {
    //if a DLE in data is followed by STX, we start again
    if (msv2->rx.escape == 1 && d == STX) {
    	msv2->rx.state = WAITING_OPCODE;
    	msv2->rx.escape = 0;
        return -1;
    }

    if (msv2->rx.state == WAITING_DLE && d == DLE) {
    	msv2->rx.state = WAITING_STX;
        return -1;
    }
    //escape in case a DLE is in the data
    if (d == DLE && msv2->rx.escape == 0) {
    	msv2->rx.escape = 1;
        return -1;
    }
    //if it is doubled, it counts as data
    if (d == DLE && msv2->rx.escape == 1) {
    	msv2->rx.escape = 0;

    }

    if (msv2->rx.state == WAITING_STX && d == STX) {
    	msv2->rx.state = WAITING_OPCODE;
        return -1;
    }

    if (msv2->rx.state == WAITING_OPCODE) {
    	msv2->rx.opcode = d;
    	msv2->rx.state = WAITING_LEN;
        return -1;
    }

    if (msv2->rx.state == WAITING_LEN) {
    	msv2->rx.length = d;
    	msv2->rx.counter = 0;
    	msv2->rx.state = WAITING_DATA;
        return -1;
    }

    if (msv2->rx.state == WAITING_DATA) {
    	msv2->rx.data[msv2->rx.counter] = d;
    	msv2->rx.counter += 1;
        //the length  is in WORDS, but we read BYTES
        if (msv2->rx.counter==2*msv2->rx.length) {
        	msv2->rx.state = WAITING_CRC1;
        }
        return -1;
    }

    if (msv2->rx.state == WAITING_CRC1) {
    	msv2->rx.crc = d;
    	msv2->rx.state = WAITING_CRC2;
        return -1;

    }

    if (msv2->rx.state == WAITING_CRC2) {
    	msv2->rx.crc += d<<8;
    	msv2->rx.state = WAITING_DLE;
        return msv2->rx.length;
    }
    msv2->rx.state=WAITING_DLE;
    return -1;
}


/* END */


