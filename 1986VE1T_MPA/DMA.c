#include "DMA.h"

extern UARTn UART1;
extern UARTn UART2;
#ifdef K1986VE3T
extern UARTn UART3;
extern UARTn UART4;
#endif


/*
������� ������������� DMA
*/
void DMA_common_init(void)
{
	// ��������� ������������ DMA
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_DMA | RST_CLK_PCLK_SSP1 |RST_CLK_PCLK_SSP2 |RST_CLK_PCLK_SSP3, ENABLE);
	#ifdef K1986VE3T
	RST_CLK_PCLKcmd(RST_CLK_PCLK_SSP4, ENABLE);
	#endif
	// ��������� ��� ����������, � ��� ����� �� SSP
	NVIC->ICPR[0] = 0xFFFFFFFF;
	NVIC->ICER[0] = 0xFFFFFFFF;
	
	MDR_SSP1->DMACR=0;
	MDR_SSP2->DMACR=0;
	MDR_SSP3->DMACR=0;
	#ifdef K1986VE3T
	MDR_SSP4->DMACR=0;
	#endif
	// �������� ��� ��������� DMA
	DMA_DeInit();
}
/*
���������� ���������� DMA
*/
void DMA_IRQHandler(void);
void DMA_IRQHandler(void)
{
	#ifdef K1986VE1T
	//���� ��������� ���������� ��� ���������� ������ ��������� UART1
	if(DMA_GetFlagStatus(DMA_Channel_REQ_UART1_RX, DMA_FLAG_CHNL_ENA) == RESET)
	{
		//if (UART1.uart_dma_ch.dma_irq_counter == ((BUFFER_SIZE/1024) - 1))
		//{
			DMA_UART_RX_init(&UART1);
		//	UART1.uart_dma_ch.dma_irq_counter = 0;
		//}
	  //else
		//{
		//	UART1.uart_dma_ch.DMA_InitStructure_UART_RX.DMA_DestBaseAddr += UART1.uart_dma_ch.DMA_InitStructure_UART_RX.DMA_CycleSize;
			// ���������������� �����
		//	DMA_Init(UART1.uart_dma_ch.dma_channel, &UART1.uart_dma_ch.DMA_Channel_UART_RX);
		//	UART1.uart_dma_ch.dma_irq_counter++;
		//}
	}
	//���� ��������� ���������� ��� ���������� ������ ��������� UART2
	if(DMA_GetFlagStatus(DMA_Channel_REQ_UART2_RX, DMA_FLAG_CHNL_ENA) == RESET)
	{
		DMA_UART_RX_init(&UART2);
	}
	#endif
	#ifdef K1986VE3T
	//���� ��������� ���������� ��� ���������� ������ ��������� UART1
	if(DMA_GetFlagStatus(DMA_Channel_REQ_UART1_RX, DMA_FLAG_CHNL_ENA) == RESET)
	{
		DMA_UART_RX_init(&UART1);
	}
	//���� ��������� ���������� ��� ���������� ������ ��������� UART2
	if(DMA_GetFlagStatus(DMA_Channel_REQ_UART1_RX, DMA_FLAG_CHNL_ENA) == RESET)
	{
		DMA_UART_RX_init(&UART2);
	}
	//���� ��������� ���������� ��� ���������� ������ ��������� UART3
	if(DMA_GetFlagStatus(DMA_Channel_SW12, DMA_FLAG_CHNL_ENA) == RESET)
	{
		DMA_UART_RX_init(&UART3);
	}
	//���� ��������� ���������� ��� ���������� ������ ��������� UART4
	if(DMA_GetFlagStatus(DMA_Channel_SW14, DMA_FLAG_CHNL_ENA) == RESET)
	{
		DMA_UART_RX_init(&UART4);
	}
	#endif
}
