/*!
 \file
 \brief ���� � ����������� API ��� ������ � ��� 1273��19�
*/

#include "math.h"
#include "1273pv19t.h"
#include "TIMER.h"
#include "external_ram.h"

extern spi_n spi_1;
extern timer_n timer_2;
adc_n adc_1;


/*
������� ���������������� ������� �� ��� ���
*/
void adc_gpio_config(void);
void adc_gpio_config(void)
{
	// ��������� ������������ ������
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK|RST_CLK_PCLK_PORTD|RST_CLK_PCLK_PORTE|RST_CLK_PCLK_PORTC, ENABLE);	
	
	PORT_InitTypeDef GPIO_init_structADC;
	PORT_StructInit(&GPIO_init_structADC);
	//������������� RESET PD0 ��� ����������� ������ ���
	GPIO_init_structADC.PORT_Pin = PIN_ADC_RST;
	GPIO_init_structADC.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_structADC.PORT_OE = PORT_OE_OUT;
	GPIO_init_structADC.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_init_structADC.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(PORT_ADC_RST, &GPIO_init_structADC);
	//��������� RESET � ��� �������
	PORT_SetBits(PORT_ADC_RST,PIN_ADC_RST);
	
	//������������� ����� ������ ������ ������ (0-10�,4-20��)
	GPIO_init_structADC.PORT_Pin = PIN_ADC_MODE_A0;
	GPIO_init_structADC.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_structADC.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_ADC_MODE, &GPIO_init_structADC);
	
	GPIO_init_structADC.PORT_Pin = PIN_ADC_MODE_A1;
	GPIO_init_structADC.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_structADC.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_ADC_MODE, &GPIO_init_structADC);
	
	//����� ������������� ������ �� �������������� A0=1;A1=0	
	PORT_SetBits(PORT_ADC_MODE,PIN_ADC_MODE_A0);
	PORT_ResetBits(PORT_ADC_MODE,PIN_ADC_MODE_A1);
	
	//������������� ������ NSS
	GPIO_init_structADC.PORT_Pin = PIN_ADC_NSS;
	GPIO_init_structADC.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_structADC.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_ADC_NSS, &GPIO_init_structADC);
	
	//��������� NSS � ��� ����
	PORT_ResetBits(PORT_ADC_NSS, PIN_ADC_NSS);
	
	//������������� ����� E ��� ������ 2 ������� 2 �� ���� ���������� SDIFS/SDOFS �� ��� (����� ������� ��� �������)
	GPIO_init_structADC.PORT_FUNC = PORT_FUNC_MAIN;
	GPIO_init_structADC.PORT_OE = PORT_OE_IN;
	GPIO_init_structADC.PORT_SPEED = PORT_SPEED_MAXFAST;
	GPIO_init_structADC.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_init_structADC.PORT_Pin = PIN_ADC_SDIFS_IRQ;
	GPIO_init_structADC.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
	PORT_Init (PORT_ADC_SDIFS_IRQ, &GPIO_init_structADC);
}
/*
������� ������������� ���
*/
void adc_init(adc_n *adc_struct)
{
	adc_gpio_config();
	//��������� ���
	PORT_SetBits(PORT_ADC_NSS, PIN_ADC_NSS);
	delay_milli(1);
	
	adc_reset();
	delay_milli(1);
	
//	//����� control, ������ � ������� B ��� ��������� ������� DMCLK �� 8��� � SCK �� 4���
//	SSP_SendData(adc_struct->spi_struct->SSPx, 0x8103);
//	delay_milli(1);
//	//��������� ������� SCK SPI
//	adc_struct->spi_struct->SPI.SSP_CPSDVSR = 36;
//	SSP_Init(adc_struct->spi_struct->SSPx,&adc_struct->spi_struct->SPI);
	
	switch (CHANEL_NUMBER)
	{
			case 1:
					//����� control, ������ � ������� D ���������� �������� ���1 � ���2
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8308);
					delay_milli(1);
					break;
			case 2:
					//����� control, ������ � ������� D ���������� �������� ���1 � ���2
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8388);
					delay_milli(1);
					break;
			case 3:
					//����� control, ������ � ������� D ���������� �������� ���1 � ���2
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8388);
					delay_milli(1);
					//����� control, ������ � ������� E ���������� �������� ���3 � ���4
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8408);
					delay_milli(1);
					break;
			case 4:
					//����� control, ������ � ������� D ���������� �������� ���1 � ���2
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8388);
					delay_milli(1);
					//����� control, ������ � ������� E ���������� �������� ���3 � ���4
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8488);
					delay_milli(1);
					break;
			case 5:
					//����� control, ������ � ������� D ���������� �������� ���1 � ���2
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8388);
					delay_milli(1);
					//����� control, ������ � ������� E ���������� �������� ���3 � ���4
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8488);
					delay_milli(1);
					//����� control, ������ � ������� F ���������� �������� ���5 � ���6
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8508);
					delay_milli(1);
					break;
			case 6:
					//����� control, ������ � ������� D ���������� �������� ���1 � ���2
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8388);
					delay_milli(1);
					//����� control, ������ � ������� E ���������� �������� ���3 � ���4
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8488);
					delay_milli(1);
					//����� control, ������ � ������� F ���������� �������� ���5 � ���6
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8588);
					delay_milli(1);
					break;
			
	}
	//����� control, ������ � ������� C ���. 5� ������,�������. ������ REFOUT, ���. ������� ������.
	SSP_SendData(adc_struct->spi_struct->SSPx, 0x82E0);
	delay_milli(1);
//	//����� ������������� �������� �������, ������ � ������� G 
//	SSP_SendData(adc_struct->spi_struct->SSPx, 0x86BF);
//	delay_milli(1);
	
	//����� control, ������ � ������� � - ������� � ����� ������
	SSP_SendData(adc_struct->spi_struct->SSPx, 0x8001);
	delay_micro(15);
	
	//������� ������ FIFO �����������
	spi_clean_fifo_rx_buf(adc_struct->spi_struct);
	
	adc_struct->init_flag = 1;
}
/*
������� ����������� ����������� ���
*/
void adc_reset(void)
{
	PORT_ResetBits(PORT_ADC_RST,PIN_ADC_RST);
	delay_milli(100);
	PORT_SetBits(PORT_ADC_RST,PIN_ADC_RST);
}


