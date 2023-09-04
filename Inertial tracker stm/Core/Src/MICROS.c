#include "MICROS.h"

static uint64_t microsCnt = 0;
static TIM_HandleTypeDef* microsTimer;

void microsInit(TIM_HandleTypeDef* timer, uint32_t timerClockFreq){
	microsTimer = timer;
	microsCnt = 0;
	HAL_TIM_Base_Start_IT(microsTimer);
	timer -> Instance -> PSC = timerClockFreq/1000000-1;
	timer -> Instance -> ARR = 1000;
}

uint64_t micros(){
	return microsCnt + microsTimer->Instance->CNT;
}

void microsIT(){
	microsCnt += microsTimer->Init.Period;
}

void microsStart( struct microsPeriod *counter){
	counter->period = 0;
	counter->prev = micros();
}

void microsUpdate(struct microsPeriod *counter){
	counter->period = micros() - counter->prev;
	counter->prev = micros();
}