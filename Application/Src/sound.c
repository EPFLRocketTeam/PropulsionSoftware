

#include <sound.h>
#include <tim.h>
#include "cmsis_os.h"



static TONE_t music[] = {
		{A4, 500},
		{A4, 500},
		{A4, 500},
		{F4, 350},
		{C5, 150},

		{A4, 500},
		{F4, 350},
		{C5, 150},
		{A4, 1000},

		{E5, 500},
		{E5, 500},
		{E5, 500},
		{F5, 350},
		{C5, 150},

		{G4H, 500},
		{F4, 350},
		{C5, 150},
		{A4, 1000},

		{A5, 500},
		{A4, 350},
		{A4, 150},
		{A5, 500},
		{G5H, 250},
		{G5, 250},

		{F5H, 125},
		{F5,  125},
		{F5H, 250},
		{0, 250},
		{A4H, 250},
		{D5H, 500},
		{D5,  250},
		{C5H, 250},

		{C5, 125},
		{B4, 125},
		{C5, 250},
		{0, 250},
		{F4, 125},
		{G4H, 500},
		{F4, 375},
		{G4H, 125},

		{C5, 500},
		{A4, 375},
		{C5, 125},
		{E5, 1000},

		{A5, 500},
		{A4, 350},
		{A4, 150},
		{A5, 500},
		{G5H, 250},
		{G5, 250},

		{F5H, 125},
		{F5, 125},
		{F5H, 250},
		{0, 250},
		{A4H, 250},
		{D5H, 500},
		{D5, 250},
		{C5H, 250},


		{C5, 125},
		{B4, 125},
		{C5, 250},
		{0, 250},
		{F4, 250},
		{G4H, 500},
		{F4, 375},
		{C5, 125},

		{A4, 500},
		{F4, 375},
		{C5, 125},
		{A4, 1000}

};


void play_sound(TIM_HandleTypeDef * timer, uint32_t channel, TONE_t * tune, uint16_t len) {
	uint16_t i = 0;
	for(i = 0; i < len; i++) {
		setFREQ_OPT(tune[i].note);
		osDelay(tune[i].time);
		setFREQ_OPT(0);
		osDelay(100);
	}
}

TONE_t * get_music(void) {
	return music;
}

uint16_t get_size(void) {
	return sizeof(music)/sizeof(TONE_t);
}


#define CLOCK	(50E6/50)
void setFREQ(TIM_HandleTypeDef * timer, uint32_t channel, uint16_t freq) {
	if(freq != 0) {
		timer->Init.Period = ((CLOCK/freq*10)-1);
		HAL_TIM_Base_Init(timer);
	}
	TIM_OC_InitTypeDef sConfigOC;
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse =  ((uint64_t)(CLOCK/freq*10-1)>>1);
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if(freq == 0) {
		sConfigOC.Pulse = 0;
	}
	HAL_TIM_PWM_ConfigChannel(timer, &sConfigOC, channel);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

void setFREQ_OPT(uint16_t freq) {
	if(freq == 0) {
		TIM2->CCR1 = 0;
	} else {
		TIM2->ARR = ((uint32_t)(CLOCK/freq*10)-1);
		TIM2->CCR1 = ((uint32_t)(CLOCK/freq*10-1)>>1);
	}
}

void init_sound(void) {
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}


void PP_soundFunc(void *argument) {
	init_sound();
	for(;;) {
		play_sound(&htim2, TIM_CHANNEL_1, music, sizeof(music)/sizeof(TONE_t));
	}
}

