 /*!
 \file
 \brief Заголовочный файл с описанием API для работы с SPI
*/

#ifndef __SPI_H
#define __SPI_H

#include "mdr32_drivers.h"

// Ножки для SSP1
#define PORT_SSP1 MDR_PORTD				///< Порт SSP1
#define PIN_SSP1_SCK PORT_Pin_4		///< Линия SCK порта SSP1
#define PIN_SSP1_RX PORT_Pin_3		///< Линия RX порта SSP1
#define PIN_SSP1_TX PORT_Pin_2		///< Линия TX порта SSP1
#define PIN_SSP1_SS PORT_Pin_5		///< Линия SS порта SSP1

#define FIFO_SIZE 8						///< Размер буфера FIFO SSP в полусловах (16 бит)
#define SPI_BUFFER_SIZE 512		///< Буфер приемника SSP
	
/**
 * @brief Структура с параметрами DMA канала SPIn
 *
 */
typedef struct spi_dma_params
{
	uint8_t 									dma_channel;							 ///< Выбор канала DMA для SPIn
	uint32_t 									dma_irq_counter;					 ///< Счетчик прерываний DMA
	DMA_CtrlDataInitTypeDef 	DMA_InitStructure_SPI_RX;  ///< Структура с настройками DMA в целом 
	DMA_ChannelInitTypeDef 		DMA_Channel_SPI_RX;  			 ///< Структура с настройками канала DMA
} 
#ifndef DOXYGEN
__attribute__((packed))
#endif
spi_n_dma_ch_params;  

/**
 * @brief Структура с конфигурационными параметрами SPI
 *
 */
typedef struct spi_config_data
{
	MDR_SSP_TypeDef* 				SSPx;            						///< Библиотечная структура с периферийными регистрами блока SPI
	spi_n_dma_ch_params 		spi_dma_ch;									///< Структура с параметрами канала DMA для SPI
	uint32_t 								RST_CLK_PCLK_SPIn;					///< Включение тактирования для SPIn
	SSP_InitTypeDef 				SPI;												///< Библиотечная структура с конфигурационными параметрами SPI
	IRQn_Type 							IRQn;												///< Выбор обработчика прерываний блока SPI
	uint8_t 								buffer_counter;							///< Счетчик слов в приемнике SPI
	uint16_t								*buffer;										///< Указатель на буфер приемника SPI
} 
#ifndef DOXYGEN
__attribute__((packed))
#endif
spi_n;

/*!
 *	\brief Инициализацирует выбранный  SPIn
 *	\param *spi_struct - Выбранный SPI для инициализации
*/
void spi_init(spi_n *spi_struct);

/*!
 *	\brief Отправляет полуслово по SPIn
 *	\param *spi_struct - Выбранный SPI 
	*	\param half_word - Полуслово для отправки
*/
void spi_transmit_halfword(spi_n *spi_struct, uint16_t half_word);

/*!
 *	\brief Отправляет массив полуслов по SPIn
 *	\param *spi_struct - Выбранный SPI
 *	\param 	message - Массив для отправки
 *	\param length - Размер массива
*/
void spi_transmit_message(spi_n *spi_struct, uint16_t message[], uint32_t length);

/*!
 *	\brief Принимает полуслово по SPIn
 *	\param *spi_struct - Выбранный SPI 
*/
uint16_t spi_receive_halfword(spi_n *spi_struct);

/*!
 *	\brief Очищает FIFO буфер приемника SPIn
 *	\param *spi_struct - Выбранный SPI 
*/
void spi_clean_fifo_rx_buf(spi_n *spi_struct);

/*!
 *	\brief Инициализирует n-ый канал DMA  на запрос от приемника SPIn
 *	\param *spi_struct - Выбранный SPI 
*/
void dma_spi_rx_init(spi_n *spi_struct);


#endif /*__SPI_H */
