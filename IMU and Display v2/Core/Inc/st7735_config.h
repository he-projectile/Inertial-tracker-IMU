/*
	Файл настроек библиотеки дисплея
*/
#ifndef __ST7735_CONF_H__
#define __ST7735_CONF_H__

#include "main.h"

/******************    ШАГ 1   ******************
 **************** Настройка пинов ***************
 ** указать название портов и пинов для дисплея *
 *********  Пины настриаваются в CubeMX *********/
 
#ifndef ST7735_RES_Pin
	#define ST7735_RES_Pin 			GPIO_PIN_4
#endif
#ifndef ST7735_RES_GPIO_Port
	#define ST7735_RES_GPIO_Port 	GPIOС
#endif

#ifndef ST7735_DC_Pin
	#define ST7735_DC_Pin 			GPIO_PIN_5
#endif
#ifndef ST7735_DC_GPIO_Port
	#define ST7735_DC_GPIO_Port 	GPIOС
#endif

#ifndef ST7735_CS_Pin
	#define ST7735_CS_Pin 			GPIO_PIN_0
#endif
#ifndef ST7735_CS_GPIO_Port
	#define ST7735_CS_GPIO_Port 	GPIOB
#endif

#ifndef ST7735_BL_Pin
	#define ST7735_BL_Pin			GPIO_PIN_1
#endif
#ifndef ST7735_BL_GPIO_Port
	#define ST7735_BL_GPIO_Port 	GPIOB
#endif

/******************    ШАГ 2   ******************
 **************** Настройка порта SPI ***********
 ** указать название структуры для работы SPI****
 *********  Порт SPI настраивается в CubeMX *****/
 
#define ST7735_SPI_PORT hspi1
#define ST7735_SPI SPI1


/******************    ШАГ 3   ******************
 ******* Настройка режима работы SPI ************
 ************* выбрать нужный режим *************
 *********** Также настроить в CubeMX ***********/
 
//#define DISPLAY_SPI_POLLING_MODE
//#define DISPLAY_SPI_INTERRUPT_MODE
#define DISPLAY_SPI_DMA_MODE


/******************    ШАГ 4  ******************
 ******* Настройка работы подсветки ************
 * Если нужно включить регулировку подсветки
 * дисплея, то нужно раскомментировать 
 * DISPLAY_DIMMING_MODE и установить defines
 * ниже для настройки ШИМ.
 *
 * Для того, чтобы просто включать/выключать
 * подсветку, оставляем DISPLAY_DIMMING_MODE
 * закомментированным
 * 
 * Если регулировка включена:
 * В CubeMX задаем пин ST7735_BL_Pin как выход ШИМ таймера N.
 * Значение регистра периода счетчика COUNTER PERIOD задаем слеующим образом:
 * задаем малое значение - порядка 10 - в случаях, когда используем кнопку для регулировки
 * задаем большее значение - порядка 100 - в случаях, когда для регулировки используем энкодер
 * делитель задаем таким образом, чтобы избежать мерцаний
 * тактовую частоту таймера рекомендуется задать: COUNTER PERIOD * 100 Hz */

#define DISPLAY_DIMMING_MODE					
#define ST7735_TIMER 					TIM3			
#define ST7735_T 							htim3		
#define ST7735_TIMER_CHANNEL	TIM_CHANNEL_4	
#define ST7735_TIMER_CCR			CCR4			
#define ST7735_STBY_LEVEL 			1				
#define ST7735_INIT_LEVEL 			5				

/******************    ШАГ 5  ******************
 ******* Настройка буфера кадра ****************
 * BUFLEVEL задает размер буфера:
 * размер буфера равен 2^BUFLEVEL, так BUFLEVEL = 2, то размер буфера = 4 байта.
 * Должно быть выбрано значение между 10 и 16:
 * 10 - позволяет хранить 1 символ Font24
 * 16 - полный кадр экрана 128x160 RGB565
 ************************************************/
 
#define BUFLEVEL 11
#define SIZEBUF 1<<BUFLEVEL


/******************    ШАГ 6  ******************
 ******* Выбор версии контроллера **************
 * В зависимости от версии контроллера ST7735 или ST7735S немного меняется инициализация дисплея
 * Раскомментировать нужную версию
 ***********************************************/

#define ST7735_DISPLAY 
//#define ST7735S_DISPLAY

#endif
