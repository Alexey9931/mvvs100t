#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "REGISTER_SPACE.h"
#include "UART.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

//все поддерживаемые протоколом команды
#define TYPE 0x00
#define INIT 0x01
#define READ 0x02
#define WRITE 0x03
#define RESET 0x04
#define CONFIG 0x05

/*
Структура с полями данных пакета:
	uint8_t header - заголовок
	uint8_t receiver_addr - адрес получателя
	uint8_t sender_addr - адрес отправителя
	uint16_t length - длина пакета
	uint8_t service_byte - сервисный байт
	uint8_t cmd - код команды
	uint8_t data[2048] - поле данных
	uint32_t data_size - размер поля данных в байтах
	uint32_t checksum - контрольная сумма
	uint16_t end - признак конца пакета
*/
typedef struct TX_RX_PACKET_STRUCT
{
	uint8_t header;
	uint8_t receiver_addr;
	uint8_t sender_addr;
	uint16_t length;
	uint8_t service_byte;
	uint8_t cmd;
	uint8_t data[2048];
	uint32_t data_size;
	uint32_t checksum;
	uint16_t end;
}FIELDS_PACKET;

/*typedef struct TX_RX_PACKET_STRUCT
{
	uint8_t header;
	uint8_t receiver_addr;
	uint8_t sender_addr;
	uint16_t length;
	uint8_t service_byte;
	uint8_t cmd;
	uint8_t data[2048];
	uint32_t data_size;
	uint32_t checksum;
	uint16_t end;
}__attribute__((packed)) FIELDS_PACKET;*/


/*
Функция для запроса пакета данных
UARTn *UART_struct - выбор UART
возвращает сообщение с результатом 0 - пакет данных получен, обработан и ответный пакет отправлен; 1- нет принятого пакета, ошибка
*/
uint8_t PROTOCOL_REQUEST_DATA(UARTn *UART_struct);
/*
Функция для отправки пакета данных
FIELDS_PACKET *received_packet - указатель на структуру с полями принятого пакета
FIELDS_PACKET *sended_packet - указатель на структуру с полями ответного пакета
UARTn *UART_struct - выбор UART
возвращает сообщение с результатом 0 - успех; 1- ошибка
*/
uint8_t PROTOCOL_WRITE(FIELDS_PACKET *received_packet, FIELDS_PACKET *sended_packet, UARTn *UART_struct);
/*
Функция для чтения пакета данных
FIELDS_PACKET *received_packet - указатель на структуру с полями принятого пакета
UARTn *UART_struct - выбор UART
возвращает сообщение с результатом 0 - успех; 1- ошибка, принятых пакетов нет либо пакет поврежден
*/
uint8_t PROTOCOL_READ(FIELDS_PACKET *received_packet, UARTn *UART_struct);
/*
Функция для выполнения требуемой команды
FIELDS_PACKET *received_packet - указатель на структуру с полями принятого пакета
FIELDS_PACKET *sended_packet - указатель на структуру с полями ответного пакета
возвращает сообщение с результатом 0 - успех; 1- ошибка
*/
uint8_t PROTOCOL_DO_CMD(FIELDS_PACKET *received_packet, FIELDS_PACKET *sended_packet);
/*
Функция вычисления контрольной суммы буфера по алгоритму CRC32
unsigned char *buf - буфер в виде массива байтов
size_t len - длина буфера
например для строки "123456789" - CRC = cbf43926
*/
uint_least32_t CRC32(unsigned char *buf, size_t len);
/*
Функция вычисления контрольной суммы пакета данных
FIELDS_PACKET *packet - указатель на структуру с полями пакета
*/
uint_least32_t PROTOCOL_FIND_CRC(FIELDS_PACKET *packet);

#endif /*__PROTOCOL_H */
