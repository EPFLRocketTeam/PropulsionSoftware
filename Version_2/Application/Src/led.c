/*  Title		: Led
 *  Filename	: led.c
 *	Author		: iacopo sprenger
 *	Date		: 20.01.2021
 *	Version		: 0.1
 *	Description	: rgb led control
 */

/**********************
 *	INCLUDES
 **********************/

#include <led.h>
#include <main.h>
#include <tim.h>

/**********************
 *	CONFIGURATION
 **********************/

#define LED_TIM			TIM8
#define RED_CHANNEL		LL_TIM_CHANNEL_CH1N
#define GREEN_CHANNEL	LL_TIM_CHANNEL_CH2N
#define BLUE_CHANNEL	LL_TIM_CHANNEL_CH3N

/**********************
 *	CONSTANTS
 **********************/

#define LED_MAX			(0xff)



/**********************
 *	MACROS
 **********************/



/**********************
 *	TYPEDEFS
 **********************/



/**********************
 *	VARIABLES
 **********************/



/**********************
 *	PROTOTYPES
 **********************/



/**********************
 *	DECLARATIONS
 **********************/

void led_init(void) {
	LL_TIM_SetAutoReload(LED_TIM, LED_MAX);
	LL_TIM_CC_EnableChannel(LED_TIM, RED_CHANNEL);
	LL_TIM_CC_EnableChannel(LED_TIM, GREEN_CHANNEL);
	LL_TIM_CC_EnableChannel(LED_TIM, BLUE_CHANNEL);
}

void led_set_color(LED_COLOR_t color) {
	LL_TIM_OC_SetCompareCH1(LED_TIM, color.r);
	LL_TIM_OC_SetCompareCH2(LED_TIM, color.g);
	LL_TIM_OC_SetCompareCH3(LED_TIM, color.b);
}


/* END */


