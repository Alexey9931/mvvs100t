#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "REGISTER_SPACE.h"
#include "UART.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

//��� �������������� ���������� �������
#define TYPE 0x00
#define INIT 0x01
#define READ 0x02
#define WRITE 0x03
#define RESET 0x04
#define CONFIG 0x05

/*
��������� � ������ ������ ������:
	uint8_t header - ���������
	uint8_t receiver_addr - ����� ����������
	uint8_t sender_addr - ����� �����������
	uint16_t length - ����� ������
	uint8_t service_byte - ��������� ����
	uint8_t cmd - ��� �������
	uint8_t data[2048] - ���� ������
	uint32_t data_size - ������ ���� ������ � ������
	uint32_t checksum - ����������� �����
	uint16_t end - ������� ����� ������
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
������� ��� ������� ������ ������
UARTn *UART_struct - ����� UART
���������� ��������� � ����������� 0 - ����� ������ �������, ��������� � �������� ����� ���������; 1- ��� ��������� ������, ������
*/
uint8_t PROTOCOL_REQUEST_DATA(UARTn *UART_struct);
/*
������� ��� �������� ������ ������
FIELDS_PACKET *received_packet - ��������� �� ��������� � ������ ��������� ������
FIELDS_PACKET *sended_packet - ��������� �� ��������� � ������ ��������� ������
UARTn *UART_struct - ����� UART
���������� ��������� � ����������� 0 - �����; 1- ������
*/
uint8_t PROTOCOL_WRITE(FIELDS_PACKET *received_packet, FIELDS_PACKET *sended_packet, UARTn *UART_struct);
/*
������� ��� ������ ������ ������
FIELDS_PACKET *received_packet - ��������� �� ��������� � ������ ��������� ������
UARTn *UART_struct - ����� UART
���������� ��������� � ����������� 0 - �����; 1- ������, �������� ������� ��� ���� ����� ���������
*/
uint8_t PROTOCOL_READ(FIELDS_PACKET *received_packet, UARTn *UART_struct);
/*
������� ��� ���������� ��������� �������
FIELDS_PACKET *received_packet - ��������� �� ��������� � ������ ��������� ������
FIELDS_PACKET *sended_packet - ��������� �� ��������� � ������ ��������� ������
���������� ��������� � ����������� 0 - �����; 1- ������
*/
uint8_t PROTOCOL_DO_CMD(FIELDS_PACKET *received_packet, FIELDS_PACKET *sended_packet);
/*
������� ���������� ����������� ����� ������ �� ��������� CRC32
unsigned char *buf - ����� � ���� ������� ������
size_t len - ����� ������
�������� ��� ������ "123456789" - CRC = cbf43926
*/
uint_least32_t CRC32(unsigned char *buf, size_t len);
/*
������� ���������� ����������� ����� ������ ������
FIELDS_PACKET *packet - ��������� �� ��������� � ������ ������
*/
uint_least32_t PROTOCOL_FIND_CRC(FIELDS_PACKET *packet);

#endif /*__PROTOCOL_H */
