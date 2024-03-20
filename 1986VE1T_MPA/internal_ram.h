/*!
 \file
 \brief Заголовочный файл с описанием API для работы с областью памяти внутреннего ОЗУ
*/

#ifndef __INTERNAL_RAM_H
#define __INTERNAL_RAM_H

#include "mdr32_drivers.h"

#define PAGE_NUM 	256			///< Кол-во страниц памяти для кучи
#define PAGE_SIZE 64			///< Размер страницы памяти для кучи

/**
 * @brief Реализация "самодельной" кучи
 *
 */
typedef struct heap_struct
{
	uint8_t                 memory_page_status[PAGE_NUM];						///< Статус каждой страница 0-свободна, 1-занята
	uint8_t 								memory_page_space[PAGE_NUM][PAGE_SIZE];	///< Место памяти для кучи разбитой на 32 страницы по 64 байта
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
int_ram_heap;

/*!
 *	\brief Выделяет свободную память во внутреннем ОЗУ (страницы памяти) - необходима для реализации кучи
 *	\param size - размер выделяемой памяти 
 *	\return Указатель на начало выделяемой памяти или 0, если нет свободного места
*/
uint8_t* malloc_ram_pages(uint32_t size);

/*!
 *	\brief Освобождает память во внутреннем ОЗУ (страницы памяти) - необходима для реализации кучи
 *	\param page_addr - адрес стартовой страницы
 *	\param size - размер освобождаемой памяти 
*/
void free_ram_pages(uint8_t *page_addr, uint32_t size);

#endif /*__INTERNAL_RAM_H */
