/*!
 \file
 \brief Заголовочный файл с описанием API для работы с UART
*/

#ifndef __UART_H
#define __UART_H

#include "mdr32_drivers.h"
#include "timers.h"
#include "string.h"

#define PORT_UART1 		MDR_PORTC			///< Порт UART1
#define PIN_UART1_RX 	PORT_Pin_4		///< Линия RX порта UART1
#define PIN_UART1_TX 	PORT_Pin_3		///< Линия TX порта UART1
#define PORT_UART2 		MDR_PORTD			///< Порт UART2
#define PIN_UART2_RX 	PORT_Pin_14		///< Линия RX порта UART2
#define PIN_UART2_TX 	PORT_Pin_13		///< Линия TX порта UART2

#define PORT_UART1_EN MDR_PORTD			///< Порт линии EN микросхемы RS485 для UART1
#define PORT_UART2_EN	MDR_PORTD			///< Порт линии EN микросхемы RS485 для UART2
#define PIN_UART1_EN 	PORT_Pin_10		///< Линия EN микросхемы RS485 для UART1
#define PIN_UART2_EN 	PORT_Pin_12		///< Линия EN микросхемы RS485 для UART2

#define UART_BUFFER_SIZE 	16384             				///< Размер кольцевого буфера UARTn (в кБайтах)
#define BUFFER_MASK 			(UART_BUFFER_SIZE-1)			///< Маска, необходимая для корректной работы кольцевого буфера

/**
 * @brief Коды ошибок работы UART
 *
 */
typedef enum errors
{
	NO_ERRORS,														///< Нет ошибок
	INIT_ERROR,														///< Ошибка инициализации UART
	WRITE_TIMEOUT_ERROR,									///< Ошибка сигнализирует о том, что превышен таймаут на запись
	READ_TIMEOUT_ERROR,										///< Ошибка сигнализирует о том, что превышен таймаут на чтение
	SIZE_ERROR,														///< Ошибка сигнализирует о том, что либо в буфере недостаточно данных для чтения, либо размер записываемых/считываемых данных больше размера самого буфера
	POSITION_ERROR												///< Ошибка установки курсора чтения
} uart_errors;

/**
 * @brief Структура с таймаутами UARTn
 *
 */
typedef struct uart_timeouts
{
	timer_n 							*timer_n_timeout;							///< Выбор таймера для отслеживания таймаутов
	uint8_t 							read_timeout_flag;  					///< Флаг таймаута на чтение
	uint8_t 							write_timeout_flag; 					///< Флаг таймаута на запись
	uint32_t 							read_val_timeout;  						///< Таймаут на чтение
	uint32_t 							write_val_timeout; 						///< Таймаут на запись
} 
#ifndef DOXYGEN
__attribute__((packed))
#endif
uart_rx_tx_timeouts;

/**
 * @brief Структура с параметрами DMA канала UARTn
 *
 */
typedef struct uart_dma_params
{
	uint8_t 									dma_channel;							 ///< Выбор канала DMA для UARTn
	uint32_t 									dma_irq_counter;					 ///< Счетчик прерываний DMA
	DMA_CtrlDataInitTypeDef 	DMA_InitStructure_UART_RX; ///< Структура с настройками DMA в целом 
	DMA_ChannelInitTypeDef 		DMA_Channel_UART_RX;  		 ///< Структура с настройками канала DMA
} 
#ifndef DOXYGEN
__attribute__((packed))
#endif
uart_dma_ch_params;  

/**
 * @brief Структура с конфигурационными параметрами UART и буфером приема
 *
 */
