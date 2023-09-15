#include "main.h"

extern UARTn UART1;
extern UARTn UART2;
#ifdef K1986VE3T
extern UARTn UART3;
extern UARTn UART4;
#endif


ram_data *ram_space_pointer;


int main(void)
{	
	//Инициализация структур для UART1-2:
	UART1.UARTx = MDR_UART1;
	UART1.DMA_Channel = DMA_Channel_REQ_UART1_RX;
	UART1.IRQn = UART1_IRQn;
	UART1.RST_CLK_PCLK_UARTn = RST_CLK_PCLK_UART1;
	UART1.UART.UART_BaudRate = 115200;
	UART1.UART.UART_WordLength = UART_WordLength8b;
	UART1.UART.UART_StopBits = UART_StopBits1;
	UART1.UART.UART_Parity = UART_Parity_No;
	UART1.UART.UART_FIFOMode = UART_FIFO_OFF;
	UART1.UART.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
	UART1.UART_HCLKdiv = UART_HCLKdiv1;

	UART2.UARTx = MDR_UART2;
	UART2.DMA_Channel = DMA_Channel_REQ_UART2_RX;
	UART2.IRQn = UART2_IRQn;
	UART2.RST_CLK_PCLK_UARTn = RST_CLK_PCLK_UART2;
	UART2.UART.UART_BaudRate = 115200;
	UART2.UART.UART_WordLength = UART_WordLength8b;
	UART2.UART.UART_StopBits = UART_StopBits1;
	UART2.UART.UART_Parity = UART_Parity_No;
	UART2.UART.UART_FIFOMode = UART_FIFO_OFF;
	UART2.UART.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
	UART2.UART_HCLKdiv = UART_HCLKdiv1;

	uart_set_read_timeout(&UART1, 100);
	uart_set_write_timeout(&UART1, 100);
	
	uint8_t data[30];
	uint8_t res = 0;
	
	CLOCK_Init();
	PortsInit();
	TIMER1_init();
	TIMER2_init();
	DMA_common_init();
	uart_init(&UART1);
	DMA_UART_RX_init(&UART1);
	ebc_ports_config();
	ebc_config();
	init_external_ram_space(ram_space_pointer);
//	rx_packet = &ram_space_pointer->packet_struct;
//	memset(rx_packet,0,sizeof(fields_packet) + NUMBER_CMDS_IN_PACKET*sizeof(fields_cmd));
	while(1)
	{
		//res = uart_read(&UART1, 5, data);
		res = request_data(&UART1);
		if (res != 1)
		{
			uint8_t a = 0;
			//UART_SendData(UART1.UARTx, 0);
		   //while (UART_GetFlagStatus(UART1.UARTx, UART_FLAG_TXFF) == SET){}
		}
		
//		//res = request_data(&UART1);
//		if (res == 0)
//		{
//			uart_write(&UART1, &res, 1);
//		}

	}
}
/*
Функция для запроса данных
*/
uint8_t request_data(UARTn *UART_struct)
{
	if(receive_packet(UART_struct) != 0) return 1;
	/*
	выполнение команды периферией (например опрашиваем каналы АЦП/ЦАП)
	switch(cmd)
	{
	
	}
	*/
	if(protocol_do_cmds() != 0) return 1; 
	//if(Protocol_write(&received_packet, &sended_packet, UART_struct) != 0) return 1;
	
	return 0;
}
