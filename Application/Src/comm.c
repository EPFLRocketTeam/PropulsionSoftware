/*
 * comm.c
 *
 * author: Iacopo Sprenger
 */

#include <comm.h>
#include "usart.h"
#include <main.h>
#include "cmsis_os.h"

void PP_commFunc(void *argument) {

	const char * msg = "hello\n\r";

	for(;;) {
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
		HAL_UART_Transmit(&huart6, (uint8_t *) msg, sizeof(msg), HAL_MAX_DELAY);
		osDelay(1000);
	}
}
