/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ �� �������� (���������� ���������� linux kernel)
*/

#ifndef __LIST_H
#define __LIST_H

#include "mdr32_drivers.h"
#include <stddef.h>
#include <stdio.h>

/// ������ ��� ������ ����������� ������ (����� ����������� �������)
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))
			 
/// ������ ��� ������ ������������� ������ (head - ��������� �� ������, pos - ������, ������������ ��� ������)
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/// ������ ���������� ����� ���������, �� ���������� ������ ����� ���� ��������� (ptr-��������� �� ���� ���������, type-��� ���������, member-��� ����� ���������)
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)
	
/// ������ ������������� ����������� ������
#define LIST_HEAD_INIT(name) { &(name), &(name) }

/// ������ �������� ����������� ������ � ������ name
#define LIST_HEAD(name) \
	list_head name = LIST_HEAD_INIT(name)

///��������� � ��������� ������������� ������
typedef struct list_head_struct 
{
	struct list_head *next;
	struct list_head *prev;
} __attribute__((packed)) list_head;

/*!
 *	\brief �������������� ������������ ������
 *	\param list - ��������� �� ��������� ������
*/
void init_list_head(list_head *list);

/*!
 *	\brief ��������� ������� � ��������� ������ (����� ����������� � ���������� ����������)
 *	\param new - ��������� �� ����� ������� ������
 *	\param prev - ��������� �� ���������� ������� ������
 *	\param next - ��������� �� ��������� ������� ������
*/
void __list_add(list_head *new, list_head *prev, list_head *next);

/*!
 *	\brief ��������� ������� � ��������� ���������� ������
 *	\param new - ��������� �� ����� ������� ������
 *	\param head - ��������� �� ��������� ������
*/
void list_add(list_head *new, list_head *head);

/*!
 *	\brief ��������� ������� � ����� ���������� ������
 *	\param new - ��������� �� ����� ������� ������
 *	\param head - ��������� �� ��������� ������
*/
void list_add_tail(list_head *new, list_head *head);

/*!
 *	\brief ������� ������� ������
 *	\param prev - ��������� �� ���������� �������
 *	\param next - ��������� �� ��������� �������
*/
void __list_del(list_head * prev, list_head * next);

/*!
 *	\brief ������� ������� ������ � ������� ���
 *	\param entry - ��������� �� ��������� �������
*/
void list_del(list_head *entry);

/*!
 *	\brief ����������, �������� �� ������� ���� ������ ��������� ����� � ������ 
 *	\param list - ������� ������� ������
 *	\param head - ��������� �� ��������� ������
 *  \return ��������� � ����������� (1 - ��������, 0 - �� ��������)
*/
int list_is_last(list_head *list, list_head *head);

/*!
 *	\brief ����������, �������� �� ��������� ������ ������
 *	\param head - ��������� �� ��������� ������
 *  \return ��������� � ����������� (1 - ������ ����, 0 - �� ����)
*/
int list_empty(list_head *head);
	


#endif /*__LIST_H */
