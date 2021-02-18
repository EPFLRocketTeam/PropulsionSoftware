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
#include <cmsis_os.h>


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
	SemaphoreHandle_t ready_sem;
	StaticSemaphore_t ready_sem_buffer;
	uint8_t record_active;
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

void storage_record_sample(STORAGE_INST_t * store);

uint32_t storage_get_used();

void storage_get_sample(STORAGE_INST_t * store, uint32_t id, void * dest);

SemaphoreHandle_t storage_get_sem(STORAGE_INST_t * store);

STORAGE_INST_t * storage_get_inst();

void storage_thread(void * arg);

void storage_enable();

void storage_disable();



#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* STORAGE_H */

/* END */
