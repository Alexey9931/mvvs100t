/*!
 \file
 \brief Заголовочный файл с описанием API для работы с SPI
*/

#ifndef __SPI_H
#define __SPI_H

#include "MDR32_Drivers.h"

///Структура с конфигурационными параметрами SPI
typedef struct SPI_ConfigData
{
	MDR_SSP_TypeDef* SSPx;            		///< Библиотечная структура с периферийными регистрами блока SPI
	uint32_t RST_CLK_PCLK_SPIn;						///< Включение тактирования для SPIn
	SSP_InitTypeDef SPI;									///< Библиотечная структура с конфигурационными параметрами SPI
	uint32_t SSP_HCLKdiv;									///< Выбор делителя тактовой частоты для тактирования блока SPIn		 																	
} SPIn;

/*!
 *	\brief Инициализацирует выбранный  SPIn
 *	\param *SPI_struct - Выбранный SPI для инициализации
*/
void spi_init(SPIn *SPI_struct);

void spi_transmit_word(MDR_SSP_TypeDef* SSPx, uint16_t WORD);

void spi_transmit_message(MDR_SSP_TypeDef* SSPx, uint16_t message[], uint32_t length);

uint16_t spi_receive_word(MDR_SSP_TypeDef* SSPx);


#endif /*__SPI_H */