/*!
 \file
 \brief Файл с реализацией API для работы с таймерами
*/

#include "timers.h"
#include "spi.h"
#include "1273pv19t.h"
#include "external_ram.h"
#include <math.h>
#include "main.h"

/// Структура с конфигурационными параметрами микросхемы АЦП
extern adc_n adc_1;
/// Указатель для обращения к внешнему ОЗУ
extern ram_data *ram_space_pointer;

/// Указатели на списки обработчиков таймеров
list_head tmr_handler_head[TIMER_NUM];

/// Структура с конфигурационными параметрами блока Timer1 МК
timer_n timer_1;
/// Структура с конфигурационными параметрами блока Timer2 МК
timer_n timer_2;
/// Структура с конфигурационными параметрами блока Timer3 МК
timer_n timer_3;

/// Структура с конфигурационными параметрами блока SPI МК
extern spi_n spi_1;

/*!
 *	\brief Инициализирует Timer1
*/
void timer1_init(timer_n *timer_struct);
void timer1_init(timer_n *timer_struct)
{
	TIMER_CntInitTypeDef TIMER1InitStruct;
	
  RST_CLK_PCLKcmd (RST_CLK_PCLK_TIMER1, ENABLE);

	TIMER_CntStructInit(&TIMER1InitStruct);
	TIMER1InitStruct.TIMER_Period = timer_struct->TIMERInitStruct.TIMER_Period;
	TIMER1InitStruct.TIMER_Prescaler = timer_struct->TIMERInitStruct.TIMER_Prescaler;
	 
	TIMER_CntInit(MDR_TIMER1, &TIMER1InitStruct);
	
  TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);

  TIMER_Cmd(MDR_TIMER1,ENABLE);
	TIMER_SetCounter(MDR_TIMER1, 0);
}
/*!
 *	\brief Инициализирует Timer3
*/
void timer3_init(timer_n *timer_struct);
void timer3_init(timer_n *timer_struct)
{
	TIMER_CntInitTypeDef TIMER3InitStruct;
	
  RST_CLK_PCLKcmd (RST_CLK_PCLK_TIMER3, ENABLE);

	TIMER_CntStructInit(&TIMER3InitStruct);
	TIMER3InitStruct.TIMER_Period = timer_struct->TIMERInitStruct.TIMER_Period;
	TIMER3InitStruct.TIMER_Prescaler = timer_struct->TIMERInitStruct.TIMER_Prescaler;
	 
	TIMER_CntInit(MDR_TIMER3, &TIMER3InitStruct);
	
  TIMER_BRGInit(MDR_TIMER3, TIMER_HCLKdiv1);

  TIMER_Cmd(MDR_TIMER3, ENABLE);
	TIMER_SetCounter(MDR_TIMER3, 0);
}
/*!
 *	\brief Инициализирует Timer2
*/
void timer2_init(timer_n *timer_struct);
void timer2_init(timer_n *timer_struct)
{
	TIMER_CntInitTypeDef TIMER2InitStruct;
	TIMER_ChnInitTypeDef sTIM2_ChnInit;

	RST_CLK_PCLKcmd (RST_CLK_PCLK_TIMER2, ENABLE);

	TIMER_CntStructInit(&TIMER2InitStruct);
	TIMER2InitStruct.TIMER_Prescaler = timer_struct->TIMERInitStruct.TIMER_Prescaler;
	TIMER2InitStruct.TIMER_Period = timer_struct->TIMERInitStruct.TIMER_Period;
	TIMER2InitStruct.TIMER_CounterDirection = timer_struct->TIMERInitStruct.TIMER_CounterDirection;
	TIMER2InitStruct.TIMER_CounterMode = timer_struct->TIMERInitStruct.TIMER_CounterMode;
	TIMER2InitStruct.TIMER_EventSource = timer_struct->TIMERInitStruct.TIMER_EventSource;
	TIMER2InitStruct.TIMER_ARR_UpdateMode = timer_struct->TIMERInitStruct.TIMER_ARR_UpdateMode;
	TIMER2InitStruct.TIMER_FilterSampling = timer_struct->TIMERInitStruct.TIMER_FilterSampling;
	TIMER2InitStruct.TIMER_ETR_FilterConf = timer_struct->TIMERInitStruct.TIMER_ETR_FilterConf;
	TIMER2InitStruct.TIMER_ETR_Prescaler = timer_struct->TIMERInitStruct.TIMER_ETR_Prescaler;
	TIMER2InitStruct.TIMER_ETR_Polarity = timer_struct->TIMERInitStruct.TIMER_ETR_Polarity;
	TIMER2InitStruct.TIMER_BRK_Polarity = timer_struct->TIMERInitStruct.TIMER_BRK_Polarity;
	TIMER_CntInit (MDR_TIMER2, &TIMER2InitStruct);
	
	TIMER_ChnStructInit (&sTIM2_ChnInit);
	sTIM2_ChnInit.TIMER_CH_Number = timer_struct->sTIM_ChnInit.TIMER_CH_Number;
  sTIM2_ChnInit.TIMER_CH_Mode = timer_struct->sTIM_ChnInit.TIMER_CH_Mode;
  sTIM2_ChnInit.TIMER_CH_EventSource = timer_struct->sTIM_ChnInit.TIMER_CH_EventSource;
  TIMER_ChnInit (MDR_TIMER2, &sTIM2_ChnInit);
	 
	TIMER_ChnCCR1_Cmd(MDR_TIMER2, sTIM2_ChnInit.TIMER_CH_Number, ENABLE);

	TIMER_ITConfig(MDR_TIMER2, timer_struct->TIMER_STATUS, ENABLE);
	TIMER_ClearITPendingBit(MDR_TIMER2, TIMER_STATUS_CNT_ARR);
	TIMER_ITConfig(MDR_TIMER2, TIMER_STATUS_CNT_ARR, DISABLE);
	NVIC_EnableIRQ(TIMER2_IRQn);
	//NVIC_SetPriority(timer_struct->IRQn, 0);

  TIMER_BRGInit(MDR_TIMER2,TIMER_HCLKdiv1);

  TIMER_Cmd(MDR_TIMER2,ENABLE);
	TIMER_SetCounter(MDR_TIMER2, 0);
}
/*
	Функция инициализации выбранного Timer 
*/
void timer_init(timer_n *timer_struct)
{
	if (timer_struct->TIMERx == MDR_TIMER1)
	{
		timer1_init(timer_struct);
	}
	else if (timer_struct->TIMERx == MDR_TIMER2)
	{
		timer2_init(timer_struct);
	}
	else if (timer_struct->TIMERx == MDR_TIMER3)
	{
		timer3_init(timer_struct);
	}
}
/*!
 *	\brief Обработчик прерываний Timer2 (Обработка прерываний путем обхода двусвязного списка реализована аналогично linux kernel)
*/
void TIMER2_IRQHandler(void);
void TIMER2_IRQHandler(void)
{
	timer_irq_list *ep;
	list_for_each_entry(ep, &tmr_handler_head[1], list)
	{
		if (TIMER_GetITStatus(timer_2.TIMERx, ep->event) == SET)
		{
			ep->handler(ep->data);
		}
	}
}
/*
	Функция реализации задержки в мс
*/
void delay_milli(uint32_t time_milli)//задержка в мс 
{ 
	TIMER_SetCounter(MDR_TIMER3, 0);
	while (TIMER_GetCounter(MDR_TIMER3) <=(time_milli*50));
}
/*
	Функция реализации задержки в мкс
*/
void delay_micro(uint32_t time_micro)//задержка в мкс (максимум 10мс -> time_micro=9999)
{ 
	TIMER_SetCounter(MDR_TIMER1, 0);
	while (TIMER_GetCounter(MDR_TIMER1) <= time_micro);
}
/*
	Функция добавления обработчиков прерываний таймеров в список обработчиков
*/
void list_tmr_handler_add_tail(uint8_t tmr_num, void (*func_ptr)(void*), void *data, TIMER_Status_Flags_TypeDef event)
{
	// Выделяем память для нового указателя
	timer_irq_list *ptr = (timer_irq_list*)malloc_ram_pages(sizeof(timer_irq_list));
	// Инициализируем элемент списка
	ptr->data = data;
	ptr->event = event;
	ptr->handler = func_ptr;
	
	list_add_tail(&ptr->list, &tmr_handler_head[tmr_num]);
}
/*
	Функция инициализации списка обработчиков прерываний таймера
*/
void list_tmr_handler_init(uint8_t tmr_num)
{
	init_list_head(&tmr_handler_head[tmr_num]);
}
