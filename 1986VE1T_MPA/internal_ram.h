/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � �������� ������ ����������� ���
*/

#ifndef __INTERNAL_RAM_H
#define __INTERNAL_RAM_H

#include "mdr32_drivers.h"

#define PAGE_NUM 	256			///< ��������-�� ������� ������ ��� ����
#define PAGE_SIZE 64			///< ����������� �������� ������ ��� ����

///���������� ����
typedef struct heap_struct
{
	uint8_t                 memory_page_status[PAGE_NUM];						///< ������ ������ �������� 0-��������, 1-������
	uint8_t 								memory_page_space[PAGE_NUM][PAGE_SIZE];	///< ���������� ������ ��� ���� �������� �� 32 �������� �� 64 �����
}__attribute__((packed)) int_ram_heap;

/*!
 *	\brief �������� ��������� ������ �� ���������� ��� (�������� ������) - ���������� ��� ���������� ����
 *	\param size - ������ ���������� ������ 
 *	\return ��������� �� ������ ���������� ������ ��� 0, ���� ��� ���������� �����
*/
uint8_t* malloc_ram_pages(uint32_t size);

/*!
 *	\brief ����������� ������ �� ���������� ��� (�������� ������) - ���������� ��� ���������� ����
 *	\param page_addr - ����� ��������� ��������
 *	\param size - ������ ������������� ������ 
*/
void free_ram_pages(uint8_t *page_addr, uint32_t size);

#endif /*__INTERNAL_RAM_H */
