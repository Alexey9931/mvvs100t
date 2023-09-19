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
	init_external_ram_space();

	while(1)
	{
		//запрос пакета по ШИНЕ1
		request_data(&UART1);
		//запрос пакета по ШИНЕ2
		request_data(&UART2);
		
//		res = request_data(&UART1);
//		if (res != 1)
//		{
//			uint8_t a = 0;
//		}
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
	
	data_exchange_errors error;
	
	error = receive_packet(UART_struct, ext_bus);
	if(error != 0)
	{
		if (error == UART_ERROR)
		{
			switch(ext_bus)
			{
				case 1:
					if (ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B1 >= ram_space_pointer->ram_register_space.PLC_NumCrcErrorsForDefect_B1)
					{
						ram_space_pointer->service_byte_pm.fail_bus_1 = 1;	//запись в сервисный байт ПМ бита несиправности шины, если кол-во подряд поврежденных пакетов больше установленного
						ram_space_pointer->ram_register_space.PLC_BusDefect_B1.many_fail_packet = 1; //запись в регистр неисправности шины бита "кол-во битых пакетов больше установленного"
					}
					break;
				case 2:
					if (ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B2 >= ram_space_pointer->ram_register_space.PLC_NumCrcErrorsForDefect_B2)
					{
						ram_space_pointer->service_byte_pm.fail_bus_2 = 1;  	//запись в сервисный байт ПМ бита несиправности шины, если кол-во подряд поврежденных пакетов больше установленного
						ram_space_pointer->ram_register_space.PLC_BusDefect_B2.many_fail_packet = 1; //запись в регистр неисправности шины бита "кол-во битых пакетов больше установленного"
					}
					break;
				default:
					break;
			}			
		}
		return 1;
	}
	else
	{
		switch(ext_bus)
		{
			case 1:
				ram_space_pointer->service_byte_pm.fail_bus_1 = 0; //снятие в сервисном байте ПМ бита несиправности шины
				ram_space_pointer->ram_register_space.PLC_BusDefect_B1.many_fail_packet = 0; //снятие в регистре неисправности шины бита "кол-во битых пакетов больше установленного"
				ram_space_pointer->ram_register_space.PLC_BusDefect_B1.fail_timeout = 0; //снятие в регистре неисправности шины бита "неисправность по таймауту"
				break;
			case 2:
				//снятие в сервисном байте ПМ бита несиправности шины
				ram_space_pointer->service_byte_pm.fail_bus_2 = 0;	//снятие в сервисном байте ПМ бита несиправности шины
				ram_space_pointer->ram_register_space.PLC_BusDefect_B2.many_fail_packet = 0; //снятие в регистре неисправности шины бита "кол-во битых пакетов больше установленного"
				ram_space_pointer->ram_register_space.PLC_BusDefect_B2.fail_timeout = 0; //снятие в регистре неисправности шины бита "неисправность по таймауту"
				break;
			default:
				break;
		}			
	}
	/*
	выполнение команды периферией (например опрашиваем каналы АЦП/ЦАП)
	switch(cmd)
	{
	
	}
	*/			
	if(protocol_do_cmds(ext_bus) != 0) return 1; 
	if(transmit_packet(UART_struct, ext_bus) != 0) return 1;
	
	return 0;
}
