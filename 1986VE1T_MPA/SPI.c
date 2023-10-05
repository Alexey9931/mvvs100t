/*!
 \file
 \brief Файл с реализацией API для работы с SPI
*/
#include "SPI.h"

spi_n spi_1;
spi_n spi_2;

/*
Функция конфигурирования выводов МК для SPI
*/
void spi_gpio_config(void);
void spi_gpio_config(void)
{
	// Включение тактирования портов
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK|RST_CLK_PCLK_PORTC, ENABLE);	
	
	// Инициализация портов SSP1
	PORT_InitTypeDef GPIO_init_struct_SPI1;
	
	PORT_StructInit(&GPIO_init_struct_SPI1);
	GPIO_init_struct_SPI1.PORT_FUNC = PORT_FUNC_ALTER;
	GPIO_init_struct_SPI1.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_init_struct_SPI1.PORT_SPEED = PORT_SPEED_MAXFAST;
	//Инициализация вывода PC7 как SCK
	GPIO_init_struct_SPI1.PORT_Pin = PIN_SSP1_SCK;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_IN;
	PORT_Init(PORT_SSP1, &GPIO_init_struct_SPI1);
	//Инициализация вывода PC6 как SSP_RX
	GPIO_init_struct_SPI1.PORT_Pin = PIN_SSP1_RX;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_IN;
	PORT_Init(PORT_SSP1, &GPIO_init_struct_SPI1);
	//Инициализация вывода PC5 как SSP_TX
	GPIO_init_struct_SPI1.PORT_Pin = PIN_SSP1_TX;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_SSP1, &GPIO_init_struct_SPI1);
	PORT_SetBits(PORT_SSP1,PORT_Pin_5);
	//Инициализация вывода PC8 как SS
	GPIO_init_struct_SPI1.PORT_Pin = PIN_SSP1_SS;
	GPIO_init_struct_SPI1.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_SSP1, &GPIO_init_struct_SPI1);
	//установка SS в лог ноль
	PORT_ResetBits(PORT_SSP1,PIN_SSP1_SS);
}
/*
Функция инициализация SPI
*/
void spi_init(spi_n *spi_struct)
{
	spi_gpio_config();
	
	//Структура для инициализации SPI
	SSP_InitTypeDef SSP_InitStruct;
	
	// Включение тактирования SPI
	RST_CLK_PCLKcmd(spi_struct->RST_CLK_PCLK_SPIn, ENABLE);

	SSP_StructInit(&SSP_InitStruct);
	
	SSP_BRGInit(spi_struct->SSPx, spi_struct->SSP_HCLKdiv);
	
	SSP_InitStruct.SSP_WordLength = spi_struct->SPI.SSP_WordLength;
	SSP_InitStruct.SSP_Mode = spi_struct->SPI.SSP_Mode;
	SSP_InitStruct.SSP_SPH = spi_struct->SPI.SSP_SPH;
  SSP_InitStruct.SSP_FRF = spi_struct->SPI.SSP_FRF;
  SSP_InitStruct.SSP_HardwareFlowControl = spi_struct->SPI.SSP_HardwareFlowControl;
	//SSP_InitStruct.SSP_SCR  = 0x10;
  SSP_InitStruct.SSP_CPSDVSR = spi_struct->SPI.SSP_CPSDVSR;//частота обмена 2МГц 
	SSP_Init(spi_struct->SSPx,&SSP_InitStruct);
	
	// Выбор источников прерываний (прием и передача данных)
  SSP_ITConfig (spi_struct->SSPx, SSP_IT_RX | SSP_IT_TX, ENABLE);
	
	//Специально не включаем SPI, т.к. МК находится в SLAVE-режиме
	SSP_Cmd(spi_struct->SSPx, DISABLE);
}
/*
Функция передачи полуслова по SPI
*/
void spi_transmit_halfword(spi_n *spi_struct, uint16_t half_word)
{
	SSP_SendData(spi_struct->SSPx, half_word);
	while (	SSP_GetFlagStatus(spi_struct->SSPx, SSP_FLAG_TFE) != SET);
}
/*
Функция передачи массива по SPI
*/
void spi_transmit_message(spi_n *spi_struct, uint16_t message[], uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		while(SSP_GetFlagStatus(spi_struct->SSPx, SSP_FLAG_TNF) != SET) {} // Ждем когда в буфере появится место и затем записываем следующий байт
		SSP_SendData(spi_struct->SSPx, message[i]);			
	}	
	while (SSP_GetFlagStatus(spi_struct->SSPx, SSP_FLAG_TFE) != SET) {}  //ждем пока байт уйдет		
}
/*
Функция приема полуслова по SPI
*/
uint16_t spi_receive_halfword(spi_n *spi_struct)
{	
	uint16_t tmpVar;
	// Обработка прерывания от приемника данных
	while (SSP_GetFlagStatus(spi_struct->SSPx, SSP_FLAG_RNE) != SET)  {}  //ждем пока появится байт  
	// Получаем данные
	tmpVar = SSP_ReceiveData(spi_struct->SSPx);
		
	return tmpVar;
}
