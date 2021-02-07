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


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif


uint32_t storage_read_mem(uint32_t address);

void storage_get_32_samples(uint16_t sample_id, uint8_t * out);

uint32_t storage_get_data_count(void);

uint32_t sorage_get_used_subsectors(void);

void storage_init();

//WRITE sample
//Read sample(s)
//Mutex for only one storage access at the same time !



#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* STORAGE_H */

/* END */
