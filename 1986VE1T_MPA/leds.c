/*!
 \file
 \brief ���� � ����������� API ��� ������ �� ������������ ������������
*/

#include "leds.h"

/*
������� ���������������� ������� �� ��� �����������-�����������
*/

void leds_gpio_config(void)
{
	// ��������� ������������ ������
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK|CLOCK_LEDS, ENABLE);	
	
	PORT_InitTypeDef gpio_init_struct_leds;
	PORT_StructInit(&gpio_init_struct_leds);
	
	//������������� ����������-���������� ���������� ������
	gpio_init_struct_leds.PORT_Pin = PIN_LED_OK_WORK;
	gpio_init_struct_leds.PORT_FUNC = PORT_FUNC_PORT;
	gpio_init_struct_leds.PORT_OE = PORT_OE_OUT;
	gpio_init_struct_leds.PORT_MODE = PORT_MODE_DIGITAL;
	gpio_init_struct_leds.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(PORT_LEDS, &gpio_init_struct_leds);
	
	//������������� ����������-���������� �������������
	gpio_init_struct_leds.PORT_Pin = PIN_LED_ERROR_WORK;
	gpio_init_struct_leds.PORT_FUNC = PORT_FUNC_PORT;
	gpio_init_struct_leds.PORT_OE = PORT_OE_OUT;
	gpio_init_struct_leds.PORT_MODE = PORT_MODE_DIGITAL;
	gpio_init_struct_leds.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(PORT_LEDS, &gpio_init_struct_leds);
}
