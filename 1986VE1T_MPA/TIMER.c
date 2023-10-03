/*!
 \file
 \brief Файл с реализацией API для работы с таймерами
*/

#include "TIMER.h"

/*
Функция конфигурации выводов МК для каналов таймеров
*/
void timer_gpio_config(void)
{
	//Инициализация порта E для канала 2 таймера 2 на вход прерывания SDIFS/SDOFS от АЦП (канал захвата для таймера)
	PORT_InitTypeDef GPIO_init_struct_TIMER2;
	
	PORT_StructInit (&GPIO_init_struct_TIMER2);
	GPIO_init_struct_TIMER2.PORT_FUNC = PORT_FUNC_MAIN;
	GPIO_init_struct_TIMER2.PORT_OE = PORT_OE_IN;
	GPIO_init_struct_TIMER2.PORT_SPEED = PORT_SPEED_MAXFAST;
	GPIO_init_struct_TIMER2.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_init_struct_TIMER2.PORT_Pin = PIN_ADC_SDIFS_IRQ;
	GPIO_init_struct_TIMER2.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
	PORT_Init (PORT_ADC_SDIFS_IRQ, &GPIO_init_struct_TIMER2);
}
/*
Функция инициализации Timer1 (настроен на период 10мс)
*/
void timer1_init(void)
{
	TIMER_CntInitTypeDef TIMER1InitStruct;
	
  RST_CLK_PCLKcmd (RST_CLK_PCLK_TIMER1, ENABLE);
	/* Initializes the TIMERx Counter */
	TIMER_CntStructInit(&TIMER1InitStruct);
	TIMER1InitStruct.TIMER_Period = 0x270F;//10000-1
	TIMER1InitStruct.TIMER_Prescaler = WORK_FREQ - 1;//128-1
	 
	TIMER_CntInit(MDR_TIMER1, &TIMER1InitStruct);
	
  /* Enable TIMER1 clock */
  TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);

	/* Enable TIMER1 */
  TIMER_Cmd(MDR_TIMER1,ENABLE);
}
/*
Функция инициализации Timer3 (настроен на период 1сек)
*/
void timer3_init(void)
{
	TIMER_CntInitTypeDef TIMER3InitStruct;
	
  RST_CLK_PCLKcmd (RST_CLK_PCLK_TIMER3, ENABLE);
	/* Initializes the TIMERx Counter */
	TIMER_CntStructInit(&TIMER3InitStruct);
	TIMER3InitStruct.TIMER_Period = 0xC34F;//50000-1
	TIMER3InitStruct.TIMER_Prescaler = WORK_FREQ*20 - 1;//2560-1
	 
	TIMER_CntInit(MDR_TIMER3, &TIMER3InitStruct);
	
  /* Enable TIMER3 clock */
  TIMER_BRGInit(MDR_TIMER3,TIMER_HCLKdiv1);

	/* Enable TIMER3 */
  TIMER_Cmd(MDR_TIMER3,ENABLE);
}
/*
Функция инициализации Timer2 (настроен для режима захвата-для нужд АЦП)
*/
void timer2_init(void)
{
	TIMER_CntInitTypeDef TIMER2InitStruct;
	TIMER_ChnInitTypeDef sTIM2_ChnInit;

	RST_CLK_PCLKcmd (RST_CLK_PCLK_TIMER2, ENABLE);

	/* Initializes the TIMERx Counter */
	TIMER_CntStructInit(&TIMER2InitStruct);
	TIMER2InitStruct.TIMER_Prescaler = 0;
	TIMER2InitStruct.TIMER_Period = 0;
	TIMER2InitStruct.TIMER_CounterDirection = TIMER_CntDir_Up;
	TIMER2InitStruct.TIMER_CounterMode = TIMER_CntMode_EvtFixedDir;
	TIMER2InitStruct.TIMER_EventSource = TIMER_EvSrc_CH2;
	TIMER2InitStruct.TIMER_ARR_UpdateMode = TIMER_ARR_Update_Immediately;
	TIMER2InitStruct.TIMER_FilterSampling = TIMER_FDTS_TIMER_CLK_div_1;
	TIMER2InitStruct.TIMER_ETR_FilterConf = TIMER_Filter_8FF_at_FTDS_div_32;
	TIMER2InitStruct.TIMER_ETR_Prescaler = TIMER_ETR_Prescaler_None;
	TIMER2InitStruct.TIMER_ETR_Polarity = TIMER_ETRPolarity_NonInverted;
	TIMER2InitStruct.TIMER_BRK_Polarity = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit (MDR_TIMER2, &TIMER2InitStruct);
	
	TIMER_ChnStructInit (&sTIM2_ChnInit);
	sTIM2_ChnInit.TIMER_CH_Number = TIMER_CHANNEL2;
  sTIM2_ChnInit.TIMER_CH_Mode = TIMER_CH_MODE_CAPTURE;
  sTIM2_ChnInit.TIMER_CH_EventSource = TIMER_CH_EvSrc_PE; //отрицательный фронт
  TIMER_ChnInit (MDR_TIMER2, &sTIM2_ChnInit);
	 
	TIMER_ChnCCR1_Cmd(MDR_TIMER2, TIMER_CHANNEL2, ENABLE);

	NVIC_EnableIRQ(TIMER2_IRQn);
	TIMER_ITConfig(MDR_TIMER2, TIMER_STATUS_CCR_CAP1_CH2, ENABLE);
	NVIC_SetPriority(TIMER2_IRQn, 0);

	/* Enable TIMER2 clock */
  TIMER_BRGInit(MDR_TIMER2,TIMER_HCLKdiv1);
	/* Enable TIMER2 */
  TIMER_Cmd(MDR_TIMER2,ENABLE);
}
/*
Обработчик преывааний по захвату Timer2
*/
void TIMER2_IRQHandler(void);
void TIMER2_IRQHandler(void)
{
  SSP_Cmd(MDR_SSP1, ENABLE);
//	FIFO_SPI_RX = SPI_ReceiveWord(MDR_SSP1);
	SSP_Cmd(MDR_SSP1, DISABLE);
	TIMER_ClearITPendingBit(MDR_TIMER2, TIMER_STATUS_CCR_CAP1_CH2);
}
/*
Функция реализации задержки в мс
*/
void delay_milli(uint32_t time_milli)//задержка в мс 
{ 
	for (uint32_t i = 0; i < time_milli; i++)
	{
		delay_micro(1000);
	}
}
/*
Функция реализации задержки в мкс
*/
void delay_micro(uint32_t time_micro)//задержка в мкс (максимум 10мс -> time_micro=9999)
{ 
	TIMER_SetCounter(MDR_TIMER1, 0);
	while(TIMER_GetCounter(MDR_TIMER1)!=time_micro);
}
