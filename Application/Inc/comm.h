/*
 * comm.h
 *
 * author: Iacopo Sprenger
 */

#ifndef COMM_H
#define COMM_H

#include "cmsis_os.h"
#include <semphr.h>


osThreadId_t PP_commMotorHandle;
osThreadId_t PP_commUserHandle;

/*
 * @brief	initialize communication module
 */
void PP_commInit(void);

/*
 * @brief	user uart comm main function
 */
void PP_commUserFunc(void *argument);

/*
 * @brief	motor uart comm main function
 */
void PP_commMotorFunc(void *argument);

void PP_canSendFunc(void *argument);

SemaphoreHandle_t get_can_sem(void);


#endif
