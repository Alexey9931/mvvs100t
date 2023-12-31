 /*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � ��� 1273��19�
*/

#ifndef __1273PV19T_H
#define __1273PV19T_H

#include "mdr32_drivers.h"
#include "spi.h"
#include "timers.h"

///����������� ����� ��� ����������� ������ ���������� ���
#define PORT_ADC_RST MDR_PORTD
#define PIN_ADC_RST PORT_Pin_0
///����������� ����� ��� ������ ������ ������(������������/�����������/���������������)
#define PORT_ADC_MODE MDR_PORTE
#define PIN_ADC_MODE_A0 PORT_Pin_11
#define PIN_ADC_MODE_A1 PORT_Pin_15
///����������� ����� ��� ��������� ���������� ���
#define PORT_ADC_NSS MDR_PORTD
#define PIN_ADC_NSS PORT_Pin_1
///����� �� ���� ���������� SDIFS/SDOFS �� ��� (����� ������� ��� �������)
#define PORT_ADC_SDIFS_IRQ MDR_PORTE
#define PIN_ADC_SDIFS_IRQ PORT_Pin_10

///��������� � ����������������� ����������� ���
typedef struct adc_config_data
{
	spi_n 		*spi_struct;      	///< SPI, �� �������� ��������� ���
	timer_n 	*timer_n_capture;		///< ����� ������� ��� ������ ������� ������� SDIFS/SDOFS
	uint8_t  	init_flag;					///< ���� ������������� ���
	uint16_t 	avg_num;						///< ���-�� ������� ��� ����������	
	uint8_t   ch_rx_num;					///< ���-�� �������� ������� � ����� ������ �� CHANEL_NUMBER ���������
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

#endif /*__1273PV19T_H */
