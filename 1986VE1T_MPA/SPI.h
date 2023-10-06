/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � SPI
*/

#ifndef __SPI_H
#define __SPI_H

#include "MDR32_Drivers.h"
#include "external_ram.h"

///����� ��� SSP1
#define PORT_SSP1 MDR_PORTC
#define PIN_SSP1_SCK PORT_Pin_7
#define PIN_SSP1_RX PORT_Pin_6
#define PIN_SSP1_TX PORT_Pin_5
#define PIN_SSP1_SS PORT_Pin_8


///��������� � ����������� DMA ������ SPIn
typedef struct spi_dma_params
{
	uint8_t 									dma_channel;							 ///< ����� ������ DMA ��� SPIn
	uint32_t 									dma_irq_counter;					 ///< ������� ���������� DMA
	DMA_CtrlDataInitTypeDef 	DMA_InitStructure_SPI_RX;  ///< ��������� � ����������� DMA � ����� 
	DMA_ChannelInitTypeDef 		DMA_Channel_SPI_RX;  			 ///< ��������� � ����������� ������ DMA
} spi_n_dma_ch_params;  

///��������� � ����������������� ����������� SPI
typedef struct spi_config_data
{
	MDR_SSP_TypeDef* 				SSPx;            						///< ������������ ��������� � ������������� ���������� ����� SPI
	spi_n_dma_ch_params 		spi_dma_ch;									///< ��������� � ����������� ������ DMA ��� SPI
	uint32_t 								RST_CLK_PCLK_SPIn;					///< ��������� ������������ ��� SPIn
	SSP_InitTypeDef 				SPI;												///< ������������ ��������� � ����������������� ����������� SPI
	SSP_Clock_BRG_TypeDef 	SSP_HCLKdiv;								///< ����� �������� �������� ������� ��� ������������ ����� SPIn	
	int16_t 								fifo_halfword;							///< ��������� � ��������� SPI
	uint16_t								buffer[MAX_CHANEL_NUMBER];	///< ����� ��������� SPI
} spi_n;

/*!
 *	\brief ���������������� ���������  SPIn
 *	\param *SPI_struct - ��������� SPI ��� �������������
*/
void spi_init(spi_n *spi_struct);

/*!
 *	\brief ���������� ��������� �� SPIn
 *	\param *spi_struct - ��������� SPI 
	*	\param half_word - ��������� ��� ��������
*/
void spi_transmit_halfword(spi_n *spi_struct, uint16_t half_word);

/*!
 *	\brief ���������� ������ �������� �� SPIn
 *	\param *spi_struct - ��������� SPI
 *	\param 	message - ������ ��� ��������
 *	\param length - ������ �������
*/
void spi_transmit_message(spi_n *spi_struct, uint16_t message[], uint32_t length);

/*!
 *	\brief ��������� ��������� �� SPIn
 *	\param *spi_struct - ��������� SPI 
*/
uint16_t spi_receive_halfword(spi_n *spi_struct);

/*!
 *	\brief �������������� n-�� ����� DMA  �� ������ �� ��������� SPIn
 *	\param *spi_struct - ��������� SPI 
*/
void dma_spi_rx_init(spi_n *spi_struct);


#endif /*__SPI_H */