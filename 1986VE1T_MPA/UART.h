#ifndef __UART_H
#define __UART_H

#include "MDR32_Drivers.h"
#include "string.h"
#include "math.h"

#define BUFFER_SIZE 64
#define BUFFER_MASK (BUFFER_SIZE-1)
/*
Структура с конфигурационными параметрами UART и буфером приема:
	Библиотечная структура с периферийными регистрами UART
	Выбор канала DMA для UARTn
	Выбор обработчика прерываний UARTn
	Включение тактирования для UARTn
	Библиотечная структура с конфигурационными параметрами UART
	Делитель тактовой частоты для UART
	
	буфер с принятыми байтами
	счетчик элементов буфера
	текущая позиция курсора чтения в буфере
*/
typedef struct UART_ConfigData
{
	MDR_UART_TypeDef* UARTx;
	uint8_t DMA_Channel;
	IRQn_Type IRQn;
	uint32_t RST_CLK_PCLK_UARTn;
	UART_InitTypeDef UART;
	uint32_t UART_HCLKdiv;
	
	uint8_t buffer[BUFFER_SIZE];
	uint32_t buffer_count;
	uint32_t read_pos;
} UARTn ;

/*
Функция инициализации UART
UARTn *UART_struct - выбор UART
возвращает сообщение с результатом 0 - успех, 1- ошибка
*/
uint8_t uart_init(UARTn *UART_struct);
/*
Функция передачи данных по UART
UARTn *UART_struct - выбор UART
uint8_t data[] - массив передаваемых байтов
uint32_t data_size - размер передаваемого массива байтов
возвращает сообщение с результатом 0 - успех, 1- ошибка
*/
uint8_t uart_write(UARTn *UART_struct, uint8_t data[], uint32_t data_size);
/*
Функция чтения данных из буфера UART
UARTn *UART_struct - выбор UART
uint32_t len - кол-во байт для чтения
uint8_t *data - указатель на массив, куда считываются байты из буфера UART
возвращает сообщение с результатом 0 - успех, 1- ошибка
*/
uint8_t uart_read(UARTn *UART_struct, uint32_t len, uint8_t *data);
/*
Функция очистки буфера UART
UARTn *UART_struct - выбор UART
*/
void uart_clean(UARTn *UART_struct);
/*
Функция установки курсора чтения в буфере
UARTn *UART_struct - выбор UART
uint32_t pos - позиция курсора в буфере
возвращает сообщение с результатом 0 - успех, 1- ошибка
*/
uint8_t uart_set_pos(UARTn *UART_struct, uint32_t pos);
/*
Функция чтения текущей позиции курсора чтения
UARTn *UART_struct - выбор UART
возвращает текущую позицию
*/
uint32_t uart_read_pos(UARTn *UART_struct);
/*
Функция чтения кол-во байт в буфере UART
UARTn *UART_struct - выбор UART
возвращает кол-во байт в буфере UART
*/
uint32_t uart_get_buf_counter(UARTn *UART_struct);
/*
Функция инициализации n-го канала DMA  на запрос от приемника UARTn
UARTn *UART_struct - выбор UART
*/
void DMA_UART_RX_init(UARTn *UART_struct);

#endif /*__UART_H */



