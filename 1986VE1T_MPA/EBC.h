/*!
 \file
 \brief Заголовочный файл с описанием API для работы с EBC
*/


#ifndef __CONFIG_H
#define __CONFIG_H

#include "MDR32_Drivers.h"

/*!
 *	\brief Конфигурирует выводы МК для EBC
*/
void ebc_gpio_config(void);

/*!
 *	\brief Инициализирует EBC
*/
void ebc_init(void);

#endif /*__CONFIG_H */
