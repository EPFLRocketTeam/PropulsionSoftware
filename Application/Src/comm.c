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


#define MENU_NAME_LEN 	16
#define LINE_LEN		32
#define NB_MENU_ITEM 	(sizeof(menu)/sizeof(MENU_ITEM_t))


#define LONG_TIME 0xffff
#define MSG_SIZE 64
#define BUFFER_LEN 64



static uint8_t rxBuffer6;
static uint8_t rxBuffer1;
static uint8_t rxBuffer3;

static uint8_t current_index = 0;
static uint8_t last_processed_index = 0;
static uint8_t comm_buffer[BUFFER_LEN];


static uint8_t msgBuffer[MSG_SIZE];


static SemaphoreHandle_t uart6_sem = NULL;
static StaticSemaphore_t uart6_semBuffer;

static SemaphoreHandle_t uart1_sem = NULL;
static StaticSemaphore_t uart1_semBuffer;

static SemaphoreHandle_t uart3_sem = NULL;
static StaticSemaphore_t uart3_semBuffer;


//new comm "layout"
/*
 * there will be a "add_to_comm_buffer function that will add strings to a circular buffer
 * the contents of the buffer will be sent to the uart whenever there is need to transmit something.
 * the reception for each uart/can interface is handled in a different thread.
 */




typedef enum {
	MENU_SELECTION,
	MENU_ENTRY
}MENU_CONTEXT_t;




typedef struct {
	uint8_t id;
	char name[MENU_NAME_LEN];
	void (*func)(void);
}MENU_ITEM_t;


static MENU_CONTEXT_t menu_context;

void show_menu(void);
void toggle_solenoid(void);
void read_sensors(void);
void sys_diag(void);
void send_msg(void);
void maxon_test(void);



MENU_ITEM_t menu[] = {
		{
				.id = 0,
				.name = "SHOW MENU",
				.func = show_menu
		},
		{
				.id = 1,
				.name = "TOGGLE SOLENOID",
				.func = toggle_solenoid
		},
		{
				.id = 2,
				.name = "READ SENSORS",
				.func = read_sensors
		},
		{
				.id = 4,
				.name = "SEND MSG",
				.func = send_msg
		},
		{
				.id = 5,
				.name = "TEST_MAXON",
				.func = maxon_test
		},

};


void show_menu(void) {
	static char msg[] = "\nPropulsion Software\nAuthor: Iacopo Sprenger\n";
	static char msg2[] = "==MENU==\n";
	static char msg3[] = ">";
	static char str[LINE_LEN];

	HAL_UART_Transmit(&huart3, (uint8_t *) msg, sizeof(msg), 500);
	HAL_UART_Transmit(&huart3, (uint8_t *) msg2, sizeof(msg2), 500);
	for(int i = 0; i < NB_MENU_ITEM; i++) {
		sprintf(str, "%d: %s\n", i, menu[i].name);
		HAL_UART_Transmit(&huart3, (uint8_t *) str, strlen(str), 500);
	}
	HAL_UART_Transmit(&huart3, (uint8_t *) msg3, sizeof(msg3), 500);

}

void toggle_solenoid(void) {
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
}

void read_sensors(void) {
	static char str[LINE_LEN];
	static char msg[] = "\n==SENSOR VALUES==\n";
	static char * sensor_name[] = {
			"PP_PRESSURE_1   ",
			"PP_PRESSURE_2   ",
			"PP_TEMPERATURE_1",
			"PP_TEMPERATURE_2",
			"PP_TEMPERATURE_3"
	};
	HAL_UART_Transmit(&huart3, (uint8_t *) msg, strlen(msg), 500);
	for(int i = 0; i < PP_NB_SENSOR; i++) {
		sprintf(str, "%s: %d\n", sensor_name[i], PP_getData(i));
		HAL_UART_Transmit(&huart3, (uint8_t *) str, strlen(str), 500);
	}
}



void send_msg(void) {
	menu_context = MENU_ENTRY;
}



