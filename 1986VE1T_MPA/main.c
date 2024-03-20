#include "main.h"

/// Структура с конфигурационными параметрами микросхемы АЦП
extern adc_n adc_1;

/// Структуры с конфигурационными параметрами блоков таймеров МК
extern timer_n timer_1;
extern timer_n timer_2;
extern timer_n timer_3;

/// Массив указателей на head списков обработчиков прерываний таймеров
extern list_head *tmr_handler_head[TIMER_NUM];

/// Структуры с конфигурационными параметрами блоков SPI МК
extern spi_n spi_1;
extern spi_n spi_2;

/// Структуры с конфигурационными параметрами блоков UART МК
extern uart_n uart_1;
extern uart_n uart_2;
#ifdef K1986VE3T
extern uart_n uart_3;
extern uart_n uart_4;
#endif

/// Указатель для обращения к внешнему ОЗУ
ram_data *ram_space_pointer;
/// Указатель для обращения к внешнему ПЗУ
rom_data *rom_space_pointer;

/// Выделение памяти во внутреннем ОЗУ МК для "самодельной" кучи
int_ram_heap heap;
/// Указатель на "самодельную" кучу
int_ram_heap *heap_ptr = &heap;

int main(void)
{	
	// Инициализация тактирования МК
	clock_init();
	// Общая инициализация блока DMA МК
	dma_common_init();
	// Прошивка внешней микросхемы ПЗУ (делается только 1 раз)
	#ifdef ROM_IS_USED
		ebc_init(EBC_ROM);
		init_external_rom_space();
	#endif
	// Инициализация внешней микросхемы ОЗУ
	ebc_init(EBC_RAM);
	init_external_ram_space(); 
	// Инициализация светодиодных индикаторов
	leds_gpio_config();
		
	// Инициализация и создание списков обработчиков прерываний таймеров
	list_tmr_handler_init(1);
	list_tmr_handler_add_tail(1, sync_adc_chanels, NULL, TIMER_STATUS_CNT_ARR);
	list_tmr_handler_add_tail(1, receive_adc_chanel_pack, NULL, TIMER_STATUS_CCR_CAP1_CH4);
	
	// Инициализация блока SSI1 МК
	spi_1.SSPx = MDR_SSP1;
	spi_1.RST_CLK_PCLK_SPIn = RST_CLK_PCLK_SSP1;
	spi_1.SPI.SSP_WordLength = SSP_WordLength16b;
	spi_1.SPI.SSP_Mode = SSP_ModeSlave;
	spi_1.SPI.SSP_SPH = SSP_SPH_2Edge;
	spi_1.SPI.SSP_FRF = SSP_FRF_SSI_TI;
	spi_1.SPI.SSP_HardwareFlowControl = SSP_HardwareFlowControl_SSE;
	spi_1.SPI.SSP_CPSDVSR = WORK_FREQ/2;
	spi_1.IRQn = SSP1_IRQn;
	spi_1.spi_dma_ch.dma_channel = DMA_Channel_REQ_SSP1_RX;
	spi_1.buffer = ram_space_pointer->spi_1_rx_buffer;
	
	spi_init(&spi_1);
	
	// Инициализация блока Timer1 МК (настроен на период 10мс)
	timer_1.TIMERInitStruct.TIMER_Period = 0x270F;//10000-1
	timer_1.TIMERInitStruct.TIMER_Prescaler = WORK_FREQ - 1;//128-1
	timer_1.TIMERx = MDR_TIMER1;
	
	timer_init(&timer_1);
	
	// Инициализация блока Timer3 МК (настроен на период 1сек)
	timer_3.TIMERInitStruct.TIMER_Period = 0xC34F;//50000-1
	timer_3.TIMERInitStruct.TIMER_Prescaler = WORK_FREQ*20 - 1;//2560-1
	timer_3.TIMERx = MDR_TIMER3;
	
	timer_init(&timer_3);
	
	// Инициализация блока Timer2 МК (настроен для режима захвата по 2 каналу таймера - для нужд АЦП, также настроен на период 10мкс)
	timer_2.TIMERInitStruct.TIMER_Period = 10 + 1;
	timer_2.TIMERInitStruct.TIMER_Prescaler = WORK_FREQ + 1;
	timer_2.sTIM_ChnInit.TIMER_CH_Number = TIMER_CHANNEL4;
	timer_2.sTIM_ChnInit.TIMER_CH_Mode = TIMER_CH_MODE_CAPTURE;
	timer_2.sTIM_ChnInit.TIMER_CH_EventSource = TIMER_CH_EvSrc_PE;
	timer_2.TIMER_STATUS = TIMER_STATUS_CCR_CAP1_CH4 | TIMER_STATUS_CNT_ARR;
	timer_2.TIMERx = MDR_TIMER2;
	
	timer_init(&timer_2);
	
	// Инициализация 6-ти канальной микросхемы АЦП1
	adc_1.spi_struct = &spi_1;
	adc_1.spi_struct->buffer_counter = 0;
	adc_1.timer_n_capture = &timer_2;
	adc_1.avg_num = find_max_halfword(ram_space_pointer->mpa_ram_register_space.AI_RomRegs.AI_NumForAverag, CHANEL_NUMBER);
	adc_1.ch_rx_num = 0;
	adc_1.init_flag = 0;
	
	adc_init(&adc_1);

	// Инициализация блоков UART1-2 МК:
	uart_1.UARTx = MDR_UART1;
	uart_1.uart_dma_ch.dma_channel = DMA_Channel_REQ_UART1_RX;
	uart_1.IRQn = UART1_IRQn;
	uart_1.UART.UART_BaudRate = 115200;
	uart_1.UART.UART_WordLength = UART_WordLength8b;
	uart_1.UART.UART_StopBits = UART_StopBits1;
	uart_1.UART.UART_Parity = UART_Parity_No;
	uart_1.UART.UART_FIFOMode = UART_FIFO_OFF;
	uart_1.UART.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
	uart_1.buffer = (uint8_t*)(ram_space_pointer->uart1_rx_buffer);
	uart_1.buffer_count = 0;
	uart_1.read_pos = 0;
	uart_1.uart_timeouts.timer_n_timeout = &timer_3;

	uart_2.UARTx = MDR_UART2;
	uart_2.uart_dma_ch.dma_channel = DMA_Channel_REQ_UART2_RX;
	uart_2.IRQn = UART2_IRQn;
	uart_2.UART.UART_BaudRate = 921600;
	uart_2.UART.UART_WordLength = UART_WordLength8b;
	uart_2.UART.UART_StopBits = UART_StopBits1;
	uart_2.UART.UART_Parity = UART_Parity_No;
	uart_2.UART.UART_FIFOMode = UART_FIFO_OFF;
	uart_2.UART.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;
	uart_2.buffer = (uint8_t*)(ram_space_pointer->uart2_rx_buffer);
	uart_2.buffer_count = 0;
	uart_2.read_pos = 0;
	uart_2.uart_timeouts.timer_n_timeout = &timer_3;

	// Установка таймаутов на ШИНАХ1-2
	//uart_set_read_timeout(&uart_1, 300);
	uart_set_read_timeout(&uart_2, 300);
	
	// Инициализация ШИНЫ1
	//uart_init(&uart_1);
	// Инициализация DMA для UART1
	//DMA_UART_RX_init(&UART1);
	
	// Инициализация ШИНЫ2
	uart_init(&uart_2);
	// Инициализация DMA для UART2
	DMA_UART_RX_init(&uart_2);

	while(1)
	{		
		//запрос пакета по ШИНЕ1
		//request_data(&uart_1);
		//запрос пакета по ШИНЕ2
		request_data(&uart_2);
	}
}
/*
	Функция для запроса данных
*/
uint8_t request_data(uart_n *uart_struct)
{
	// Номер шины, по которой запрашиваются данные
	uint8_t ext_bus; 
	
	// Определение шины, по которой идет обмен данными
	RECOGNIZE_BUS(ext_bus, uart_struct);
	
	// Прием пакета данных по шине
	if(receive_packet(uart_struct, ext_bus) != NO_ERROR)
	{
		return 1;
	}
	
	// Выполнение команды периферией (для МПА - опрос каналов АЦП)
	do_mpa_task(&adc_1);
		
	// Выполнение соответствующих протокольных команд
 	if(protocol_do_cmds(ext_bus) != 0)
	{
		return 1; 
	}
	
	// Передача ответноно пакета
	if(transmit_packet(uart_struct, ext_bus) != NO_ERROR)
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
	// Темповые переменные, необходиме для обработки данных АЦП
	int adc_code[MAX_CHANEL_NUMBER] = {0};
	int16_t adc_value;
	
	// TODO: на текущий момент данная ф-ция обрабатывает данные каналов МПА только для напряжений 0-10В (для тока в карту регистров надо добавлять свои полиномы, т.к. они отличаются)
	
	// Указатель на пространство регистров МПА
	mpa_ram_registers *ptr = &ram_space_pointer->mpa_ram_register_space;
	
	// Ниже приведены аппроксимирующие полиномы, полученные экспериментально для всех режимов работы отладочной платы АЦП
	/*
		для двуполярного вх напр (-5.4 ... 5.4 В ) на мультиплексоре A0=A1=0
				U = 1.6474f*pow(10,-4)*adc_code;
				delta = 6.5627f*pow(10,-6)*adc_code + 0.00039f;
		для однополярного вх напр (0 ... 10.8 В ) на мультиплексоре A0=1;A1=0	
				U = 1.6474f*pow(10,-4)*adc_code + 5.398f;
				delta = 6.6962f*pow(10,-6)*adc_code + 0.4252307f;
		для однополярного вх тока (0 ... 21.6 мА ) на мультиплексоре A0=1;A1=0
				I = 3.052f*pow(10,-4)*adc_code + 10.0f;
				delta = -11.9006f*pow(10,-6)*adc_code + 0.03072506f;
		самодиагностика для двуполярного случая	на мультиплексоре A0=1;A1=1 (на выходе должно быть 0В)
				U = 1.6474f*pow(10,-4)*adc_code;
				delta = 6.5627f*pow(10,-6)*adc_code + 0.00039f;			
		самодиагностика для однополярного случая	на мультиплексоре A0=0;A1=1 (на выходе должно быть 0В)
				U = 1.6474f*pow(10,-4)*adc_code + 5.398f;
				delta = 6.6962f*pow(10,-6)*adc_code + 0.4252307f;
				
		Результирующее напряжение после аппроксимации U = U - delta;
	*/

	// Производим усреднение по максим кол-ву выборок, если кол-во выборок различается для разных каналов МПА
	adc_1.avg_num = find_max_halfword(ptr->AI_RomRegs.AI_NumForAverag, CHANEL_NUMBER);
	
	for (uint8_t k = 0; k < CHANEL_NUMBER; k++)
	{
		for (uint8_t i = 0; i < ptr->AI_RomRegs.AI_NumForAverag[k]; i++)
		{		
			memcpy(&adc_value, adc_struct->spi_struct->buffer + (i*CHANEL_NUMBER) + k, sizeof(adc_value));
			adc_code[k] += (int16_t)(~adc_value + 1);	
		}
		adc_code[k] /= ptr->AI_RomRegs.AI_NumForAverag[k];
		// Кладем в соответсвующий регистр МПА полученные код АЦП для текущего канала МПА
		memcpy(&(ptr->AI_CodeADC[k]), &adc_code[k], sizeof(adc_code[k]));
		// В зависимости от режима работы канала МПА (ток/напряжение) вычисляем по аппроксимирующему полиному значение напряжения/тока и кладем 
		// результат в соответсвующий регистр МПА для текущего канала МПА
		switch ( TEST_BIT(k, ptr->AI_RomRegs.AI_OperMode.adc_chs_mode))
		{
			case 0:
					// Напряжение 0-10В
					ptr->AI_PhysQuantFloat[k] = ptr->AI_RomRegs.AI_PolynConst0[k] + (ptr->AI_RomRegs.AI_PolynConst1[k])*(ptr->AI_CodeADC[k]);
					break;
			
			case 1:
					// Ток 0-20мА
					ptr->AI_PhysQuantFloat[k] = 4.004f*(ptr->AI_RomRegs.AI_PolynConst0[k] + (ptr->AI_RomRegs.AI_PolynConst1[k])*(ptr->AI_CodeADC[k]));
					break;
			
			default:
					break;
		}
	}
}
/*
	Функция синхронизации каналов АЦП (выполняется при срабатывании прерывания Timer2 по переполнению счетчика CNT)
*/
void sync_adc_chanels(void *data)
{
	// Только, если инициализирована микросхема АЦП
	if ((adc_1.init_flag == 1))
	{
		// Выключаем прерывания таймера, срабатываемое при переполнении счетчика таймера
		TIMER_ITConfig(adc_1.timer_n_capture->TIMERx, TIMER_STATUS_CNT_ARR, DISABLE);	
		
		// Считываем FIFO буфер SPI
		uint16_t spi_rx_value[FIFO_SIZE];
		for (uint8_t i = 0; i < FIFO_SIZE; i++)
		{
			spi_rx_value[i] = adc_1.spi_struct->SSPx->DR;
		}
		
		// Только, если получили данные всех каналов микросхемы АЦП, то переносим данные из FIFO буфера SPI в буфер SPI, расположенный во внешней ОЗУ
		if (adc_1.ch_rx_num == CHANEL_NUMBER)
		{
			memcpy(ram_space_pointer->spi_1_rx_buffer + spi_1.buffer_counter, spi_rx_value, CHANEL_NUMBER*sizeof(spi_rx_value[0]));
			spi_1.buffer_counter += CHANEL_NUMBER;
			if (adc_1.spi_struct->buffer_counter >= (CHANEL_NUMBER*adc_1.avg_num))
			{
				adc_1.spi_struct->buffer_counter = 0;
			}
		}
		adc_1.ch_rx_num = 0;
	}
	TIMER_ClearITPendingBit(adc_1.timer_n_capture->TIMERx, TIMER_STATUS_CNT_ARR);
}
/*
	Функция приема пакета с результатами измерений одного канала (выполняется при срабатывании прерывания Timer2 по захвату)
*/
void receive_adc_chanel_pack(void *data)
{
	// Только, если инициализирована микросхема АЦП
	if ((adc_1.init_flag == 1))
	{
		// Выключаем прерывания таймера, срабатываемое при переполнении счетчика таймера
		TIMER_ITConfig(adc_1.timer_n_capture->TIMERx, TIMER_STATUS_CNT_ARR, DISABLE);
		// Ведем счетчик принятых данных каналов АЦП
		adc_1.ch_rx_num++;
		// Если счетчик переваливает максимальное число каналов АЦП, значит принятые данные соответсвуют первому каналу АЦП
		if (adc_1.ch_rx_num == (CHANEL_NUMBER+1))
		{
			adc_1.ch_rx_num = 1;
		}
		// Сброс счетчика таймера, тем самым устанавливается таймаут на временной интервал между пакетами данных от микросхемы АЦП
		TIMER_SetCounter(adc_1.timer_n_capture->TIMERx, 0);	
		TIMER_ClearITPendingBit(adc_1.timer_n_capture->TIMERx, TIMER_STATUS_CNT_ARR);
		// Включаем прерывания таймера, срабатываемое при переполнении счетчика таймера
		TIMER_ITConfig(adc_1.timer_n_capture->TIMERx, TIMER_STATUS_CNT_ARR, ENABLE);			
	}
	TIMER_ClearITPendingBit(adc_1.timer_n_capture->TIMERx, TIMER_STATUS_CCR_CAP1_CH4);
}
