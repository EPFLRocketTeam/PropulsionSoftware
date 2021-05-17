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

#include <sensor.h>
#include <lut.h>
#include <adc.h>
#include <tim.h>
#include <main.h>
#include <cmsis_os.h>
#include <util.h>
#include <storage.h>

/**********************
 *	CONFIGURATION
 **********************/

#define SENSOR_TIMER	htim3
#define SENSOR_ADC		hadc1


#define ADC_HEART_BEAT 	0.05  /* ms */
#define ADC_ARR			1000  /* cycles */


#define SAMPLE_BUFFER_LEN	(128)

/**********************
 *	CONSTANTS
 **********************/

#define DT_THRESH		500 /* 0.1deg */

#define SAMPLING_TIME 	20 //ms
#define ADC_FREQ		3200 //Hz
#define NB_SAMPLES		64

#define SAMPLE_BUFFER_LEN_HALF	(SAMPLE_BUFFER_LEN/2)
#define SAMPLE_BUFFER_DIV		(6)




#define SEND_RATE	20

#define CALIBRATION_CYCLES		(64)

#define CALIBRATION_CYCLES_DIV	(6)

//this needs to be done correctly
#define MS_2_SENSOR_TIMER(ms)	72e6*(ms)/1000

/*
 * KULITE CALIBRATION DATA
 */
#define R1_VAL	1799
#define R2_VAL	4700

#define R3_VAL	1798
#define R4_VAL	4700

#define KULITE_322_CAL			64341	//uV/bar
#define KULITE_322_DECODE(val)	((int64_t)(val) * (R3_VAL+R4_VAL) * 3300 * 1000000 / KULITE_322_CAL / 4096 / R4_VAL)

#define KULITE_323_CAL			64271	//uV/bar
#define KULITE_323_DECODE(val)	((int64_t)(val) * (R1_VAL+R2_VAL) * 3300 * 1000000 / KULITE_323_CAL / 4096 / R2_VAL)


/**********************
 *	MACROS
 **********************/



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

static uint16_t adc_buffer[NB_SENSOR*SAMPLE_BUFFER_LEN];


static SENSOR_DATA_t last_data = {0};

static SENSOR_DATA_t offset = {0};

static uint8_t calib = 0;
static uint16_t calib_counter = 0;

static uint8_t new_data_storage;
static uint8_t new_calib_storage;
static uint8_t new_data_can;

static SemaphoreHandle_t adc_conv_sem = NULL;
static StaticSemaphore_t adc_conv_sem_buffer;

static uint8_t first_half;

/**********************
 *	PROTOTYPES
 **********************/

static void sensor_init(void);





/**********************
 *	DECLARATIONS
 **********************/

void sensor_calib(void) {
	calib_counter = 0;
	calib = 1;
}

uint8_t sensor_calib_done(void) {
	return !calib;
}

SENSOR_DATA_t sensor_get_last(void) {
	return last_data;
}

SENSOR_DATA_t sensor_get_calib(void) {
	return offset;
}

void sensor_set_calib(SENSOR_DATA_t calib) {
	offset = calib;
}

uint8_t sensor_new_data_storage() {
	uint8_t tmp = new_data_storage;
	new_data_storage = 0;
	return tmp;
}

uint8_t sensor_new_data_can() {
	uint8_t tmp = new_data_can;
	new_data_can = 0;
	return tmp;
}

uint8_t sensor_new_calib_storage() {
	uint8_t tmp = new_calib_storage;
	new_calib_storage = 0;
	return tmp;
}



void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(hadc->Instance == SENSOR_ADC.Instance) {
		first_half = 0;
		xSemaphoreGiveFromISR( adc_conv_sem, &xHigherPriorityTaskWoken );
	}
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(hadc->Instance == SENSOR_ADC.Instance) {
		first_half = 1;
		xSemaphoreGiveFromISR( adc_conv_sem, &xHigherPriorityTaskWoken );
	}
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


static void sensor_init(void) {


	adc_conv_sem = xSemaphoreCreateBinaryStatic(&adc_conv_sem_buffer);


	SENSOR_TIMER.Instance->ARR = MS_2_SENSOR_TIMER(ADC_HEART_BEAT);
	HAL_TIM_OC_Start(&SENSOR_TIMER, TIM_CHANNEL_1);
	HAL_ADC_Start_DMA(&SENSOR_ADC, (uint32_t*)adc_buffer, NB_SENSOR*SAMPLE_BUFFER_LEN);

}


