/*!
 \file
 \brief ���� � ����������� API ��� ������ �� �������� (���������� ���������� linux kernel)
*/

#include "list.h"

// ������� ������������� ����������� ������
void init_list_head(list_head *list)
{
	list->next = list;
	list->prev = list;
}

// ������� ������� �������� � ��������� ������ (����� ����������� � ���������� ����������)
void __list_add(list_head *new, list_head *prev, list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}
 
// ������� ������� �������� ����� ����� head
void list_add(list_head *new, list_head *head)
{
	__list_add(new, head, head->next);
}
 
// ������� ������� �������� ����� head (�.�. � ����� ����)
void list_add_tail(list_head *new, list_head *head)
{
	__list_add(new, head->prev, head);
}

// ������� �������� �������� ������
void __list_del(list_head * prev, list_head * next)
{
  next->prev = prev;
  prev->next = next;
}
 
// ������� �������� �������� ������ � ��� ������� 
void list_del(list_head *entry)
{
  __list_del(entry->prev, entry->next);
  entry->next = NULL;
  entry->prev = NULL;
}

// �������, ������������, �������� �� ������� ���� ������ ��������� ����� � ������ 
int list_is_last(list_head *list, list_head *head)
{
	return list->next == head;
}

// �������, ������������, �������� �� ��������� ������ ������, ���������� true, ���� ����, � ��������� ������ ���������� false 
int list_empty(list_head *head)
{
	return head->next == head;
}