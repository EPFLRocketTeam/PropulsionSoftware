/*  Title       : Template
 *  Filename    : template.h
 *  Author      : iacopo sprenger
 *  Date        : 07.02.2021
 *  Version     : 0.1
 *  Description : storage on the onboard flash memory
 *  			  Requires FlashAPI
 */

#ifndef STORAGE_H
#define STORAGE_H



/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>

/**********************
 *  CONSTANTS
 **********************/


/**********************
 *  MACROS
 **********************/


/**********************
 *  TYPEDEFS
 **********************/

typedef struct STORAGE_INST {
	uint32_t id;
	uint32_t used_subsectors;
	uint32_t data_counter;
}STORAGE_INST_t;


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif


void storage_init(STORAGE_INST_t * store);



#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* STORAGE_H */

/* END */
