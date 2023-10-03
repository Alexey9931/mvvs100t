 #include "PORTS.h"

void ports_init(void)
{
	// Включение тактирования портов
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB|RST_CLK_PCLK_PORTC|RST_CLK_PCLK_PORTD|RST_CLK_PCLK_PORTE, ENABLE);	

	// Инициализации портов UART1
	PORT_InitTypeDef GPIO_init_structUART1;

	PORT_StructInit(&GPIO_init_structUART1);
	GPIO_init_structUART1.PORT_FUNC = PORT_FUNC_MAIN;
	GPIO_init_structUART1.PORT_SPEED = PORT_SPEED_MAXFAST;
	GPIO_init_structUART1.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_init_structUART1.PORT_PULL_UP = PORT_PULL_UP_OFF;
	GPIO_init_structUART1.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
	GPIO_init_structUART1.PORT_PD_SHM = PORT_PD_SHM_OFF;
	GPIO_init_structUART1.PORT_PD = PORT_PD_DRIVER;
	GPIO_init_structUART1.PORT_GFEN = PORT_GFEN_OFF;
	// Инициализация вывода PC3 как UART_TX (передача)
	GPIO_init_structUART1.PORT_Pin = PORT_Pin_3;
	GPIO_init_structUART1.PORT_OE = PORT_OE_OUT;
	PORT_Init(MDR_PORTC, &GPIO_init_structUART1);
	// Инициализация вывода PC4 как UART_RX (прием)
	GPIO_init_structUART1.PORT_Pin = PORT_Pin_4;
	GPIO_init_structUART1.PORT_OE = PORT_OE_IN;
	PORT_Init(MDR_PORTC, &GPIO_init_structUART1);

	//инициализация ножки разрешения записи данных по UART1 (для микросхемы rs485)
	GPIO_init_structUART1.PORT_Pin = PIN_UART1_EN;
	GPIO_init_structUART1.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_structUART1.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_UART1_EN, &GPIO_init_structUART1);
	//дезактивирование микросхемы RS485 на выдачу данных
	PORT_WriteBit(PORT_UART1_EN, PIN_UART1_EN, 0);


	// Инициализация портов UART2
	PORT_InitTypeDef GPIO_init_structUART2;
	
	PORT_StructInit(&GPIO_init_structUART2);
	GPIO_init_structUART2.PORT_FUNC = PORT_FUNC_MAIN;
	GPIO_init_structUART2.PORT_SPEED = PORT_SPEED_MAXFAST;
	GPIO_init_structUART2.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_init_structUART2.PORT_PULL_UP = PORT_PULL_UP_OFF;
	GPIO_init_structUART2.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
	GPIO_init_structUART2.PORT_PD_SHM = PORT_PD_SHM_OFF;
	GPIO_init_structUART2.PORT_PD = PORT_PD_DRIVER;
	GPIO_init_structUART2.PORT_GFEN = PORT_GFEN_OFF;
	// Инициализация вывода PD13 как UART_TX (передача)
	GPIO_init_structUART2.PORT_Pin = PORT_Pin_13;
	GPIO_init_structUART2.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_UART2_EN, &GPIO_init_structUART2);
	// Инициализация вывода PD14 как UART_RX (прием)
	GPIO_init_structUART2.PORT_Pin = PORT_Pin_14;
	GPIO_init_structUART2.PORT_OE = PORT_OE_IN;
	PORT_Init(PORT_UART2_EN, &GPIO_init_structUART2);

	//инициализация ножки разрешения записи данных по UART2 (для микросхемы rs485)
	GPIO_init_structUART2.PORT_Pin = PIN_UART2_EN;
	GPIO_init_structUART2.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_structUART2.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_UART2_EN, &GPIO_init_structUART2);
	//дезактивирование микросхемы RS485 на выдачу данных
	PORT_WriteBit(PORT_UART2_EN, PIN_UART2_EN, 0);


	// Инициализация портов SSP1
	PORT_InitTypeDef GPIO_init_struct_SPI1;
	
	PORT_StructInit(&GPIO_init_struct_SPI1);
	GPIO_init_struct_SPI1.PORT_FUNC = PORT_FUNC_ALTER;
	GPIO_init_struct_SPI1.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_init_struct_SPI1.PORT_SPEED = PORT_SPEED_MAXFAST;
	//Инициализация вывода PC7 как SCK
	GPIO_init_struct_SPI1.PORT_Pin = PORT_Pin_7;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_IN;
	PORT_Init(MDR_PORTC, &GPIO_init_struct_SPI1);
	//Инициализация вывода PC6 как SSP_RX
	GPIO_init_struct_SPI1.PORT_Pin = PORT_Pin_6;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_IN;
	PORT_Init(MDR_PORTC, &GPIO_init_struct_SPI1);
	//Инициализация вывода PC5 как SSP_TX
	GPIO_init_struct_SPI1.PORT_Pin = PORT_Pin_5;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_OUT;
	PORT_Init(MDR_PORTC, &GPIO_init_struct_SPI1);
	PORT_SetBits(MDR_PORTC,PORT_Pin_5);
	//Инициализация вывода PC8 как SS
	GPIO_init_struct_SPI1.PORT_Pin = PORT_Pin_8;
	GPIO_init_struct_SPI1.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_OUT;
	PORT_Init(MDR_PORTC, &GPIO_init_struct_SPI1);
	//установка SS в лог ноль
	PORT_ResetBits(MDR_PORTC,PORT_Pin_8);
	
	//инициализация RESET PD9 для аппаратного сброса ацп
	GPIO_init_struct_SPI1.PORT_Pin = PIN_ADC_RST;
	GPIO_init_struct_SPI1.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_ADC_RST, &GPIO_init_struct_SPI1);
	//установка RESET в лог единицу
	PORT_SetBits(PORT_ADC_RST,PIN_ADC_RST);
	
	
	//Инициализация порта E для канала 2 таймера 2
	PORT_InitTypeDef GPIO_init_struct_TIMER2;
	
	PORT_StructInit (&GPIO_init_struct_TIMER2);
	GPIO_init_struct_TIMER2.PORT_FUNC = PORT_FUNC_MAIN;
	GPIO_init_struct_TIMER2.PORT_OE = PORT_OE_IN;
	GPIO_init_struct_TIMER2.PORT_SPEED = PORT_SPEED_MAXFAST;
	GPIO_init_struct_TIMER2.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_init_struct_TIMER2.PORT_Pin = PIN_ADC_SDIFS_IRQ;
	GPIO_init_struct_TIMER2.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
	PORT_Init (PORT_ADC_SDIFS_IRQ, &GPIO_init_struct_TIMER2);


}