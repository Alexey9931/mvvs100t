#ifndef __CONFIG_C
#define __CONFIG_C

#include "EBC.h"

void ebc_ports_config(void)
{
		// Структура для инициализации линий ввода-вывода системной шины
		PORT_InitTypeDef ExtBusInitStruct;
		
		/* Enables the clock on PORTS */
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);	

		/* Configure PORTA pins 0..15 for inout ExtBus data  D0-D15*/
		ExtBusInitStruct.PORT_Pin   	= (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7 | PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 | PORT_Pin_13 | PORT_Pin_14 | PORT_Pin_15);
		ExtBusInitStruct.PORT_FUNC  	= PORT_FUNC_MAIN;
		ExtBusInitStruct.PORT_MODE  	= PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED 	= PORT_SPEED_FAST;

		PORT_Init(MDR_PORTA, &ExtBusInitStruct);

		/* Configure PORTB pins 0..15 for inout ExtBus data  D16-D31*/
		ExtBusInitStruct.PORT_Pin   	= (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7 | PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 | PORT_Pin_13 | PORT_Pin_14 | PORT_Pin_15);
		ExtBusInitStruct.PORT_FUNC  	= PORT_FUNC_MAIN;
		ExtBusInitStruct.PORT_MODE  	= PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED 	= PORT_SPEED_FAST;

		PORT_Init(MDR_PORTB, &ExtBusInitStruct);
		
		//Config ~OE, ~WE
		ExtBusInitStruct.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1  );
		ExtBusInitStruct.PORT_FUNC  = PORT_FUNC_MAIN;
		ExtBusInitStruct.PORT_MODE  	= PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED 	= PORT_SPEED_FAST;

		PORT_Init(MDR_PORTC, &ExtBusInitStruct);
		
		//Config BE3, BE2, BE1, BE0
		ExtBusInitStruct.PORT_Pin   = ( PORT_Pin_9 | PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 );
		ExtBusInitStruct.PORT_FUNC  = PORT_FUNC_ALTER;
		ExtBusInitStruct.PORT_MODE  	= PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED 	= PORT_SPEED_FAST;

		PORT_Init(MDR_PORTC, &ExtBusInitStruct);

		/* Configure PORTF pins 5..15 for output ExtBus adress  A2-A12*/ 
		ExtBusInitStruct.PORT_Pin   = ( PORT_Pin_5 | PORT_Pin_6 | PORT_Pin_7 | PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10 | PORT_Pin_11 | PORT_Pin_12 | PORT_Pin_13 | PORT_Pin_14 | PORT_Pin_15);
		ExtBusInitStruct.PORT_FUNC  = PORT_FUNC_ALTER;
		ExtBusInitStruct.PORT_MODE  = PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED = PORT_SPEED_FAST;

		PORT_Init(MDR_PORTF, &ExtBusInitStruct);

		/* Configure PORTD pin 15 for output ExtBus adress  A13*/
		ExtBusInitStruct.PORT_Pin   = PORT_Pin_15;
		ExtBusInitStruct.PORT_FUNC  = PORT_FUNC_ALTER;
		ExtBusInitStruct.PORT_MODE  	= PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED 	= PORT_SPEED_FAST;

		PORT_Init(MDR_PORTD, &ExtBusInitStruct);

		/* Configure PORTE pins 0..5 for output ExtBus adress  A14-A19*/ 
		ExtBusInitStruct.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5);
		ExtBusInitStruct.PORT_FUNC  = PORT_FUNC_ALTER;
		ExtBusInitStruct.PORT_MODE  = PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED = PORT_SPEED_FAST;

		PORT_Init(MDR_PORTE, &ExtBusInitStruct);

		//Config CS1 - PE7(A21) и CS0 - PE6(A20)
		ExtBusInitStruct.PORT_Pin   = (PORT_Pin_7 | PORT_Pin_6);
		ExtBusInitStruct.PORT_FUNC  = PORT_FUNC_ALTER;
		ExtBusInitStruct.PORT_MODE  	= PORT_MODE_DIGITAL;
		ExtBusInitStruct.PORT_SPEED 	= PORT_SPEED_FAST;

		PORT_Init(MDR_PORTE, &ExtBusInitStruct);
}

void ebc_config(void)
{
		EBC_InitTypeDef EBC_InitStruct;
    EBC_MemRegionInitTypeDef EBC_MemRegionInitStruct;

    /* Enables the HSI clock for EBC control */
    RST_CLK_PCLKcmd(RST_CLK_PCLK_EBC, ENABLE);

    /* Initializes EBC_InitStruct by default values*/
    EBC_StructInit(&EBC_InitStruct);
    EBC_InitStruct.EBC_Mode = EBC_MODE_RAM;
    /* Initializes the EBC peripheral registers */
    EBC_Init(&EBC_InitStruct);

    /* Initializes the EBC memory region 0х50000000-0х5FFFFFFF */
    /* Struct init */
    EBC_MemRegionStructInit(&EBC_MemRegionInitStruct);
    EBC_MemRegionInitStruct.WS_Setup    = 3;
    EBC_MemRegionInitStruct.WS_Active   = 9;
    EBC_MemRegionInitStruct.WS_Hold     = 3;
    EBC_MemRegionInitStruct.Enable_Tune = ENABLE;
    /* Initializes the EBC memory region  */
    EBC_MemRegionInit(&EBC_MemRegionInitStruct, EBC_MEM_REGION_50000000);
}

#endif	//__CONFIG_C
