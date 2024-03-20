/*!
 \file
 \brief Файл с реализацией API для работы с АЦП 1273ПВ19Т
*/

/*
 *	Микросхема АЦП 1273ПВ19Т подключена по интерфейсу SSI
 *	(АЦП) SDI->SSP_TX (МК)
 *	(АЦП) SDO->SSP_RX (МК)
 *	(АЦП) SDIFS(SDOFS)->CS_SSI,TIMER2_CH2 (можно любой прямой канал таймера на захват)
 *	SSI удобен тем, что обмен данными между буферами приемника и передатчика происходит самостоятельно по прибытию сигнала SDIFS(SDOFS) 
 *	на CS_SSI - так аппаратно работает SSI.
 *	В то же время, когда приходит сигнал SDIFS(SDOFS), срабатывает прерывание по захвату таймера-увеличиваем счетчик канала, обнуляем счетчик CNT,
 *	настроенный под определенный таймаут. Если следующее прерывании таймера срабатывает от переполнения CNT (превышается таймаут), то если 
 *	счетчик каналов меньше установленного кол-ва каналов АЦП - значит обнуляем счетчик каналов и следующий канал будет первым, иначе считываем буфер FIFO SSI,
 *	в котором лежат результаты всех 6-ти каналов. Так устроена синхронизация каналов.
*/

#include "math.h"
#include "1273pv19t.h"
#include "timers.h"
#include "external_ram.h"

/// Структура с конфигурационными параметрами блока SPI1 МК
extern spi_n spi_1;	
/// Структура с конфигурационными параметрами блока Timer2 МК
extern timer_n timer_2;
/// Структура с конфигурационными параметрами микросхемы АЦП
adc_n adc_1;

/*!
 *	\brief Конфигурирует выводы МК для АЦП
*/
void adc_gpio_config(void);
void adc_gpio_config(void)
{
	// Включение тактирования портов
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK|RST_CLK_PCLK_PORTD|RST_CLK_PCLK_PORTE|RST_CLK_PCLK_PORTC, ENABLE);	
	
	PORT_InitTypeDef GPIO_init_structADC;
	PORT_StructInit(&GPIO_init_structADC);
	
	// Инициализация RESET PD0 для аппаратного сброса ацп
	GPIO_init_structADC.PORT_Pin = PIN_ADC_RST;
	GPIO_init_structADC.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_structADC.PORT_OE = PORT_OE_OUT;
	GPIO_init_structADC.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_init_structADC.PORT_SPEED = PORT_SPEED_MAXFAST;
	PORT_Init(PORT_ADC_RST, &GPIO_init_structADC);
	// Установка RESET в лог единицу
	PORT_SetBits(PORT_ADC_RST,PIN_ADC_RST);
	
	// Инициализация ножек выбора режима работы (0-10В,4-20мА)
	GPIO_init_structADC.PORT_Pin = PIN_ADC_MODE_A0;
	GPIO_init_structADC.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_structADC.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_ADC_MODE, &GPIO_init_structADC);
	
	GPIO_init_structADC.PORT_Pin = PIN_ADC_MODE_A1;
	GPIO_init_structADC.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_structADC.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_ADC_MODE, &GPIO_init_structADC);
	
	// Выбор однополярного режима на мультиплексоре A0=1;A1=0	
	PORT_SetBits(PORT_ADC_MODE,PIN_ADC_MODE_A0);
	PORT_ResetBits(PORT_ADC_MODE,PIN_ADC_MODE_A1);
	
	// Инициализация вывода NSS
	GPIO_init_structADC.PORT_Pin = PIN_ADC_NSS;
	GPIO_init_structADC.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_structADC.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_ADC_NSS, &GPIO_init_structADC);
	
	// Установка NSS в лог ноль
	PORT_ResetBits(PORT_ADC_NSS, PIN_ADC_NSS);
	
	// Инициализация порта E для канала 2 таймера 2 на вход прерывания SDIFS/SDOFS от АЦП (канал захвата для таймера)
	GPIO_init_structADC.PORT_FUNC = PORT_FUNC_MAIN;
	GPIO_init_structADC.PORT_OE = PORT_OE_IN;
	GPIO_init_structADC.PORT_SPEED = PORT_SPEED_MAXFAST;
	GPIO_init_structADC.PORT_MODE = PORT_MODE_DIGITAL;
	GPIO_init_structADC.PORT_Pin = PIN_ADC_SDIFS_IRQ;
	GPIO_init_structADC.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
	PORT_Init (PORT_ADC_SDIFS_IRQ, &GPIO_init_structADC);
}
/*
	Функция инициализации АЦП
*/
void adc_init(adc_n *adc_struct)
{
	adc_gpio_config();

	// Активация микросхемы АЦП
	PORT_SetBits(PORT_ADC_NSS, PIN_ADC_NSS);
	delay_milli(1);
	
	adc_reset();
	delay_milli(1);
	
	switch (CHANEL_NUMBER)
	{
			case 1:
					// Режим control, запись в регистр D управление питанием АЦП1 и АЦП2
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8308);
					delay_milli(1);
					break;
			case 2:
					// Режим control, запись в регистр D управление питанием АЦП1 и АЦП2
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8388);
					delay_milli(1);
					break;
			case 3:
					// Режим control, запись в регистр D управление питанием АЦП1 и АЦП2
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8388);
					delay_milli(1);
					// Режим control, запись в регистр E управление питанием АЦП3 и АЦП4
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8408);
					delay_milli(1);
					break;
			case 4:
					// Режим control, запись в регистр D управление питанием АЦП1 и АЦП2
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8388);
					delay_milli(1);
					// Режим control, запись в регистр E управление питанием АЦП3 и АЦП4
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8488);
					delay_milli(1);
					break;
			case 5:
					// Режим control, запись в регистр D управление питанием АЦП1 и АЦП2
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8388);
					delay_milli(1);
					// Режим control, запись в регистр E управление питанием АЦП3 и АЦП4
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8488);
					delay_milli(1);
					// Режим control, запись в регистр F управление питанием АЦП5 и АЦП6
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8508);
					delay_milli(1);
					break;
			case 6:
					// Режим control, запись в регистр D управление питанием АЦП1 и АЦП2
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8388);
					delay_milli(1);
					// Режим control, запись в регистр E управление питанием АЦП3 и АЦП4
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8488);
					delay_milli(1);
					// Режим control, запись в регистр F управление питанием АЦП5 и АЦП6
					SSP_SendData(adc_struct->spi_struct->SSPx, 0x8588);
					delay_milli(1);
					break;
			
	}
	// Режим control, запись в регистр C вкл. 5В режима,использ. вывода REFOUT, вкл. опорное напряж.
	SSP_SendData(adc_struct->spi_struct->SSPx, 0x82E0);
	delay_milli(1);
	
	// Режим control, запись в регистр А - перевод в режим данных
	SSP_SendData(adc_struct->spi_struct->SSPx, 0x8001);
	delay_micro(10);
	
	// Очистка буфера FIFO SPI передатчика
	spi_clean_fifo_rx_buf(adc_struct->spi_struct);
	
	adc_struct->init_flag = 1;
}
/*
	Функция аппаратного перезапуска АЦП
*/
void adc_reset(void)
{
	PORT_ResetBits(PORT_ADC_RST,PIN_ADC_RST);
	delay_milli(100);
	PORT_SetBits(PORT_ADC_RST,PIN_ADC_RST);
}
