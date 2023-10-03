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

/*!
 *	\brief Конфигурирует выводы МК для каналов таймеров
*/
void timer_gpio_config(void);

/*!
 *	\brief Инициализацирует Timer1
*/
void timer1_init(void);

/*!
 *	\brief Инициализацирует Timer3
*/
void timer3_init(void);

/*!
 *	\brief Инициализацирует Timer2
*/
void timer2_init(void);

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