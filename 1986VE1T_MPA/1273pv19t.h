/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � ��� 1273��19�
*/

#ifndef __1273PV19T_H
#define __1273PV19T_H

#include "MDR32_Drivers.h"
#include "SPI.h"
#include "TIMER.h"

///����������� ����� ��� ����������� ������ ���������� ���
#define PORT_ADC_RST MDR_PORTD
#define PIN_ADC_RST PORT_Pin_0
///����������� ����� ��� ������ ������ ������(������������/�����������/���������������)
#define PORT_ADC_MODE MDR_PORTE
#define PIN_ADC_MODE_A0 PORT_Pin_11
#define PIN_ADC_MODE_A1 PORT_Pin_15

///��������� � ����������������� ����������� ���
typedef struct adc_config_data
{
	spi_n 		*spi_struct;      	///< SPI, �� �������� ��������� ���
	timer_n 	*timer_n_timeout;		///< ����� ������� ��� ������������ ���������
	timer_n 	*timer_n_capture;		///< ����� ������� ��� ������ ������� ������� SDIFS/SDOFS
	uint32_t	read_timeout;				///< ������� �� ������ ������ ���
	uint8_t 	timeout_flag;  ///< ���� �������� �� ������
} adc_n;

/*!
 *	\brief �������������� ���������� ��� 1273��19�
 *	\param *SPI_struct - ��������� � ������. ����������� SPI, �� �������� �������� ���
*/
void adc_init(adc_n *adc_struct);

/*!
 *	\brief ���������� ����� ���������� ��� 1273��19�
*/
void adc_reset(void);

/*!
 *	\brief ������ ������ ���������� ��� 1273��19�
 *	\param *SPI_struct - ��������� � ������. ����������� SPI, �� �������� �������� ���
*/
int16_t adc_read(adc_n *adc_struct);

/*!
 *	\brief ��������� �������� (� ���) �� ������ ������ ���������� ��� 1273��19�
*/
void adc_set_read_timeout(adc_n *adc_struct, uint32_t read_timeout);

#endif /*__1273PV19T_H */
