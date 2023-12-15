/*!
 \file
 \brief Файл с реализацией API для работы со списками (реализация аналогична linux kernel)
*/

#include "list.h"

// Функция инициализации двусвязного списка
void init_list_head(list_head *list)
{
	list->next = list;
	list->prev = list;
}

// Функция вставки элемента в связанный список (между предыдущими и следующими элементами)
void __list_add(list_head *new, list_head *prev, list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}
 
// Функция вставки элемента сразу после head
void list_add(list_head *new, list_head *head)
{
	__list_add(new, head, head->next);
}
 
// Функция вставки элемента перед head (т.е. в хвост узла)
void list_add_tail(list_head *new, list_head *head)
{
	__list_add(new, head->prev, head);
}

// Функция удаления элемента списка
void __list_del(list_head * prev, list_head * next)
{
  next->prev = prev;
  prev->next = next;
}
 
// Функция удаления элемента списка и его очистки 
void list_del(list_head *entry)
{
  __list_del(entry->prev, entry->next);
  entry->next = NULL;
  entry->prev = NULL;
}

// Функция, определяющая, является ли текущий узел списка последним узлом в списке 
int list_is_last(list_head *list, list_head *head)
{
	return list->next == head;
}

// Функция, определяющая, является ли связанный список пустым, возвращает true, если пуст, в противном случае возвращает false 
int list_empty(list_head *head)
{
	return head->next == head;
}