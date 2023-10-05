/*!
 \file
 \brief Заголовочный файл с описанием API для работы с таймерами
*/

#ifndef __TIMER_H
#define __TIMER_H

#include "MDR32_Drivers.h"
#include <stdio.h>

///ножка на вход прерывания SDIFS/SDOFS от АЦП (канал захвата для таймера)
#define PORT_ADC_SDIFS_IRQ MDR_PORTE
#define PIN_ADC_SDIFS_IRQ PORT_Pin_8

///Структура с конфигурационными параметрами Таймеров 
typedef struct timer_config_struct
{
	MDR_TIMER_TypeDef* 						TIMERx;            							///< Библиотечная структура с периферийными регистрами блока TIMER
	TIMER_CntInitTypeDef 					TIMERInitStruct;            		///< Библиотечная структура с конфигурационными параметрами блока TIMER
	TIMER_ChnInitTypeDef 					sTIM_ChnInit;										///< Библиотечная структура с конфигурационными параметрами каналов блока TIMER (используются например для режима захвата)
	uint32_t 											RST_CLK_PCLK_TIMERn;						///< Включение тактирования для блока TIMER
	TIMER_Clock_BRG_TypeDef 			TIMER_HCLKdiv;									///< Выбор делителя тактовой частоты для тактирования блока TIMER	
	IRQn_Type 										IRQn;														///< Выбор обработчика прерываний блока TIMER
	TIMER_Status_Flags_TypeDef		TIMER_STATUS;										///< Настрока события, по которому происходит прерывание блока TIMER	
} timer_n;	


/*!
 *	\brief Инициализацирует выбранный Timer
 *	\param timer - Таймер для инициализации
*/
void timer_init(timer_n *timer_struct);

/*!
 *	\brief Реализует задержку в мс
 *	\param time_milli - Задержка в мс
*/
void delay_milli(uint32_t time_milli);

/*!
 *	\brief Реализует задержку в мкс
 *	\param time_micro - Задержка в мкс
*/
void delay_micro(uint32_t time_micro);


#endif /*__TIMER_H */