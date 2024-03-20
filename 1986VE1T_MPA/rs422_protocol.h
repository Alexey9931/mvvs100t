/*!
 \file
 \brief Заголовочный файл с описанием API протокола обмена данными по интерфейсу RS-422
*/

#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "uart.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#define NUMBER_CMDS_IN_PACKET 255 ///< Максимально возможное число команд в одном пакете

/// Заголовок пакета
#define PACKET_HEAD 0x55	
/// Хвост пакета
#define PACKET_TAIL 0xAAAA

// Состояния автомата выбора УМ
#define PLC_CM_UNKNOWN_STATE 		0x10	///< Неизвестное состояние
#define PLC_CM_INIT_2_BUS 			0x09	///< Модуль инициализирован, управление по шине 2
#define PLC_CM_CRITICAL_FAULT 	0x06	///< Критическая неисправность
#define PLC_CM_REMOVE_INIT 			0x05	///< Управление не осуществляется (снятие инициализации)
#define PLC_CM_INIT_1_BUS 			0x04	///< Модуль инициализирован, управление по шине 1
#define PLC_CM_NOT_INIT 				0x01	///< Модуль не инициализирован

// Все поддерживаемые протоколом команды
#define TYPE_CMD 	0x00		///< Код команды TYPE
#define INIT_CMD 	0x01		///< Код команды INIT
#define READ_CMD 	0x02		///< Код команды READ
#define WRITE_CMD 0x03		///< Код команды WRITE
#define RESET_CMD 0x04		///< Код команды RESET
#define CONFIG 		0x05		///< Код команды CONFIG

/**
 * @brief Коды ошибок, которые могут возникать в процессе обмена данными по шине
 *
 */
typedef enum protocol_errors
{
	NO_ERROR,											///< Нет ошибок
	UART_ERROR,										///< Ошибка работы UART
	CRC_ERROR,										///< Ошибка контрольной суммы
	PM_ADDR_ERROR,								///< Ошибка адресации
	PACKET_ERROR									///< Ошибка структуры пакета
} protocol_error;

/**
 * @brief Структура с заголовком для каждой команды (субпакета) внутри одного пакета
 *
 */
typedef struct cmd_header_struct
{
	uint8_t cmd;					///< Команда
	uint16_t result;			///< Результат выполнения команды
	uint16_t length;			///< Длина команды
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
fields_cmd_header;

/**
 * @brief Структура с полями данных для каждой команды (субпакета) внутри одного пакета
 *
 */
typedef struct cmd_struct
{
	fields_cmd_header 	header;		///< Заголовок
	uint8_t 						*data;		///< Данные
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
fields_cmd;

/**
 * @brief Структура с полями заголовка пакета
 *
 */
typedef struct packet_header_struct
{
	uint8_t 	header;							///< Заголовок
	uint8_t 	receiver_addr;			///< Адрес получателя
	uint8_t 	sender_addr;				///< Адрес отправителя
	uint16_t 	packet_length;			///< Длина пакета
	uint8_t 	service_byte;				///< Сервисный байт
	uint8_t 	cmd_number;					///< Количество команд
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
fields_packet_header;

/**
 * @brief Структура с полями конца пакета
 *
 */
typedef struct packet_tail_Struct
{
	uint32_t checksum;		///< Контрольная сумма (CRC32)
	uint16_t end;					///< Конец пакета
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
fields_packet_tail;

/**
 * @brief Структура пакета данных согласно утвержденному протоколу обмена данными
 *
 */
typedef struct tx_rx_packet_struct
{
	fields_packet_header 	packet_header;																	///< Заголовочные поля
	fields_cmd 						cmd_with_data[NUMBER_CMDS_IN_PACKET];						///< Массив полей (субпакетов) с содержимым каждой команды
	fields_packet_tail 		packet_tail;																		///< Поля хвоста пакета
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
fields_packet;

/*!
 *	\brief Отправляет пакет данных
 *	\param *uart_struct - Выбранный UART МК
 *	\param ext_bus - Номер шины
 *	\return Код ошибки protocol_error
*/
protocol_error transmit_packet(uart_n *uart_struct, uint8_t ext_bus);

/*!
 *	\brief Читает пакет данных
 *	\param *uart_struct - Выбранный UART МК
 *	\param ext_bus - Номер шины
 *	\return Код ошибки protocol_error
*/
protocol_error receive_packet(uart_n *uart_struct, uint8_t ext_bus);

/*!
 *	\brief Выполняет требуемые команды
 *	\param ext_bus - Номер шины
 *	\return Код ошибки protocol_error
*/
uint8_t protocol_do_cmds(uint8_t ext_bus);

/*!
 *	\brief Вычисляет контрольную сумму по алгоритму CRC32
 *	\param *buf - Буфер с данными для расчета контрольной суммы
 *	\param len - Длина буфера
 *	\return Контрольная сумма
*/
uint_least32_t crc32(uint8_t *buf, size_t len);

/*!
 *	\brief Заполняет таблицу CRC32
*/
void fill_crc32_table(void);

/*!
 *	\brief Обрабатывает ошибки приема пакетов
 *	\param error - Код ошибки
 *	\param ext_bus - Номер шины
*/
void rx_error_handler(protocol_error error, uint8_t ext_bus);

/*!
 *	\brief Обрабатывает сервисный байт УМ
 *	\param ext_bus - Номер шины
*/
void um_service_byte_handler(uint8_t ext_bus);

#endif /*__PROTOCOL_H */
