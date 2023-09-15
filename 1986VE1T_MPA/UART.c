/*!
 \file
 \brief Файл с реализацией API для работы с UART
*/
#include "UART.h"

/*Глобальные экземпляры структур с конфигурационными параметрами UART и буфером приема 
 *(необходимо добавить аттрибут, который прописан в файле Objects/.sct,
 * т.к. для работы с DMA адрес буфера назначения должен лежать в определенной области памяти)
*/
UARTn UART1 IAR_SECTION ("EXECUTABLE_MEMORY_SECTION") __attribute__((section("EXECUTABLE_MEMORY_SECTION")));
UARTn UART2 IAR_SECTION ("EXECUTABLE_MEMORY_SECTION") __attribute__((section("EXECUTABLE_MEMORY_SECTION")));

#ifdef K1986VE3T
UARTn UART3;
UARTn UART4;
#endif

//обработчик прерываний UART1
void UART1_IRQHandler(void);
void UART1_IRQHandler(void)
{
		UART1.buffer_count &= BUFFER_MASK;
		while (UART_GetFlagStatus (UART1.UARTx, UART_FLAG_TXFE)!= SET){}
		UART1.buffer_count++;
		UART_ReceiveData(UART1.UARTx);
		UART_ClearITPendingBit(UART1.UARTx, UART_IT_RX);
}
//обработчик прерываний UART2
void UART2_IRQHandler(void);
void UART2_IRQHandler(void)
{
		UART2.buffer_count &= BUFFER_MASK;
		while (UART_GetFlagStatus (UART2.UARTx, UART_FLAG_TXFE)!= SET){}
		UART2.buffer_count++;
		UART_ReceiveData(UART2.UARTx);
		UART_ClearITPendingBit(UART2.UARTx, UART_IT_RX);
}
#ifdef K1986VE3T
//обработчик прерываний UART3
void UART3_IRQHandler(void);
void UART3_IRQHandler(void)
{
	UART3.buffer_count &= BUFFER_MASK;
	while (UART_GetFlagStatus (UART3.UARTx, UART_FLAG_TXFE)!= SET){}
	UART3.buffer_count++;
	UART_ReceiveData(UART3.UARTx);
	UART_ClearITPendingBit(UART3.UARTx, UART_IT_RX);
}
//обработчик прерываний UART4
void UART4_IRQHandler(void);
void UART4_IRQHandler(void)
{
	UART4.buffer_count &= BUFFER_MASK;
	while (UART_GetFlagStatus (UART4.UARTx, UART_FLAG_TXFE)!= SET){}
	UART4.buffer_count++;
	UART_ReceiveData(UART4.UARTx);
	UART_ClearITPendingBit(UART4.UARTx, UART_IT_RX);
}
#endif
/*
Функция инициализация UART
*/
uint8_t uart_init(UARTn *UART_struct)
{ 
	// Объявление структуры для инициализации контроллера UART
	UART_InitTypeDef UART_InitStructure;
	uint8_t res = 0;
	
	// Включение тактирования UART
	RST_CLK_PCLKcmd(UART_struct->RST_CLK_PCLK_UARTn, ENABLE);

	// Делитель тактовой частоты UART
	UART_BRGInit(UART_struct->UARTx, UART_struct->UART_HCLKdiv);

	NVIC_EnableIRQ( UART_struct->IRQn );
	
	// Конфигурация UART
	UART_InitStructure.UART_BaudRate = UART_struct->UART.UART_BaudRate;
	UART_InitStructure.UART_WordLength = UART_struct->UART.UART_WordLength;
	UART_InitStructure.UART_StopBits = UART_struct->UART.UART_StopBits;
	UART_InitStructure.UART_Parity = UART_struct->UART.UART_Parity;
	UART_InitStructure.UART_FIFOMode = UART_struct->UART.UART_FIFOMode;
	UART_InitStructure.UART_HardwareFlowControl = UART_struct->UART.UART_HardwareFlowControl;

	// Инициализация UART с заданными параметрами
	res = UART_Init(UART_struct->UARTx, &UART_InitStructure);
	if (res != SUCCESS) 
	{
		return 1;
	}

	//Включение прерываний UART
	//NVIC_SetPriority (UART_struct->IRQn, 1);
	UART_ITConfig( UART_struct->UARTx, UART_IT_RX, ENABLE );
	
	// Включить сконфигурированный UART
	UART_Cmd(UART_struct->UARTx, ENABLE);
	
	return 0;
}
/*
Функция передачи данных по UART
*/
uint8_t uart_write(UARTn *UART_struct, uint8_t *data, uint32_t data_size)
{
	if (UART_struct->UARTx_timeouts->write_timeout_flag == 1)
	{
		MDR_TIMER1->CNT = 0;
	}
	if (data_size > UINT32_MAX)
	{
		return 1;
	}
	for (int i = 0; i < data_size; i++)
	{
		UART_SendData(UART_struct->UARTx, data[i]);
		while (UART_GetFlagStatus(UART_struct->UARTx, UART_FLAG_TXFF) == SET)
		{
			if (UART_struct->UARTx_timeouts->write_timeout_flag == 1)
			{
				if (TIMER_GetCounter(MDR_TIMER1)==(UART_struct->UARTx_timeouts->write_val_timeout*50)) return 1;
			}
		}
	}
	return 0;
}
/*
Функция чтения данных из буффера UART
*/
uint8_t uart_read(UARTn *UART_struct, uint32_t len, uint8_t *data)
{
	//если длина превышает размер буфера
	if (len > BUFFER_SIZE)
	{
		return 1;
	}
	//если последний принятый байт перевалит границу буфера и байты будут перезаписываться в буфере с самого начала
	if (((UART_struct->read_pos)+len) > BUFFER_SIZE)
	{
		//если задан таймаут 
		if (UART_struct->UARTx_timeouts->read_timeout_flag == 1)
		{
			MDR_TIMER1->CNT = 0;
			while ((int)((UART_struct->buffer_count) - (UART_struct->read_pos)) >= 0)
			{
				if (TIMER_GetCounter(MDR_TIMER1)==(UART_struct->UARTx_timeouts->write_val_timeout*50)) return 1;
			}
			while ((BUFFER_SIZE - (UART_struct->read_pos) + (UART_struct->buffer_count)) < len)
			{
				if (TIMER_GetCounter(MDR_TIMER1)==(UART_struct->UARTx_timeouts->write_val_timeout*50)) return 1;
			}
		}
		
		if ((int)((UART_struct->buffer_count) - (UART_struct->read_pos)) >= 0)
		{
			return 1;
		}
		else
		{
			if ((BUFFER_SIZE + (UART_struct->buffer_count) - (UART_struct->read_pos)) < len) return 1;
			memcpy(data, (UART_struct->buffer) + (UART_struct->read_pos), BUFFER_SIZE-(UART_struct->read_pos));
			memcpy(data + BUFFER_SIZE-(UART_struct->read_pos), UART_struct->buffer, len+(UART_struct->read_pos)-BUFFER_SIZE);
			UART_struct->read_pos = (UART_struct->read_pos) + len-BUFFER_SIZE;
		}
	}
	//если последний принятый байт не перевалит границу буфера
	else
	{
		//если задан таймаут 
		if (UART_struct->UARTx_timeouts->read_timeout_flag == 1)
		{
			MDR_TIMER1->CNT = 0;
			while (((UART_struct->buffer_count) - (UART_struct->read_pos)) < len)
			{
				if (TIMER_GetCounter(MDR_TIMER1)==(UART_struct->UARTx_timeouts->write_val_timeout*50)) return 1;
			}
		}
		
		if (((UART_struct->buffer_count) - (UART_struct->read_pos)) < len) return 1;
		memcpy(data, (UART_struct->buffer) + (UART_struct->read_pos), len);
		UART_struct->read_pos = (UART_struct->read_pos) + len;
	}
	
	return 0;
}
/*
Функция установки курсора чтения в буфере UART
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
Функция чтения текущей позиции курсора чтения
*/
uint32_t uart_read_pos(UARTn *UART_struct)
{
	return UART_struct->read_pos;
}
/*
Функция чтения кол-во байт в буфере UART
*/
uint32_t uart_get_buf_counter(UARTn *UART_struct)
{
	return UART_struct->buffer_count;
}
/*
Функция очистки буфера UART
*/
void uart_clean(UARTn *UART_struct)
{
	memset(UART_struct->buffer, 0, sizeof(UART_struct->buffer));
}
/*
Функция инициализации n-го канала DMA  на запрос от приемника UARTn
*/
void DMA_UART_RX_init(UARTn *UART_struct)
{
	//Структура с настройками DMA в целом 
	DMA_CtrlDataInitTypeDef DMA_InitStructure_UART_RX;
	//Структура с настройками канала DMA
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
	
	// Задать структуру канала
	DMA_Channel_UART_RX.DMA_PriCtrlData = &DMA_InitStructure_UART_RX;
	DMA_Channel_UART_RX.DMA_Priority = DMA_Priority_High;
	DMA_Channel_UART_RX.DMA_UseBurst = DMA_BurstClear;
	DMA_Channel_UART_RX.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;
	
	// Инициализировать канал
	DMA_Init(UART_struct->DMA_Channel, &DMA_Channel_UART_RX);
	
	MDR_DMA->CHNL_REQ_MASK_CLR = 1 << UART_struct->DMA_Channel;
	MDR_DMA->CHNL_USEBURST_CLR = 1 << UART_struct->DMA_Channel;
	
	UART_DMACmd(UART_struct->UARTx,UART_DMA_RXE, ENABLE);
	// Разрешить работу DMA с UART
	DMA_Cmd (UART_struct->DMA_Channel, ENABLE);
	
	//NVIC_SetPriority (DMA_IRQn, 2);
	NVIC_EnableIRQ(DMA_IRQn);
}
/*
Функция установки таймаута UARTn на чтение
*/
void uart_set_read_timeout(UARTn *UART_struct, uint32_t read_timeout)
{
	UART_struct->UARTx_timeouts->read_timeout_flag = 1;
	UART_struct->UARTx_timeouts->read_val_timeout = read_timeout;
}
/*
Функция установки таймаута UARTn на запись
*/
void uart_set_write_timeout(UARTn *UART_struct, uint32_t write_timeout)
{
	UART_struct->UARTx_timeouts->write_timeout_flag = 1;
	UART_struct->UARTx_timeouts->write_val_timeout = write_timeout;
}