/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � ���������
*/

#ifndef __TIMER_H
#define __TIMER_H

#include "MDR32_Drivers.h"
#include <stdio.h>

///����� �� ���� ���������� SDIFS/SDOFS �� ��� (����� ������� ��� �������)
#define PORT_ADC_SDIFS_IRQ MDR_PORTE
#define PIN_ADC_SDIFS_IRQ PORT_Pin_8

/*!
 *	\brief ������������� ������ �� ��� ������� ��������
*/
void timer_gpio_config(void);

/*!
 *	\brief ���������������� Timer1
*/
void timer1_init(void);

/*!
 *	\brief ���������������� Timer3
*/
void timer3_init(void);

/*!
 *	\brief ���������������� Timer2
*/
void timer2_init(void);

/*!
 *	\brief ��������� �������� � ��
 *	\param time_milli - �������� � ��
*/
void delay_milli(uint32_t time_milli);

/*!
 *	\brief ��������� �������� � ���
 *	\param time_micro - �������� � ���
*/
void delay_micro(uint32_t time_micro);


#endif /*__TIMER_H */