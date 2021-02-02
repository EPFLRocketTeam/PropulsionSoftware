/*  Title		: Debug
 *  Filename	: debug.c
 *	Author		: iacopo sprenger
 *	Date		: 20.01.2021
 *	Version		: 0.1
 *	Description	: debug interface code
 *	What needs to be available:
 *	Access the internal state
 *	Trigger ignition and flight sequence
 *	Trigger calibration
 *	Read sensor data
 *	Read stored data
 *	Configure ignition sequence parameters
 */

/**********************
 *	INCLUDES
 **********************/

#include <debug.h>
#include <usart.h>
#include <control.h>


/**********************
 *	CONSTANTS
 **********************/

#define DEBUG_UART huart3


#define PP_PARAMS_LEN (36)


#define ERROR_LO	(0xce)
#define ERROR_HI	(0xec)

#define OK_LO	(0xc5)
#define OK_HI	(0x5c)

/**********************
 *	MACROS
 **********************/


/**********************
 *	TYPEDEFS
 **********************/


/**********************
 *	VARIABLES
 **********************/


/**********************
 *	PROTOTYPES
 **********************/

//debug routines
static void debug_read_state(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len);
static void debug_move_abs(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len);
static void debug_move_rel(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len);
static void debug_move_abs_imm(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len);
static void debug_move_rel_imm(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len);
static void debug_set_pp_params(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len);
static void debug_get_pp_params(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len);

/**********************
 *	DEBUG FCN ARRAY
 **********************/
void (*debug_fcn[]) (uint8_t *, uint16_t, uint8_t *, uint16_t *) = {
		debug_read_state,	//0x00
		debug_move_abs,		//0x01
		debug_move_rel,		//0x02
		debug_move_abs_imm,	//0x03
		debug_move_rel_imm,	//0x04
		debug_set_pp_params,//0x05
		debug_get_pp_params //0x06
};

/**********************
 *	DECLARATIONS
 **********************/


//Requires an instance of type debug
SERIAL_RET_t debug_decode_fcn(void * inst, uint8_t data) {
	static uint8_t send_data[MSV2_MAX_DATA_LEN];
	static uint16_t length = 0;
	static uint16_t bin_length = 0;
	DEBUG_INST_t * debug = (DEBUG_INST_t *) inst;
	MSV2_ERROR_t tmp = msv2_decode_fragment(&debug->msv2, data);

	if(tmp == MSV2_SUCCESS) {

		debug_fcn[debug->msv2.rx.opcode](debug->msv2.rx.data, debug->msv2.rx.length, send_data, &length);
		//length is in words
		bin_length = msv2_create_frame(&debug->msv2, debug->msv2.rx.opcode, length/2, send_data);
		serial_send(&debug->ser, msv2_tx_data(&debug->msv2), bin_length);
	}

	if(tmp == MSV2_WRONG_CRC) {
		send_data[0] = ERROR_LO;
		send_data[1] = ERROR_HI;
		length = 1; //in words
		bin_length = msv2_create_frame(&debug->msv2, debug->msv2.rx.opcode, length, send_data);
		serial_send(&debug->ser, msv2_tx_data(&debug->msv2), bin_length);
	}

	return tmp;
}

void debug_init(DEBUG_INST_t * debug) {
	static uint32_t id_counter = 0;
	msv2_init(&debug->msv2);
	serial_init(&debug->ser, &DEBUG_UART, debug, debug_decode_fcn);
	debug->id = id_counter++;
}

static void debug_read_state(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len) {
	CONTROL_STATE_t state = control_get_state();
	resp[0] = state;
	resp[1] = 0x00;
	*resp_len = 2;
}

static void debug_move_abs(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len) {

}

static void debug_move_rel(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len) {

}

static void debug_move_abs_imm(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len) {

}

static void debug_move_rel_imm(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len) {

}

static void debug_set_pp_params(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len) {
	if(data_len == PP_PARAMS_LEN){
		CONTROL_PP_PARAMS_t params;
		params.acc = util_decode_u32(data);
		params.dec = util_decode_u32(data+4);
		params.speed = util_decode_u32(data+8);
		params.half_speed = util_decode_u32(data+12);
		params.countdown_wait = util_decode_u32(data+16);
		params.half_wait = util_decode_u32(data+20);
		params.full_wait = util_decode_u32(data+24);
		params.half_angle = util_decode_i32(data+28);
		params.full_angle = util_decode_i32(data+32);
		control_set_pp_params(params);
		resp[0] = OK_LO;
		resp[1] = OK_HI;
		*resp_len = 2;
	} else {
		resp[0] = ERROR_LO;
		resp[1] = ERROR_HI;
		*resp_len = 2;
	}
}

static void debug_get_pp_params(uint8_t * data, uint16_t data_len, uint8_t * resp, uint16_t * resp_len) {
	CONTROL_PP_PARAMS_t params = control_get_pp_params();
	util_encode_u32(resp, params.acc);
	util_encode_u32(resp+4, params.dec);
	util_encode_u32(resp+8, params.speed);
	util_encode_u32(resp+12, params.half_speed);
	util_encode_u32(resp+16, params.countdown_wait);
	util_encode_u32(resp+20, params.half_wait);
	util_encode_u32(resp+24, params.full_wait);
	util_encode_i32(resp+28, params.half_angle);
	util_encode_i32(resp+32, params.full_angle);
	*resp_len = PP_PARAMS_LEN;
}

/* END */


