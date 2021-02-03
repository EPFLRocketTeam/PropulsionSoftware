/*
 * comm.c
 *
 * author: Iacopo Sprenger
 */

#include <comm.h>
#include "usart.h"
#include <main.h>
#include "cmsis_os.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sensor.h>
#include <control.h>
#include <semphr.h>
#include <maxon_comm.h>
#include <led.h>
#include <debug_ui.h>
#include <error.h>
#include <can_comm.h>


#define USE_SEM		1  //TASK notify marche pas alors on utilise les semaphores

#define MENU_NAME_LEN 	16
#define LINE_LEN		32
#define NB_MENU_ITEM 	(sizeof(menu)/sizeof(MENU_ITEM_t))


#define LONG_TIME 0xffff
#define MSG_SIZE 64
#define BUFFER_LEN 64



static uint8_t rxBufferM;
static uint8_t rxBufferU;


//universal comm rx buffer
//CIRCULAR BUFFER CODE
typedef struct {
	uint8_t c_ix;
	uint8_t l_ix;
	uint8_t buffer[BUFFER_LEN];
}RX_BUFFER_t;

void rx_buffer_init(RX_BUFFER_t * bfr) {
	bfr->c_ix = 0;
	bfr->l_ix = 0;
}

void rx_buffer_add(RX_BUFFER_t * bfr, uint8_t d) {
	bfr->buffer[bfr->c_ix++] = d; //store recieved in a buffer
	if(bfr->c_ix == BUFFER_LEN) bfr->c_ix = 0; //loop around
}

uint8_t rx_buffer_get(RX_BUFFER_t * bfr) {
	uint8_t tmp = bfr->buffer[bfr->l_ix++];
	if(bfr->l_ix == BUFFER_LEN) bfr->l_ix=0;
	return tmp;
}

uint8_t rx_buffer_is_empty(RX_BUFFER_t * bfr) {
	return bfr->l_ix == bfr->c_ix;
}
//END OF CIRCULAR BUFFER CODE



static RX_BUFFER_t motor_rx_buffer;
static RX_BUFFER_t user_rx_buffer;


#if USE_SEM == 1

static SemaphoreHandle_t uartM_sem = NULL;
static StaticSemaphore_t uartM_semBuffer;

static SemaphoreHandle_t uartU_sem = NULL;
static StaticSemaphore_t uartU_semBuffer;

#endif

static SemaphoreHandle_t ready2send_sem = NULL;
static StaticSemaphore_t ready2send_semBuffer;

SemaphoreHandle_t get_can_sem(void) {
	return ready2send_sem;
}


//investigate SEMAPHORE FROM ISR!!!
//I should Use TASK NOTIFY instead of semaphores as they appear to be faster for
//the purpose of unblocking a task afetr an interrupt has occured.

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	//HAL callbacks are called from ISR so they are part of the ISR!!!
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(huart->Instance == MOTOR_UART.Instance) {
		rx_buffer_add(&motor_rx_buffer, rxBufferM);
#if USE_SEM == 1
		xSemaphoreGiveFromISR( uartM_sem, &xHigherPriorityTaskWoken );
#else
		vTaskNotifyGiveFromISR(PP_comm6Handle, &xHigherPriorityTaskWoken);
#endif
	}

	if(huart->Instance == huart3.Instance) {
		rx_buffer_add(&user_rx_buffer, rxBufferU);
#if USE_SEM == 1
		xSemaphoreGiveFromISR( uartU_sem, &xHigherPriorityTaskWoken );
#else
		vTaskNotifyGiveFromISR(PP_comm3Handle, &xHigherPriorityTaskWoken);
#endif
	}
	//yield from ISR if a higher priority task has  woken!
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}


void PP_commInit(void) {
	//rxtx comm init
	rxBufferM = 0;
	rxBufferU = 0;
	HAL_UART_Receive_DMA(&MOTOR_UART, &rxBufferM, 1);
	HAL_UART_Receive_DMA(&UI_UART, &rxBufferU, 1);
#if USE_SEM == 1
	uartM_sem = xSemaphoreCreateBinaryStatic( &uartM_semBuffer );
	uartU_sem = xSemaphoreCreateBinaryStatic( &uartU_semBuffer );
#endif
	ready2send_sem = xSemaphoreCreateBinaryStatic( &ready2send_semBuffer );

	rx_buffer_init(&motor_rx_buffer);
	rx_buffer_init(&user_rx_buffer);

	//can comm init
	CAN_Config(CAN_ID_PROPULSION_BOARD);

}



//I use uart6 instead of 1 because 1 is broken!!!!!
//due to this, the analog reads must be changed!!
void PP_commMotorFunc(void *argument) {
	for(;;) {
#if USE_SEM == 1
		if( xSemaphoreTake( uartM_sem, LONG_TIME ) == pdTRUE ) {
#else
		if( ulTaskNotifyTake( pdTRUE, LONG_TIME ) == pdTRUE ) {
#endif
			while(!rx_buffer_is_empty(&motor_rx_buffer)) {
				maxon_comm_receive(rx_buffer_get(&motor_rx_buffer));
			}
		}
	}
}



void PP_commUserFunc(void *argument) {
	for(;;) {
#if USE_SEM == 1
		if( xSemaphoreTake( uartU_sem, LONG_TIME ) == pdTRUE ) {
#else
		if( ulTaskNotifyTake( pdTRUE, LONG_TIME ) == pdTRUE ) {
#endif
			while(!rx_buffer_is_empty(&user_rx_buffer)) {
				debug_ui_receive(rx_buffer_get(&user_rx_buffer));
			}
		}
	}
}


void PP_canSendFunc(void *argument) {
	for(;;) {
		if( xSemaphoreTake( ready2send_sem, LONG_TIME ) == pdTRUE ) {

			SENSOR_DATA_t data = sensor_get_data_struct();
			can_setFrame((uint32_t) data.press_1, DATA_ID_PRESS_1, data.time);
			can_setFrame((uint32_t) data.press_2, DATA_ID_PRESS_2, data.time);
			can_setFrame((uint32_t) data.temp_1, DATA_ID_TEMP_1, data.time);
			can_setFrame((uint32_t) data.temp_2, DATA_ID_TEMP_2, data.time);
			can_setFrame((uint32_t) data.temp_3, DATA_ID_TEMP_3, data.time);
			can_setFrame((uint32_t) get_global_status(), DATA_ID_STATUS, data.time);
			can_setFrame((uint32_t) INC2DDEG(motor_get_pos()), DATA_ID_MOT_POS, motor_get_time());



		}
	}

}
