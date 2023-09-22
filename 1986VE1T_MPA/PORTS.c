 #include "PORTS.h"

void PortsInit(void)
{
// Включение тактирования портов
RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB|RST_CLK_PCLK_PORTC|RST_CLK_PCLK_PORTD, ENABLE);
	
// Объявление структуры для инициализации портов UART
PORT_InitTypeDef GPIO_init_structUART1;
// Инициализация порта B для функции UART1
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
GPIO_init_structUART1.PORT_Pin = PORT_Pin_7;
GPIO_init_structUART1.PORT_FUNC = PORT_FUNC_PORT;
GPIO_init_structUART1.PORT_OE = PORT_OE_OUT;
PORT_Init(MDR_PORTC, &GPIO_init_structUART1);

// Объявление структуры для инициализации портов UART
PORT_InitTypeDef GPIO_init_struct2;
// Инициализация порта B для функции UART2
PORT_StructInit(&GPIO_init_struct2);
GPIO_init_struct2.PORT_FUNC = PORT_FUNC_MAIN;
GPIO_init_struct2.PORT_SPEED = PORT_SPEED_MAXFAST;
GPIO_init_struct2.PORT_MODE = PORT_MODE_DIGITAL;
GPIO_init_struct2.PORT_PULL_UP = PORT_PULL_UP_OFF;
GPIO_init_struct2.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
GPIO_init_struct2.PORT_PD_SHM = PORT_PD_SHM_OFF;
GPIO_init_struct2.PORT_PD = PORT_PD_DRIVER;
GPIO_init_struct2.PORT_GFEN = PORT_GFEN_OFF;
	
// Инициализация вывода PD13 как UART_TX (передача)
GPIO_init_struct2.PORT_Pin = PORT_Pin_13;
GPIO_init_struct2.PORT_OE = PORT_OE_OUT;
PORT_Init(MDR_PORTD, &GPIO_init_struct2);
// Инициализация вывода PD14 как UART_RX (прием)
GPIO_init_struct2.PORT_Pin = PORT_Pin_14;
GPIO_init_struct2.PORT_OE = PORT_OE_IN;
PORT_Init(MDR_PORTD, &GPIO_init_struct2);

//// Объявление структуры для инициализации 0-3 каналов АЦП
//PORT_InitTypeDef GPIO_init_ADC_0_3;
//PORT_StructInit(&GPIO_init_ADC_0_3);
//// Инициализация вывода PC1 как вход канала 3
//GPIO_init_ADC_0_3.PORT_Pin = PORT_Pin_1;
//GPIO_init_ADC_0_3.PORT_OE = PORT_OE_IN;
//GPIO_init_ADC_0_3.PORT_FUNC = PORT_FUNC_PORT;
//GPIO_init_ADC_0_3.PORT_MODE = PORT_MODE_ANALOG;
//// Инициализация вывода PC2 как вход канала 2
//GPIO_init_ADC_0_3.PORT_Pin = PORT_Pin_2;
//GPIO_init_ADC_0_3.PORT_OE = PORT_OE_IN;
//GPIO_init_ADC_0_3.PORT_FUNC = PORT_FUNC_PORT;
//GPIO_init_ADC_0_3.PORT_MODE = PORT_MODE_ANALOG;
//// Инициализация вывода PC3 как вход канала 1
//GPIO_init_ADC_0_3.PORT_Pin = PORT_Pin_3;
//GPIO_init_ADC_0_3.PORT_OE = PORT_OE_IN;
//GPIO_init_ADC_0_3.PORT_FUNC = PORT_FUNC_PORT;
//GPIO_init_ADC_0_3.PORT_MODE = PORT_MODE_ANALOG;
//// Инициализация вывода PC4 как вход канала 0
//GPIO_init_ADC_0_3.PORT_Pin = PORT_Pin_4;
//GPIO_init_ADC_0_3.PORT_OE = PORT_OE_IN;
//GPIO_init_ADC_0_3.PORT_FUNC = PORT_FUNC_PORT;
//GPIO_init_ADC_0_3.PORT_MODE = PORT_MODE_ANALOG;

//PORT_Init(MDR_PORTC, &GPIO_init_ADC_0_3);

//// Объявление структуры для инициализации 4-7 каналов АЦП
//PORT_InitTypeDef GPIO_init_ADC_4_7;
//PORT_StructInit(&GPIO_init_ADC_4_7);
//// Инициализация вывода PB6 как вход канала 7
//GPIO_init_ADC_4_7.PORT_Pin = PORT_Pin_6;
//GPIO_init_ADC_4_7.PORT_OE = PORT_OE_IN;
//GPIO_init_ADC_4_7.PORT_FUNC = PORT_FUNC_PORT;
//GPIO_init_ADC_4_7.PORT_MODE = PORT_MODE_ANALOG;
//// Инициализация вывода PB7 как вход канала 6
//GPIO_init_ADC_4_7.PORT_Pin = PORT_Pin_7;
//GPIO_init_ADC_4_7.PORT_OE = PORT_OE_IN;
//GPIO_init_ADC_4_7.PORT_FUNC = PORT_FUNC_PORT;
//GPIO_init_ADC_4_7.PORT_MODE = PORT_MODE_ANALOG;
//// Инициализация вывода PB8 как вход канала 5
//GPIO_init_ADC_4_7.PORT_Pin = PORT_Pin_8;
//GPIO_init_ADC_4_7.PORT_OE = PORT_OE_IN;
//GPIO_init_ADC_4_7.PORT_FUNC = PORT_FUNC_PORT;
//GPIO_init_ADC_4_7.PORT_MODE = PORT_MODE_ANALOG;
//// Инициализация вывода PB9 как вход канала 4
//GPIO_init_ADC_4_7.PORT_Pin = PORT_Pin_9;
//GPIO_init_ADC_4_7.PORT_OE = PORT_OE_IN;
//GPIO_init_ADC_4_7.PORT_FUNC = PORT_FUNC_PORT;
//GPIO_init_ADC_4_7.PORT_MODE = PORT_MODE_ANALOG;

//PORT_Init(MDR_PORTB, &GPIO_init_ADC_4_7);


}