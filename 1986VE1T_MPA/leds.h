/*!
 \file
 \brief Заголовочный файл с описанием API для работы со светоиодными индикаторами
*/

#ifndef __LEDS_H
#define __LEDS_H

#include "mdr32_drivers.h"

#define PORT_LEDS MDR_PORTD
#define CLOCK_LEDS RST_CLK_PCLK_PORTD

//светодиоды для индикации работы/неисправности
///светодиод-индикатор корректной работы
#define PIN_LED_OK_WORK PORT_Pin_7
///светодиод-индикатор неисправности
#define PIN_LED_ERROR_WORK PORT_Pin_8

#define SET_LED_OK_WORK()		 		{PORT_LEDS->SETTX = PIN_LED_OK_WORK;}
#define RESET_LED_OK_WORK() 		{PORT_LEDS->CLRTX = PIN_LED_OK_WORK;}

#define SET_LED_ERROR_WORK() 		{PORT_LEDS->SETTX = PIN_LED_ERROR_WORK;}
#define RESET_LED_ERROR_WORK() 	{PORT_LEDS->CLRTX = PIN_LED_ERROR_WORK;}

/*!
 *	\brief Конфигурирует выводы МК для светодиодных индикаторов
*/
void leds_gpio_config(void);


#endif /*__LEDS_H */