/*
 * Common.h
 *
 *  Created on: 8 сент. 2021 г.
 *      Author: Тлехас Алий
 */

#ifndef COMMON_INC_COMMON_H_
#define COMMON_INC_COMMON_H_

#include "main.h"


void _swap_uint16_t(uint16_t *a, uint16_t *b);
void _swap_int16_t(int16_t *a, int16_t *b);

void bitWrite(uint16_t* word, uint8_t pos, _Bool bit);
_Bool bitRead(uint16_t word, uint8_t pos);


#endif /* COMMON_INC_COMMON_H_ */
