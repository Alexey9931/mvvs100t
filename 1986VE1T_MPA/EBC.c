/*!
 \file
 \brief Файл с реализацией API для работы с EBC
*/

#include "ebc.h"

/*!
 *	\brief Конфигурует выводы МК EBC для внешнего ОЗУ или ПЗУ
*/
void ebc_gpio_config(ebc_devices device)
{	
		// Структура для инициализации линий ввода-вывода системной шины
		PORT_InitTypeDef ExtBusInitStruct;
		
		// Включение тактирования портов
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);	

		// Конфигурация линии данных
		if (device == EBC_RAM)
		{
			// 32-битный режим данных
			ExtBusInitStruct.PORT_Pin = (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7 | PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 | PORT_Pin_13 | PORT_Pin_14 | PORT_Pin_15);
		}
		else if (device == EBC_ROM)
		{
			// 8-битный режим данных
			ExtBusInitStruct.PORT_Pin = (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7 );
		}
		ExtBusInitStruct.PORT_FUNC = PORT_FUNC_MAIN;
		ExtBusInitStruct.PORT_MODE = PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED = PORT_SPEED_FAST;

		PORT_Init(MDR_PORTA, &ExtBusInitStruct);

		ExtBusInitStruct.PORT_Pin = (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7 | PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 | PORT_Pin_13 | PORT_Pin_14 | PORT_Pin_15);
		if (device == EBC_RAM)
		{
			// 32-битный режим данных
			ExtBusInitStruct.PORT_FUNC = PORT_FUNC_MAIN;
		}
		else if (device == EBC_ROM)
		{
			// 8-битный режим данных
			ExtBusInitStruct.PORT_FUNC = PORT_FUNC_PORT;
		}
		ExtBusInitStruct.PORT_MODE = PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED = PORT_SPEED_FAST;

		PORT_Init(MDR_PORTB, &ExtBusInitStruct);
		
		// Конфигурация OE, WE
		ExtBusInitStruct.PORT_Pin = (PORT_Pin_0 | PORT_Pin_1);
		ExtBusInitStruct.PORT_FUNC = PORT_FUNC_MAIN;
		ExtBusInitStruct.PORT_MODE = PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED = PORT_SPEED_FAST;

		PORT_Init(MDR_PORTC, &ExtBusInitStruct);
		
		// Конфигурация BE3, BE2, BE1, BE0
		ExtBusInitStruct.PORT_Pin = ( PORT_Pin_9 | PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 );
		ExtBusInitStruct.PORT_FUNC = PORT_FUNC_ALTER;
		ExtBusInitStruct.PORT_MODE = PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED = PORT_SPEED_FAST;

		PORT_Init(MDR_PORTC, &ExtBusInitStruct);

		// Конфигурация линии адреса
		if (device == EBC_RAM)
		{
			// Сдвиг адресов A2->A0
			ExtBusInitStruct.PORT_Pin = (PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7 | PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 | PORT_Pin_13 | PORT_Pin_14 | PORT_Pin_15);
		}
		else if (device == EBC_ROM)
		{
			// Нет сдвига адресов A0->A0
			ExtBusInitStruct.PORT_Pin = (PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7 | PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 | PORT_Pin_13 | PORT_Pin_14 | PORT_Pin_15);
		}
		ExtBusInitStruct.PORT_FUNC  = PORT_FUNC_ALTER;
		ExtBusInitStruct.PORT_MODE  = PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED = PORT_SPEED_FAST;

		PORT_Init(MDR_PORTF, &ExtBusInitStruct);

		// Конфигурация линии адреса A13
		ExtBusInitStruct.PORT_Pin = PORT_Pin_15;
		ExtBusInitStruct.PORT_FUNC = PORT_FUNC_ALTER;
		ExtBusInitStruct.PORT_MODE = PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED = PORT_SPEED_FAST;

		PORT_Init(MDR_PORTD, &ExtBusInitStruct);

		// Конфигурация линии адреса A14-A19
		ExtBusInitStruct.PORT_Pin = (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5);
		ExtBusInitStruct.PORT_FUNC = PORT_FUNC_ALTER;
		ExtBusInitStruct.PORT_MODE = PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED = PORT_SPEED_FAST;

		PORT_Init(MDR_PORTE, &ExtBusInitStruct);

		// Конфигурация CS1-PE7(A21) и CS0-PE6(A20)(Ножки выбора ОЗУ)
		ExtBusInitStruct.PORT_Pin = (PORT_Pin_7 | PORT_Pin_6);
		ExtBusInitStruct.PORT_FUNC = PORT_FUNC_ALTER;
		ExtBusInitStruct.PORT_MODE = PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED = PORT_SPEED_FAST;

		PORT_Init(MDR_PORTE, &ExtBusInitStruct);
		
		// Конфигурация CS2-PE8(A22)(Ножка выбора ПЗУ)*/
		ExtBusInitStruct.PORT_Pin = PORT_Pin_8;
		ExtBusInitStruct.PORT_OE  = PORT_OE_OUT;
		ExtBusInitStruct.PORT_FUNC = PORT_FUNC_ALTER;
		ExtBusInitStruct.PORT_MODE = PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_PD = PORT_PD_DRIVER;
		ExtBusInitStruct.PORT_SPEED = PORT_SPEED_SLOW;
		ExtBusInitStruct.PORT_GFEN = PORT_GFEN_OFF;
		ExtBusInitStruct.PORT_PULL_UP = PORT_PULL_UP_OFF;
		ExtBusInitStruct.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;

		PORT_Init(MDR_PORTE, &ExtBusInitStruct);
}
/*
	Функция инициализации EBC для внешнего ОЗУ или ПЗУ
*/
void ebc_init(ebc_devices device)
 {
		ebc_gpio_config(device);
	
		EBC_InitTypeDef EBC_InitStruct;
    EBC_MemRegionInitTypeDef EBC_MemRegionInitStruct;

    // Включение тактирования EBC
    RST_CLK_PCLKcmd(RST_CLK_PCLK_EBC, ENABLE);

    EBC_StructInit(&EBC_InitStruct);
	
		if (device == EBC_RAM)
		{
			EBC_InitStruct.EBC_Mode = EBC_MODE_RAM;
		}
		else if (device == EBC_ROM)
		{
			EBC_InitStruct.EBC_Mode = EBC_MODE_ROM;
		}

    EBC_Init(&EBC_InitStruct);
		
		MDR_EBC->CONTROL = 0x00008001;
		if (device == EBC_RAM)
		{
			MDR_EBC->CONTROL &= ~(1<<5); // 32-битный режим данных
		}
		else if (device == EBC_ROM)
		{
			MDR_EBC->CONTROL |= (1<<5);// 8-битный режим данных
		}

    EBC_MemRegionStructInit(&EBC_MemRegionInitStruct);
    EBC_MemRegionInitStruct.WS_Setup    = 3;
    EBC_MemRegionInitStruct.WS_Active   = 9;
    EBC_MemRegionInitStruct.WS_Hold     = 3;
    EBC_MemRegionInitStruct.Enable_Tune = ENABLE;
    
    EBC_MemRegionInit(&EBC_MemRegionInitStruct, EBC_MEM_REGION_50000000);
}
