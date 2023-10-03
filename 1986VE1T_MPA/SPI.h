/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � SPI
*/

#ifndef __SPI_H
#define __SPI_H

#include "MDR32_Drivers.h"

///����� ��� SSP1
#define PORT_SSP1 MDR_PORTC
#define PIN_SSP1_SCK PORT_Pin_7
#define PIN_SSP1_RX PORT_Pin_6
#define PIN_SSP1_TX PORT_Pin_5
#define PIN_SSP1_SS PORT_Pin_8

///��������� � ����������������� ����������� SPI
typedef struct SPI_ConfigData
{
	MDR_SSP_TypeDef* SSPx;            		///< ������������ ��������� � ������������� ���������� ����� SPI
	uint32_t RST_CLK_PCLK_SPIn;						///< ��������� ������������ ��� SPIn
	SSP_InitTypeDef SPI;									///< ������������ ��������� � ����������������� ����������� SPI
	uint32_t SSP_HCLKdiv;									///< ����� �������� �������� ������� ��� ������������ ����� SPIn		 																	
} SPIn;

/*!
 *	\brief ������������� ������ �� ��� SPI
*/
void spi_gpio_config(void);

/*!
 *	\brief ���������������� ���������  SPIn
 *	\param *SPI_struct - ��������� SPI ��� �������������
*/
void spi_init(SPIn *SPI_struct);

/*!
 *	\brief ���������� ��������� �� SPIn
 *	\param *SPI_struct - ��������� SPI 
	*	\param half_word - ��������� ��� ��������
*/
void spi_transmit_halfword(SPIn *SPI_struct, uint16_t half_word);

/*!
 *	\brief ���������� ������ �������� �� SPIn
 *	\param *SPI_struct - ��������� SPI
 *	\param 	message - ������ ��� ��������
 *	\param length - ������ �������
*/
void spi_transmit_message(SPIn *SPI_struct, uint16_t message[], uint32_t length);

/*!
 *	\brief ��������� ��������� �� SPIn
 *	\param *SPI_struct - ��������� SPI 
*/
uint16_t spi_receive_halfword(SPIn *SPI_struct);


#endif /*__SPI_H */