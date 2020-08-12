/*
 * comm.h
 *
 * author: Iacopo Sprenger
 */

#ifndef COMM_H
#define COMM_H

#include "cmsis_os.h"

osThreadId_t PP_comm6Handle;
osThreadId_t PP_comm3Handle;


void PP_commInit(void);
void PP_comm3Func(void *argument);
void PP_comm6Func(void *argument);


#endif
