/*!
 \file
 \brief ������������ ���� � ��������� API ��������� ������ ������� �� ���������� RS-422
*/

#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "UART.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#define NUMBER_CMDS_IN_PACKET 255             		///< ������������ ����� ������ � ����� ������

////��� �������������� ���������� �������
#define TYPE_CMD 0x00
#define INIT_CMD 0x01
#define READ_CMD 0x02
#define WRITE_CMD 0x03
#define RESET_CMD 0x04
#define CONFIG 0x05

///���� ������ ������ �������
typedef enum protocol_errors
{
	NO_ERROR,															///< ��� ������
	UART_ERROR,														///< ������ ������ UART
	CRC_ERROR,														///< ������ ����������� �����
	PM_ADDR_ERROR,												///< ������ ���������
	PACKET_ERROR													///< ������ ��������� ������
} protocol_error;

///��������� � ���������� ��� ������ ������� ������ ������ ������
typedef struct cmd_header_struct
{
	uint8_t cmd;			///< �������
	uint16_t result;	///< ��������� ���������� �������
	uint16_t length;	///< ����� �������
}__attribute__((packed)) fields_cmd_header;

///��������� � ������ ������ ��� ������ ������� ������ ������ ������
typedef struct cmd_struct
{
	fields_cmd_header 	header;		///< ���������
	uint8_t 						*data;		///< ������
}__attribute__((packed)) fields_cmd;

///��������� � ������ ��������� ������
typedef struct packet_header_struct
{
	uint8_t 	header;							///< ���������
	uint8_t 	receiver_addr;			///< ����� ����������
	uint8_t 	sender_addr;				///< ����� �����������
	uint16_t 	packet_length;			///< ����� ������
	uint8_t 	service_byte;				///< ��������� ����
	uint8_t 	cmd_number;					///< ���������� ������
}__attribute__((packed)) fields_packet_header;

///��������� � ������ ����� ������
typedef struct packet_tail_Struct
{
	uint32_t checksum;		///< ����������� ����� (CRC32)
	uint16_t end;					///< ����� ������
}__attribute__((packed)) fields_packet_tail;

///��������� �� ����� ������ ������ ������
typedef struct tx_rx_packet_struct
{
	fields_packet_header 	packet_header;																	///< ������������ ����
	fields_cmd 						cmd_with_data[NUMBER_CMDS_IN_PACKET];						///< ������ ����� � ���������� ������ �������
	fields_packet_tail 		packet_tail;																		///< ���� ������ ������
}__attribute__((packed)) fields_packet;


/*!
 *	\brief ���������� ����� ������
 *	\param *UART_struct - ��������� UART 
 *	\param ext_bus - ����� ����
 *	\return ��������� � ����������� (0 - �����, 1- ������)
*/
protocol_error transmit_packet(UARTn *UART_struct, uint8_t ext_bus);

/*!
 *	\brief ������ ����� ������
 *	\param *UART_struct - ��������� UART 
 *	\param ext_bus - ����� ����
 *	\return ��� ������ protocol_error
*/
protocol_error receive_packet(UARTn *UART_struct, uint8_t ext_bus);

/*!
 *	\brief ��������� ��������� �������
 *	\param ext_bus - ����� ����
 *	\return ��������� � ����������� (0 - �����, 1- ������)
*/
uint8_t protocol_do_cmds(uint8_t ext_bus);

/*!
 *	\brief ��������� ����������� ����� �� ��������� CRC32
 *	\param *buf - ����� � ������� ��� ������� ����������� �����
 *	\param len - ����� ������
 *	\return ����������� �����
*/
uint_least32_t crc32(uint8_t *buf, size_t len);

/*!
 *	\brief ��������� ������� CRC32
*/
void fill_crc32_table(void);

/*!
 *	\brief ������������ ������
 *	\param error - ��� ������
 *	\param ext_bus - ����� ����
*/
void error_handler(protocol_error error, uint8_t ext_bus);

/*!
 *	\brief ����������� ����� ��� �������� � ����
 *	\param src - �����
 *	\return ���������������� �����
*/
uint32_t* htonl(uint32_t *src);

/*!
 *	\brief ����������� ��������� ��� �������� � ����
 *	\param src - ���������
 *	\return ���������������� ���������
*/
uint16_t* htons(uint16_t *src);

#endif /*__PROTOCOL_H */
