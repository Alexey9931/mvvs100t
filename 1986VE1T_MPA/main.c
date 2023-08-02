#include "main.h"
/*
		Инициализация структур для UART1-2:
			Выбор UART1-2 для работы
			Выбор каналов DMA для UART1-2
			Включение тактирования для UART1-2
			
			Скорость передачи данных UART_BaudRate = 115200 бод
			Количество бит в посылке = 8
			Один стоп-бит
			Без проверки четности
			Выключена работа буфера FIFO приемника и передатчика, т.е. передача осуществляется по одному байту
			Разрешены прием и передача данных
		
			Делитель тактовой частоты для UART = 1
	*/
UARTn UART1 IAR_SECTION ("EXECUTABLE_MEMORY_SECTION") __attribute__((section("EXECUTABLE_MEMORY_SECTION"))) = {MDR_UART1, DMA_Channel_REQ_UART1_RX, UART1_IRQn, RST_CLK_PCLK_UART1,{115200, UART_WordLength8b, UART_StopBits1, UART_Parity_No, UART_FIFO_OFF, UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE}, UART_HCLKdiv1,{0}, 0 ,0 };
UARTn UART2 IAR_SECTION ("EXECUTABLE_MEMORY_SECTION") __attribute__((section("EXECUTABLE_MEMORY_SECTION"))) = {MDR_UART2, DMA_Channel_REQ_UART2_RX, UART2_IRQn, RST_CLK_PCLK_UART2,{921600, UART_WordLength8b, UART_StopBits1, UART_Parity_No, UART_FIFO_OFF, UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE}, UART_HCLKdiv1,{0}, 0 ,0 };


int main(void)
{	
	uint8_t data[30];
	uint8_t res = 0;
	
	CLOCK_Init();
	PortsInit();
	TIMER1_init();
	TIMER2_init();
	DMA_common_init();
	uart_init(&UART1);
	DMA_UART_RX_init(&UART1);
	

	while(1)
	{
//		res = uart_read(&UART1, 5, data);
//		if (res != 1)
//		{
//			uart_write(&UART1, &res, 1);
//		}
		res = PROTOCOL_REQUEST_DATA(&UART1);
		if (res == 0)
		{
			uart_write(&UART1, &res, 1);
		}

	}
}
