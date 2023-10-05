#include "main.h"

extern adc_n adc_1;

extern timer_n timer_1;
extern timer_n timer_2;
extern timer_n timer_3;

extern spi_n spi_1;
extern spi_n spi_2;

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
	
	DMA_common_init();
	ebc_init();
	init_external_ram_space();
	
	//инициализация SPI1
	spi_1.SSPx = MDR_SSP1;
	spi_1.RST_CLK_PCLK_SPIn = RST_CLK_PCLK_SSP1;
	spi_1.SSP_HCLKdiv = SSP_HCLKdiv1;
	spi_1.SPI.SSP_WordLength = SSP_WordLength16b;
	spi_1.SPI.SSP_Mode = SSP_ModeSlave;
	spi_1.SPI.SSP_SPH = SSP_SPH_2Edge;
	spi_1.SPI.SSP_FRF = SSP_FRF_SPI_Motorola;
	spi_1.SPI.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;
	spi_1.SPI.SSP_CPSDVSR = 64;
	
	spi_init(&spi_1);
	
	//инициализация Timer1
	timer_1.RST_CLK_PCLK_TIMERn = RST_CLK_PCLK_TIMER1;
	timer_1.TIMERInitStruct.TIMER_Period = 0x270F;//10000-1
	timer_1.TIMERInitStruct.TIMER_Prescaler = WORK_FREQ - 1;//128-1
	timer_1.TIMERx = MDR_TIMER1;
	timer_1.TIMER_HCLKdiv = TIMER_HCLKdiv1;
	
	timer_init(&timer_1);
	
	//инициализация Timer3
	timer_3.RST_CLK_PCLK_TIMERn = RST_CLK_PCLK_TIMER3;
	timer_3.TIMERInitStruct.TIMER_Period = 0xC34F;//50000-1
	timer_3.TIMERInitStruct.TIMER_Prescaler = WORK_FREQ*20 - 1;//2560-1
	timer_3.TIMERx = MDR_TIMER3;
	timer_3.TIMER_HCLKdiv = TIMER_HCLKdiv1;
	
	timer_init(&timer_3);
	
	//инициализация Timer2
	timer_2.RST_CLK_PCLK_TIMERn = RST_CLK_PCLK_TIMER2;
	timer_2.TIMERInitStruct.TIMER_Period = 0x00;
	timer_2.TIMERInitStruct.TIMER_Prescaler = 0x00;
	timer_2.TIMERInitStruct.TIMER_CounterDirection = TIMER_CntDir_Up;
	timer_2.TIMERInitStruct.TIMER_CounterMode = TIMER_CntMode_EvtFixedDir;
	timer_2.TIMERInitStruct.TIMER_EventSource = TIMER_EvSrc_CH2;
	timer_2.TIMERInitStruct.TIMER_ARR_UpdateMode = TIMER_ARR_Update_Immediately;
	timer_2.TIMERInitStruct.TIMER_FilterSampling = TIMER_FDTS_TIMER_CLK_div_1;
	timer_2.TIMERInitStruct.TIMER_ETR_FilterConf = TIMER_Filter_8FF_at_FTDS_div_32;
	timer_2.TIMERInitStruct.TIMER_ETR_Prescaler = TIMER_ETR_Prescaler_None;
	timer_2.TIMERInitStruct.TIMER_ETR_Polarity = TIMER_ETRPolarity_NonInverted;
	timer_2.TIMERInitStruct.TIMER_BRK_Polarity = TIMER_BRKPolarity_NonInverted;
	timer_2.TIMERx = MDR_TIMER2;
	timer_2.TIMER_HCLKdiv = TIMER_HCLKdiv1;
	timer_2.sTIM_ChnInit.TIMER_CH_Number = TIMER_CHANNEL2;
	timer_2.sTIM_ChnInit.TIMER_CH_Mode = TIMER_CH_MODE_CAPTURE;
	timer_2.sTIM_ChnInit.TIMER_CH_EventSource = TIMER_CH_EvSrc_PE;
	timer_2.IRQn = TIMER2_IRQn;
	timer_2.TIMER_STATUS = TIMER_STATUS_CCR_CAP1_CH2;
	
	timer_init(&timer_2);
	
	//инициализация АЦП1
	adc_1.spi_struct = &spi_1;
	adc_1.timer_n_timeout = &timer_1;
	adc_1.timer_n_capture = &timer_2;
	
	//adc_set_read_timeout(&adc_1, 200);
	adc_init(&adc_1);

	//Инициализация UART1-2:
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
	UART1.UARTx_timeouts.timer_n_timeout = &timer_3;

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
	UART2.UARTx_timeouts.timer_n_timeout = &timer_3;

	uart_set_read_timeout(&UART1, 100);
	uart_set_read_timeout(&UART2, 100);

	uart_init(&UART1);
	DMA_UART_RX_init(&UART1);
	uart_init(&UART2);
	DMA_UART_RX_init(&UART2);
	

	while(1)
	{
		do_mpa_task(&adc_1);
		//SSP_SendData(MDR_SSP1, 0x7FFF);
		//запрос пакета по ШИНЕ1
		//request_data(&UART1);
		//запрос пакета по ШИНЕ2
		//request_data(&UART2);
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
	
	//выполнение команды периферией (опрос каналов АЦП)
	do_mpa_task(&adc_1);
			
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
/*
Функция для выполнения периферийной задачи МПА
*/
void do_mpa_task(adc_n *adc_struct)
{
	//delay_micro(125);
	//TODO: пока что читает каналы МПА только для напряжений 0-10В (для тока в карту регистров надо добавлять свои полиномы)
	
	//указатель на пространство регистров МПА
	mpa_ram_registers *ptr = &ram_space_pointer->mpa_ram_register_space;
	
//для двуполярного вх напр (-5.4 ... 5.4 В ) на мультиплексоре A0=A1=0
//		U = 1.6474f*pow(10,-4)*adc_code;
//		delta = 6.5627f*pow(10,-6)*adc_code + 0.00039f;
//для однополярного вх напр (0 ... 10.8 В ) на мультиплексоре A0=1;A1=0	
//		U = 1.6474f*pow(10,-4)*adc_code + 5.398f;
//		delta = 6.6962f*pow(10,-6)*adc_code + 0.4252307f;
//для однополярного вх тока (0 ... 21.6 мА ) на мультиплексоре A0=1;A1=0
//		I = 3.052f*pow(10,-4)*adc_code + 10.0f;
//		delta = -11.9006f*pow(10,-6)*adc_code + 0.03072506f;
//самодиагностика для двуполярного случая	на мультиплексоре A0=1;A1=1 (на выходе должно быть 0В)
//		U = 1.6474f*pow(10,-4)*adc_code;
//		delta = 6.5627f*pow(10,-6)*adc_code + 0.00039f;			
//самодиагностика для однополярного случая	на мультиплексоре A0=0;A1=1 (на выходе должно быть 0В)
//		U = 1.6474f*pow(10,-4)*adc_code + 5.398f;
//		delta = 6.6962f*pow(10,-6)*adc_code + 0.4252307f;
	for (uint8_t i = 0; i < CHANEL_NUMBER; i++)
	{
		//инициируем чтение канала АЦП записью в буфер передатчика SPI 0x7FFF
		ptr->AI_CodeADC[i] = adc_read(adc_struct);
		switch ( TEST_BIT(i, ptr->AI_OperMode.adc_chs_mode))
		{
			case 0:
					//напряжение 0-10В
					ptr->AI_PhysQuantFloat[i] = ptr->AI_PolynConst0[i] + (ptr->AI_PolynConst1[i])*(ptr->AI_CodeADC[i]);
					break;
			
			case 1:
					//ток 4-20мА
			
					break;
			
			default:
					break;
		}
	}
}
