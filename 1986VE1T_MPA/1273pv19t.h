/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � ��� 1273��19�
*/

#ifndef __1273PV19T_H
#define __1273PV19T_H

#include "MDR32_Drivers.h"

///����������� ����� ��� ����������� ������ ���������� ���
#define PORT_ADC_RST MDR_PORTD
#define PIN_ADC_RST PORT_Pin_9
///����������� ����� ��� ������ ������ ������(������������/�����������/���������������)
#define PORT_ADC_MODE MDR_PORTE
#define PIN_ADC_MODE_A0 PORT_Pin_11
#define PIN_ADC_MODE_A1 PORT_Pin_15

///����-�� ������� ���
#define ADC_CH_NUMBER 6

/*!
 *	\brief ������������� ������ �� ��� ���� ���
*/
void adc_gpio_config(void);

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