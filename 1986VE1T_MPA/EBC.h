/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � EBC
*/

#ifndef __CONFIG_H
#define __CONFIG_H

#include "MDR32_Drivers.h"

///���� ��������� ��� ����������� �� EBC
typedef enum devices
{
	EBC_RAM,		///< RAM
	EBC_ROM			///< ROM
} ebc_devices;

/*!
 *	\brief �������������� EBC ��� �������� ��� ��� ���
 *	\param device: 0-RAM, 1-ROM 
*/
void ebc_init(ebc_devices device);

#endif /*__CONFIG_H */
