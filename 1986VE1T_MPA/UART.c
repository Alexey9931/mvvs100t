#include "UART.h"

extern UARTn UART1;
extern UARTn UART2;
#ifdef K1986VE3T
	extern UARTn UART3;
	extern UARTn UART4;
#endif

//���������� ���������� UART1
void UART1_IRQHandler(void);
void UART1_IRQHandler(void)
{
	UART_ClearITPendingBit(UART1.UARTx, UART_IT_RX);
  while (UART_GetFlagStatus (UART1.UARTx, UART_FLAG_TXFE)!= SET){}
	UART1.buffer_count &= BUFFER_MASK;
	UART1.buffer_count++;
}
//���������� ���������� UART2
void UART2_IRQHandler(void);
void UART2_IRQHandler(void)
{
	UART_ClearITPendingBit(UART2.UARTx, UART_IT_RX);
  while (UART_GetFlagStatus (UART2.UARTx, UART_FLAG_TXFE)!= SET){}
	UART2.buffer_count &= BUFFER_MASK;
	UART2.buffer_count++;
}
#ifdef K1986VE3T
	//���������� ���������� UART3
	void UART3_IRQHandler(void);
	void UART3_IRQHandler(void)
	{
		UART_ClearITPendingBit(UART3.UARTx, UART_IT_RX);
		while (UART_GetFlagStatus (UART3.UARTx, UART_FLAG_TXFE)!= SET){}
		UART3.buffer_count &= BUFFER_MASK;
		UART3.buffer_count++;
	}
	//���������� ���������� UART4
	void UART4_IRQHandler(void);
	void UART4_IRQHandler(void)
	{
		UART_ClearITPendingBit(UART4.UARTx, UART_IT_RX);
		while (UART_GetFlagStatus (UART4.UARTx, UART_FLAG_TXFE)!= SET){}
		UART4.buffer_count &= BUFFER_MASK;
		UART4.buffer_count++;
	}
