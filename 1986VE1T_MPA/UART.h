#ifndef __UART_H
#define __UART_H

#include "MDR32_Drivers.h"
#include "string.h"

#define BUFFER_SIZE 16
#define BUFFER_MASK (BUFFER_SIZE-1)

/*
Структура с таймаутами UARTn
	Флаг таймаута на чтение
	Флаг таймаута на запись
	Таймаут на чтение
	Таймаут на запись
*/
typedef struct UART_Timeouts
{
	uint8_t read_timeout_flag;
	uint8_t write_timeout_flag;
	uint32_t read_val_timeout;
	uint32_t write_val_timeout;
} UARTn_RX_TX_Timeouts;
/*
Структура с конфигурационными параметрами UART и буфером приема:
	Библиотечная структура с периферийными регистрами UART
	Структура с таймаутами UARTn
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
	UARTn_RX_TX_Timeouts* UARTx_timeouts;
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
uint8_t uart_write(UARTn *UART_struct, uint8_t *data, uint32_t data_size);
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
/*
Функция установки таймаута UARTn на чтение
UARTn *UART_struct - выбор UART
uint32_t read_timeout - таймаут на чтение
*/
void uart_set_read_timeout(UARTn *UART_struct, uint32_t read_timeout);
/*
Функция установки таймаута UARTn на запись
UARTn *UART_struct - выбор UART
uint32_t write_timeout - таймаут на запись
*/
void uart_set_write_timeout(UARTn *UART_struct, uint32_t write_timeout);

#endif /*__UART_H */



