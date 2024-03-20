/*!
 \file
 \brief Заголовочный файл с описанием API для работы с EBC (контроллер внешней системной шины)
*/

#ifndef __EBC_H
#define __EBC_H

#include "mdr32_drivers.h"

/**
 * @brief Типы устройств для подключение по EBC
 *
 */
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

#endif /*__EBC_H */
 