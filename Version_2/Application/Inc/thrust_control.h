/*  Title       : Thrust control
 *  Filename    : thrust_control.h
 *  Author      : iacopo sprenger
 *  Date        : 03.03.2021
 *  Version     : 0.1
 *  Description : thrust control algorithm
 *  		      The algorithm was developped by Antoine Faltz and
 *				  Luca Rezzonico
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H



/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>

/**********************
 *  CONSTANTS
 **********************/

#define TC_TRUST_PRESS_COEFF		(1310) /* [Pa]/[N] */


/**********************
 *  MACROS
 **********************/


#define TC_MBAR_2_PASCAL(pres)			((pres)*100)

#define TC_THRUST_2_PRESSURE(thrust)	((thrust)*TC_TRUST_PRESS_COEFF)

#define TC_PRESSURE_2_THRUST(pres)		((pres)/TC_TRUST_PRESS_COEFF)


/**********************
 *  TYPEDEFS
 **********************/


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif

void tc_init();

int32_t tc_compute(int32_t cc_pressure, int32_t desired_thrust);


#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* TEMPLATE_H */

/* END */
