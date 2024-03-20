 /*!
 \file
 \brief Заголовочный файл с описанием API для работы с микросхемой АЦП 1273ПВ19Т
*/

#ifndef __1273PV19T_H
#define __1273PV19T_H

#include "mdr32_drivers.h"
#include "spi.h"
#include "timers.h"


#define PORT_ADC_RST MDR_PORTD					///< Порт линии аппаратного сброса микросхемы АЦП
#define PIN_ADC_RST PORT_Pin_0					///< Вывод линии аппаратного сброса микросхемы АЦП

#define PORT_ADC_MODE MDR_PORTE					///< Порт линии выбора режима работы АЦП
#define PIN_ADC_MODE_A0 PORT_Pin_11			///< Вывод линии выбора режима работы АЦП A0
#define PIN_ADC_MODE_A1 PORT_Pin_15			///< Вывод линии выбора режима работы АЦП A1

#define PORT_ADC_NSS MDR_PORTD					///< Порт линии активации микросхемы АЦП
#define PIN_ADC_NSS PORT_Pin_1					///< Вывод линии активации микросхемы АЦП

#define PORT_ADC_SDIFS_IRQ MDR_PORTE		///< Порт линии входа прерывания SDIFS/SDOFS от АЦП (канал захвата для таймера)
#define PIN_ADC_SDIFS_IRQ PORT_Pin_10		///< Вывод линии входа прерывания SDIFS/SDOFS от АЦП (канал захвата для таймера)

/**
 * @brief Структура с конфигурационными параметрами АЦП
 *
 */
typedef struct adc_config_data
{
	spi_n 		*spi_struct;      	///< SPI, по которому подключен АЦП
	timer_n 	*timer_n_capture;		///< Выбор таймера для режима захвата сигнала SDIFS/SDOFS
	uint8_t  	init_flag;					///< Флаг инициализации АЦП
	uint16_t 	avg_num;						///< Кол-во выборок для усреднения	
	uint8_t   ch_rx_num;					///< Кол-во принятых каналов в одном пакете из CHANEL_NUMBER возможных
} 
#ifndef DOXYGEN
__attribute__((packed))
#endif
adc_n;	

/*!
 *	\brief Инициализирует микросхему АЦП 1273ПВ19Т
 *	\param *adc_struct - Структура с конфиг. параметрами АЦП
*/
void adc_init(adc_n *adc_struct);

/*!
 *	\brief Аппаратный сброс микросхемы АЦП 1273ПВ19Т
*/
void adc_reset(void);

#endif /*__1273PV19T_H */
