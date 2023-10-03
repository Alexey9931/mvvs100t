/*!
 \file
 \brief Заголовочный файл с описанием API для работы с АЦП 1273ПВ19Т
*/

#ifndef __1273PV19T_H
#define __1273PV19T_H

#include "MDR32_Drivers.h"

///управляющая ножка для аппаратного сброса микросхемы АЦП
#define PORT_ADC_RST MDR_PORTD
#define PIN_ADC_RST PORT_Pin_9
///управляющая ножка для выбора режима работы(однополярный/двуполярный/самодиагностика)
#define PORT_ADC_MODE MDR_PORTE
#define PIN_ADC_MODE_A0 PORT_Pin_11
#define PIN_ADC_MODE_A1 PORT_Pin_15

///колв-во каналов АЦП
#define ADC_CH_NUMBER 6

/*!
 *	\brief Конфигурирует выводы МК для нужд АЦП
*/
void adc_gpio_config(void);

/*!
 *	\brief Инициализирует микросхему АЦП 1273ПВ19Т
*/
void adc_init(void);

/*!
 *	\brief Аппаратный сброс микросхемы АЦП 1273ПВ19Т
*/
void adc_reset(void);

/*!
 *	\brief Чтение канала микросхемы АЦП 1273ПВ19Т
*/
float adc_read(void);

#endif /*__1273PV19T_H */