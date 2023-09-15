/*!
 \file
 \brief Заголовочный файл с описанием API для работы с UART
*/

#ifndef __UART_H
#define __UART_H

#include "MDR32_Drivers.h"
#include "string.h"

#define BUFFER_SIZE 256             		///< Размер кольцевого буфера UARTn
#define BUFFER_MASK (BUFFER_SIZE-1)			///< Маска, необходимая для корректной работы кольцевого буфера

///Структура с таймаутами UARTn
typedef struct UART_Timeouts
{
	uint8_t read_timeout_flag;  					///< Флаг таймаута на чтение
	uint8_t write_timeout_flag; 					///< Флаг таймаута на запись
	uint32_t read_val_timeout;  					///< Таймаут на чтение
	uint32_t write_val_timeout; 					///< Таймаут на запись
} UARTn_RX_TX_Timeouts;    

///Структура с конфигурационными параметрами UART и буфером приема
typedef struct UART_ConfigData
{
	MDR_UART_TypeDef* UARTx;            	///< Библиотечная структура с периферийными регистрами блока UART
	UARTn_RX_TX_Timeouts* UARTx_timeouts;	///< Структура с таймаутами UARTn
	uint8_t DMA_Channel;									///< Выбор канала DMA для UARTn
	IRQn_Type IRQn;												///< Выбор обработчика прерываний UARTn
	uint32_t RST_CLK_PCLK_UARTn;					///< Включение тактирования для UARTn
	UART_InitTypeDef UART;								///< Библиотечная структура с конфигурационными параметрами UART
	uint32_t UART_HCLKdiv;								///< Выбор делителя тактовой частоты для тактирования блока UARTn		 																	
	uint8_t buffer[BUFFER_SIZE];					///< Буфер с принятыми байтами
	uint32_t buffer_count;								///< Счетчик элементов буфера
	uint32_t read_pos;										///< Текущая позиция курсора чтения в буфере
} UARTn ;

/*!
 *	\brief Инициализацирует выбранный  UARTn
 *	\param *UART_struct - Выбранный UART для инициализации
 *	\return Сообщение с результатом (0 - успех, 1- ошибка)
*/
uint8_t uart_init(UARTn *UART_struct);

/*!
 *  \brief Передаёт данные по UART
 *  \param *UART_struct - Выбранный UART для инициализации
 *  \param *data - Указатель на массив данных
 *  \param data_size - Размер данных в байтах
 *  \return Сообщение с результатом (0 - успех, 1- ошибка)
*/
uint8_t uart_write(UARTn *UART_struct, uint8_t *data, uint32_t data_size);

/*!
 * \brief Читает данные из буфера UART
 * \param *UART_struct - Выбранный UART для инициализации
 * \param len - Кол-во байт для чтения
 * \param *data - Указатель на массив, куда считываются байты из буфера UART
 * \return Сообщение с результатом (0 - успех, 1- ошибка)
*/
uint8_t uart_read(UARTn *UART_struct, uint32_t len, uint8_t *data);

/*!
 * \brief Очищает кольцевой буфер приемника UART
 * \param *UART_struct - Выбранный UART для инициализации
*/
void uart_clean(UARTn *UART_struct);

/*!
 * \brief Устанавливает курсор чтения в кольцевом буфере
 * \param *UART_struct - Выбранный UART для инициализации
 * \param pos - Позиция курсора в буфере
 * \return Сообщение с результатом (0 - успех, 1- ошибка)
*/
uint8_t uart_set_pos(UARTn *UART_struct, uint32_t pos);

/*!
 * \brief Читает текущую позицию курсора чтения в кольцевом буфере
 * \param *UART_struct - Выбранный UART для инициализации
 * \return Текущая позиция курсора
*/
uint32_t uart_read_pos(UARTn *UART_struct);

/*!
 * \brief Читает кол-во байт в кольцевом буфере UART
 * \param *UART_struct - Выбранный UART для инициализации
 * \return Кол-во байт в кольцевом буфере UART
*/
uint32_t uart_get_buf_counter(UARTn *UART_struct);

/*!
 * \brief Инициализирует n-ый канал DMA на запрос от приемника UARTn
 * \param *UART_struct - Выбранный UART для инициализации
*/
void DMA_UART_RX_init(UARTn *UART_struct);

/*!
 * \brief Устанавливает таймаут UARTn на чтение
 * \param *UART_struct - Выбранный UART для инициализации
 * \param read_timeout - Таймаут на чтение
*/
void uart_set_read_timeout(UARTn *UART_struct, uint32_t read_timeout);

/*!
 * \brief Устанавливает таймаут UARTn на запись
 * \param *UART_struct - Выбранный UART для инициализации
 * \param write_timeout - Таймаут на запись
*/
void uart_set_write_timeout(UARTn *UART_struct, uint32_t write_timeout);

#endif /*__UART_H */
