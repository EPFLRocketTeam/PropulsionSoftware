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


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif


void storage_init();

void storage_record_sample();



uint32_t storage_get_used();

void storage_get_sample(uint32_t id, void * dest);

void storage_give_sem();

void storage_thread(void * arg);

void storage_enable();

void storage_restart();

void storage_disable();

void storage_notify();



#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* STORAGE_H */

/* END */
