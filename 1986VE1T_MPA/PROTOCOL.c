#include "PROTOCOL.h"

/*
������� ��� �������� ������ ������
*/
uint8_t Protocol_write(Fields_packet *received_packet, Fields_packet *sended_packet, UARTn *UART_struct)
{
	uint8_t DATA[2048] = {0};//������������ ������ ������
	
	sended_packet->header = 0x55; //���������
	sended_packet->receiver_addr = received_packet->sender_addr; //����� ����������
	sended_packet->sender_addr = received_packet->receiver_addr; //����� �����������
	sended_packet->end = 0xAAAA; //������� ����� ������

	DATA[0] = sended_packet->header;
	DATA[1] = sended_packet->receiver_addr;
	DATA[2] = sended_packet->sender_addr;
	DATA[3] = sended_packet->length & 0xFF;
	DATA[4] = sended_packet->length>>8;
	DATA[5] = sended_packet->service_byte;
	DATA[6] = sended_packet->cmd;
	for (uint8_t len = 0;len < sended_packet->data_size; len++)
	{
		DATA[7+len] = sended_packet->data[len];
	}
	
	//���������� ����������� �����
	sended_packet->checksum = Protocol_find_crc(sended_packet);
	
	DATA[7+sended_packet->data_size] = sended_packet->checksum & 0xFF;
	DATA[7+sended_packet->data_size+1] = sended_packet->checksum>>8;
	DATA[7+sended_packet->data_size+2] = sended_packet->checksum>>16;
	DATA[7+sended_packet->data_size+3] = sended_packet->checksum>>24;
	
	DATA[7+sended_packet->data_size+4] = sended_packet->end & 0xFF;
	DATA[7+sended_packet->data_size+5] = sended_packet->end>>8;

	uart_write(UART_struct, DATA, 13+sended_packet->data_size);
}
/*
������� ��� ������ ������ ������
*/
uint8_t Protocol_read(Fields_packet *received_packet, UARTn *UART_struct)
{
	uint8_t data[2048];//���������� (���� ����� � ������ ������)
	uint8_t buffer[2048];//���� ��������� ����� (����� �� ���� �����, � ���� ������ ������� � �������)
	
//	//��������� ������ 5 ���� ����������, �� ��� ���������� ����� ������
//	if (abs(uart_read_pos(UART_struct) - uart_get_buf_counter(UART_struct)) < 5) //�������� ����, ���� ����� ������ 5 ���� ��� �� ������ ������� � ������
//	{
//		return 1;
//	}
	if (uart_read(UART_struct, 5,  buffer) == 1)
	{
		return 1;
	}
//	//���������� ����� ������
//	for (int len = 4; len >= 3; len--)
//	{
//		received_packet->length = received_packet->length << 8;
//		received_packet->length |= buffer[len];
//	}
//	received_packet->length += 3; //� ������ �������� ������ � �����
//	
//	//������ ��������� ���� ����� �� ����������� ����� ������
//	uart_set_pos(UART_struct, uart_read_pos(UART_struct) - 5); //������� ������� ������ ������ �� �������� �������
//	memset(buffer,0,sizeof(buffer));
//	uart_read(&UART_struct, received_packet->length, buffer);
//	//������������� ������ ������
//	uint32_t count = 0;
//	while ((buffer[count] != 0xAA)&&(buffer[count-1] != 0xAA))
//	{
//		//���� ������ ���� ����� ��� � �� ����� ����� ������, �� ����� � �������
//		if(count == 2049)
//		{
//			return 1;
//		}
//		data[count] = buffer[count];
//		count++;
//	}
//	data[count] = buffer[count];
//	data[count+1] = buffer[count+1];
//	//data_size = count+2;
//	//uart_set_pos(data_size + uart_read_pos());
//	//uart_write(data, data_size);
//	
//	//������������� ����� ������
//	//���������
//	received_packet->header = data[0];
//	//����� ����������
//	received_packet->receiver_addr = data[1];	
//	//����� �����������
//	received_packet->sender_addr = data[2];
//	//��������� ����
//	received_packet->service_byte = data[5];
//	//��� ������� 
//	received_packet->cmd = data[6];
//	//���� ������
//	received_packet->data_size = received_packet->length-13;//���-�� ���� ���� ������
//	for (uint8_t len = 0; len < received_packet->data_size; len++)
//	{
//		received_packet->data[len] = data[7+len];
//	}
//	//����������� �����
//	for (int len = 3; len >= 0; len--)
//	{
//		received_packet->checksum = received_packet->checksum << 8;
//		received_packet->checksum |= data[7+received_packet->data_size+len];
//	}
//	//������� ����� ������
//	uint16_t end = 0;
//	for (int len = 1; len >= 0; len--)
//	{
//		received_packet->end = received_packet->end << 8;
//		received_packet->end |= data[11+received_packet->data_size+len];
//	}
//	
//	// ���������� ����������� ����� � ��������� �� � ���, ��� � ����������
//	/*
//	��� ������� ��� ������ ������ "0x55 0x11 0x22 0x10 0x11 0x80 0x01 0x22 0x5E 0xAD 0xC9 0xC3 0xAA 0xAA " - CRC = 0x5E 0xAD 0xC9 0xC3
//	*/
//	uint32_t real_checksum = PROTOCOL_FIND_CRC(received_packet);
//	//���� ����������� ����� �� �����, �� ����� �� �������
//	if (real_checksum != received_packet->checksum)
//	{
//		return 1;
//	}
	return 0;
}
/*
������� ��� ���������� ��������� �������
*/
uint8_t Protocol_do_cmd(Fields_packet *received_packet, Fields_packet *sended_packet)
{
	//	//�������� ������
//	uint8_t SendBuf[16];
//	switch (cmd)
//	{
//		case TYPE:
//			//PLC_SoftVer
//			for(int i = 0; i < 4; i++)
//			{
//				SendBuf[i] = read_register(0, 4, COMMON)[i];
//			}
//			//PLC_DeviceType
//			for(int i = 0; i < 4; i++)
//			{
//				SendBuf[i+4] = read_register(1160, 4, COMMON)[i];
//			}
//			//PLC_Config
//			for(int i = 0; i < 4; i++)
//			{
//				SendBuf[i+8] = read_register(4, 2, COMMON)[i];
//			}
//			//PLC_SerialNumber
//			for(int i = 0; i < 4; i++)
//			{
//				SendBuf[i+10] = read_register(1164, 4, COMMON)[i];
//			}
//			MODBUS_send_data(address_sender&0xF , address_sender>>4, address_receiver&0xF , address_receiver>>4, TYPE, SendBuf, 14, 0);
//			break;
//		case INIT:
//			for(int i = 0; i < 4; i++)
//			{
//				SendBuf[i] = read_register(1164, 4, COMMON)[i];
//			}
//			MODBUS_send_data(address_sender&0xF , address_sender>>4, address_receiver&0xF , address_receiver>>4, INIT, SendBuf, 4, 0);
//			break;
//		case READ:	
//			//���� ����� �������� ����� 1662 
//			if((DATA[0] == 0x7E)&&(DATA[1] == 0x06))
//			{
//				for(int i = 0; i < 16; i++)
//				{
//					SendBuf[i] = read_register(1662, 16, MPA)[i];
//				}	
//				MODBUS_send_data(address_sender&0xF , address_sender>>4, address_receiver&0xF , address_receiver>>4, READ, SendBuf, 16, 0);
//			}
//			else if ((DATA[0] == 0xAE)&&(DATA[1] == 0x06))
//			{
//				for(int i = 0; i < 16; i++)
//				{
//					SendBuf[i] = read_register(1710, 16, MPA)[i];
//				}				
//				MODBUS_send_data(address_sender&0xF , address_sender>>4, address_receiver&0xF , address_receiver>>4, READ, SendBuf, 16, 0);
//			}
//			break;
//		case WRITE:
//			break;
//		case RESET:
//			SendBuf[0] = 0x04;
//			MODBUS_send_data(address_sender&0xF , address_sender>>4, address_receiver&0xF , address_receiver>>4, RESET, SendBuf, 1, 0);
//			//����� ���� ���������
//			//������������� ������������ ���������
//			registers_ini();
//			break;
//		case CONFIG:
//			break;
//	}
		return 0;
}
/*
������� ���������� ����������� ����� ������ �� ��������� CRC32
*/
uint_least32_t Crc32(unsigned char *buf, size_t len)
{
	uint_least32_t crc_table[256];
	uint_least32_t crc; int i, j;

	for (i = 0; i < 256; i++)
	{
		crc = i;
		for (j = 0; j < 8; j++)
			crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

		crc_table[i] = crc;
	}

	crc = 0xFFFFFFFFUL;

	while (len--)
		crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

	return crc ^ 0xFFFFFFFFUL;
}
/*
������� ���������� ����������� ����� ������ ������
*/
uint_least32_t Protocol_find_crc(Fields_packet *packet)
{
	uint8_t buf_checksum[5 + packet->data_size];//������ ��� ������� ����������� �����
	buf_checksum[0] = packet->sender_addr;
	buf_checksum[1] = packet->length & 0xFF;
	buf_checksum[2] = packet->length >> 8;
	buf_checksum[3] = packet->service_byte;
	buf_checksum[4] = packet->cmd;
	for (int k = 0; k < packet->data_size; k++)
	{
		buf_checksum[k+5] = packet->data[k];
	}
	return Crc32(buf_checksum, 5 + packet->data_size);
}
