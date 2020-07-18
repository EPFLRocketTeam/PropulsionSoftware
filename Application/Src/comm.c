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


#define MENU_NAME_LEN 	16
#define LINE_LEN		32
#define NB_MENU_ITEM 	(sizeof(menu)/sizeof(MENU_ITEM_t))


#define LONG_TIME 0xffff
#define MSG_SIZE 64


static uint8_t rxBuffer6;
static uint8_t rxBuffer1;
static uint8_t msgBuffer[MSG_SIZE];


static SemaphoreHandle_t uart6_sem = NULL;
static StaticSemaphore_t uart6_semBuffer;

static SemaphoreHandle_t uart1_sem = NULL;
static StaticSemaphore_t uart1_semBuffer;


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
				.id = 3,
				.name = "SYS DIAG",
				.func = sys_diag
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

	HAL_UART_Transmit(&huart6, (uint8_t *) msg, sizeof(msg), 500);
	HAL_UART_Transmit(&huart6, (uint8_t *) msg2, sizeof(msg2), 500);
	for(int i = 0; i < NB_MENU_ITEM; i++) {
		sprintf(str, "%d: %s\n", i, menu[i].name);
		HAL_UART_Transmit(&huart6, (uint8_t *) str, strlen(str), 500);
	}
	HAL_UART_Transmit(&huart6, (uint8_t *) msg3, sizeof(msg3), 500);

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
	HAL_UART_Transmit(&huart6, (uint8_t *) msg, strlen(msg), 500);
	for(int i = 0; i < PP_NB_SENSOR; i++) {
		sprintf(str, "%s: %d\n", sensor_name[i], PP_getData(i));
		HAL_UART_Transmit(&huart6, (uint8_t *) str, strlen(str), 500);
	}
}

void sys_diag(void) {
	static char msg[] = "\n==SYS DIAG==\n";
	HAL_UART_Transmit(&huart6, (uint8_t *) msg, strlen(msg), 500);

	PP_SYSTEM_REPORT_t report = PP_selfDiagnose();
	static char str[LINE_LEN];

	if(report.s1_plugged) {
		sprintf(str, "S1 PLUGGED: PASS\n");
	} else {
		sprintf(str, "S1 PLUGGED: FAIL\n");
	}
	HAL_UART_Transmit(&huart6, (uint8_t *) str, strlen(str), 500);

	if(report.s2_plugged) {
		sprintf(str, "S2 PLUGGED: PASS\n");
	} else {
		sprintf(str, "S2 PLUGGED: FAIL\n");
	}
	HAL_UART_Transmit(&huart6, (uint8_t *) str, strlen(str), 500);

	if(report.pres1_stable) {
		sprintf(str, "PRESSURE_1 STABLE: PASS\n");
	} else {
		sprintf(str, "PRESSURE_1 STABLE: FAIL\n");
	}
	HAL_UART_Transmit(&huart6, (uint8_t *) str, strlen(str), 500);

	if(report.pres2_stable) {
		sprintf(str, "PRESSURE_2 STABLE: PASS\n");
	} else {
		sprintf(str, "PRESSURE_2 STABLE: FAIL\n");
	}
	HAL_UART_Transmit(&huart6, (uint8_t *) str, strlen(str), 500);

	if(report.temp1_stable) {
		sprintf(str, "TEMPERATURE_1 STABLE: PASS\n");
	} else {
		sprintf(str, "TEMPERATURE_1 STABLE: FAIL\n");
	}
	HAL_UART_Transmit(&huart6, (uint8_t *) str, strlen(str), 500);

	if(report.temp2_stable) {
		sprintf(str, "TEMPERATURE_2 STABLE: PASS\n");
	} else {
		sprintf(str, "TEMPERATURE_2 STABLE: FAIL\n");
	}
	HAL_UART_Transmit(&huart6, (uint8_t *) str, strlen(str), 500);

	if(report.temp3_stable) {
		sprintf(str, "TEMPERATURE_3 STABLE: PASS\n");
	} else {
		sprintf(str, "TEMPERATURE_3 STABLE: FAIL\n");
	}
	HAL_UART_Transmit(&huart6, (uint8_t *) str, strlen(str), 500);
}

void send_msg(void) {
	menu_context = MENU_ENTRY;
}



void maxon_test(void) {
	static uint8_t data[DATA_SIZE];
	Read_object(0x6402, 0x00, data);
	//HAL_UART_Transmit(&huart6, (uint8_t *) data, DATA_SIZE, 500);
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
		static BaseType_t xHigherPriorityTaskWoken6;
		xHigherPriorityTaskWoken6 = pdFALSE;
		xSemaphoreGiveFromISR( uart6_sem, &xHigherPriorityTaskWoken6 );
	}
	if(huart->Instance == huart1.Instance) {
		static BaseType_t xHigherPriorityTaskWoken1;
		xHigherPriorityTaskWoken1 = pdFALSE;
		xSemaphoreGiveFromISR( uart1_sem, &xHigherPriorityTaskWoken1 );
	}

}


void PP_commInit(void) {
	rxBuffer6 = 0;
	rxBuffer1 = 0;
	HAL_UART_Receive_DMA(&huart6, &rxBuffer6, 1);
	HAL_UART_Receive_DMA(&huart1, &rxBuffer1, 1);
	uart6_sem = xSemaphoreCreateBinaryStatic( &uart6_semBuffer );
	uart1_sem = xSemaphoreCreateBinaryStatic( &uart1_semBuffer );

	menu_context = MENU_SELECTION;
	osDelay(1000);
	maxon_test();
}



//I use uart6 instead of 1 because 1 is broken!!!!!
void PP_comm6Func(void *argument) {
	for(;;) {

		if( xSemaphoreTake( uart6_sem, LONG_TIME ) == pdTRUE ) {
//			if(menu_context == MENU_SELECTION) {
//				uint8_t i = rxBuffer6-'0';
//				if(i < NB_MENU_ITEM) {
//					menu[i].func();
//				}
//			} else if(menu_context == MENU_ENTRY){
//				int32_t msg_len = PP_read_entry(rxBuffer6, msgBuffer, MSG_SIZE);
//				if(msg_len != -1) {
//					HAL_UART_Transmit(&huart6, msgBuffer, msg_len, 500);
//					menu_context = MENU_SELECTION;
//				}
//			}
		}
	}
}

void PP_comm1Func(void *argument) {

	for(;;) {
		if( xSemaphoreTake( uart1_sem, LONG_TIME ) == pdTRUE ) {
			Reception(rxBuffer1);
		}

	}
}
