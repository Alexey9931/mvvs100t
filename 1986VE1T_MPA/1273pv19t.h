/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � ��� 1273��19�
*/

#ifndef __1273PV19T_H
#define __1273PV19T_H

#include "MDR32_Drivers.h"

/*!
 *	\brief �������������� ���������� ��� 1273��19�
*/
void adc_init(void);
/*!
 *	\brief ���������� ����� ���������� ��� 1273��19�
*/
void adc_reset(void);
/*!
 *	\brief ������ ������ ���������� ��� 1273��19�
*/
float adc_read(void);

#endif /*__1273PV19T_H */