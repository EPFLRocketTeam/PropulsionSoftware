/*  Title       : Led
 *  Filename    : led.h
 *  Author      : iacopo sprenger
 *  Date        : 20.01.2021
 *  Version     : 0.1
 *  Description : rgb led control
 */

#ifndef LED_H
#define LED_H



/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>

/**********************
 *  CONSTANTS
 **********************/


/**********************
 *  MACROS
 **********************/


/**********************
 *  TYPEDEFS
 **********************/

typedef struct LED_COLOR{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}LED_COLOR_t;


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif

void led_init(void);

void led_set_color(LED_COLOR_t color);

#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* LED_H */

/* END */
