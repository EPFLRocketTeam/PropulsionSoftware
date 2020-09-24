/*	threads.c
 *
 * 	author: Iacopo Sprenger
 *	threads initialization
 *
 *
 */


#include <threads.h>
#include <control.h>
#include <sensor.h>
#include <main.h>
#include <led.h>
#include <comm.h>
#include <maxon_comm.h>
#include <sound.h>


typedef StaticTask_t osStaticThreadDef_t;



//control thread

osThreadId_t PP_controlHandle;
uint32_t PP_controlBuffer[ 128 ];
osStaticThreadDef_t PP_controlControlBlock;
const osThreadAttr_t PP_control_attributes = {
  .name = "PP_control",
  .stack_mem = &PP_controlBuffer[0],
  .stack_size = sizeof(PP_controlBuffer),
  .cb_mem = &PP_controlControlBlock,
  .cb_size = sizeof(PP_controlControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};

//sensor thread
/*
osThreadId_t PP_sensorHandle;
uint32_t PP_sensorBuffer[ 128 ];
osStaticThreadDef_t PP_sensorControlBlock;
const osThreadAttr_t PP_sensor_attributes = {
  .name = "PP_sensor",
  .stack_mem = &PP_sensorBuffer[0],
  .stack_size = sizeof(PP_sensorBuffer),
  .cb_mem = &PP_sensorControlBlock,
  .cb_size = sizeof(PP_sensorControlBlock),
  .priority = (osPriority_t) osPriorityRealtime,
};
*/



uint32_t PP_commMotorBuffer[ 128 ];
osStaticThreadDef_t PP_commMotorControlBlock;
const osThreadAttr_t PP_commMotor_attributes = {
  .name = "PP_commMotor",
  .stack_mem = &PP_commMotorBuffer[0],
  .stack_size = sizeof(PP_commMotorBuffer),
  .cb_mem = &PP_commMotorControlBlock,
  .cb_size = sizeof(PP_commMotorControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};


uint32_t PP_commUserBuffer[ 1024 ];
osStaticThreadDef_t PP_commUserControlBlock;
const osThreadAttr_t PP_commUser_attributes = {
  .name = "PP_commUser",
  .stack_mem = &PP_commUserBuffer[0],
  .stack_size = sizeof(PP_commUserBuffer),
  .cb_mem = &PP_commUserControlBlock,
  .cb_size = sizeof(PP_commUserControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t motor_commHandle;
uint32_t motor_commBuffer[ 128 ];
osStaticThreadDef_t motor_commControlBlock;
const osThreadAttr_t motor_comm_attributes = {
  .name = "motor_comm",
  .stack_mem = &motor_commBuffer[0],
  .stack_size = sizeof(motor_commBuffer),
  .cb_mem = &motor_commControlBlock,
  .cb_size = sizeof(motor_commControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t soundHandle;
uint32_t soundBuffer[ 128 ];
osStaticThreadDef_t soundControlBlock;
const osThreadAttr_t sound_attributes = {
  .name = "sound",
  .stack_mem = &soundBuffer[0],
  .stack_size = sizeof(soundBuffer),
  .cb_mem = &soundControlBlock,
  .cb_size = sizeof(soundControlBlock),
  .priority = (osPriority_t) osPriorityLow,
};


osThreadId_t PP_canSendHandle;
uint32_t PP_canSendBuffer[ 128 ];
osStaticThreadDef_t PP_canSendControlBlock;
const osThreadAttr_t PP_canSend_attributes = {
  .name = "PP_canSend",
  .stack_mem = &PP_canSendBuffer[0],
  .stack_size = sizeof(PP_canSendBuffer),
  .cb_mem = &PP_canSendControlBlock,
  .cb_size = sizeof(PP_canSendControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};









void PP_initThreads(void) {



	//sensor init
	PP_sensorInit();
	//plus besoin d'un thread pour les capteurs vu que tout est géré par tim2 et adc+dma
	//PP_sensorHandle = osThreadNew(PP_sensorFunc, NULL, &PP_sensor_attributes);

	maxon_comm_init();
	motor_commHandle = osThreadNew(motor_mainloop, NULL, &motor_comm_attributes);

	PP_commInit();
	//comm thread va gerer les interruptions de "basse importance"
	PP_commMotorHandle = osThreadNew(PP_commMotorFunc, NULL, &PP_commMotor_attributes);
	PP_commUserHandle = osThreadNew(PP_commUserFunc, NULL, &PP_commUser_attributes);

	//control thread init
	PP_controlHandle = osThreadNew(PP_controlFunc, NULL, &PP_control_attributes);
	PP_canSendHandle =osThreadNew(PP_canSendFunc, NULL, &PP_canSend_attributes);

	soundHandle = osThreadNew(PP_soundFunc, NULL, &sound_attributes);

}


