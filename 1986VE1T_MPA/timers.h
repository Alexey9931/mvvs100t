/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � ���������
*/

#ifndef __TIMERS_H
#define __TIMERS_H

#include "mdr32_drivers.h"
#include <stdio.h>
#include "list.h"

///��������� � ����������������� ����������� �������� 
typedef struct timer_config_struct
{
	MDR_TIMER_TypeDef* 						TIMERx;            							///< ������������ ��������� � ������������� ���������� ����� TIMER
	TIMER_CntInitTypeDef 					TIMERInitStruct;            		///< ������������ ��������� � ����������������� ����������� ����� TIMER
	TIMER_ChnInitTypeDef 					sTIM_ChnInit;										///< ������������ ��������� � ����������������� ����������� ������� ����� TIMER (������������ �������� ��� ������ �������)
	TIMER_Status_Flags_TypeDef		TIMER_STATUS;										///< �������� �������, �� �������� ���������� ���������� ����� TIMER	
	uint32_t											timer_cnt;											///< ������� ��� TIMER (����� ���� ����������� ��� ������ �����)	
} __attribute__((packed)) timer_n;	


///���������-���������� �������������� ������ ������������ ���������� ��������
typedef struct timer_irq_list_struct
{
	void(*handler)(void*);						///< ��������� �� �������-���������� ����������			
	TIMER_Status_Flags_TypeDef event;	///< �������, �� �������� ����������� ����������
	void *data;												///< ��������� �� ������, ������������ � ���������� (��� �������������)
	list_head list;										///< ������ ����������
} __attribute__((packed)) timer_irq_list;

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

/*!
 *	\brief ��������� ���������� ���������� ������� � ������ ������������
 *	\param tmr_num - ����� �������
 *	\param func_ptr - ��������� �� �������-���������� ����������
 *	\param data - ��������� �� ������ � ����������� ����������
 *	\param event - �������, �� �������� ���������� ������ ����������
*/
void list_tmr_handler_add_tail(uint8_t tmr_num, void (*func_ptr)(void*), void *data, TIMER_Status_Flags_TypeDef event);

/*!
 *	\brief �������������� ������ ������������ ���������� �������
 *	\param tmr_num - ����� �������, ��� �������� ���������������� ������
*/
void list_tmr_handler_init(uint8_t tmr_num);

#endif /*__TIMERS_H */
