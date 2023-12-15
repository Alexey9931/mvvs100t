/*!
 \file
 \brief Заголовочный файл с описанием API для работы с таймерами
*/

#ifndef __TIMERS_H
#define __TIMERS_H

#include "mdr32_drivers.h"
#include <stdio.h>
#include "list.h"

///Структура с конфигурационными параметрами Таймеров 
typedef struct timer_config_struct
{
	MDR_TIMER_TypeDef* 						TIMERx;            							///< Библиотечная структура с периферийными регистрами блока TIMER
	TIMER_CntInitTypeDef 					TIMERInitStruct;            		///< Библиотечная структура с конфигурационными параметрами блока TIMER
	TIMER_ChnInitTypeDef 					sTIM_ChnInit;										///< Библиотечная структура с конфигурационными параметрами каналов блока TIMER (используются например для режима захвата)
	TIMER_Status_Flags_TypeDef		TIMER_STATUS;										///< Настрока событий, по которому происходит прерывание блока TIMER	
	uint32_t											timer_cnt;											///< Счетчик для TIMER (может быть использован для разных целей)	
} __attribute__((packed)) timer_n;	


///структура-реализация односвязанного списка обработчиков прерываний таймеров
typedef struct timer_irq_list_struct
{
	void(*handler)(void*);						///< Указатель на функцию-обработчик прерывания			
	TIMER_Status_Flags_TypeDef event;	///< Событие, по которому срабатывает прерывание
	void *data;												///< Указатель на данные, передаваемые в обработчик (при необходимости)
	list_head list;										///< Список прерываний
} __attribute__((packed)) timer_irq_list;

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

/*!
 *	\brief Добавляет обработчик прерывания таймера в список обработчиков
 *	\param tmr_num - Номер таймера
 *	\param func_ptr - Указатель на функцию-обработчик прерывания
 *	\param data - Указатель на данные в обработчике прерывания
 *	\param event - Событие, по которому вызывается данное прерывание
*/
void list_tmr_handler_add_tail(uint8_t tmr_num, void (*func_ptr)(void*), void *data, TIMER_Status_Flags_TypeDef event);

/*!
 *	\brief Инициализирует список обработчиков прерываний таймера
 *	\param tmr_num - Номер таймера, для которого инициализируется список
*/
void list_tmr_handler_init(uint8_t tmr_num);

#endif /*__TIMERS_H */
