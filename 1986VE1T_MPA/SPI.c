/*!
 \file
 \brief ���� � ����������� API ��� ������ � SPI
*/
#include "SPI.h"

/*���������� ���������� �������� � ����������������� ����������� SPI � ������� ������ 
 *(���������� �������� ��������, ������� �������� � ����� Objects/.sct,
 * �.�. ��� ������ � DMA ����� ������ ���������� ������ ������ � ������������ ������� ������)
 */
spi_n spi_1 IAR_SECTION ("EXECUTABLE_MEMORY_SECTION") __attribute__((section("EXECUTABLE_MEMORY_SECTION")));
spi_n spi_2 IAR_SECTION ("EXECUTABLE_MEMORY_SECTION") __attribute__((section("EXECUTABLE_MEMORY_SECTION")));

/*
������� ���������������� ������� �� ��� SPI
*/
void spi_gpio_config(void);
void spi_gpio_config(void)
{
	// ��������� ������������ ������
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK|RST_CLK_PCLK_PORTD, ENABLE);	
	
	// ������������� ������ SSP1
	PORT_InitTypeDef GPIO_init_struct_SPI1;
	
	PORT_StructInit(&GPIO_init_struct_SPI1);
	GPIO_init_struct_SPI1.PORT_FUNC = PORT_FUNC_ALTER;
	GPIO_init_struct_SPI1.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_init_struct_SPI1.PORT_SPEED = PORT_SPEED_MAXFAST;
	//������������� ������ SCK
	GPIO_init_struct_SPI1.PORT_Pin = PIN_SSP1_SCK;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_IN;
	PORT_Init(PORT_SSP1, &GPIO_init_struct_SPI1);
	//������������� ������ SSP_RX
	GPIO_init_struct_SPI1.PORT_Pin = PIN_SSP1_RX;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_IN;
	PORT_Init(PORT_SSP1, &GPIO_init_struct_SPI1);
	//������������� ������ SSP_TX
	GPIO_init_struct_SPI1.PORT_Pin = PIN_SSP1_TX;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_SSP1, &GPIO_init_struct_SPI1);
	PORT_ResetBits(PORT_SSP1,PIN_SSP1_TX);
	//������������� ������ SS (���� SDIFS)
	GPIO_init_struct_SPI1.PORT_Pin = PIN_SSP1_SS;
	GPIO_init_struct_SPI1.PORT_FUNC = PORT_FUNC_ALTER;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_IN;
	PORT_Init(PORT_SSP1, &GPIO_init_struct_SPI1);
	
}
/*
������� ������������� SPI
*/
void spi_init(spi_n *spi_struct)
{
	spi_gpio_config();
	
	//��������� ��� ������������� SPI
	SSP_InitTypeDef SSP_InitStruct;
	
	// ��������� ������������ SPI
	RST_CLK_PCLKcmd(spi_struct->RST_CLK_PCLK_SPIn, ENABLE);

	SSP_StructInit(&SSP_InitStruct);
	
	SSP_BRGInit(spi_struct->SSPx, spi_struct->SSP_HCLKdiv);
	
	SSP_InitStruct.SSP_WordLength = spi_struct->SPI.SSP_WordLength;
	SSP_InitStruct.SSP_Mode = spi_struct->SPI.SSP_Mode;
	SSP_InitStruct.SSP_SPH = spi_struct->SPI.SSP_SPH;
  SSP_InitStruct.SSP_FRF = spi_struct->SPI.SSP_FRF;
  SSP_InitStruct.SSP_HardwareFlowControl = spi_struct->SPI.SSP_HardwareFlowControl;
	//SSP_InitStruct.SSP_SCR  = 0x10;
  SSP_InitStruct.SSP_CPSDVSR = spi_struct->SPI.SSP_CPSDVSR;//������� ������ 2��� 
	SSP_Init(spi_struct->SSPx,&SSP_InitStruct);
	
	// ����� ���������� ���������� (����� � �������� ������)
  SSP_ITConfig (spi_struct->SSPx, SSP_IT_RX | SSP_IT_TX, ENABLE);
	
	//���������� �� �������� SPI, �.�. �� ��������� � SLAVE-������
	SSP_Cmd(spi_struct->SSPx, ENABLE);
	
}
/*
������� �������� ��������� �� SPI
*/
void spi_transmit_halfword(spi_n *spi_struct, uint16_t half_word)
{
	SSP_SendData(spi_struct->SSPx, half_word);
	while (	SSP_GetFlagStatus(spi_struct->SSPx, SSP_FLAG_TFE) != SET);
}
/*
������� �������� ������� �� SPI
*/
void spi_transmit_message(spi_n *spi_struct, uint16_t message[], uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		while(SSP_GetFlagStatus(spi_struct->SSPx, SSP_FLAG_TNF) != SET) {} // ���� ����� � ������ �������� ����� � ����� ���������� ��������� ����
		SSP_SendData(spi_struct->SSPx, message[i]);			
	}	
	while (SSP_GetFlagStatus(spi_struct->SSPx, SSP_FLAG_TFE) != SET) {}  //���� ���� ���� �����		
}
/*
������� ������ ��������� �� SPI
*/
uint16_t spi_receive_halfword(spi_n *spi_struct)
{	
	uint16_t tmpVar;
	// ��������� ���������� �� ��������� ������
	while (SSP_GetFlagStatus(spi_struct->SSPx, SSP_FLAG_RNE) != SET)  {}  //���� ���� �������� ����  
	// �������� ������
	tmpVar = SSP_ReceiveData(spi_struct->SSPx);
		
	return tmpVar;
}
/*
������� ������� FIFO ������ ��������� SPI
*/
void spi_clean_fifo_rx_buf(spi_n *spi_struct)
{	
	uint16_t a;
	for (uint8_t i = 0; i < FIFO_SIZE; i++)
	{
		a = spi_struct->SSPx->DR;
	}
}

