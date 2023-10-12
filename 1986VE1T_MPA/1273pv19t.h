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

///Структура с конфигурационными параметрами АЦП
typedef struct adc_config_data
{
	spi_n 		*spi_struct;      	///< SPI, по которому подключен АЦП
	timer_n 	*timer_n_capture;		///< Выбор таймера для режима захвата сигнала SDIFS/SDOFS
	uint8_t  	init_flag;					///< Флаг инициализации АЦП
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
