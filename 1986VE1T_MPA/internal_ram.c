/*!
 \file
 \brief Файл с реализацией API для работы с областью памяти внутреннего ОЗУ
*/

#include "internal_ram.h"

extern int_ram_heap *heap_ptr;

/*!
	Функция выделения свободной памяти во внешнем ОЗУ (страниц памяти)
*/
uint8_t* malloc_ram_pages(uint32_t size)
{
	uint32_t page_num = size/PAGE_SIZE + 1; //кол-во выделяемых страниц
	uint32_t page_cnt; //счетчик страниц
	uint32_t consecutive_page_cnt; //счетчик подряд идущих пустых страниц
	 
	for (page_cnt = 0; (page_cnt + page_num) < PAGE_SIZE; page_cnt++)
	{
		//проверяем что подряд идут пустые страницы памяти
		for (consecutive_page_cnt = 0; consecutive_page_cnt < page_num; consecutive_page_cnt++)
		{
			if (heap_ptr->memory_page_status[page_cnt + consecutive_page_cnt] != 0)
			{
				page_cnt += consecutive_page_cnt;
				break;
			}
		}
		//если удалось найти требуемое кол-во подряд идущих пустых страницы, то возвращаяем указатель
		if (consecutive_page_cnt == page_num)	
		{
			//обновляем статус страниц
			for (consecutive_page_cnt = 0; consecutive_page_cnt < page_num; consecutive_page_cnt++)
			{
				memset(&heap_ptr->memory_page_status[page_cnt + consecutive_page_cnt], 1, sizeof(heap_ptr->memory_page_status[page_cnt + consecutive_page_cnt]));
			}
			return &heap_ptr->memory_page_space[page_cnt][0];
		}
	}
	
	return 0;
}
/*!
	Функция освобождения памяти во внешнем ОЗУ (страниц памяти)
*/
void free_ram_pages(uint8_t *page_addr, uint32_t size)
{
	uint32_t page_num = size/PAGE_SIZE + 1; //кол-во освобождаемых страниц
	uint32_t page_cnt; //счетчик страниц
	 
	for (page_cnt = 0; (page_cnt + page_num) < PAGE_SIZE; page_cnt++)
	{
		if (page_addr == &heap_ptr->memory_page_space[page_cnt][PAGE_SIZE])
		{
			//обновляем статус страниц
			for (uint32_t i = 0; i < page_num; i++)
			{
				memset(&heap_ptr->memory_page_status[page_cnt + i], 0, sizeof(heap_ptr->memory_page_status[page_cnt + i]));
			}
		}
	}
}