/*
������� ������������� n-�� ������ DMA  �� ������ �� ��������� SPIn
*/
void dma_spi_rx_init(spi_n *spi_struct)
{

	DMA_StructInit(&spi_struct->spi_dma_ch.DMA_Channel_SPI_RX);
	spi_struct->spi_dma_ch.DMA_InitStructure_SPI_RX.DMA_SourceBaseAddr = (uint32_t)(&(spi_struct->SSPx->DR));
	spi_struct->spi_dma_ch.DMA_InitStructure_SPI_RX.DMA_DestBaseAddr = (uint32_t)(spi_struct->buffer);
	spi_struct->spi_dma_ch.DMA_InitStructure_SPI_RX.DMA_CycleSize = 1;
	spi_struct->spi_dma_ch.DMA_InitStructure_SPI_RX.DMA_SourceIncSize = DMA_SourceIncNo;
	spi_struct->spi_dma_ch.DMA_InitStructure_SPI_RX.DMA_DestIncSize = DMA_DestIncHalfword;
	spi_struct->spi_dma_ch.DMA_InitStructure_SPI_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	spi_struct->spi_dma_ch.DMA_InitStructure_SPI_RX.DMA_NumContinuous = DMA_Transfers_1;
	spi_struct->spi_dma_ch.DMA_InitStructure_SPI_RX.DMA_SourceProtCtrl = DMA_SourcePrivileged;
	spi_struct->spi_dma_ch.DMA_InitStructure_SPI_RX.DMA_DestProtCtrl = DMA_DestPrivileged;
	spi_struct->spi_dma_ch.DMA_InitStructure_SPI_RX.DMA_Mode = DMA_Mode_Basic;
	
	// ������ ��������� ������
	spi_struct->spi_dma_ch.DMA_Channel_SPI_RX.DMA_PriCtrlData = &spi_struct->spi_dma_ch.DMA_InitStructure_SPI_RX;
	spi_struct->spi_dma_ch.DMA_Channel_SPI_RX.DMA_Priority = DMA_Priority_High;
	spi_struct->spi_dma_ch.DMA_Channel_SPI_RX.DMA_UseBurst = DMA_BurstClear;
	spi_struct->spi_dma_ch.DMA_Channel_SPI_RX.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;
	
	// ���������������� �����
	DMA_Init(spi_struct->spi_dma_ch.dma_channel, &spi_struct->spi_dma_ch.DMA_Channel_SPI_RX);
	
	MDR_DMA->CHNL_REQ_MASK_CLR = 1 << spi_struct->spi_dma_ch.dma_channel;
	MDR_DMA->CHNL_USEBURST_CLR = 1 << spi_struct->spi_dma_ch.dma_channel;
	
	SSP_DMACmd(spi_struct->SSPx, SSP_DMA_RXE, DISABLE);
	// ��������� ������ DMA � SPI
	DMA_Cmd (spi_struct->spi_dma_ch.dma_channel, DISABLE);
	
	//NVIC_SetPriority (DMA_IRQn, 2);
	NVIC_EnableIRQ(DMA_IRQn);
}

