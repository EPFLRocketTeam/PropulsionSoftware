/*	led.c
 *
 * author: Iacopo Sprenger
 *
 *
 */


#include <led.h>
#include <main.h>
#include "tim.h"





void PP_initLed(void) {
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
}

void PP_setLed(uint8_t r, uint8_t g, uint8_t b) {
	setPWM(htim8, TIM_CHANNEL_1, r);
	setPWM(htim8, TIM_CHANNEL_2, g);
	setPWM(htim8, TIM_CHANNEL_3, b);
}

void PP_clearLed(void) {
	PP_setLed(0, 0, 0);
}