typedef struct uart_config_data
{
	MDR_UART_TypeDef* 		UARTx;            					///< Библиотечная структура с периферийными регистрами блока UART
	uart_rx_tx_timeouts 	uart_timeouts;							///< Структура с таймаутами UARTn
	uart_dma_ch_params 		uart_dma_ch;								///< Структура с параметрами канала DMA для UART
	IRQn_Type 						IRQn;												///< Выбор обработчика прерываний UARTn
	uint32_t 							RST_CLK_PCLK_UARTn;					///< Включение тактирования для UARTn
	UART_InitTypeDef 			UART;												///< Библиотечная структура с конфигурационными параметрами UART
	uint32_t 							UART_HCLKdiv;								///< Выбор делителя тактовой частоты для тактирования блока UARTn		 																	
	uint8_t* 							buffer;											///< Указатель на буфер преимника UART
	uint32_t 							buffer_count;								///< Счетчик элементов буфера
	uint32_t 							read_pos;										///< Текущая позиция курсора чтения в буфере
} 
#ifndef DOXYGEN
__attribute__((packed))
#endif
uart_n;

/*!
 *	\brief Инициализацирует выбранный  UARTn
 *	\param *uart_struct - Выбранный UART для инициализации
 *	\return Код ошибки uart_errors
*/
uart_errors uart_init(uart_n *uart_struct);

/*!
 *  \brief Передаёт данные по UART
 *  \param *uart_struct - Выбранный UART для инициализации
 *  \param *data - Указатель на массив данных
 *  \param data_size - Размер данных в байтах
 *  \return Код ошибки uart_errors
*/
uart_errors uart_write(uart_n *uart_struct, uint8_t *data, uint32_t data_size);

/*!
 * \brief Читает данные из буфера UART
 * \param *uart_struct - Выбранный UART для инициализации
 * \param len - Кол-во байт для чтения
 * \param *data - Указатель на массив, куда считываются байты из буфера UART
 * \return Код ошибки uart_errors
*/
uart_errors uart_read(uart_n *uart_struct, uint32_t len, uint8_t *data);

/*!
 * \brief Очищает кольцевой буфер приемника UART
 * \param *uart_struct - Выбранный UART для инициализации
*/
void uart_clean(uart_n *uart_struct);

/*!
 * \brief Устанавливает курсор чтения в кольцевом буфере
 * \param *uart_struct - Выбранный UART для инициализации
 * \param pos - Позиция курсора в буфере
 * \return Код ошибки uart_errors
*/
uart_errors uart_set_pos(uart_n *uart_struct, uint32_t pos);

/*!
 * \brief Читает текущую позицию курсора чтения в кольцевом буфере
 * \param *uart_struct - Выбранный UART для инициализации
 * \return Текущая позиция курсора
*/
uint32_t uart_read_pos(uart_n *uart_struct);

/*!
 * \brief Читает кол-во байт в кольцевом буфере UART
 * \param *uart_struct - Выбранный UART для инициализации
 * \return Кол-во байт в кольцевом буфере UART
*/
uint32_t uart_get_buf_counter(uart_n *uart_struct);

/*!
 * \brief Инициализирует n-ый канал DMA на запрос от приемника UARTn
 * \param *uart_struct - Выбранный UART для инициализации
*/
void DMA_UART_RX_init(uart_n *uart_struct);

/*!
 * \brief Устанавливает таймаут UARTn на чтение
 * \param *uart_struct - Выбранный UART для инициализации
 * \param read_timeout - Таймаут на чтение (в мс)
*/
void uart_set_read_timeout(uart_n *uart_struct, uint32_t read_timeout);

/*!
 * \brief Устанавливает таймаут UARTn на запись
 * \param *uart_struct - Выбранный UART для инициализации
 * \param write_timeout - Таймаут на запись (в мс)
*/
void uart_set_write_timeout(uart_n *uart_struct, uint32_t write_timeout);

/// Макрос для определения по какой шине идет обмен данными
#define RECOGNIZE_BUS(ext_bus, uart_struct) ({\
	if(uart_struct->UARTx == MDR_UART1) ext_bus = 1;\
	else if(uart_struct->UARTx == MDR_UART2) ext_bus = 2;\
})

/// Макрос возвращающий указатель на буфер UART
#define GET_UART_BUF_PTR uart_struct->buffer

#endif /*__UART_H */
