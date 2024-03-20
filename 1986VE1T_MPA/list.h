/*!
 \file
 \brief Заголовочный файл с описанием API для работы со списками (реализация аналогична linux kernel)
*/

#ifndef __LIST_H
#define __LIST_H

#include "mdr32_drivers.h"
#include <stddef.h>
#include <stdio.h>

/// Макрос для обхода двусвязного списка (более оптимальный вариант)
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))
			 
/// Макрос для обхода двусвязанного списка (head - указывает на список, pos - курсор, используемый для обхода)
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/// Макрос определяет адрес структуры по известному адресу члена этой структуры (ptr-указатель на член структуры, type-тип структуры, member-имя члена структуры)
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

/// Макрос обертка container_of
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)
	
/// Макрос инициализации двусвязного списка
#define LIST_HEAD_INIT(name) { &(name), &(name) }

/// Макрос создания двусвязного списка с именем name
#define LIST_HEAD(name) \
	list_head name = LIST_HEAD_INIT(name)

/**
 * @brief Структура с описанием двусвязанного списка
 *
 */
typedef struct list_head_struct 
{
	struct list_head *next;				///< Указатель на следующий элемент списка
	struct list_head *prev;				///< Указатель на предыдущий элемент списка
} 
#ifndef DOXYGEN
__attribute__((packed))
#endif
list_head;

/*!
 *	\brief Инициализирует двусвязанный список
 *	\param list - указатель на заголовок списка
*/
void init_list_head(list_head *list);

/*!
 *	\brief Вставляет элемент в связанный список (между предыдущими и следующими элементами)
 *	\param new - указатель на новый элемент списка
 *	\param prev - указатель на предыдущий элемент списка
 *	\param next - указатель на следующий элемент списка
*/
void __list_add(list_head *new, list_head *prev, list_head *next);

/*!
 *	\brief Вставляет элемент в заголовок связанного списка
 *	\param new - указатель на новый элемент списка
 *	\param head - указатель на заголовок списка
*/
void list_add(list_head *new, list_head *head);

/*!
 *	\brief Вставляет элемент в конец связанного списка
 *	\param new - указатель на новый элемент списка
 *	\param head - указатель на заголовок списка
*/
void list_add_tail(list_head *new, list_head *head);

/*!
 *	\brief Удаляет элемент списка
 *	\param prev - указатель на предыдущий элемент
 *	\param next - указатель на следующий элемент
*/
void __list_del(list_head * prev, list_head * next);

/*!
 *	\brief Удаляет элемент списка и очищает его
 *	\param entry - указатель на удаляемый элемент
*/
void list_del(list_head *entry);

/*!
 *	\brief Определяет, является ли текущий узел списка последним узлом в списке 
 *	\param list - текущий элемент списка
 *	\param head - указатель на заголовок списка
 *  \return Сообщение с результатом (1 - является, 0 - не является)
*/
int list_is_last(list_head *list, list_head *head);

/*!
 *	\brief Определяет, является ли связанный список пустым
 *	\param head - указатель на заголовок списка
 *  \return Сообщение с результатом (1 - список пуст, 0 - не пуст)
*/
int list_empty(list_head *head);

#endif /*__LIST_H */
