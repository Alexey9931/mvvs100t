/*!
 \file
 \brief Файл с реализацией API для работы с таймерами
*/

#include "TIMER.h"
#include "SPI.h"
#include "1273pv19t.h"


extern adc_n adc_1;

timer_n timer_1;
timer_n timer_2;
timer_n timer_3;

extern spi_n spi_1;
/*
Функция конфигурации выводов МК для каналов Timer2
*/
void timer2_gpio_config(void);
void timer2_gpio_config(void)
{
	// Включение тактирования портов
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK|RST_CLK_PCLK_PORTE, ENABLE);	
	
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
void timer1_init(timer_n *timer_struct);
void timer1_init(timer_n *timer_struct)
{
	TIMER_CntInitTypeDef TIMER1InitStruct;
	
  RST_CLK_PCLKcmd (timer_struct->RST_CLK_PCLK_TIMERn, ENABLE);
	/* Initializes the TIMERx Counter */
	TIMER_CntStructInit(&TIMER1InitStruct);
	TIMER1InitStruct.TIMER_Period = timer_struct->TIMERInitStruct.TIMER_Period;
	TIMER1InitStruct.TIMER_Prescaler = timer_struct->TIMERInitStruct.TIMER_Prescaler;
	 
	TIMER_CntInit(timer_struct->TIMERx, &TIMER1InitStruct);
	
  /* Enable TIMER1 clock */
  TIMER_BRGInit(timer_struct->TIMERx,timer_struct->TIMER_HCLKdiv);

	/* Enable TIMER1 */
  TIMER_Cmd(timer_struct->TIMERx,ENABLE);
}
/*
Функция инициализации Timer3 (настроен на период 1сек)
*/
void timer3_init(timer_n *timer_struct);
void timer3_init(timer_n *timer_struct)
{
	TIMER_CntInitTypeDef TIMER3InitStruct;
	
  RST_CLK_PCLKcmd (timer_struct->RST_CLK_PCLK_TIMERn, ENABLE);
	/* Initializes the TIMERx Counter */
	TIMER_CntStructInit(&TIMER3InitStruct);
	TIMER3InitStruct.TIMER_Period = timer_struct->TIMERInitStruct.TIMER_Period;
	TIMER3InitStruct.TIMER_Prescaler = timer_struct->TIMERInitStruct.TIMER_Prescaler;
	 
	TIMER_CntInit(timer_struct->TIMERx, &TIMER3InitStruct);
	
  /* Enable TIMER3 clock */
  TIMER_BRGInit(timer_struct->TIMERx,timer_struct->TIMER_HCLKdiv);

	/* Enable TIMER3 */
  TIMER_Cmd(timer_struct->TIMERx,ENABLE);
}
/*
Функция инициализации Timer2 (настроен для режима захвата-для нужд АЦП)
*/
void timer2_init(timer_n *timer_struct);
void timer2_init(timer_n *timer_struct)
{
	TIMER_CntInitTypeDef TIMER2InitStruct;
	TIMER_ChnInitTypeDef sTIM2_ChnInit;

	RST_CLK_PCLKcmd (timer_struct->RST_CLK_PCLK_TIMERn, ENABLE);

	/* Initializes the TIMERx Counter */
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
	TIMER_CntInit (timer_struct->TIMERx, &TIMER2InitStruct);
	
	TIMER_ChnStructInit (&sTIM2_ChnInit);
	sTIM2_ChnInit.TIMER_CH_Number = timer_struct->sTIM_ChnInit.TIMER_CH_Number;
  sTIM2_ChnInit.TIMER_CH_Mode = timer_struct->sTIM_ChnInit.TIMER_CH_Mode;
  sTIM2_ChnInit.TIMER_CH_EventSource = timer_struct->sTIM_ChnInit.TIMER_CH_EventSource;
  TIMER_ChnInit (timer_struct->TIMERx, &sTIM2_ChnInit);
	 
	TIMER_ChnCCR1_Cmd(timer_struct->TIMERx, sTIM2_ChnInit.TIMER_CH_Number, ENABLE);

	NVIC_EnableIRQ(timer_struct->IRQn);
	TIMER_ITConfig(timer_struct->TIMERx, timer_struct->TIMER_STATUS, ENABLE);
	//NVIC_SetPriority(timer_struct->IRQn, 0);

	/* Enable TIMER2 clock */
  TIMER_BRGInit(timer_struct->TIMERx,timer_struct->TIMER_HCLKdiv);
	/* Enable TIMER2 */
  TIMER_Cmd(timer_struct->TIMERx,ENABLE);
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
		timer2_gpio_config();
		timer2_init(timer_struct);
	}
	else if (timer_struct->TIMERx == MDR_TIMER3)
	{
		timer3_init(timer_struct);
	}
}
/*
Обработчик преывааний по захвату Timer2
*/
void TIMER2_IRQHandler(void);
void TIMER2_IRQHandler(void)
{
	//MDR_PORTE->SETTX = PORT_Pin_11;
	//включение SPI
//	spi_1.SSPx->CR1 |= 0x0002;
	if (adc_1.init_flag == 1)
	{
		//MDR_PORTE->SETTX = PORT_Pin_11;
		//включение SPI
		//spi_1.SSPx->CR1 |= 0x0002;
//		//отключаем прерывание от таймера
//		NVIC_DisableIRQ(timer_2.IRQn);
//		TIMER_Cmd(timer_2.TIMERx,DISABLE);
		if (spi_1.spi_dma_ch.dma_irq_counter == 0)
		{
			MDR_PORTE->SETTX = PORT_Pin_11;
			//включение SPI
			spi_1.SSPx->CR1 |= 0x0002;
			//включаем DMA от SPI1
			//SSP_DMACmd(spi_1.SSPx, SSP_DMA_RXE, ENABLE);
			spi_1.SSPx->DMACR |= SSP_DMA_RXE;
			// Разрешить работу DMA с SPI
			MDR_DMA->CHNL_ENABLE_SET = (1 << spi_1.spi_dma_ch.dma_channel);
			//DMA_Cmd (spi_1.spi_dma_ch.dma_channel, ENABLE);
		}
//		//включаем DMA от SPI1
//		SSP_DMACmd(spi_1.SSPx, SSP_DMA_RXE, ENABLE);
//		// Разрешить работу DMA с SPI
//		DMA_Cmd (spi_1.spi_dma_ch.dma_channel, ENABLE);
		
		//SSP_SendData(adc_1.spi_struct->SSPx, 0x7FFF);
//		while ((DMA_Ctrl_Rx & 3) != 0){}
//		//выключаем DMA от SPI1
//		SSP_DMACmd(spi_1.SSPx, SSP_DMA_RXE, DISABLE);
//		// Запретить работу DMA с SPI
//		DMA_Cmd (spi_1.spi_dma_ch.dma_channel, DISABLE);
//			//включаем прерывание от таймера
//		NVIC_EnableIRQ(timer_2.IRQn);
//		TIMER_Cmd(timer_2.TIMERx,ENABLE);	
//		//выключение SPI
//		spi_1.SSPx->CR1 &= 0xFFFD;
//		timer_2.TIMERx->STATUS = ~timer_2.TIMER_STATUS;
	}
	else
	{
		//включение SPI
		spi_1.SSPx->CR1 |= 0x0002;
		delay_micro(7);
		spi_1.fifo_halfword = (spi_1.SSPx->DR);
		//выключение SPI
		spi_1.SSPx->CR1 &= 0xFFFD;
	}
	//while (SSP_GetFlagStatus(spi_1.SSPx, SSP_FLAG_RNE) != SET)  {}  //ждем пока появится байт  
//	delay_micro(7);
//	MDR_TIMER1->CNT = 0;
//	while((MDR_TIMER1->CNT)!=7);
	//spi_1.fifo_halfword = (spi_1.SSPx->DR);
	//выключение SPI
	//spi_1.SSPx->CR1 &= 0xFFFD;
	//timer_2.TIMERx->STATUS = ~timer_2.TIMER_STATUS;
	//TIMER_ClearITPendingBit(timer_2.TIMERx, timer_2.TIMER_STATUS);
	//MDR_PORTE->CLRTX = PORT_Pin_11;
	timer_2.TIMERx->STATUS = ~timer_2.TIMER_STATUS;
}
/*
Функция реализации задержки в мс
*/
void delay_milli(uint32_t time_milli)//задержка в мс 
{ 
	TIMER_SetCounter(MDR_TIMER3, 0);
	while(TIMER_GetCounter(MDR_TIMER3)!=(time_milli*50));
}
/*
Функция реализации задержки в мкс
*/
void delay_micro(uint32_t time_micro)//задержка в мкс (максимум 10мс -> time_micro=9999)
{ 
	TIMER_SetCounter(MDR_TIMER1, 0);
	while(TIMER_GetCounter(MDR_TIMER1)!=time_micro);
}
