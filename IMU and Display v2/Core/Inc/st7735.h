/*
	Библиотека для дисплея на ST7735, основана на: https://github.com/ScarsFun/STM32F103C8T6_cubeIDE_ST7735_1.8_DMA
*/
#ifndef __ST7735_H__
#define __ST7735_H__

#include "fonts.h"
#include "st7735_config.h"
#include <stdbool.h>

extern SPI_HandleTypeDef ST7735_SPI_PORT;

#ifdef	DISPLAY_DIMMING_MODE
extern TIM_HandleTypeDef ST7735_T;
#endif

/****************************/

#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MV  0x20
#define ST7735_MADCTL_ML  0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04

/****************************/

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1



// (Для стандратной ориентации дисплея)
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160
#ifdef ST7735_DISPLAY
	#define ST7735_XSTART 2
	#define ST7735_YSTART 1
#endif
#ifdef ST7735S_DISPLAY
	#define ST7735_XSTART 2
	#define ST7735_YSTART 1
#endif
#define ST7735_ROTATION (ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_RGB)


/****************************/ 

// Список стандартных цветов
#define	ST7735_RED				0xF800
#define	ST7735_GREEN			0x07E0
#define	ST7735_BLUE				0x001F
#define ST7735_YELLOW			0xFFE0
#define ST7735_MAGENTA		0xF81F
#define ST7735_CYAN				0x07FF
#define	ST7735_D_RED 			0xC000
#define	ST7735_D_GREEN		0x0600
#define	ST7735_D_BLUE			0x0018
#define ST7735_D_YELLOW		0xC600
#define ST7735_D_MAGENTA	0xC018
#define ST7735_D_CYAN			0x0618
#define	ST7735_DD_RED			0x8000
#define	ST7735_DD_GREEN		0x0400
#define ST7735_DD_BLUE		0x0010
#define ST7735_DD_YELLOW	0x8400
#define ST7735_DD_MAGENTA	0x8010
#define ST7735_DD_CYAN		0x0410
#define ST7735_WHITE			0xFFFF
#define ST7735_D_WHITE		0xC618
#define ST7735_DD_WHITE		0x8410
#define ST7735_DDD_WHITE	0x4208
#define ST7735_DDDD_WHITE	0x2104
#define	ST7735_BLACK			0x0000

// Макрос для генерации произвольного цвета по RGB565
#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))


typedef enum {
	Displ_Orientat_0,
	Displ_Orientat_90,
	Displ_Orientat_180,
	Displ_Orientat_270
} ST7735_Orientat_t;

extern int16_t _width;       								///< (oriented) display width
extern int16_t _height;      								///< (oriented) display height

/***************************
	* Задать нижний предел длинный данных после которого
  * включается DMA вместо обычного режима SPI
 ***************************/
#define DISPL_DMA_CUTOFF 	20

#define SPI_COMMAND GPIO_PIN_RESET  
#define SPI_DATA GPIO_PIN_SET		


void ST7735_Init(ST7735_Orientat_t m); // Инициализация дисплея, задаем начальную ориентацию экрана, рекомендуется Displ_Orientat_90
void ST7735_SetRotation(ST7735_Orientat_t m); // Изменение ориентации дисплея
ST7735_Orientat_t ST7735_GetRotation(void); // Получение информации о текущей ориентации дисплея


/* Установка уровня подсветки:
	!Учитываем, если "DIMMING MODE" отключен то доступны только команды: 'F', '1', '0', 'Q'
	'S' - перевести дисплей в режим ожидания (минимальный уровень яркости задается ST7735_STBY_LEVEL)
	'W' - вернуться из режима ожидания с прошлым значением яркости
	'+' - увеличить значение яркости на 1
	'-' - уменьшить значение яркости на 1
	'F', '1' - установить уровень яркости на маскимум
	'0' - установить яркость на минимум
	'I' - Инициализация для 'DIMMING MODE', обязательно выполняется во время начальной инициализации программы
	'Q' - Ничего не делать - возвращает текущее значение яркости                                             */
uint32_t ST7735_SetBacklight(uint8_t cmd); 

void ST7735_InvertColors(bool invert); // Инвертировать цветаы

void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7735_FillScreen(uint16_t color);
void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7735_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void ST7735_DrawBorder(int16_t x, int16_t y, uint16_t w, uint16_t h, int16_t t, uint16_t color);

void ST7735_DrawWChar(uint16_t x, uint16_t y, char ch, sFONT font, uint8_t size, uint16_t color, uint16_t bgcolor);
void ST7735_DrawWString(uint16_t x, uint16_t y, const char* str, sFONT font, uint8_t size, uint16_t color, uint16_t bgcolor);
void ST7735_DrawCSString(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* str, sFONT font, uint8_t size, uint16_t color, uint16_t bgcolor);

void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);

void ST7735_DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void ST7735_FillTriangle( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void ST7735_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void ST7735_FillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void ST7735_DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void ST7735_FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
void ST7735_DrawEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color);
void ST7735_FillEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color);
void ST7735_DrawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void ST7735_FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);

#endif
