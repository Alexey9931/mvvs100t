/*!
 \file
 \brief ���� � ����������� API ��� ������ � SPI
*/
#include "SPI.h"

SPIn SPI1;
SPIn SPI2;

void spi_init(SPIn *SPI_struct)
{
	// ��������� ������������ SPI
	RST_CLK_PCLKcmd(SPI_struct->RST_CLK_PCLK_SPIn, ENABLE);

	SSP_StructInit(&SPI_struct->SPI);
	
	SSP_BRGInit(SPI_struct->SSPx, SPI_struct->SSP_HCLKdiv);
	
	SPI_struct->SPI.SSP_WordLength = SPI_struct->SPI.SSP_WordLength;
	SPI_struct->SPI.SSP_Mode = SPI_struct->SPI.SSP_Mode;
	SPI_struct->SPI.SSP_SPH = SPI_struct->SPI.SSP_SPH;
  SPI_struct->SPI.SSP_FRF = SPI_struct->SPI.SSP_FRF;
  SPI_struct->SPI.SSP_HardwareFlowControl = SPI_struct->SPI.SSP_HardwareFlowControl;
	//SSP_InitStruct.SSP_SCR  = 0x10;
  SPI_struct->SPI.SSP_CPSDVSR = SPI_struct->SPI.SSP_CPSDVSR;//������� ������ 2��� 
	SSP_Init(SPI_struct->SSPx,&SPI_struct->SPI);
	
	// ����� ���������� ���������� (����� � �������� ������)
  SSP_ITConfig (SPI_struct->SSPx, SSP_IT_RX | SSP_IT_TX, ENABLE);
	
	SSP_Cmd(SPI_struct->SSPx, DISABLE);
	
}
void spi_transmit_word(MDR_SSP_TypeDef* SSPx, uint16_t WORD)
{
	SSP_SendData(SSPx, WORD);
	while (	SSP_GetFlagStatus(SSPx, SSP_FLAG_TFE) != SET);
}
void spi_transmit_message(MDR_SSP_TypeDef* SSPx, uint16_t message[], uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		while(SSP_GetFlagStatus(SSPx, SSP_FLAG_TNF) != SET) {} // ���� ����� � ������ �������� ����� � ����� ���������� ��������� ����
		SSP_SendData(SSPx, message[i]);			
	}	
	while (SSP_GetFlagStatus(SSPx, SSP_FLAG_TFE) != SET) {}  //���� ���� ���� �����		
}
uint16_t spi_receive_word(MDR_SSP_TypeDef* SSPx)
{	
	uint16_t tmpVar;
	// ��������� ���������� �� ��������� ������
	while (SSP_GetFlagStatus(SSPx, SSP_FLAG_RNE) != SET)  {}  //���� ���� �������� ����  
	// �������� ������
	tmpVar = SSP_ReceiveData(SSPx);
		
	return tmpVar;
}
