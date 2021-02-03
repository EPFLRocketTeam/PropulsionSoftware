#ifndef DEBUG_UI_H
#define DEBUG_UI_H

#include <main.h>
#include "usart.h"

#define DUI_NAME_LEN		32
#define DUI_ENTRY_LEN		256

#define DUI_RESP_LEN		1024

#define UI_UART				huart3



#define DUI_MAX_ARGS		8

/*
 * @brief				receive a char from user input
 * @param recvBuffer	buffer containing the char to receive.
 */
void debug_ui_receive(uint8_t recvBuffer);





#endif
