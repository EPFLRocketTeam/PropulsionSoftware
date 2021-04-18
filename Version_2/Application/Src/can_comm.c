/*  Title		: CAN communication
 *  Filename	: can_comm.c
 *	Author		: Tim Lebailly (modified by Iacopo Sprenger)
 *	Date		: 23.02.2019
 *	Version		: 0.1
 *	Description	: can communication
 */
/*
 * CAN_communication.c
 *
 *
 *  Created on: Feb 23, 2019
 *      Author: Tim Lebailly
 */

/**********************
 *	INCLUDES
 **********************/

#include "can.h"
#include <cmsis_os.h>
#include <main.h>
#include <can_comm.h>
#include <sensor.h>
#include <control.h>


#define CAN_BUFFER_DEPTH 64

#define CAN_HEART_BEAT 20


CAN_TxHeaderTypeDef   TxHeader;
CAN_RxHeaderTypeDef   RxHeader;
uint8_t               RxData[8];
uint32_t              TxMailbox;

volatile CAN_msg can_current_msg;

CAN_msg can_buffer[CAN_BUFFER_DEPTH];
volatile int32_t can_buffer_pointer_rx = 0;
volatile int32_t can_buffer_pointer_tx = 0;

uint32_t can_readFrame(void);

uint32_t pointer_inc(uint32_t val, uint32_t size) {
	return (val + 1) % size;
}

void can_addMsg(CAN_msg msg) {
	can_buffer[can_buffer_pointer_tx] = msg;
	can_buffer_pointer_tx = pointer_inc(can_buffer_pointer_tx, CAN_BUFFER_DEPTH);

	if (can_buffer_pointer_tx == can_buffer_pointer_rx) { // indicates overflow
		can_buffer_pointer_rx = pointer_inc(can_buffer_pointer_rx, CAN_BUFFER_DEPTH); // skip one msg in the rx buffer
	}
}

/*
 * Configures CAN protocol for 250kbit/s without interrupt for reading (only polling).
 */
void CAN_Config(uint32_t id)
{
    CAN_FilterTypeDef  sFilterConfig;

    /*##-1- Configure the CAN peripheral #######################################*/
    // Done in MX_CAN1_Init()

    /*##-2- Configure the CAN Filter ###########################################*/
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
    {
        /* Filter configuration Error */
        //      _Error_Handler(__FILE__, __LINE__);
    }

    /*##-3- Start the CAN peripheral ###########################################*/
    if (HAL_CAN_Start(&hcan1) != HAL_OK)
    {
        /* Start Error */
        //      _Error_Handler(__FILE__, __LINE__);
    }

    /*##-4- Activate CAN RX notification #######################################*/
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
        /* Notification Error */
    	//_Error_Handler(__FILE__, __LINE__);
    }

    /*##-5- Configure Transmission process #####################################*/
    TxHeader.StdId = id;
    TxHeader.ExtId = id; // not needed
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.IDE = CAN_ID_STD;
    TxHeader.DLC = 8;
    TxHeader.TransmitGlobalTime = DISABLE;
}

/*
 * Sends a frame of 8 bytes (payload) on the CAN bus using our predefined protocol.
 * byte 0..3 --> some uint32_t
 * byte 4    --> data_id, see CAN_communication.h
 * byte 5..7 --> timestamp
 */
void can_setFrame(uint32_t data, uint8_t data_id, uint32_t timestamp) {
	uint8_t TxData[8] = {0};
	TxData[0] = (uint8_t) (data >> 24);
    TxData[1] = (uint8_t) (data >> 16);
    TxData[2] = (uint8_t) (data >> 8);
    TxData[3] = (uint8_t) (data >> 0);
    TxData[4] = data_id;
    TxData[5] = (uint8_t) (timestamp >> 16);
    TxData[6] = (uint8_t) (timestamp >> 8);
    TxData[7] = (uint8_t) (timestamp >> 0);

	while (HAL_CAN_IsTxMessagePending(&hcan1, TxMailbox)) {
		osDelay(1);
	} // wait for CAN to be ready

	CAN_msg message = (CAN_msg) {data, data_id, timestamp, TxHeader.StdId};

    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) == HAL_OK) {
    	can_addMsg(message);

    } else { // something bad happen
    	// not sure what to do
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	can_readFrame();
	can_addMsg(can_current_msg);
}

