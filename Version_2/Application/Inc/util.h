/*  Title       : Utilities
 *  Filename    : util.h
 *  Author      : iacopo sprenger
 *  Date        : 28.01.2021
 *  Version     : 0.1
 *  Description : utilities
 */

#ifndef UTIL_H
#define UTIL_H



/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>
#include <sensor.h>

/**********************
 *  CONSTANTS
 **********************/


/**********************
 *  MACROS
 **********************/

#define UTIL_GENERATE_BUFFER(type, name)   	\
	typedef struct UTIL_BUFFER_##name{   	\
		uint16_t c_ix;                	\
		uint16_t l_ix;            		\
		uint16_t bfr_len;           	\
		type * buffer;            		\
	}UTIL_BUFFER_##name##_t;             	\
static inline void util_buffer_##name##_init(UTIL_BUFFER_##name##_t * bfr, type * buffer, uint16_t bfr_len) { \
	bfr->c_ix = 0;                                                                                   \
	bfr->l_ix = 0;                                                                                   \
	bfr->bfr_len = bfr_len;                                                                          \
	bfr->buffer = buffer;                                                                            \
}                                                                                                    \
static inline void util_buffer_##name##_add(UTIL_BUFFER_##name##_t * bfr, type d) {                          \
	bfr->buffer[bfr->c_ix++] = d;                                                                    \
	if(bfr->c_ix == bfr->bfr_len) bfr->c_ix = 0;                                                     \
}                                                                                                    \
static inline type util_buffer_##name##_get(UTIL_BUFFER_##name##_t * bfr) {                               \
	type tmp = bfr->buffer[bfr->l_ix++];                                                          	 \
	if(bfr->l_ix == bfr->bfr_len) bfr->l_ix=0;                                                       \
	return tmp;                                                                                      \
}                                                                                                    \
static inline uint8_t util_buffer_##name##_isempty(UTIL_BUFFER_##name##_t * bfr) {                           \
	return bfr->l_ix == bfr->c_ix;                                                                   \
}

/**********************
 *  TYPEDEFS
 **********************/

typedef struct UTIL_BUFFER_U8{
	uint16_t c_ix;
	uint16_t l_ix;
	uint16_t bfr_len;
	uint8_t * buffer;
}UTIL_BUFFER_U8_t;

typedef struct UTIL_BUFFER_U16{
	uint16_t c_ix;
	uint16_t l_ix;
	uint16_t bfr_len;
	uint16_t * buffer;
}UTIL_BUFFER_U16_t;

typedef struct UTIL_BUFFER_I16{
	uint16_t c_ix;
	uint16_t l_ix;
	uint16_t bfr_len;
	int16_t * buffer;
}UTIL_BUFFER_I16_t;




/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif


//U8 BUFFER
static inline void util_buffer_u8_init(UTIL_BUFFER_U8_t * bfr, uint8_t * buffer, uint16_t bfr_len) {
	bfr->c_ix = 0;
	bfr->l_ix = 0;
	bfr->bfr_len = bfr_len;
	bfr->buffer = buffer;
}

static inline void util_buffer_u8_add(UTIL_BUFFER_U8_t * bfr, uint8_t d) {
	bfr->buffer[bfr->c_ix++] = d;
	if(bfr->c_ix == bfr->bfr_len) bfr->c_ix = 0;
}

static inline uint8_t util_buffer_u8_get(UTIL_BUFFER_U8_t * bfr) {
	uint8_t tmp = bfr->buffer[bfr->l_ix++];
	if(bfr->l_ix == bfr->bfr_len) bfr->l_ix=0;
	return tmp;
}

static inline uint8_t util_buffer_u8_isempty(UTIL_BUFFER_U8_t * bfr) {
	return bfr->l_ix == bfr->c_ix;
}

//U16 BUFFER
static inline void util_buffer_u16_init(UTIL_BUFFER_U16_t * bfr, uint16_t * buffer, uint16_t bfr_len) {
	bfr->c_ix = 0;
	bfr->l_ix = 0;
	bfr->bfr_len = bfr_len;
	bfr->buffer = buffer;
}

static inline void util_buffer_u16_add(UTIL_BUFFER_U16_t * bfr, uint16_t d) {
	bfr->buffer[bfr->c_ix++] = d;
	if(bfr->c_ix == bfr->bfr_len) bfr->c_ix = 0;
}

static inline uint16_t util_buffer_u16_get(UTIL_BUFFER_U16_t * bfr) {
	uint16_t tmp = bfr->buffer[bfr->l_ix++];
	if(bfr->l_ix == bfr->bfr_len) bfr->l_ix=0;
	return tmp;
}

static inline uint8_t util_buffer_u16_isempty(UTIL_BUFFER_U16_t * bfr) {
	return bfr->l_ix == bfr->c_ix;
}

//I16 BUFFER
static inline void util_buffer_i16_init(UTIL_BUFFER_I16_t * bfr, int16_t * buffer, uint16_t bfr_len) {
	bfr->c_ix = 0;
	bfr->l_ix = 0;
	bfr->bfr_len = bfr_len;
	bfr->buffer = buffer;
}

static inline void util_buffer_i16_add(UTIL_BUFFER_I16_t * bfr, int16_t d) {
	bfr->buffer[bfr->c_ix++] = d;
	if(bfr->c_ix == bfr->bfr_len) bfr->c_ix = 0;
}

static inline int16_t util_buffer_i16_get(UTIL_BUFFER_I16_t * bfr) {
	int16_t tmp = bfr->buffer[bfr->l_ix++];
	if(bfr->l_ix == bfr->bfr_len) bfr->l_ix=0;
	return tmp;
}

static inline uint8_t util_buffer_i16_isempty(UTIL_BUFFER_I16_t * bfr) {
	return bfr->l_ix == bfr->c_ix;
}


#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* UTIL_H */

/* END */
