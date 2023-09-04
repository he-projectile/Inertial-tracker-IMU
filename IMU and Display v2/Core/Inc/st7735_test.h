/*
	Библиотека для дисплея на ST7735, основана на: https://github.com/ScarsFun/STM32F103C8T6_cubeIDE_ST7735_1.8_DMA
*/
#ifndef ST7735_TEST_H_
#define ST7735_TEST_H_

#include "st7735.h"
#include <stdio.h>
#include <stdlib.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))

#define BGCOLOR ST7735_BLACK

void Displ_PerfTest();
void Displ_ColorTest();
void Displ_TestAll();

#endif 
