/*!
 \file
 \brief ���� � ����������� API ��� ������ � ��� 1273��19�
*/

#include "math.h"
#include "1273pv19t.h"
#include "TIMER.h"
#include "external_ram.h"

adc_n adc_1;

/*
������� ���������������� ������� �� ��� ���
*/
void adc_gpio_config(void);
void adc_gpio_config(void)
{
	// ��������� ������������ ������
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK|RST_CLK_PCLK_PORTD|RST_CLK_PCLK_PORTE, ENABLE);	
	
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
	
	PORT_SetBits(PORT_ADC_MODE,PIN_ADC_MODE_A0);
	PORT_ResetBits(PORT_ADC_MODE,PIN_ADC_MODE_A1);
	
}
/*
������� ������������� ���
*/
void adc_init(adc_n *adc_struct)
{
	PORT_SetBits(PORT_SSP1,PIN_SSP1_SS);
	adc_gpio_config();
	delay_milli(1);
	
	adc_reset();
	delay_milli(1);
	
//	//����� control, ������ � ������� B ��� ��������� ������� SCK �� 16���
//	SSP_SendData(SPI_struct->SSPx, 0x810C);
//	delay_milli(1);
//	//��������� ������� SCK SPI
//	SPI_struct->SPI.SSP_CPSDVSR = 8;
//	SSP_Init(SPI_struct->SSPx,&SPI_struct->SPI);
	
	//����� control, ������ � ������� D ���������� �������� ���1 � ���2
	SSP_SendData(adc_struct->spi_struct->SSPx, 0x8388);
	delay_milli(1);
	//����� control, ������ � ������� E ���������� �������� ���3 � ���4
	SSP_SendData(adc_struct->spi_struct->SSPx, 0x8488);
	delay_milli(1);
	//����� control, ������ � ������� F ���������� �������� ���5 � ���6
	SSP_SendData(adc_struct->spi_struct->SSPx, 0x8588);
	delay_milli(1);
	//����� control, ������ � ������� C ���. 5� ������,�������. ������ REFOUT, ���. ������� ������.
	SSP_SendData(adc_struct->spi_struct->SSPx, 0x82E0);
	delay_milli(1);
	//����� control, ������ � ������� � - ������� � ����� ������
	SSP_SendData(adc_struct->spi_struct->SSPx, 0x8001);
	delay_milli(1);
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
/*
������� ������ ������ ���
*/
int16_t adc_read(adc_n *adc_struct)
{
	int16_t value = 0;
	uint32_t timer_cnt = 0;
	
	if (adc_struct->timeout_flag == 1)
	{
		TIMER_SetCounter(adc_struct->timer_n_timeout->TIMERx, 0);
	}
	//���������� ������ ������ ��� ������� � ����� ����������� SPI 0x7FFF
	SSP_SendData(adc_struct->spi_struct->SSPx, 0x7FFF);
	//����, ���� ������ SDIFS/SDOFS
	while(((adc_struct->timer_n_capture->TIMERx->STATUS)&(adc_struct->timer_n_capture->TIMER_STATUS)) == 0) 
	{
		if (adc_struct->timeout_flag == 1)
		{
			//���� ����� �������, �� �������
			timer_cnt = TIMER_GetCounter(adc_struct->timer_n_timeout->TIMERx);
			if (timer_cnt==(adc_struct->read_timeout)) 
			{ 
				return value; 
			}
	}
	}
	//delay_micro(8);
 	value = ~(adc_struct->spi_struct->fifo_halfword) + 1;
	
	return value;
}
/*
������� ��������� �������� �� ������ ������ ���
*/
void adc_set_read_timeout(adc_n *adc_struct, uint32_t read_timeout)
{
	adc_struct->read_timeout = read_timeout;
	adc_struct->timeout_flag = 1;
}
