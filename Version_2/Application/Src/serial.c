/*  Title		: Serial
 *  Filename	: serial.c
 *	Author		: iacopo sprenger
 *	Date		: 31.01.2021
 *	Version		: 0.1
 *	Description	: serial communication driver
 */

/**********************
 *	INCLUDES
 **********************/

#include <serial.h>
#include <util.h>
#include <cmsis_os.h>

/**********************
 *	CONSTANTS
 **********************/


/**********************
 *	MACROS
 **********************/


/**********************
 *	TYPEDEFS
 **********************/


/*
 * One thread for serial communication with a semaphore to sync between the ISR and the BH
 */

/**********************
 *	VARIABLES
 **********************/

static SERIAL_INST_t * serial_devices[SERIAL_MAX_INST];
static SERIAL_INST_t * serial_epos4_device;
static SERIAL_INST_t * serial_debug_device;
static uint16_t serial_devices_count = 0;



static SemaphoreHandle_t serial_rx_sem = NULL;
static StaticSemaphore_t serial_rx_sem_buffer;

#if SERIAL_USE_GENERIC == 0

static SemaphoreHandle_t serial_epos4_rx_sem = NULL;
static StaticSemaphore_t serial_epos4_rx_sem_buffer;

static SemaphoreHandle_t serial_degug_rx_sem = NULL;
static StaticSemaphore_t serial_debug_rx_sem_buffer;

#endif

/**********************
 *	PROTOTYPES
 **********************/


/**********************
 *	DECLARATIONS
 **********************/

/*
 * UART RX ISR
 */

#if SERIAL_USE_GENERIC == 1

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	for(uint16_t i = 0; i < serial_devices_count; i++) {
		if(serial_devices[i]->uart == huart) {
			util_buffer_u8_add(&serial_devices[i]->bfr, serial_devices[i]->dma_buffer);
			xSemaphoreGiveFromISR( serial_rx_sem, &xHigherPriorityTaskWoken );
			break;
		}
	}
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

#else

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(serial_epos4_device->uart == huart) {
		util_buffer_u8_add(serial_epos4_device->bfr, serial_epos4_device->dma_buffer);
		xSemaphoreGiveFromISR( serial_epos4_rx_sem, &xHigherPriorityTaskWoken );
	}
	if(serial_debug_device->uart == huart) {
		util_buffer_u8_add(serial_debug_device->bfr, serial_debug_device->dma_buffer);
		xSemaphoreGiveFromISR( serial_debug_device, &xHigherPriorityTaskWoken );
	}
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

#endif


#if SERIAL_USE_GENERIC == 1

void serial_global_init(void) {
	serial_rx_sem = xSemaphoreCreateBinaryStatic( &serial_rx_sem_buffer );
}

#else

void serial_global_init(void) {
	serial_epos4_rx_sem = xSemaphoreCreateBinaryStatic( &serial_epos4_rx_sem_buffer );
	serial_debug_rx_sem = xSemaphoreCreateBinaryStatic( &serial_debug_rx_sem_buffer );
}

#endif

void serial_epos4_init(SERIAL_INST_t * ser, UART_HandleTypeDef * uart, void * inst, SERIAL_RET_t (*decode_fcn)(void *, uint8_t)) {
	ser->id = 1;
	ser->uart = uart;
	ser->inst = inst;
	ser->decode_fcn = decode_fcn;
	util_buffer_u8_init(&ser->bfr, ser->buffer, SERIAL_FIFO_LEN);
	HAL_UART_Receive_DMA(uart, &ser->dma_buffer, 1);
	serial_epos4_device = ser;
}

void serial_debug_init(SERIAL_INST_t * ser, UART_HandleTypeDef * uart, void * inst, SERIAL_RET_t (*decode_fcn)(void *, uint8_t)) {
	ser->id = 0;
	ser->uart = uart;
	ser->inst = inst;
	ser->decode_fcn = decode_fcn;
	util_buffer_u8_init(&ser->bfr, ser->buffer, SERIAL_FIFO_LEN);
	HAL_UART_Receive_DMA(uart, &ser->dma_buffer, 1);
	serial_debug_device = ser;
}




void serial_init(SERIAL_INST_t * ser, UART_HandleTypeDef * uart, void * inst, SERIAL_RET_t (*decode_fcn)(void *, uint8_t)) {
	ser->id = serial_devices_count;
	ser->uart = uart;
	ser->inst = inst;
	ser->decode_fcn = decode_fcn;
	util_buffer_u8_init(&ser->bfr, ser->buffer, SERIAL_FIFO_LEN);
	if(serial_devices_count < SERIAL_MAX_INST) {
		HAL_UART_Receive_DMA(uart, &ser->dma_buffer, 1);
		serial_devices[serial_devices_count] = ser;
	}
	serial_devices_count++;
}

void serial_send(SERIAL_INST_t * ser, uint8_t * data, uint16_t length) {
	HAL_UART_Transmit_DMA(ser->uart, data, length);
	//HAL_UART_Transmit(ser->uart, data, length, 500);
}


void serial_thread(void * arg) {

	serial_global_init();

	for(;;) {
		if( xSemaphoreTake(serial_rx_sem, 0xffff) == pdTRUE ) {
			for(uint16_t i = 0; i < serial_devices_count; i++) {
				while(!util_buffer_u8_isempty(&serial_devices[i]->bfr)) {
					serial_devices[i]->decode_fcn(serial_devices[i]->inst, util_buffer_u8_get(&serial_devices[i]->bfr));
				}
			}
		}
	}
}



#if SERIAL_USE_GENERIC == 0


void serial_debug_thread(void * arg) {
	serial_global_init();
	for(;;) {
		if( xSemaphoreTake(serial_debug_rx_sem, 0xffff) == pdTRUE ) {
			while(!util_buffer_u8_isempty(&serial_devices[i]->bfr)) {
				serial_devices[i]->decode_fcn(serial_devices[i]->inst, util_buffer_u8_get(&serial_devices[i]->bfr));
			}
		}
	}
}


void serial_epos4_thread(void * arg) {

	serial_global_init();
	for(;;) {
		if( xSemaphoreTake(serial_epos4_rx_sem, 0xffff) == pdTRUE ) {
			while(!util_buffer_u8_isempty(&serial_devices[i]->bfr)) {
				serial_devices[i]->decode_fcn(serial_devices[i]->inst, util_buffer_u8_get(&serial_devices[i]->bfr));
			}
		}
	}
}

#endif

/* END */