void maxon_test(void) {
	static uint8_t data[DATA_SIZE] = {0x00, 18, 0x00, 0x00};
	Write_object(0x6098, 0x00, data);
	Read_object(0x6098, 0x00, data);
	HAL_UART_Transmit(&huart3, (uint8_t *) data, DATA_SIZE, 500);


//	static uint8_t test_data[] = {0x90, 0x02, 0x00, 0x04, 0x90, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0x65, 0x50, 0x23};
//	for(int i = 0; i < sizeof(test_data); i++) {
//		Reception(test_data[i]);
//	}
}



int32_t PP_read_entry(uint8_t entry_buffer, uint8_t * exit_buffer, uint16_t exit_buffer_size) {
	static uint16_t bytes_read = 0;
	if(bytes_read<exit_buffer_size && entry_buffer != '\n') {
		exit_buffer[bytes_read] = entry_buffer;
		bytes_read++;
	}else{
		int32_t tmp = bytes_read;
		bytes_read = 0;
		return tmp;
	}
	return -1;

}







void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	//HAL callbacks are called from ISR so they are part of the ISR!!!

	if(huart->Instance == huart6.Instance) {
		comm_buffer[current_index++] = rxBuffer6; //store recieved in a buffer
		if(current_index == BUFFER_LEN) current_index = 0; //loop around
		static BaseType_t xHigherPriorityTaskWoken6;
		xHigherPriorityTaskWoken6 = pdFALSE;
		xSemaphoreGiveFromISR( uart6_sem, &xHigherPriorityTaskWoken6 );
	}
//	if(huart->Instance == huart1.Instance) {
//		static BaseType_t xHigherPriorityTaskWoken1;
//		xHigherPriorityTaskWoken1 = pdFALSE;
//		xSemaphoreGiveFromISR( uart1_sem, &xHigherPriorityTaskWoken1 );
//	}
	if(huart->Instance == huart3.Instance) {
		static BaseType_t xHigherPriorityTaskWoken3;
		xHigherPriorityTaskWoken3 = pdFALSE;
		xSemaphoreGiveFromISR( uart3_sem, &xHigherPriorityTaskWoken3 );
	}
}


void PP_commInit(void) {
	rxBuffer6 = 0;
	rxBuffer1 = 0;
	rxBuffer3 = 0;
	HAL_UART_Receive_DMA(&huart6, &rxBuffer6, 1);
	//HAL_UART_Receive_DMA(&huart1, &rxBuffer1, 1);
	HAL_UART_Receive_DMA(&huart3, &rxBuffer3, 1);
	uart6_sem = xSemaphoreCreateBinaryStatic( &uart6_semBuffer );
	uart1_sem = xSemaphoreCreateBinaryStatic( &uart1_semBuffer );
	uart3_sem = xSemaphoreCreateBinaryStatic( &uart3_semBuffer );
	maxon_comm_init();
	menu_context = MENU_SELECTION;
	show_menu();
	PP_setLed(0, 0, 5);
	//osDelay(1000);
}



//I use uart6 instead of 1 because 1 is broken!!!!!
void PP_comm6Func(void *argument) {
	for(;;) {

		if( xSemaphoreTake( uart6_sem, LONG_TIME ) == pdTRUE ) {
			while(last_processed_index != current_index) {
				Reception(comm_buffer[last_processed_index++]);
				if(last_processed_index == BUFFER_LEN) last_processed_index=0;
			}
		}
	}
}

void PP_comm3Func(void *argument) {
	for(;;) {

		if( xSemaphoreTake( uart3_sem, LONG_TIME ) == pdTRUE ) {
			if(menu_context == MENU_SELECTION) {
				uint8_t i = rxBuffer3-'0';
				if(i < NB_MENU_ITEM) {
					menu[i].func();
				}
			} else if(menu_context == MENU_ENTRY){
				int32_t msg_len = PP_read_entry(rxBuffer3, msgBuffer, MSG_SIZE);
				if(msg_len != -1) {
					HAL_UART_Transmit(&huart3, msgBuffer, msg_len, 500);
					menu_context = MENU_SELECTION;
				}
			}
		}
	}
}

void PP_comm1Func(void *argument) {

	for(;;) {
		if( xSemaphoreTake( uart1_sem, LONG_TIME ) == pdTRUE ) {

		}

	}
}
