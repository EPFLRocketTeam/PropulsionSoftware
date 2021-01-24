/*
 * storage.h
 *
 * author: Iacopo Sprenger
 *
 */


#ifndef STORAGE_H
#define STORAGE_H

#include <maxon_comm.h>
#include <sensor.h>
#include "cmsis_os.h"
#include <semphr.h>



uint32_t read_mem(uint32_t address);

void storage_start(void);
void storage_stop(void);
void storage_resume(void);

void get_32_samples(uint16_t sample_id, uint8_t * out);

uint32_t get_data_count(void);
SemaphoreHandle_t get_storage_sem(void);

void storage_init();

uint32_t get_used_subsectors(void);

void PP_storageFunc(void *argument);

#endif
