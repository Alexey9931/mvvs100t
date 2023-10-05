/*!
 \file
 \brief ���� � ����������� API ��� ������ � SPI
*/
#include "SPI.h"

spi_n spi_1;
spi_n spi_2;

/*
������� ���������������� ������� �� ��� SPI
*/
void spi_gpio_config(void);
void spi_gpio_config(void)
{
	// ��������� ������������ ������
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK|RST_CLK_PCLK_PORTC, ENABLE);	
	
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
void spi_init(spi_n *spi_struct)
{
	spi_gpio_config();
	
	//��������� ��� ������������� SPI
	SSP_InitTypeDef SSP_InitStruct;
	
	// ��������� ������������ SPI
	RST_CLK_PCLKcmd(spi_struct->RST_CLK_PCLK_SPIn, ENABLE);

	SSP_StructInit(&SSP_InitStruct);
	
	SSP_BRGInit(spi_struct->SSPx, spi_struct->SSP_HCLKdiv);
	
	SSP_InitStruct.SSP_WordLength = spi_struct->SPI.SSP_WordLength;
	SSP_InitStruct.SSP_Mode = spi_struct->SPI.SSP_Mode;
	SSP_InitStruct.SSP_SPH = spi_struct->SPI.SSP_SPH;
  SSP_InitStruct.SSP_FRF = spi_struct->SPI.SSP_FRF;
  SSP_InitStruct.SSP_HardwareFlowControl = spi_struct->SPI.SSP_HardwareFlowControl;
	//SSP_InitStruct.SSP_SCR  = 0x10;
  SSP_InitStruct.SSP_CPSDVSR = spi_struct->SPI.SSP_CPSDVSR;//������� ������ 2��� 
	SSP_Init(spi_struct->SSPx,&SSP_InitStruct);
	
	// ����� ���������� ���������� (����� � �������� ������)
  SSP_ITConfig (spi_struct->SSPx, SSP_IT_RX | SSP_IT_TX, ENABLE);
	
	//���������� �� �������� SPI, �.�. �� ��������� � SLAVE-������
	SSP_Cmd(spi_struct->SSPx, DISABLE);
}
/*
������� �������� ��������� �� SPI
*/
void spi_transmit_halfword(spi_n *spi_struct, uint16_t half_word)
{
	SSP_SendData(spi_struct->SSPx, half_word);
	while (	SSP_GetFlagStatus(spi_struct->SSPx, SSP_FLAG_TFE) != SET);
}
/*
������� �������� ������� �� SPI
*/
void spi_transmit_message(spi_n *spi_struct, uint16_t message[], uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		while(SSP_GetFlagStatus(spi_struct->SSPx, SSP_FLAG_TNF) != SET) {} // ���� ����� � ������ �������� ����� � ����� ���������� ��������� ����
		SSP_SendData(spi_struct->SSPx, message[i]);			
	}	
	while (SSP_GetFlagStatus(spi_struct->SSPx, SSP_FLAG_TFE) != SET) {}  //���� ���� ���� �����		
}
/*
������� ������ ��������� �� SPI
*/
uint16_t spi_receive_halfword(spi_n *spi_struct)
{	
	uint16_t tmpVar;
	// ��������� ���������� �� ��������� ������
	while (SSP_GetFlagStatus(spi_struct->SSPx, SSP_FLAG_RNE) != SET)  {}  //���� ���� �������� ����  
	// �������� ������
	tmpVar = SSP_ReceiveData(spi_struct->SSPx);
		
	return tmpVar;
}
