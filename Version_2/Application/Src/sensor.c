/*  Title		: Sensor
 *  Filename	: sensor.c
 *	Author		: iacopo sprenger
 *	Date		: 28.01.2021
 *	Version		: 0.1
 *	Description	: Sensor acquisition and processing
 */

/**********************
 *	INCLUDES
 **********************/

#include "sensor.h"
#include <lut.h>
#include <adc.h>
#include <tim.h>
#include <main.h>
#include <cmsis_os.h>
#include <util.h>

/**********************
 *	CONFIGURATION
 **********************/

#define SENSOR_TIMER	htim3
#define SENSOR_ADC		hadc1

#define SENSOR_HEART_BEAT 10  /* ms */

/**********************
 *	CONSTANTS
 **********************/

#define SAMPLING_TIME 	20 //ms
#define ADC_FREQ		3200 //Hz
#define NB_SAMPLES		32

#define SAMPLE_BUFFER_LEN	(256)

#define SEND_RATE	20

//this needs to be done correctly
#define MS_2_SENSOR_TIMER(ms)	72e6/32*(ms)/1000

/*
 * KULITE CALIBRATION DATA
 */
#define R1_VAL	1799
#define R2_VAL	4700

#define R3_VAL	1798
#define R4_VAL	4700

#define KULITE_322_CAL			64341	//uV/bar
#define KULITE_322_DECODE(val)	((uint64_t)(val) * (R3_VAL+R4_VAL) * 3300 * 1000000 / KULITE_322_CAL / 4096 / R4_VAL)

#define KULITE_323_CAL			64271	//uV/bar
#define KULITE_323_DECODE(val)	((uint64_t)(val) * (R1_VAL+R2_VAL) * 3300 * 1000000 / KULITE_323_CAL / 4096 / R2_VAL)


/**********************
 *	MACROS
 **********************/

/*
 * we generate the following functions:
 * static inline util_buffer_SENSOR_init(UTIL_BUFFER_SENSOR_t * bfr, SENSOR_DATA_t * buffer, uint16_t bfr_len)
 * static inline util_buffer_SENSOR_add(UTIL_BUFFER_SENSOR_t * bfr, SENSOR_DATA_t d)
 * static inline SENSOR_DATA_t  util_buffer_SENSOR_get(UTIL_BUFFER_SENSOR_t * bfr)
 * static inline uint8_t util_buffer_SENSOR_isempty(UTIL_BUFFER_SENSOR_t * bfr)
 */
UTIL_GENERATE_BUFFER(SENSOR_DATA_t, SENSOR)





/**********************
 *	TYPEDEFS
 **********************/

typedef enum SENSOR_TYPE{ //Same order as adc in ioc
	PRESSURE_1, 		// PC0 | A0 S2  m2  tank
	PRESSURE_2, 		// PA2 | A1 S2  m1  cc
	TEMPERATURE_1, 		// PA0 | D0 S1
	TEMPERATURE_2, 		// PA1 | D1 S1
	TEMPERATURE_3,		// PA3 | A0 S1
	NB_SENSOR
}SENSOR_TYPE_t;



/**********************
 *	VARIABLES
 **********************/

static uint16_t adc_buffer[NB_SENSOR];

static UTIL_BUFFER_SENSOR_t sample_bfr;
static SENSOR_DATA_t sample_buffer[SAMPLE_BUFFER_LEN];


/**********************
 *	PROTOTYPES
 **********************/

static void sensor_init(void);





/**********************
 *	DECLARATIONS
 **********************/


/*
 * ADC ISR
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	if(hadc->Instance == SENSOR_ADC.Instance) {
		//put samples into a fifo buffer
		SENSOR_DATA_t data = {
				adc_buffer[PRESSURE_1],
				adc_buffer[PRESSURE_2]
		};
		if(adc_buffer[TEMPERATURE_1] >= LUT_TEMP_MIN && adc_buffer[TEMPERATURE_1] < LUT_TEMP_MAX) {
				data.temperature[0] = lut_temp[adc_buffer[TEMPERATURE_1]-LUT_TEMP_MIN];
				data.temperature_valid[0] = 1;
		} else {
			data.temperature_valid[0] = 0;
		}
		if(adc_buffer[TEMPERATURE_2] >= LUT_TEMP_MIN && adc_buffer[TEMPERATURE_2] < LUT_TEMP_MAX) {
				data.temperature[1] = lut_temp[adc_buffer[TEMPERATURE_2]-LUT_TEMP_MIN];
				data.temperature_valid[1] = 1;
		} else {
			data.temperature_valid[1] = 0;
		}
		if(adc_buffer[TEMPERATURE_3] >= LUT_TEMP_MIN && adc_buffer[TEMPERATURE_3] < LUT_TEMP_MAX) {
				data.temperature[2] = lut_temp[adc_buffer[TEMPERATURE_3]-LUT_TEMP_MIN];
				data.temperature_valid[2] = 1;
		} else {
			data.temperature_valid[2] = 0;
		}
		util_buffer_SENSOR_add(&sample_bfr, data);
	}
}

static void sensor_init(void) {
	SENSOR_TIMER.Instance->ARR = MS_2_SENSOR_TIMER(5);
	HAL_TIM_OC_Start(&SENSOR_TIMER, TIM_CHANNEL_1);
	HAL_ADC_Start_DMA(&SENSOR_ADC, (uint32_t*)adc_buffer, NB_SENSOR);


	util_buffer_SENSOR_init(&sample_bfr, sample_buffer, SAMPLE_BUFFER_LEN);

}

void sensor_calib(void) {

}

void sensor_thread(void * arg) {
	//perform averaging on the fifo contents
	//perform data processing (Kalman??)
	//save data to internal storage (release semaphore for the storage thread)
	//send data through CAN --> here or another thread..
	static TickType_t last_wake_time;
	static const TickType_t period = pdMS_TO_TICKS(SENSOR_HEART_BEAT);

	last_wake_time = xTaskGetTickCount();

	sensor_init();

	for(;;) {


		vTaskDelayUntil( &last_wake_time, period );
	}
}



/* END */


