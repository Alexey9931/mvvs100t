/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � SPI
*/

#ifndef __SPI_H
#define __SPI_H

#include "MDR32_Drivers.h"

///��������� � ����������������� ����������� SPI
typedef struct SPI_ConfigData
{
	MDR_SSP_TypeDef* SSPx;            		///< ������������ ��������� � ������������� ���������� ����� SPI
	uint32_t RST_CLK_PCLK_SPIn;						///< ��������� ������������ ��� SPIn
	SSP_InitTypeDef SPI;									///< ������������ ��������� � ����������������� ����������� SPI
	uint32_t SSP_HCLKdiv;									///< ����� �������� �������� ������� ��� ������������ ����� SPIn		 																	
} SPIn;

/*!
 *	\brief ���������������� ���������  SPIn
 *	\param *SPI_struct - ��������� SPI ��� �������������
*/
void spi_init(SPIn *SPI_struct);

void spi_transmit_word(MDR_SSP_TypeDef* SSPx, uint16_t WORD);

void spi_transmit_message(MDR_SSP_TypeDef* SSPx, uint16_t message[], uint32_t length);

uint16_t spi_receive_word(MDR_SSP_TypeDef* SSPx);


#endif /*__SPI_H */