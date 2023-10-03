/*!
 \file
 \brief Заголовочный файл с описанием API для работы с SPI
*/

#ifndef __SPI_H
#define __SPI_H

#include "MDR32_Drivers.h"

///ножки для SSP1
#define PORT_SSP1 MDR_PORTC
#define PIN_SSP1_SCK PORT_Pin_7
#define PIN_SSP1_RX PORT_Pin_6
#define PIN_SSP1_TX PORT_Pin_5
#define PIN_SSP1_SS PORT_Pin_8

///Структура с конфигурационными параметрами SPI
typedef struct SPI_ConfigData
{
	MDR_SSP_TypeDef* SSPx;            		///< Библиотечная структура с периферийными регистрами блока SPI
	uint32_t RST_CLK_PCLK_SPIn;						///< Включение тактирования для SPIn
	SSP_InitTypeDef SPI;									///< Библиотечная структура с конфигурационными параметрами SPI
	uint32_t SSP_HCLKdiv;									///< Выбор делителя тактовой частоты для тактирования блока SPIn		 																	
} SPIn;

/*!
 *	\brief Конфигурирует выводы МК для SPI
*/
void spi_gpio_config(void);

/*!
 *	\brief Инициализацирует выбранный  SPIn
 *	\param *SPI_struct - Выбранный SPI для инициализации
*/
void spi_init(SPIn *SPI_struct);

/*!
 *	\brief Отправляет полуслово по SPIn
 *	\param *SPI_struct - Выбранный SPI 
	*	\param half_word - Полуслово для отправки
*/
void spi_transmit_halfword(SPIn *SPI_struct, uint16_t half_word);

/*!
 *	\brief Отправляет массив полуслов по SPIn
 *	\param *SPI_struct - Выбранный SPI
 *	\param 	message - Массив для отправки
 *	\param length - Размер массива
*/
void spi_transmit_message(SPIn *SPI_struct, uint16_t message[], uint32_t length);

/*!
 *	\brief Принимает полуслово по SPIn
 *	\param *SPI_struct - Выбранный SPI 
*/
uint16_t spi_receive_halfword(SPIn *SPI_struct);


#endif /*__SPI_H */