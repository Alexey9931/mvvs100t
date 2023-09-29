#include "main.h"

extern SPIn SPI1;
extern SPIn SPI2;

extern UARTn UART1;
extern UARTn UART2;
#ifdef K1986VE3T
extern UARTn UART3;
extern UARTn UART4;
#endif

//указатель для обращения к внешнему ОЗУ
ram_data *ram_space_pointer;


int main(void)
{	
	CLOCK_Init();
	PortsInit();
	TIMER1_init();
	TIMER2_init();

	DMA_common_init();
	ebc_ports_config();
	ebc_config();
	init_external_ram_space();
	
	//Инициализация структур для UART1-2:
	UART1.UARTx = MDR_UART1;
	UART1.uart_dma_ch.dma_channel = DMA_Channel_REQ_UART1_RX;
	UART1.IRQn = UART1_IRQn;
	UART1.RST_CLK_PCLK_UARTn = RST_CLK_PCLK_UART1;
	UART1.UART.UART_BaudRate = 921600;
	UART1.UART.UART_WordLength = UART_WordLength8b;
	UART1.UART.UART_StopBits = UART_StopBits1;
	UART1.UART.UART_Parity = UART_Parity_No;
	UART1.UART.UART_FIFOMode = UART_FIFO_OFF;
	UART1.UART.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
	UART1.UART_HCLKdiv = UART_HCLKdiv1;
	UART1.buffer = (uint8_t*)(ram_space_pointer->uart1_rx_buffer);
	UART1.buffer_count = 0;
	UART1.read_pos = 0;

	UART2.UARTx = MDR_UART2;
	UART2.uart_dma_ch.dma_channel = DMA_Channel_REQ_UART2_RX;
	UART2.IRQn = UART2_IRQn;
	UART2.RST_CLK_PCLK_UARTn = RST_CLK_PCLK_UART2;
	UART2.UART.UART_BaudRate = 921600;
	UART2.UART.UART_WordLength = UART_WordLength8b;
	UART2.UART.UART_StopBits = UART_StopBits1;
	UART2.UART.UART_Parity = UART_Parity_No;
	UART2.UART.UART_FIFOMode = UART_FIFO_OFF;
	UART2.UART.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
	UART2.UART_HCLKdiv = UART_HCLKdiv1;
	UART2.buffer = (uint8_t*)(ram_space_pointer->uart2_rx_buffer);
	UART2.buffer_count = 0;
	UART2.read_pos = 0;

	uart_set_read_timeout(&UART1, 100);
	uart_set_read_timeout(&UART2, 100);

	uart_init(&UART1);
	DMA_UART_RX_init(&UART1);
	uart_init(&UART2);
	DMA_UART_RX_init(&UART2);
	
	//инициализация структур для работы с SPI
	SPI1.SSPx = MDR_SSP1;
	SPI1.RST_CLK_PCLK_SPIn = RST_CLK_PCLK_SSP1;
	SPI1.SSP_HCLKdiv = 1;
	SPI1.SPI.SSP_WordLength = SSP_WordLength16b;
	SPI1.SPI.SSP_Mode = SSP_ModeSlave;
	SPI1.SPI.SSP_SPH = SSP_SPH_2Edge;
	SPI1.SPI.SSP_FRF = SSP_FRF_SPI_Motorola;
	SPI1.SPI.SSP_CPSDVSR = 72;
	
	spi_init(&SPI1);


	while(1)
	{
		//запрос пакета по ШИНЕ1
		//request_data(&UART1);
		//запрос пакета по ШИНЕ2
		request_data(&UART2);
	}
}
/*
Функция для запроса данных
*/
uint8_t request_data(UARTn *UART_struct)
{
	uint8_t ext_bus; //определение шины, по которой идет обмен данными
	if(UART_struct->UARTx == MDR_UART1)
	{
		ext_bus = 1;
	}
	else if(UART_struct->UARTx == MDR_UART2)
	{
		ext_bus = 2;
	}
	
	if((receive_packet(UART_struct, ext_bus)) != NO_ERROR)
	{
		return 1;
	}
	/*
	выполнение команды периферией (например опрашиваем каналы АЦП/ЦАП)
	switch(cmd)
	{
	
	}
	*/			
	if(protocol_do_cmds(ext_bus) != 0)
	{
		return 1; 
	}
	if(transmit_packet(UART_struct, ext_bus) != NO_ERROR)
	{
		return 1;
	}
	
	return 0;
}
