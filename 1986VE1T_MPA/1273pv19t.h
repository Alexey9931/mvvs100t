/*!
 \file
 \brief Заголовочный файл с описанием API для работы с АЦП 1273ПВ19Т
*/

#ifndef __1273PV19T_H
#define __1273PV19T_H

#include "MDR32_Drivers.h"
#include "SPI.h"
#include "TIMER.h"

///управляющая ножка для аппаратного сброса микросхемы АЦП
#define PORT_ADC_RST MDR_PORTD
#define PIN_ADC_RST PORT_Pin_0
///управляющая ножка для выбора режима работы(однополярный/двуполярный/самодиагностика)
#define PORT_ADC_MODE MDR_PORTE
#define PIN_ADC_MODE_A0 PORT_Pin_11
#define PIN_ADC_MODE_A1 PORT_Pin_15
///управляющая ножка для активации микросхемы АЦП
#define PORT_ADC_NSS MDR_PORTD
#define PIN_ADC_NSS PORT_Pin_1
///ножка на вход прерывания SDIFS/SDOFS от АЦП (канал захвата для таймера)
#define PORT_ADC_SDIFS_IRQ MDR_PORTE
#define PIN_ADC_SDIFS_IRQ PORT_Pin_8

///Структура с конфигурационными параметрами АЦП
typedef struct adc_config_data
{
	spi_n 		*spi_struct;      	///< SPI, по которому подключен АЦП
	timer_n 	*timer_n_capture;		///< Выбор таймера для режима захвата сигнала SDIFS/SDOFS
	timer_n 	*timer_n_sample;		///< Выбор таймера для измерения задержки между пакетами с каналами
	uint8_t  	init_flag;					///< Флаг инициализации АЦП
	uint16_t 	avg_num;						///< Кол-во выборок для усреднения
	uint32_t	sample_timer_cnt;		///< Счетчик задержки между пакетами с каналами		
	uint8_t   last_ch_rx;					///< Крайний номер канала в последнем принятом пакете(нумерация начинается с 1)	
	uint8_t   pre_last_ch_rx;			///< Крайний номер канала в предпоследнем принятом пакете(нумерация начинается с 1)	
} adc_n;	

/*!
 *	\brief Инициализирует микросхему АЦП 1273ПВ19Т
 *	\param *SPI_struct - Структура с конфиг. параметрами SPI, по которому подлючен АЦП
*/
void adc_init(adc_n *adc_struct);

/*!
 *	\brief Аппаратный сброс микросхемы АЦП 1273ПВ19Т
*/
void adc_reset(void);

#endif /*__1273PV19T_H */
