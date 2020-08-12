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
static uint8_t rxBuffer3;


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



static uint8_t msgBuffer[MSG_SIZE];


static SemaphoreHandle_t uart6_sem = NULL;
static StaticSemaphore_t uart6_semBuffer;

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
				.id = 5,
				.name = "SETUP_MOTOR",
				.func = motor_config_ppm
		},
		{
				.id = 5,
				.name = "ENABLE MOTOR",
				.func = motor_enable
		},
		{
				.id = 5,
				.name = "DISABLE MOTOR",
				.func = motor_disable
		},
		{
				.id = 5,
				.name = "STOP MOTOR",
				.func = motor_quickstop
		},
		{
				.id = 5,
				.name = "RUN MOTOR",
				.func = maxon_test
		},
		{
				.id = 5,
				.name = "VOLTAGE OFF",
				.func = motor_disable_voltage
		},
		{
				.id = 5,
				.name = "RESET",
				.func = motor_fault_rst
		}

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
	uint8_t data[DATA_SIZE];
	read_status_word(data);
	static uint8_t state = 1;
	//HAL_UART_Transmit(&huart3, data, DATA_SIZE, 500);
	if(state) {
		motor_set_target(100000);
	} else {
		motor_set_target(0);
	}
	state = !state;
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





//investigate SEMAPHORE FROM ISR!!!
//I should Use TASK NOTIFY instead of semaphores as they appear to be faster for
//the purpose of unblocking a task afetr an interrupt has  occured.

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	//HAL callbacks are called from ISR so they are part of the ISR!!!
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(huart->Instance == huart6.Instance) {
		rx_buffer_add(&motor_rx_buffer, rxBuffer6);
		//xSemaphoreGiveFromISR( uart6_sem, &xHigherPriorityTaskWoken );
		vTaskNotifyGiveFromISR(PP_comm6Handle, &xHigherPriorityTaskWoken);
	}

	if(huart->Instance == huart3.Instance) {
		//xSemaphoreGiveFromISR( uart3_sem, &xHigherPriorityTaskWoken );
		vTaskNotifyGiveFromISR(PP_comm3Handle, &xHigherPriorityTaskWoken);
	}
	//yield from ISR if a higher priority task has  woken!
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}


void PP_commInit(void) {
	rxBuffer6 = 0;
	rxBuffer3 = 0;
	HAL_UART_Receive_DMA(&huart6, &rxBuffer6, 1);
	HAL_UART_Receive_DMA(&huart3, &rxBuffer3, 1);
	uart6_sem = xSemaphoreCreateBinaryStatic( &uart6_semBuffer );
	uart3_sem = xSemaphoreCreateBinaryStatic( &uart3_semBuffer );
	maxon_comm_init();
	rx_buffer_init(&motor_rx_buffer);
	rx_buffer_init(&user_rx_buffer);
	menu_context = MENU_SELECTION;
	show_menu();
	PP_setLed(0, 0, 5);
	//osDelay(1000);
}



//I use uart6 instead of 1 because 1 is broken!!!!!
//due to this, the analog reads must be changed!!
void PP_comm6Func(void *argument) {
	for(;;) {
		//if( xSemaphoreTake( uart6_sem, LONG_TIME ) == pdTRUE ) {
		if( ulTaskNotifyTake( pdTRUE, LONG_TIME ) == pdTRUE ) {
			while(!rx_buffer_is_empty(&motor_rx_buffer)) {
				maxon_comm_receive(rx_buffer_get(&motor_rx_buffer));
			}
		}
	}
}



void PP_comm3Func(void *argument) {
	for(;;) {

		//if( xSemaphoreTake( uart3_sem, LONG_TIME ) == pdTRUE ) {
		if( ulTaskNotifyTake( pdTRUE, LONG_TIME ) == pdTRUE ) {
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
