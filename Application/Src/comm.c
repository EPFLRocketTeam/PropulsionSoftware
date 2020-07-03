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


#define MENU_NAME_LEN 	16
#define LINE_LEN		32
#define NB_MENU_ITEM 	(sizeof(menu)/sizeof(MENU_ITEM_t))


static uint8_t rxBuffer;




typedef struct {
	uint8_t id;
	char name[MENU_NAME_LEN];
	void (*func)(void);
}MENU_ITEM_t;




void show_menu(void);
void toggle_solenoid(void);
void read_sensors(void);
void sys_diag(void);



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
				.id = 2,
				.name = "SYS DIAG",
				.func = sys_diag
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



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	uint8_t i = rxBuffer-'0';
	if(i < NB_MENU_ITEM) {
		menu[i].func();
	}
}


void PP_commInit(void) {
	HAL_UART_Receive_DMA(&huart6, &rxBuffer, 1);
	show_menu();
}




void PP_commFunc(void *argument) {




	for(;;) {

		osDelay(1000);
	}
}
