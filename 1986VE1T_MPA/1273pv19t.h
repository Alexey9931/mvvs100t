/*!
 \file
 \brief Заголовочный файл с описанием API для работы с АЦП 1273ПВ19Т
*/

#ifndef __1273PV19T_H
#define __1273PV19T_H

#include "MDR32_Drivers.h"

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