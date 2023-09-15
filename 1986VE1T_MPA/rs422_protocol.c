#include "rs422_protocol.h"
#include "external_ram.h"

extern ram_data *ram_space_pointer;
/*
Функция для отправки пакета данных
*/
uint8_t transmit_packet(UARTn *UART_struct)
{
//	uint8_t DATA[2048] = {0};//отправляемый массив байтов
//	
//	sended_packet->header = 0x55; //заголовок
//	sended_packet->receiver_addr = received_packet->sender_addr; //адрес получателя
//	sended_packet->sender_addr = received_packet->receiver_addr; //адрес отправителя
//	sended_packet->end = 0xAAAA; //признак конца пакета

//	DATA[0] = sended_packet->header;
//	DATA[1] = sended_packet->receiver_addr;
//	DATA[2] = sended_packet->sender_addr;
//	DATA[3] = sended_packet->length & 0xFF;
//	DATA[4] = sended_packet->length>>8;
//	DATA[5] = sended_packet->service_byte;
//	DATA[6] = sended_packet->cmd;
//	for (uint8_t len = 0;len < sended_packet->data_size; len++)
//	{
//		DATA[7+len] = sended_packet->data[len];
//	}
//	
//	//вычисление контрольной суммы
//	sended_packet->checksum = Protocol_find_crc(sended_packet);
//	
//	DATA[7+sended_packet->data_size] = sended_packet->checksum & 0xFF;
//	DATA[7+sended_packet->data_size+1] = sended_packet->checksum>>8;
//	DATA[7+sended_packet->data_size+2] = sended_packet->checksum>>16;
//	DATA[7+sended_packet->data_size+3] = sended_packet->checksum>>24;
//	
//	DATA[7+sended_packet->data_size+4] = sended_packet->end & 0xFF;
//	DATA[7+sended_packet->data_size+5] = sended_packet->end>>8;

//	uart_write(UART_struct, DATA, 13+sended_packet->data_size);
}
/*
Функция для чтения пакета данных
*/
uint8_t receive_packet(UARTn *UART_struct)
{
	uint8_t buffer_rx[BUFFER_SIZE];//локальный буфер с принятым пакетом данных
	
	//считываем байты заголовка телеграммы для определения длины пакета
	if (uart_read(UART_struct, sizeof(ram_space_pointer->rx_packet_struct.packet_header),  buffer_rx))
	{
		return 1;
	}
	memcpy(&(ram_space_pointer->rx_packet_struct.packet_header) , buffer_rx, sizeof(ram_space_pointer->rx_packet_struct.packet_header));	
	
	//считываем весь пакет вычисленной длины
	uart_set_pos(UART_struct, uart_read_pos(UART_struct) - sizeof(ram_space_pointer->rx_packet_struct.packet_header)); //возврат курсора чтения буфера на исходную позицию
	memset(buffer_rx,0,sizeof(buffer_rx));
	if (uart_read(UART_struct, ((ram_space_pointer->rx_packet_struct.packet_header).packet_length)+sizeof(ram_space_pointer->rx_packet_struct.packet_header.header)+sizeof(ram_space_pointer->rx_packet_struct.packet_tail.end), buffer_rx))
	{
		return 1;
	}
	
	//считываем заголовок пакета
	memcpy(&(ram_space_pointer->rx_packet_struct.packet_header) , buffer_rx, sizeof(ram_space_pointer->rx_packet_struct.packet_header));	
	
	//считываем хвост пакета
	memcpy(&(ram_space_pointer->rx_packet_struct.packet_tail), buffer_rx + (ram_space_pointer->rx_packet_struct.packet_header.packet_length) - sizeof(ram_space_pointer->rx_packet_struct.packet_tail.checksum) + sizeof(ram_space_pointer->rx_packet_struct.packet_header.header), sizeof(fields_packet_tail));
	
	//считываем массив команд (команда - данные)
	uint16_t k = 0;
	for (uint32_t i = 0; i < ((ram_space_pointer->rx_packet_struct.packet_header).cmd_number); i++)
	{
		//считываем команда, результат, данные
		memcpy(&(ram_space_pointer->rx_cmd_packet[i]), buffer_rx + sizeof(ram_space_pointer->rx_packet_struct.packet_header) + k,5);
		//считываем указатель на данные
		ram_space_pointer->rx_cmd_packet[i].data = buffer_rx + sizeof(ram_space_pointer->rx_packet_struct.packet_header) + k + 5;
		k += ram_space_pointer->rx_cmd_packet[i].length;
	}
	
	// вычисление контрольной суммы и сравнение ее с той, что в телеграмме
	uint8_t buffer_crc[BUFFER_SIZE];
	uint32_t offset;//смещение по buffer_crc
	memcpy(buffer_crc, &(ram_space_pointer->rx_packet_struct.packet_header.receiver_addr), sizeof(fields_packet_header)-sizeof(ram_space_pointer->rx_packet_struct.packet_header.header));
	offset = sizeof(fields_packet_header)-sizeof(ram_space_pointer->rx_packet_struct.packet_header.header);
	for (uint32_t i = 0; i < ((ram_space_pointer->rx_packet_struct.packet_header).cmd_number); i++)
	{
		memcpy(buffer_crc + offset  ,&(ram_space_pointer->rx_cmd_packet[i].cmd), sizeof(fields_cmd) - sizeof(ram_space_pointer->rx_cmd_packet[i].data));
		memcpy(buffer_crc + offset + sizeof(fields_cmd) - sizeof(ram_space_pointer->rx_cmd_packet[i].data), (ram_space_pointer->rx_cmd_packet[i].data), (ram_space_pointer->rx_cmd_packet[i].length)-sizeof(ram_space_pointer->rx_cmd_packet[i].length)-sizeof(ram_space_pointer->rx_cmd_packet[i].cmd)-sizeof(ram_space_pointer->rx_cmd_packet[i].result));
		offset += ram_space_pointer->rx_cmd_packet[i].length;
	}
	
	uint32_t real_checksum = crc32(buffer_crc, ram_space_pointer->rx_packet_struct.packet_header.packet_length - sizeof(ram_space_pointer->rx_packet_struct.packet_tail.checksum));
	//если контрольная сумма не верна, то выход из функции
	if (real_checksum != (ram_space_pointer->rx_packet_struct.packet_tail.checksum))
	{
		return 1;
	}
	return 0;
}
/*
Функция для выполнения требуемой команды
*/
uint8_t protocol_do_cmds(void)
{
	for (uint8_t i = 0; i < ram_space_pointer->rx_packet_struct.packet_header.cmd_number; i++)
	{
		switch (ram_space_pointer->rx_cmd_packet->cmd)
		{
			case 0:
				//memcpy(ram_space_pointer->tx_cmd_packet[i].data,ram_space_pointer->ram_register_space.PLC_SoftVer, sizeof(ram_space_pointer->ram_register_space.PLC_SoftVer));
				
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			default:
				break;
		}
	}
	return 0;
}
/*
Функция вычисления контрольной суммы буфера по алгоритму CRC32
*/
uint_least32_t crc32(unsigned char *buf, size_t len)
{
	uint_least32_t crc_table[BUFFER_SIZE];
	uint_least32_t crc; int i, j;

	for (i = 0; i < BUFFER_SIZE; i++)
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

