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
	static uint8_t state = 1;

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
	rxBufferM = 0;
	rxBufferU = 0;
	HAL_UART_Receive_DMA(&MOTOR_UART, &rxBufferM, 1);
	HAL_UART_Receive_DMA(&UI_UART, &rxBufferU, 1);
#if USE_SEM == 1
	uartM_sem = xSemaphoreCreateBinaryStatic( &uartM_semBuffer );
	uartU_sem = xSemaphoreCreateBinaryStatic( &uartU_semBuffer );
#endif
	maxon_comm_init();
	rx_buffer_init(&motor_rx_buffer);
	rx_buffer_init(&user_rx_buffer);
	//menu_context = MENU_SELECTION;
	//PP_setLed(0, 0, 5);
	//osDelay(1000);
}



//I use uart6 instead of 1 because 1 is broken!!!!!
//due to this, the analog reads must be changed!!
void PP_comm6Func(void *argument) {
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



void PP_comm3Func(void *argument) {
	for(;;) {
#if USE_SEM == 1
		if( xSemaphoreTake( uartU_sem, LONG_TIME ) == pdTRUE ) {
#else
		if( ulTaskNotifyTake( pdTRUE, LONG_TIME ) == pdTRUE ) {
#endif
			while(!rx_buffer_is_empty(&user_rx_buffer)) {
				debug_ui_receive(rx_buffer_get(&user_rx_buffer));
			}
			//OLD MENU
			/*
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
			*/
		}
	}
}
