/*!
 \file
 \brief Заголовочный файл с описанием API для работы с EBC
*/

#ifndef __CONFIG_H
#define __CONFIG_H

#include "MDR32_Drivers.h"

///Типы устройств для подключение по EBC
typedef enum devices
{
	EBC_RAM,		///< RAM
	EBC_ROM			///< ROM
} ebc_devices;

/*!
 *	\brief Инициализирует EBC для внешнего ОЗУ или ПЗУ
 *	\param device: 0-RAM, 1-ROM 
*/
void ebc_init(ebc_devices device);

#endif /*__CONFIG_H */
