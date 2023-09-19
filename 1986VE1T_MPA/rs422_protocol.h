/*!
 \file
 \brief Заголовочный файл с описанием API протокола обмена данными по интерфейсу RS-422
*/

#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "UART.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#define NUMBER_CMDS_IN_PACKET 255             		///< Максимальное число команд в одном пакете

////все поддерживаемые протоколом команды
#define TYPE 0x00
#define INIT 0x01
#define READ 0x02
#define WRITE 0x03
#define RESET 0x04
#define CONFIG 0x05

///Коды ошибок обмена данными
typedef enum protocol_errors
{
	NO_ERROR,															///< Нет ошибок
	UART_ERROR,														///< Ошибка работы UART
	CRC_ERROR,														///< Ошибка контрольной суммы
	PM_ADDR_ERROR,												///< Ошибка адресации
	PACKET_ERROR													///< Ошибка структуры пакета
} data_exchange_errors;

///Структура с полями данных для каждой команды внутри одного пакета
typedef struct cmd_struct
{
	uint8_t cmd;			///< Команда
	uint16_t result;	///< Результат выполнения команды
	uint16_t length;	///< Длина команды
	uint8_t *data;		///< Данные
}__attribute__((packed)) fields_cmd;

///Структура с полями заголовка пакета
typedef struct packet_header_struct
{
	uint8_t header;							///< Заголовок
	uint8_t receiver_addr;			///< Адрес получателя
	uint8_t sender_addr;				///< Адрес отправителя
	uint16_t packet_length;			///< Длина пакета
	uint8_t service_byte;				///< Сервисный байт
	uint8_t cmd_number;					///< Количество команд
}__attribute__((packed)) fields_packet_header;

///Структура с полями конца пакета
typedef struct packet_tail_Struct
{
	uint32_t checksum;		///< Контрольная сумма (CRC32)
	uint16_t end;					///< Конец пакета
}__attribute__((packed)) fields_packet_tail;

///Структура со всеми полями данных пакета
typedef struct tx_rx_packet_struct
{
	fields_packet_header packet_header;	///< Заголовочные поля
	fields_cmd *cmd_with_data;						///< Поля с содержимым каждой команды
	fields_packet_tail packet_tail;			///< Поля хвоста пакета
}__attribute__((packed)) fields_packet;

///Структура с полями, необходимыми для расчета контрольной суммы
typedef struct protocol_crc
{
	uint8_t receiver_addr;			///< Адрес получателя
	uint8_t sender_addr;				///< Адрес отправителя
	uint16_t packet_length;			///< Длина пакета
	uint8_t service_byte;				///< Сервисный байт
	uint8_t cmd_number;					///< Количество команд
	fields_cmd *cmd_with_data;	///< Поля с содержимым каждой команды
}__attribute__((packed)) fields_crc;

/*!
 *	\brief Отправляет пакет данных
 *	\param *UART_struct - Выбранный UART 
 *	\return Сообщение с результатом (0 - успех, 1- ошибка)
*/
uint8_t transmit_packet(UARTn *UART_struct, uint8_t ext_bus);

/*!
 *	\brief Читает пакет данных
 *	\param *UART_struct - Выбранный UART 
 *	\return Сообщение с результатом (0 - успех, 1- ошибка)
*/
uint8_t receive_packet(UARTn *UART_struct, uint8_t ext_bus);

/*!
 *	\brief Выполняет требуемые команды
 *	\return Сообщение с результатом (0 - успех, 1- ошибка)
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
 *	\brief Преобразует слово для передачи в сеть
 *	\param src - Слово
 *	\return Преобразованнное слово
*/
uint32_t* htonl(uint32_t *src);

/*!
 *	\brief Преобразует полуслово для передачи в сеть
 *	\param src - Полуслово
 *	\return Преобразованнное полуслово
*/
uint16_t* htons(uint16_t *src);

#endif /*__PROTOCOL_H */
