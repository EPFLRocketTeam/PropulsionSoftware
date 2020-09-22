
#ifndef SOUND_H
#define SOUND_H
#include <note.h>
#include <main.h>

typedef struct {
	uint16_t note;
	uint16_t time;
}TONE_t;


TONE_t * get_music(void);

uint16_t get_size(void);


void play_sound(TIM_HandleTypeDef * timer, uint32_t channel, TONE_t * tune, uint16_t len);
void setPWM(TIM_HandleTypeDef * timer, uint32_t channel, uint16_t pulse);
void setFREQ(TIM_HandleTypeDef * timer, uint32_t channel, uint16_t freq);



#endif


