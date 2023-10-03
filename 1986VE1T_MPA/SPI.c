/*!
 \file
 \brief ���� � ����������� API ��� ������ � SPI
*/
#include "SPI.h"

SPIn SPI1;
SPIn SPI2;

/*
������� ���������������� ������� �� ��� SPI
*/
void spi_gpio_config(void)
{
	// ��������� ������������ ������
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);	
	
	// ������������� ������ SSP1
	PORT_InitTypeDef GPIO_init_struct_SPI1;
	
	PORT_StructInit(&GPIO_init_struct_SPI1);
	GPIO_init_struct_SPI1.PORT_FUNC = PORT_FUNC_ALTER;
	GPIO_init_struct_SPI1.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_init_struct_SPI1.PORT_SPEED = PORT_SPEED_MAXFAST;
	//������������� ������ PC7 ��� SCK
	GPIO_init_struct_SPI1.PORT_Pin = PIN_SSP1_SCK;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_IN;
	PORT_Init(PORT_SSP1, &GPIO_init_struct_SPI1);
	//������������� ������ PC6 ��� SSP_RX
	GPIO_init_struct_SPI1.PORT_Pin = PIN_SSP1_RX;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_IN;
	PORT_Init(PORT_SSP1, &GPIO_init_struct_SPI1);
	//������������� ������ PC5 ��� SSP_TX
	GPIO_init_struct_SPI1.PORT_Pin = PIN_SSP1_TX;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_SSP1, &GPIO_init_struct_SPI1);
	PORT_SetBits(PORT_SSP1,PORT_Pin_5);
	//������������� ������ PC8 ��� SS
	GPIO_init_struct_SPI1.PORT_Pin = PIN_SSP1_SS;
	GPIO_init_struct_SPI1.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_struct_SPI1.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_SSP1, &GPIO_init_struct_SPI1);
	//��������� SS � ��� ����
	PORT_ResetBits(PORT_SSP1,PIN_SSP1_SS);
}
/*
������� ������������� SPI
*/
void spi_init(SPIn *SPI_struct)
{
	spi_gpio_config();
	
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
	
	//���������� �� �������� SPI, �.�. �� ��������� � SLAVE-������
	SSP_Cmd(SPI_struct->SSPx, DISABLE);
}
/*
������� �������� ��������� �� SPI
*/
void spi_transmit_halfword(SPIn *SPI_struct, uint16_t half_word)
{
	SSP_SendData(SPI_struct->SSPx, half_word);
	while (	SSP_GetFlagStatus(SPI_struct->SSPx, SSP_FLAG_TFE) != SET);
}
/*
������� �������� ������� �� SPI
*/
void spi_transmit_message(SPIn *SPI_struct, uint16_t message[], uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		while(SSP_GetFlagStatus(SPI_struct->SSPx, SSP_FLAG_TNF) != SET) {} // ���� ����� � ������ �������� ����� � ����� ���������� ��������� ����
		SSP_SendData(SPI_struct->SSPx, message[i]);			
	}	
	while (SSP_GetFlagStatus(SPI_struct->SSPx, SSP_FLAG_TFE) != SET) {}  //���� ���� ���� �����		
}
/*
������� ������ ��������� �� SPI
*/
uint16_t spi_receive_halfword(SPIn *SPI_struct)
{	
	uint16_t tmpVar;
	// ��������� ���������� �� ��������� ������
	while (SSP_GetFlagStatus(SPI_struct->SSPx, SSP_FLAG_RNE) != SET)  {}  //���� ���� �������� ����  
	// �������� ������
	tmpVar = SSP_ReceiveData(SPI_struct->SSPx);
		
	return tmpVar;
}
