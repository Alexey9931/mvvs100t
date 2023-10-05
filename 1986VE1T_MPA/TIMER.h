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

///��������� � ����������������� ����������� �������� 
typedef struct timer_config_struct
{
	MDR_TIMER_TypeDef* 						TIMERx;            							///< ������������ ��������� � ������������� ���������� ����� TIMER
	TIMER_CntInitTypeDef 					TIMERInitStruct;            		///< ������������ ��������� � ����������������� ����������� ����� TIMER
	TIMER_ChnInitTypeDef 					sTIM_ChnInit;										///< ������������ ��������� � ����������������� ����������� ������� ����� TIMER (������������ �������� ��� ������ �������)
	uint32_t 											RST_CLK_PCLK_TIMERn;						///< ��������� ������������ ��� ����� TIMER
	TIMER_Clock_BRG_TypeDef 			TIMER_HCLKdiv;									///< ����� �������� �������� ������� ��� ������������ ����� TIMER	
	IRQn_Type 										IRQn;														///< ����� ����������� ���������� ����� TIMER
	TIMER_Status_Flags_TypeDef		TIMER_STATUS;										///< �������� �������, �� �������� ���������� ���������� ����� TIMER	
} timer_n;	


/*!
 *	\brief ���������������� ��������� Timer
 *	\param timer - ������ ��� �������������
*/
void timer_init(timer_n *timer_struct);

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