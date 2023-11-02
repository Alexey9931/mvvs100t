/*!
 \file
 \brief Файл с реализацией API для работы со светоиодными индикаторами
*/

#include "leds.h"

/*
Функция конфигурирования выводов МК для светодиодов-индикаторов
*/

void leds_gpio_config(void)
{
	// Включение тактирования портов
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK|CLOCK_LEDS, ENABLE);	
	
	PORT_InitTypeDef gpio_init_struct_leds;
	PORT_StructInit(&gpio_init_struct_leds);
	
	//инициализация светодиода-индикатора корректной работы
	gpio_init_struct_leds.PORT_Pin = PIN_LED_OK_WORK;
	gpio_init_struct_leds.PORT_FUNC = PORT_FUNC_PORT;
	gpio_init_struct_leds.PORT_OE = PORT_OE_OUT;
	gpio_init_struct_leds.PORT_MODE = PORT_MODE_DIGITAL;
	gpio_init_struct_leds.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(PORT_LEDS, &gpio_init_struct_leds);
	
	//инициализация светодиода-индикатора неисправности
	gpio_init_struct_leds.PORT_Pin = PIN_LED_ERROR_WORK;
	gpio_init_struct_leds.PORT_FUNC = PORT_FUNC_PORT;
	gpio_init_struct_leds.PORT_OE = PORT_OE_OUT;
	gpio_init_struct_leds.PORT_MODE = PORT_MODE_DIGITAL;
	gpio_init_struct_leds.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(PORT_LEDS, &gpio_init_struct_leds);
}