uint32_t can_msgPending() {
	int32_t diff = can_buffer_pointer_tx - can_buffer_pointer_rx;
	if (diff < 0) {
		diff += CAN_BUFFER_DEPTH;
	}

	return diff;
}

CAN_msg can_readBuffer() {
	CAN_msg ret = {0};

	if (can_msgPending() > 0) {
		ret = can_buffer[can_buffer_pointer_rx];
		can_buffer_pointer_rx = pointer_inc(can_buffer_pointer_rx, CAN_BUFFER_DEPTH);
	} else { // no message actually pending
		// do nothing, will return the {0} CAN_msg
	}


	return ret;
}

/*
 * Reads the CAN bus and sets global CAN_msg current_msg struct (see CAN_communication.h)
 * Returns the fill level when the function was called
 * This way, the caller knows if the variable current_msg was updated by the funciton call or not.
 *
 * byte 0..3 --> some uint32_t
 * byte 4    --> data_id, see CAN_communication.h
 * byte 5..7 --> timestamp
 */
uint32_t can_readFrame(void) {
    uint32_t fill_level = HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0);
    if (fill_level > 0) {
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData);

        can_current_msg.data = 0;
        can_current_msg.data += (uint32_t) RxData[0] << 24;
        can_current_msg.data += (uint32_t) RxData[1] << 16;
        can_current_msg.data += (uint32_t) RxData[2] << 8;
		can_current_msg.data += (uint32_t) RxData[3] << 0;

        can_current_msg.id = RxData[4];

        can_current_msg.timestamp = 0;
        can_current_msg.timestamp += (uint32_t) RxData[5] << 16;
        can_current_msg.timestamp += (uint32_t) RxData[6] << 8;
		can_current_msg.timestamp += (uint32_t) RxData[7] << 0;

        can_current_msg.id_CAN = RxHeader.StdId;
    }
    return fill_level;
}


void can_init(void) {
	CAN_Config(CAN_ID_PROPULSION_BOARD);
}


void can_send_thread(void * arg) {
	static TickType_t last_wake_time;
	static const TickType_t period = pdMS_TO_TICKS(CAN_HEART_BEAT);

	last_wake_time = xTaskGetTickCount();


	for(;;) {

		SENSOR_DATA_t sensor_data = sensor_get_last();
		CONTROL_STATUS_t control_data = control_get_status();
		uint32_t status = (control_data.state & 0xff) | ((control_data.venting<<8) & 0xff00) | ((control_data.pp_psu_voltage<<16) & 0xffff0000);
		can_setFrame((uint32_t) sensor_data.pressure_1, DATA_ID_PRESS_1, sensor_data.time);
		can_setFrame((uint32_t) sensor_data.pressure_2, DATA_ID_PRESS_2, sensor_data.time);
		can_setFrame((uint32_t) sensor_data.temperature[0], DATA_ID_TEMP_1, sensor_data.time);
		can_setFrame((uint32_t) sensor_data.temperature[1], DATA_ID_TEMP_2, sensor_data.time);
		can_setFrame((uint32_t) sensor_data.temperature[2], DATA_ID_TEMP_3, sensor_data.time);
		can_setFrame((uint32_t) status, DATA_ID_STATUS, control_data.time);
		can_setFrame((uint32_t) control_data.pp_position, DATA_ID_MOTOR_POS, control_data.time);

		vTaskDelayUntil( &last_wake_time, period );
	}
}



/* END */


