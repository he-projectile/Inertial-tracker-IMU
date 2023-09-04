#ifndef __MICROS_H
#define __MICROS_H

#include "main.h"

struct microsPeriod{
	uint32_t period;
	uint64_t prev;
};

void microsInit(TIM_HandleTypeDef* timer, uint32_t timerClockFreq);
void microsIT();
uint64_t micros();
void microsStart(struct microsPeriod *counter);
void microsUpdate(struct microsPeriod *counter);

#endif /* __MICROS_H */
