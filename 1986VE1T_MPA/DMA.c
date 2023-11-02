/*!
 \file
 \brief Заголовочный файл с реализацией API для работы с DMA
*/

/* 
 *	Буферы назначения (куда копирует данные DMA) должен находится в определенной области памяти (начиная с 0x20100000).
*/

#include "dma.h"
#include "spi.h"
#include "1273pv19t.h"


extern spi_n spi_1;
extern spi_n spi_2;

extern adc_n adc_1;

extern timer_n timer_1;
extern timer_n timer_2;
extern timer_n timer_3;

extern uart_n uart_1;
extern uart_n uart_2;
#ifdef K1986VE3T
extern uart_n uart_3;
extern uart_n uart_4;
#endif

/*
Функция общей инициализация DMA
*/
void dma_common_init(void)
{
	// Разрешить тактирование DMA
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_DMA | RST_CLK_PCLK_SSP1 |RST_CLK_PCLK_SSP2 |RST_CLK_PCLK_SSP3, ENABLE);
	#ifdef K1986VE3T
	RST_CLK_PCLKcmd(RST_CLK_PCLK_SSP4, ENABLE);
	#endif
	// Запретить все прерывания, в том числе от SSP
	NVIC->ICPR[0] = 0xFFFFFFFF;
	NVIC->ICER[0] = 0xFFFFFFFF;
	
	MDR_SSP1->DMACR=0;
	MDR_SSP2->DMACR=0;
	MDR_SSP3->DMACR=0;
	#ifdef K1986VE3T
	MDR_SSP4->DMACR=0;
	#endif
	// Сбросить все настройки DMA
	DMA_DeInit();
}
/*
Обработчик прерываний DMA
*/
void DMA_IRQHandler(void);
void DMA_IRQHandler(void)
{
	//если сработало прерывание при заполнении буфера приемника UART1
	if(DMA_GetFlagStatus(DMA_Channel_REQ_UART1_RX, DMA_FLAG_CHNL_ENA) == 0)
	{
		if (uart_1.uart_dma_ch.dma_irq_counter == ((UART_BUFFER_SIZE/1024) - 1))
		{
			DMA_UART_RX_init(&uart_1);
			uart_1.uart_dma_ch.dma_irq_counter = 0;
			uart_1.buffer_count = 0;
		}
	  else
		{
			uart_1.uart_dma_ch.DMA_InitStructure_UART_RX.DMA_DestBaseAddr += uart_1.uart_dma_ch.DMA_InitStructure_UART_RX.DMA_CycleSize;
			//Инициализировать канал
			DMA_Init(uart_1.uart_dma_ch.dma_channel, &uart_1.uart_dma_ch.DMA_Channel_UART_RX);
			uart_1.uart_dma_ch.dma_irq_counter++;
		}
	}
	//если сработало прерывание при заполнении буфера приемника UART2
	if(DMA_GetFlagStatus(DMA_Channel_REQ_UART2_RX, DMA_FLAG_CHNL_ENA) == 0)
	{
		if (uart_2.uart_dma_ch.dma_irq_counter == ((UART_BUFFER_SIZE/1024) - 1))
		{
			DMA_UART_RX_init(&uart_2);
			uart_2.uart_dma_ch.dma_irq_counter = 0;
		}
	  else
		{
			uart_2.uart_dma_ch.DMA_InitStructure_UART_RX.DMA_DestBaseAddr += uart_2.uart_dma_ch.DMA_InitStructure_UART_RX.DMA_CycleSize;
			 //Инициализировать канал
			DMA_Init(uart_2.uart_dma_ch.dma_channel, &uart_2.uart_dma_ch.DMA_Channel_UART_RX);
			uart_2.uart_dma_ch.dma_irq_counter++;
		}
	}
//	//если сработало прерывание от SPI1
//	if(DMA_GetFlagStatus(DMA_Channel_REQ_SSP1_RX, DMA_FLAG_CHNL_ENA) == 0)
//	{
//		MDR_PORTE->SETTX |= PORT_Pin_11;
//		if ((adc_1.init_flag == 1))
//		{		
//			if (spi_1.spi_dma_ch.dma_irq_counter == (CHANEL_NUMBER-1))
//			{				
//				dma_spi_rx_init(&spi_1);
//				spi_1.spi_dma_ch.dma_irq_counter = 0;
//				//MDR_PORTE->CLRTX = PORT_Pin_11;
//			}
//			else
//			{
//				spi_1.spi_dma_ch.DMA_InitStructure_SPI_RX.DMA_DestBaseAddr += spi_1.spi_dma_ch.DMA_InitStructure_SPI_RX.DMA_CycleSize*2;
//				DMA_Init(spi_1.spi_dma_ch.dma_channel, &spi_1.spi_dma_ch.DMA_Channel_SPI_RX);
//				spi_1.spi_dma_ch.dma_irq_counter++;
//				//MDR_PORTE->CLRTX = PORT_Pin_11;
//			}
//			//spi_1.SSPx->DR = 0x7FFF;
//			//выключение SPI
//  		//spi_1.SSPx->CR1 &= 0xFFFD;
//		}
//		MDR_PORTE->CLRTX = PORT_Pin_11;
//	}
	#ifdef K1986VE3T
	//если сработало прерывание при заполнении буфера приемника UART3
	if(DMA_GetFlagStatus(DMA_Channel_SW12, DMA_FLAG_CHNL_ENA) == RESET)
	{
		if (uart_3.uart_dma_ch.dma_irq_counter == ((UART_BUFFER_SIZE/1024) - 1))
		{
			DMA_UART_RX_init(&uart_3);
			uart_3.uart_dma_ch.dma_irq_counter = 0;
			uart_3.buffer_count = 0;
		}
	  else
		{
			uart_3.uart_dma_ch.DMA_InitStructure_UART_RX.DMA_DestBaseAddr += uart_3.uart_dma_ch.DMA_InitStructure_UART_RX.DMA_CycleSize;
			//Инициализировать канал
			DMA_Init(uart_3.uart_dma_ch.dma_channel, &uart_3.uart_dma_ch.DMA_Channel_UART_RX);
			uart_3.uart_dma_ch.dma_irq_counter++;
		}
	}
	//если сработало прерывание при заполнении буфера приемника UART4
	if(DMA_GetFlagStatus(DMA_Channel_SW14, DMA_FLAG_CHNL_ENA) == RESET)
	{
		if (uart_4.uart_dma_ch.dma_irq_counter == ((UART_BUFFER_SIZE/1024) - 1))
		{
			DMA_UART_RX_init(&uart_4);
			uart_4.uart_dma_ch.dma_irq_counter = 0;
			uart_4.buffer_count = 0;
		}
	  else
		{
			uart_4.uart_dma_ch.DMA_InitStructure_UART_RX.DMA_DestBaseAddr += uart_4.uart_dma_ch.DMA_InitStructure_UART_RX.DMA_CycleSize;
			//Инициализировать канал
			DMA_Init(uart_4.uart_dma_ch.dma_channel, &uart_4.uart_dma_ch.DMA_Channel_UART_RX);
			uart_4.uart_dma_ch.dma_irq_counter++;
		}
	}
	#endif
}
