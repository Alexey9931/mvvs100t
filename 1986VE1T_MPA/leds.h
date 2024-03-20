/*!
 \file
 \brief Заголовочный файл с описанием API для работы со светодиодными индикаторами
*/

#ifndef __LEDS_H
#define __LEDS_H

#include "mdr32_drivers.h"

#define PORT_LEDS MDR_PORTD								///< Порт выводов на светодиоды
#define CLOCK_LEDS RST_CLK_PCLK_PORTD			///< Выбор тактирования выводов для светодиодов

#define PIN_LED_OK_WORK PORT_Pin_7				///< Светодиод-индикатор корректной работы
#define PIN_LED_ERROR_WORK PORT_Pin_8			///< Светодиод-индикатор неисправности	

#define SET_LED_OK_WORK()		 		{PORT_LEDS->SETTX = PIN_LED_OK_WORK;}			///< Макрос включения светодиода корректной работы
#define RESET_LED_OK_WORK() 		{PORT_LEDS->CLRTX = PIN_LED_OK_WORK;}			///< Макрос выключения светодиода корректной работы

#define SET_LED_ERROR_WORK() 		{PORT_LEDS->SETTX = PIN_LED_ERROR_WORK;}	///< Макрос включения светодиода некорректной работы
#define RESET_LED_ERROR_WORK() 	{PORT_LEDS->CLRTX = PIN_LED_ERROR_WORK;}	///< Макрос выключения светодиода некорректной работы

/*!
 *	\brief Конфигурирует выводы МК для светодиодных индикаторов
*/
void leds_gpio_config(void); 


#endif /*__LEDS_H */
