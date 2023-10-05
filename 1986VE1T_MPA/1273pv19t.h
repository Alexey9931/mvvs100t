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

///Структура с конфигурационными параметрами АЦП
typedef struct adc_config_data
{
	spi_n 		*spi_struct;      	///< SPI, по которому подключен АЦП
	timer_n 	*timer_n_timeout;		///< Выбор таймера для отслеживания таймаутов
	timer_n 	*timer_n_capture;		///< Выбор таймера для режима захвата сигнала SDIFS/SDOFS
	uint32_t	read_timeout;				///< Таймаут на чтение канала АЦП
	uint8_t 	timeout_flag;  ///< Флаг таймаута на чтение
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

/*!
 *	\brief Чтение канала микросхемы АЦП 1273ПВ19Т
 *	\param *SPI_struct - Структура с конфиг. параметрами SPI, по которому подлючен АЦП
*/
int16_t adc_read(adc_n *adc_struct);

/*!
 *	\brief Установка таймаута (в мкс) на чтение канала микросхемы АЦП 1273ПВ19Т
*/
void adc_set_read_timeout(adc_n *adc_struct, uint32_t read_timeout);

#endif /*__1273PV19T_H */