#endif
/*
������� ������������� UART
*/
uint8_t uart_init(UARTn *UART_struct)
{ 
	// ���������� ��������� ��� ������������� ����������� UART
	UART_InitTypeDef UART_InitStructure;
	uint8_t res = 0;
	
	// ��������� ������������ UART
	RST_CLK_PCLKcmd(UART_struct->RST_CLK_PCLK_UARTn, ENABLE);

	// �������� �������� ������� UART
	UART_BRGInit(UART_struct->UARTx, UART_struct->UART_HCLKdiv);

	
	// ������������ UART
	UART_InitStructure.UART_BaudRate = UART_struct->UART.UART_BaudRate;
	UART_InitStructure.UART_WordLength = UART_struct->UART.UART_WordLength;
	UART_InitStructure.UART_StopBits = UART_struct->UART.UART_StopBits;
	UART_InitStructure.UART_Parity = UART_struct->UART.UART_Parity;
	UART_InitStructure.UART_FIFOMode = UART_struct->UART.UART_FIFOMode;
	UART_InitStructure.UART_HardwareFlowControl = UART_struct->UART.UART_HardwareFlowControl;

	// ������������� UART � ��������� �����������
	res = UART_Init(UART_struct->UARTx, &UART_InitStructure);
	if (res != SUCCESS) 
	{
		return 1;
	}
	
	// �������� ������������������ UART
	UART_Cmd(UART_struct->UARTx, ENABLE);
	
	//UART_DMAConfig(UART_struct.UARTx, UART_IT_FIFO_LVL_2words, UART_IT_FIFO_LVL_2words);
	//UART_DMACmd(UART_struct.UARTx, DMA_Channel_REQ_UART1_RX, ENABLE);

	//��������� ���������� UART
	NVIC_SetPriority (UART_struct->IRQn, 1);
	UART_ITConfig( UART_struct->UARTx, UART_IT_RX, ENABLE );
	NVIC_EnableIRQ( UART_struct->IRQn );
	
	return 0;
}
/*
������� �������� ������ �� UART
*/
uint8_t uart_write(UARTn *UART_struct, uint8_t data[], uint32_t data_size)
{
	if (data_size > UINT32_MAX)
	{
		return 1;
	}
	for (int i = 0; i < data_size; i++)
	{
		UART_SendData(UART_struct->UARTx, data[i]);
		while (UART_GetFlagStatus(UART_struct->UARTx, UART_FLAG_TXFF) == SET);
	}
	return 0;
}
/*
������� ������ ������ �� ������� UART
*/
uint8_t uart_read(UARTn *UART_struct, uint32_t len, uint8_t *data)
{
	if (len > BUFFER_SIZE)
	{
		return 1;
	}
	//проверка на то, чтобы кол-во вновь прибывших байтов было не меньше считываемой длины
	if(abs(UART_struct->buffer_count - UART_struct->read_pos) < len)
	{
		return 1;
	}
	if ((UART_struct->read_pos+len) > BUFFER_SIZE)
	{
		//проверка на то, чтобы кол-во вновь прибывших байтов было не меньше считываемой длины
		if ((BUFFER_SIZE-UART_struct->read_pos+UART_struct->buffer_count) < len)
		{
			return 1;
		}
		memcpy(data, UART_struct->buffer + UART_struct->read_pos, BUFFER_SIZE-UART_struct->read_pos);
		memcpy(data + BUFFER_SIZE-UART_struct->read_pos, UART_struct->buffer, len-BUFFER_SIZE+UART_struct->read_pos);
		UART_struct->read_pos += len-BUFFER_SIZE;
	}
	else
	{
		memcpy(data, UART_struct->buffer + UART_struct->read_pos, len);
		UART_struct->read_pos += len;
	}
	
	return 0;
}
/*
������� ��������� ������� ������ � ������ UART
*/
uint8_t uart_set_pos(UARTn *UART_struct, uint32_t pos)
{
	if (pos > BUFFER_SIZE)
	{
		return 1;
	}
	UART_struct->read_pos = pos;
	return 0;
}
/*
������� ������ ������� ������� ������� ������
*/
uint32_t uart_read_pos(UARTn *UART_struct)
{
	return UART_struct->read_pos;
}
/*
������� ������ ���-�� ���� � ������ UART
*/
uint32_t uart_get_buf_counter(UARTn *UART_struct)
{
	return UART_struct->buffer_count;
}
/*
������� ������� ������ UART
*/
void uart_clean(UARTn *UART_struct)
{
	memset(UART_struct->buffer, 0, sizeof(UART_struct->buffer));
}
/*
������� ������������� n-�� ������ DMA  �� ������ �� ��������� UARTn
*/
void DMA_UART_RX_init(UARTn *UART_struct)
{
	//��������� � ����������� DMA � ����� 
	DMA_CtrlDataInitTypeDef DMA_InitStructure_UART_RX;
	//��������� � ����������� ������ DMA
	DMA_ChannelInitTypeDef DMA_Channel_UART_RX;
	
	DMA_StructInit(&DMA_Channel_UART_RX);
	DMA_InitStructure_UART_RX.DMA_SourceBaseAddr = (uint32_t)(&(UART_struct->UARTx->DR));
	DMA_InitStructure_UART_RX.DMA_DestBaseAddr = (uint32_t) &(UART_struct->buffer);
	DMA_InitStructure_UART_RX.DMA_CycleSize = BUFFER_SIZE;
	DMA_InitStructure_UART_RX.DMA_SourceIncSize = DMA_SourceIncNo;
	DMA_InitStructure_UART_RX.DMA_DestIncSize = DMA_DestIncByte;
	DMA_InitStructure_UART_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure_UART_RX.DMA_NumContinuous = DMA_Transfers_1;
	DMA_InitStructure_UART_RX.DMA_SourceProtCtrl = DMA_SourcePrivileged;
	DMA_InitStructure_UART_RX.DMA_DestProtCtrl = DMA_DestPrivileged;
	DMA_InitStructure_UART_RX.DMA_Mode = DMA_Mode_Basic;
	
	// ������ ��������� ������
	DMA_Channel_UART_RX.DMA_PriCtrlData = &DMA_InitStructure_UART_RX;
	DMA_Channel_UART_RX.DMA_Priority = DMA_Priority_High;
	DMA_Channel_UART_RX.DMA_UseBurst = DMA_BurstClear;
	DMA_Channel_UART_RX.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;
	
	// ���������������� �����
	DMA_Init(UART_struct->DMA_Channel, &DMA_Channel_UART_RX);
	
	MDR_DMA->CHNL_REQ_MASK_CLR = 1 << UART_struct->DMA_Channel;
	MDR_DMA->CHNL_USEBURST_CLR = 1 << UART_struct->DMA_Channel;
	
	UART_DMACmd(UART_struct->UARTx,UART_DMA_RXE, ENABLE);
	// ��������� ������ DMA � UART
	DMA_Cmd (UART_struct->DMA_Channel, ENABLE);
	
	NVIC_SetPriority (DMA_IRQn, 2);
	NVIC_EnableIRQ(DMA_IRQn);
}
