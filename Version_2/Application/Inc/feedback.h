/*  Title       : Feedback
 *  Filename    : feedback.h
 *  Author      : iacopo sprenger
 *  Date        : 24.03.2021
 *  Version     : 0.1
 *  Description : used feedback
 */

#ifndef FEEDBACK_H
#define FEEDBACK_H



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


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif

void feedback_thread(void * arg);

#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* FEEDBACK_H */

/* END */