void sensor_thread(void * arg) {

	sensor_init();

	for(;;) {

		if(xSemaphoreTake(adc_conv_sem, 0xffff) == pdTRUE) {

			int64_t temp_val[3] = {0};
			int64_t pres_val[2] = {0};
			SENSOR_DATA_t last_valid_data;
			SENSOR_DATA_t data;
			for(uint16_t i = 0; i < SAMPLE_BUFFER_LEN_HALF; i++) {

				uint16_t ofs = ((i)+(first_half?0:SAMPLE_BUFFER_LEN_HALF))*NB_SENSOR;

				data.pressure_1 = KULITE_322_DECODE(adc_buffer[ofs+PRESSURE_1])-offset.pressure_1;
				data.pressure_2 = KULITE_323_DECODE(adc_buffer[ofs+PRESSURE_2])-offset.pressure_2;

				if(adc_buffer[ofs+TEMPERATURE_1] >= LUT_TEMP_MIN && adc_buffer[ofs+TEMPERATURE_1] < LUT_TEMP_MAX) {
						data.temperature[0] = lut_temp[adc_buffer[ofs+TEMPERATURE_1]-LUT_TEMP_MIN];
						last_valid_data.temperature[0] = data.temperature[0];
						data.temperature_valid[0] = 1;
				} else {
					data.temperature_valid[0] = 0;
				}
				if(adc_buffer[ofs+TEMPERATURE_2] >= LUT_TEMP_MIN && adc_buffer[ofs+TEMPERATURE_2] < LUT_TEMP_MAX) {
						data.temperature[1] = lut_temp[adc_buffer[ofs+TEMPERATURE_2]-LUT_TEMP_MIN];
						last_valid_data.temperature[1] = data.temperature[1];
						data.temperature_valid[1] = 1;
				} else {
					data.temperature_valid[1] = 0;
				}
				if(adc_buffer[ofs+TEMPERATURE_3] >= LUT_TEMP_MIN && adc_buffer[ofs+TEMPERATURE_3] < LUT_TEMP_MAX) {
						data.temperature[2] = lut_temp[adc_buffer[ofs+TEMPERATURE_3]-LUT_TEMP_MIN];
						last_valid_data.temperature[2] = data.temperature[2];
						data.temperature_valid[2] = 1;
				} else {
					data.temperature_valid[2] = 0;
				}

				pres_val[0] += data.pressure_1;
				pres_val[1] += data.pressure_2;
				temp_val[0] += data.temperature_valid[0]?data.temperature[0]:last_valid_data.temperature[0];
				temp_val[1] += data.temperature_valid[1]?data.temperature[1]:last_valid_data.temperature[1];
				temp_val[2] += data.temperature_valid[2]?data.temperature[2]:last_valid_data.temperature[2];
			}

			data.pressure_1 = pres_val[0]>>SAMPLE_BUFFER_DIV;
			data.pressure_2 = pres_val[1]>>SAMPLE_BUFFER_DIV;
			data.temperature[0] = temp_val[0]>>SAMPLE_BUFFER_DIV;
			data.temperature[1] = temp_val[1]>>SAMPLE_BUFFER_DIV;
			data.temperature[2] = temp_val[2]>>SAMPLE_BUFFER_DIV;

			data.time = HAL_GetTick();



			// this is while I build the filtering functions
			static uint16_t h = 0;
			static int32_t d_temperature[3] = {0};

			h = last_data.time - data.time;
			if(h) {
				for(uint8_t i = 0; i < 3; i++) {
					int32_t next = last_data.temperature[i] + d_temperature[i]*h;
					if(util_abs(data.temperature[i])>DT_THRESH) {
						data.temperature[i] = next;
					}
					//compute next
					d_temperature[i] = (data.temperature[i] - last_data.temperature[i])/h;
				}
			}


			last_data = data;

			//Predictor and wrong measurements rejector (Temperature only)

			//predict next state according to the derivative
			//if measurement is too far, use an estimation from the derivative and the time to get the next measurement

			//CALIBRATION (PRESSURE)
			static int64_t pressure_1 = 0;
			static int64_t pressure_2 = 0;
			if(calib) {
				if(calib_counter == 0) {
					offset.pressure_1 = 0;
					offset.pressure_2 = 0;
					pressure_1 = 0;
					pressure_2 = 0;
				} else {
					pressure_1 += data.pressure_1;
					pressure_2 += data.pressure_2;
					if(calib_counter >= CALIBRATION_CYCLES) {
						offset.pressure_1 = pressure_1 >> CALIBRATION_CYCLES_DIV;
						offset.pressure_2 = pressure_2 >> CALIBRATION_CYCLES_DIV;
						calib = 0;
						calib_counter = 0;
						new_calib_storage = 1;
					}
				}
				calib_counter++;
			}


			//The CAN thread will periodically get the data
			new_data_can = 1;

			//The storage thread will periodically get the data

			new_data_storage = 1;

			storage_notify();

			//Results accessible from DEBUG UART
		}
	}
}



/* END */


