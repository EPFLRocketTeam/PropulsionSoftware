/*  Title       : Dynamixel Serial v2
 *  Filename    : dsv2.h
 *  Author      : iacopo sprenger
 *  Date        : 22.03.2021
 *  Version     : 0.1
 *  Description : Dynamixel serial v2 data transport layer
 */

#ifndef DSV2_H
#define DSV2_H

/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>
#include <serial.h>

/**********************
 *  CONSTANTS
 **********************/

#define DSV2_MAX_FRAME_LEN	(1024)

#define DSV2_MAX_DATA_LEN	(512)


/**********************
 *  MACROS
 **********************/


/**********************
 *  TYPEDEFS
 **********************/

typedef enum DSV2_ERROR {
	DSV2_SUCCESS = 0,
	DSV2_PROGRESS,
	DSV2_WRONG_CRC,
	DSV2_ERROR
}DSV2_ERROR_t;

typedef enum DSV2_DECODE_STATE{
	DSV2_WAITING_H1,
	DSV2_WAITING_H2,
	DSV2_WAITING_H3,
	DSV2_WAITING_H4,
	DSV2_WAITING_ID,
	DSV2_WAITING_LEN1,
	DSV2_WAITING_LEN2,
	DSV2_WAITING_INST,
	DSV2_WAITING_DATA,
	DSV2_WAITING_CRC1,
	DSV2_WAITING_CRC2
}DSV2_DECODE_STATE_t;

typedef struct DSV2_RX_DATA{
	uint8_t dev_id;
	uint16_t data_len;
	uint16_t crc;
	uint8_t inst;
	DSV2_DECODE_STATE_t state;
	DSV2_DECODE_STATE_t restart_state;
	uint8_t escape;
	uint16_t length;
	uint16_t counter;
	uint8_t data[DSV2_MAX_FRAME_LEN];
	uint8_t crc_data[DSV2_MAX_FRAME_LEN];
}DSV2_RX_DATA_t;

typedef struct DSV2_TX_DATA{
	uint8_t dev_id;
	uint16_t data_len;
	uint16_t crc;
	uint8_t data[DSV2_MAX_FRAME_LEN];
}DSV2_TX_DATA_t;

typedef struct DSV2_INST{
	uint32_t id;
	DSV2_RX_DATA_t rx;
	DSV2_TX_DATA_t tx;
}DSV2_INST_t;




/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif

SERIAL_RET_t dsv2_decode_func(void * inst, uint8_t data);

DSV2_ERROR_t dsv2_decode_fragment(DSV2_INST_t * dsv2, uint8_t d);

void dsv2_init(DSV2_INST_t * dsv2);

uint16_t dsv2_create_frame(DSV2_INST_t * dsv2, uint8_t dev_id, uint16_t data_len, uint8_t inst,  uint8_t * data);

uint8_t * dsv2_rx_data(DSV2_INST_t * dsv2);

uint8_t * dsv2_tx_data(DSV2_INST_t * dsv2);

#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* DSV2_H */

/* END */
