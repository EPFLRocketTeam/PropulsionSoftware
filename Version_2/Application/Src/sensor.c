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

#define FILTER_BUFFER_LEN	(16)

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


#define FILTER_LEN	5


/**********************
 *	MACROS
 **********************/

/*
 * we generate the following functions:
 * static inline util_buffer_SENSOR_init(UTIL_BUFFER_SENSOR_t * bfr, SENSOR_DATA_t * buffer, uint16_t bfr_len)
 * static inline util_buffer_SENSOR_add(UTIL_BUFFER_SENSOR_t * bfr, SENSOR_DATA_t d)
 * static inline SENSOR_DATA_t util_buffer_SENSOR_get(UTIL_BUFFER_SENSOR_t * bfr)
 * static inline SENSOR_DATA_t util_buffer_SENSOR_access(UTIL_BUFFER_SENSOR_t * bfr, uint16_t ix)
 * static inline uint8_t util_buffer_SENSOR_isempty(UTIL_BUFFER_SENSOR_t * bfr)
 */
UTIL_GENERATE_BUFFER(SENSOR_DATA_t, SENSOR)


#define FLOAT_2_FIX_20_12(f)	((uint32_t) ((f) * (1<<12)))


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
static UTIL_BUFFER_SENSOR_t filter_bfr;
static SENSOR_DATA_t filter_buffer[FILTER_BUFFER_LEN] = {0};

//filter computations in 20.12 floating point
static const uint32_t filter_coefficients[] = {	FLOAT_2_FIX_20_12(0.6),
												FLOAT_2_FIX_20_12(0.2),
												FLOAT_2_FIX_20_12(0.1),
												FLOAT_2_FIX_20_12(0.07),
												FLOAT_2_FIX_20_12(0.03)};


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
		//samples are preprocessed
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
	util_buffer_SENSOR_init(&filter_bfr, filter_buffer, FILTER_BUFFER_LEN);

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

		//second processing i.e. empty buffer and average all samples
		//trend prediction
		//average value of each sample collected in the fifo buffer
		uint16_t temp_count[3] = {0};
		int32_t temp_val[3] = {0};
		uint16_t pres_count = 0;
		uint32_t pres_val[2] = {0};
		while(!util_buffer_SENSOR_isempty(&sample_bfr)) {
			SENSOR_DATA_t data = util_buffer_SENSOR_get(&sample_bfr);
			pres_val[0] += data.pressure_1;
			pres_val[1] += data.pressure_2;
			pres_count += 1;
			temp_val[0] += data.temperature_valid[0]?data.temperature[0]:0;
			temp_count[0] += data.temperature_valid[0];
			temp_val[1] += data.temperature_valid[1]?data.temperature[1]:0;
			temp_count[1] += data.temperature_valid[1];
			temp_val[2] += data.temperature_valid[2]?data.temperature[2]:0;
			temp_count[2] += data.temperature_valid[2];
		}
		if(pres_count != 0) {
			pres_val[0] /= pres_count;
			pres_val[1] /= pres_count;
		}
		if(temp_count[0] != 0) {
			temp_val[0] /= temp_count[0];
		}
		if(temp_count[1] != 0) {
			temp_val[1] /= temp_count[1];
		}
		if(temp_count[2] != 0) {
			temp_val[2] /= temp_count[2];
		}
		SENSOR_DATA_t data = {
				pres_val[0],
				pres_val[1],
				{
						temp_val[0],
						temp_val[1],
						temp_val[2]
				}
		};
		util_buffer_SENSOR_add(&filter_bfr, data);



		//Low pass filter
		SENSOR_DATA_t filtered = {0};
		for(uint8_t i = 0; i < FILTER_LEN; i++) {
			SENSOR_DATA_t filter_data = util_buffer_SENSOR_access(&filter_bfr, i);
			filtered.pressure_1 += filter_data.pressure_1*filter_coefficients[i] >> 12;
			filtered.pressure_2 += filter_data.pressure_2*filter_coefficients[i] >> 12;
			filtered.temperature[0] += filter_data.temperature[0]*filter_coefficients[i] >> 12;
			filtered.temperature[1] += filter_data.temperature[1]*filter_coefficients[i] >> 12;
			filtered.temperature[2] += filter_data.temperature[2]*filter_coefficients[i] >> 12;
		}

		//Predictor and wrong measurements rejector (Temperature only)

		//predict next state according to the derivative
		//if measurement is too far, use an estimation from the derivative and the time to get the next measurement



		//Notify CAN for transfer

		//Notify storage for storage

		//Results accessible from DEBUG UART

		vTaskDelayUntil( &last_wake_time, period );
	}
}



/* END */


