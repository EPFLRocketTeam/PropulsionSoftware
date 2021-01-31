/*  Title       : Maxon Serial v2
 *  Filename    : msv2.h
 *  Author      : iacopo sprenger
 *  Date        : 20.01.2021
 *  Version     : 0.1
 *  Description : Maxon serial v2 data transport layer
 */

#ifndef MSV2_H
#define MSV2_H

/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>

/**********************
 *  CONSTANTS
 **********************/

#define MSV2_MAX_FRAME_LEN	(1024)


/**********************
 *  MACROS
 **********************/


/**********************
 *  TYPEDEFS
 **********************/

typedef enum MSV2_ERROR {
	MSV2_SUCCESS = 0,
	MSV2_PROGRESS,
	MSV2_WRONG_CRC,
	MSV2_ERROR
}MSV2_ERROR_t;

typedef struct MSV2_INST MSV2_INST_t;




/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif

MSV2_ERROR_t msv2_decode_fragment(MSV2_INST_t * msv2, uint8_t d);

void msv2_init(MSV2_INST_t * msv2);

uint16_t msv2_create_frame(MSV2_INST_t * msv2, uint8_t opcode, uint8_t data_len, uint8_t * data);

uint8_t * msv2_rx_data(MSV2_INST_t * msv2);

uint8_t * msv2_tx_data(MSV2_INST_t * msv2);

#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* MSV2_H */

/* END */
