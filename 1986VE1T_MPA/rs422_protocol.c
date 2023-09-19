#include "rs422_protocol.h"
#include "external_ram.h"

extern ram_data *ram_space_pointer;
/*
Функция для отправки пакета данных
*/
uint8_t transmit_packet(UARTn *UART_struct)
{	
	//записываем в буфер заголовок пакета
	memcpy(ram_space_pointer->buffer_tx, &(ram_space_pointer->tx_packet_struct.packet_header), sizeof(ram_space_pointer->tx_packet_struct.packet_header));
	//записываем в буфер массив команд (команда - данные)
	uint16_t buffer_offset = 0; //перемешение по буферу
	for (uint32_t i = 0; i < ((ram_space_pointer->tx_packet_struct.packet_header).cmd_number); i++)
	{
		//записываем команда, результат, данные
		memcpy((ram_space_pointer->buffer_tx) + sizeof(ram_space_pointer->tx_packet_struct.packet_header) + buffer_offset, &(ram_space_pointer->tx_cmd_packet[i]), sizeof(ram_space_pointer->tx_cmd_packet[i]) - sizeof(ram_space_pointer->tx_cmd_packet[i].data));
		memcpy((ram_space_pointer->buffer_tx) + sizeof(ram_space_pointer->tx_packet_struct.packet_header) + buffer_offset + sizeof(ram_space_pointer->tx_cmd_packet[i]) - sizeof(ram_space_pointer->tx_cmd_packet[i].data), ram_space_pointer->tx_cmd_packet[i].data, (ram_space_pointer->tx_cmd_packet->length) - sizeof(ram_space_pointer->tx_cmd_packet[i]) + sizeof(ram_space_pointer->tx_cmd_packet[i].data));
		buffer_offset += ram_space_pointer->tx_cmd_packet[i].length;
	}
	//записываем в буфер хвост пакета
	memcpy((ram_space_pointer->buffer_tx) + sizeof(ram_space_pointer->tx_packet_struct.packet_header) + buffer_offset, &(ram_space_pointer->tx_packet_struct.packet_tail), sizeof(ram_space_pointer->tx_packet_struct.packet_tail));
	
	//отправляем данные по UART
	if (uart_write(UART_struct, ram_space_pointer->buffer_tx, ram_space_pointer->tx_packet_struct.packet_header.packet_length + sizeof(ram_space_pointer->tx_packet_struct.packet_header.header) + sizeof(ram_space_pointer->tx_packet_struct.packet_tail.end)) != 0)
	{
		return 1;
	}
	
	return 0;
}
/*
Функция для чтения пакета данных
*/
uint8_t receive_packet(UARTn *UART_struct)
{
	//очистка всех структур данных связанных с принимаемым и отправляемым пакетом
	memset(&(ram_space_pointer->rx_packet_struct), 0, sizeof(ram_data) - sizeof(ram_space_pointer->start_struct) - sizeof(ram_space_pointer->Reserv) - sizeof(ram_space_pointer->ram_register_space) - sizeof(ram_space_pointer->service_byte_pm) - sizeof(ram_space_pointer->service_byte_um));
	
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
	
	//распознавание сервисного байта УМ
	memcpy(&(ram_space_pointer->service_byte_um) , &(ram_space_pointer->rx_packet_struct.packet_header.service_byte), sizeof(ram_space_pointer->rx_packet_struct.packet_header.service_byte));	
	
	//считываем хвост пакета
	memcpy(&(ram_space_pointer->rx_packet_struct.packet_tail), buffer_rx + (ram_space_pointer->rx_packet_struct.packet_header.packet_length) - sizeof(ram_space_pointer->rx_packet_struct.packet_tail.checksum) + sizeof(ram_space_pointer->rx_packet_struct.packet_header.header), sizeof(fields_packet_tail));
	
	//считываем массив команд (команда - данные)
	uint16_t buffer_offset = 0; //перемешение по буферу
	for (uint32_t i = 0; i < ((ram_space_pointer->rx_packet_struct.packet_header).cmd_number); i++)
	{
		//считываем команда, результат, данные
		memcpy(&(ram_space_pointer->rx_cmd_packet[i]), buffer_rx + sizeof(ram_space_pointer->rx_packet_struct.packet_header) + buffer_offset, sizeof(ram_space_pointer->rx_cmd_packet[i]) - sizeof(ram_space_pointer->rx_cmd_packet[i].data));
		//считываем указатель на данные
		ram_space_pointer->rx_cmd_packet[i].data = buffer_rx + sizeof(ram_space_pointer->rx_packet_struct.packet_header) + buffer_offset + sizeof(ram_space_pointer->rx_cmd_packet[i]) - sizeof(ram_space_pointer->rx_cmd_packet[i].data);
		buffer_offset += ram_space_pointer->rx_cmd_packet[i].length;
	}
	
	// вычисление контрольной суммы и сравнение ее с той, что в телеграмме
	uint8_t buffer_crc[BUFFER_SIZE];
	uint32_t offset_crc;//смещение по buffer_crc
	memcpy(buffer_crc, &(ram_space_pointer->rx_packet_struct.packet_header.receiver_addr), sizeof(fields_packet_header)-sizeof(ram_space_pointer->rx_packet_struct.packet_header.header));
	offset_crc = sizeof(fields_packet_header)-sizeof(ram_space_pointer->rx_packet_struct.packet_header.header);
	for (uint32_t i = 0; i < ((ram_space_pointer->rx_packet_struct.packet_header).cmd_number); i++)
	{
		memcpy(buffer_crc + offset_crc  ,&(ram_space_pointer->rx_cmd_packet[i].cmd), sizeof(fields_cmd) - sizeof(ram_space_pointer->rx_cmd_packet[i].data));
		memcpy(buffer_crc + offset_crc + sizeof(fields_cmd) - sizeof(ram_space_pointer->rx_cmd_packet[i].data), (ram_space_pointer->rx_cmd_packet[i].data), (ram_space_pointer->rx_cmd_packet[i].length)-sizeof(ram_space_pointer->rx_cmd_packet[i].length)-sizeof(ram_space_pointer->rx_cmd_packet[i].cmd)-sizeof(ram_space_pointer->rx_cmd_packet[i].result));
		offset_crc += ram_space_pointer->rx_cmd_packet[i].length;
	}
	
	uint32_t real_checksum = crc32(buffer_crc, ram_space_pointer->rx_packet_struct.packet_header.packet_length - sizeof(ram_space_pointer->rx_packet_struct.packet_tail.checksum));
	//если контрольная сумма не верна, то выход из функции
	if (real_checksum != (ram_space_pointer->rx_packet_struct.packet_tail.checksum))
	{
		return 1;
	}
	//проверка адресации
	if(ram_space_pointer->rx_packet_struct.packet_header.receiver_addr != PM_ADDR)
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
	uint32_t offset = 0; //указатель на данные для каждой команды
	uint16_t start_addr = 0; //начальный адрес для чтения/записи регистров
	uint16_t size = 0;	//кол-во байт для чтения/записи
			
	//заполнение поля данных
	for (uint8_t i = 0; i < ram_space_pointer->rx_packet_struct.packet_header.cmd_number; i++)
	{
		switch (ram_space_pointer->rx_cmd_packet->cmd)
		{
			case TYPE:
				//по команде TYPE кладем в поле дата регистры PLC_SoftVer, PLC_Config, PLC_DeviceType, PLC_SerialNumber
				memcpy((ram_space_pointer->tx_data) + offset, &(ram_space_pointer->ram_register_space.PLC_SoftVer), sizeof(ram_space_pointer->ram_register_space.PLC_SoftVer));
				memcpy((ram_space_pointer->tx_data) + offset + sizeof(ram_space_pointer->ram_register_space.PLC_SoftVer), &(ram_space_pointer->ram_register_space.PLC_Config), sizeof(ram_space_pointer->ram_register_space.PLC_Config));
				memcpy((ram_space_pointer->tx_data) + offset + sizeof(ram_space_pointer->ram_register_space.PLC_SoftVer) + sizeof(ram_space_pointer->ram_register_space.PLC_Config), &(ram_space_pointer->ram_register_space.PLC_DeviceType), sizeof(ram_space_pointer->ram_register_space.PLC_DeviceType));
				memcpy((ram_space_pointer->tx_data) + offset + sizeof(ram_space_pointer->ram_register_space.PLC_SoftVer) + sizeof(ram_space_pointer->ram_register_space.PLC_Config) + sizeof(ram_space_pointer->ram_register_space.PLC_DeviceType), &(ram_space_pointer->ram_register_space.PLC_SerialNumber), sizeof(ram_space_pointer->ram_register_space.PLC_SerialNumber));		
		    ram_space_pointer->tx_cmd_packet[i].data = (ram_space_pointer->tx_data) + offset;
				offset += sizeof(ram_space_pointer->ram_register_space.PLC_SoftVer) + sizeof(ram_space_pointer->ram_register_space.PLC_Config) + sizeof(ram_space_pointer->ram_register_space.PLC_DeviceType) + sizeof(ram_space_pointer->ram_register_space.PLC_SerialNumber);
				ram_space_pointer->tx_cmd_packet[i].cmd = TYPE;
				//TODO:разобраться для чего поле result
				ram_space_pointer->tx_cmd_packet[i].result = 0;
				ram_space_pointer->tx_cmd_packet[i].length = sizeof(ram_space_pointer->tx_cmd_packet[i].cmd) + sizeof(ram_space_pointer->tx_cmd_packet[i].result) + sizeof(ram_space_pointer->tx_cmd_packet[i].length) + sizeof(ram_space_pointer->ram_register_space.PLC_SoftVer) + sizeof(ram_space_pointer->ram_register_space.PLC_Config) + sizeof(ram_space_pointer->ram_register_space.PLC_DeviceType) + sizeof(ram_space_pointer->ram_register_space.PLC_SerialNumber);
				break;
			case INIT:
				//по команде INIT кладем в поле дата регистр PLC_SerialNumber, если выполнен ряд условий
				if((ram_space_pointer->service_byte_um.ready_to_control != 1) || )
				{
					break;
				}
				
				memcpy((ram_space_pointer->tx_data) + offset, &(ram_space_pointer->ram_register_space.PLC_SerialNumber), sizeof(ram_space_pointer->ram_register_space.PLC_SerialNumber));
				ram_space_pointer->tx_cmd_packet[i].data = (ram_space_pointer->tx_data) + offset;
				offset += sizeof(ram_space_pointer->ram_register_space.PLC_SerialNumber);
				ram_space_pointer->tx_cmd_packet[i].cmd = INIT;
				//TODO:разобраться для чего поле result
				ram_space_pointer->tx_cmd_packet[i].result = 0;
				ram_space_pointer->tx_cmd_packet[i].length = sizeof(ram_space_pointer->tx_cmd_packet[i].cmd) + sizeof(ram_space_pointer->tx_cmd_packet[i].result) + sizeof(ram_space_pointer->tx_cmd_packet[i].length) + sizeof(ram_space_pointer->ram_register_space.PLC_SerialNumber);
				//заносим инфу в сервисный байт ПМ
				ram_space_pointer->service_byte_pm.init = 1;
				break;
			case READ:
				memcpy(&start_addr, ram_space_pointer->rx_cmd_packet[i].data, sizeof(start_addr));
				memcpy(&size, (ram_space_pointer->rx_cmd_packet[i].data) + sizeof(start_addr), sizeof(size));
				//по команде READ кладем в поле дата size байт начиная с start_addr
				memcpy((ram_space_pointer->tx_data) + offset, &(ram_space_pointer->ram_register_space) + start_addr, size);
				ram_space_pointer->tx_cmd_packet[i].data = (ram_space_pointer->tx_data) + offset;
				offset += size;
				ram_space_pointer->tx_cmd_packet[i].cmd = READ;
				//TODO:разобраться для чего поле result
				ram_space_pointer->tx_cmd_packet[i].result = 0;
				ram_space_pointer->tx_cmd_packet[i].length = sizeof(ram_space_pointer->tx_cmd_packet[i].cmd) + sizeof(ram_space_pointer->tx_cmd_packet[i].result) + sizeof(ram_space_pointer->tx_cmd_packet[i].length) + size;
				break;
			case WRITE:
				memcpy(&start_addr, ram_space_pointer->rx_cmd_packet[i].data, sizeof(start_addr));
				memcpy(&size, (ram_space_pointer->rx_cmd_packet[i].data) + sizeof(start_addr), sizeof(size));
				//по команде WRITE кладем по адресу start_addr size принятых байт для записи и в поле данных кладем код команды
				memcpy(&(ram_space_pointer->ram_register_space) + start_addr, (ram_space_pointer->rx_cmd_packet[i].data) + sizeof(start_addr) + sizeof(size), size);
				memset((ram_space_pointer->tx_data) + offset, WRITE, 1);
				ram_space_pointer->tx_cmd_packet[i].data = (ram_space_pointer->tx_data) + offset;
				offset++;
				ram_space_pointer->tx_cmd_packet[i].cmd = WRITE;
				//TODO:разобраться для чего поле result
				ram_space_pointer->tx_cmd_packet[i].result = 0;
				ram_space_pointer->tx_cmd_packet[i].length = sizeof(ram_space_pointer->tx_cmd_packet[i].cmd) + sizeof(ram_space_pointer->tx_cmd_packet[i].result) + sizeof(ram_space_pointer->tx_cmd_packet[i].length) + 1;
				break;
			case RESET:
				//по команде RESET сбрасываем регистры и в поле данных кладем код команды
				init_external_ram_space();
				memset((ram_space_pointer->tx_data) + offset, RESET, 1);
				ram_space_pointer->tx_cmd_packet[i].data = (ram_space_pointer->tx_data) + offset;
				offset++;
				ram_space_pointer->tx_cmd_packet[i].cmd = RESET;
				//TODO:разобраться для чего поле result
				ram_space_pointer->tx_cmd_packet[i].result = 0;
				ram_space_pointer->tx_cmd_packet[i].length = sizeof(ram_space_pointer->tx_cmd_packet[i].cmd) + sizeof(ram_space_pointer->tx_cmd_packet[i].result) + sizeof(ram_space_pointer->tx_cmd_packet[i].length) + 1;
				break;
			case CONFIG:
				//проверка на то, что ПМ находится в сервисном режиме
				if (ram_space_pointer->ram_register_space.PLC_PMAddr != 0x00)
				{
					break;
				}
				//по команде CONFIG регистры зеркализируются в ПЗУ и в поле данных кладем код команды
				memcpy(&start_addr, ram_space_pointer->rx_cmd_packet[i].data, sizeof(start_addr));
				memcpy(&size, (ram_space_pointer->rx_cmd_packet[i].data) + sizeof(start_addr), sizeof(size));
				//запись данных в ПЗУ
				//TODO: добавить работу с ПЗУ
				memset((ram_space_pointer->tx_data) + offset, CONFIG, 1);
				ram_space_pointer->tx_cmd_packet[i].data = (ram_space_pointer->tx_data) + offset;
				offset++;
				ram_space_pointer->tx_cmd_packet[i].cmd = CONFIG;
				//TODO:разобраться для чего поле result
				ram_space_pointer->tx_cmd_packet[i].result = 0;
				ram_space_pointer->tx_cmd_packet[i].length = sizeof(ram_space_pointer->tx_cmd_packet[i].cmd) + sizeof(ram_space_pointer->tx_cmd_packet[i].result) + sizeof(ram_space_pointer->tx_cmd_packet[i].length) + 1;
				break;
			default:
				break;
		}
		ram_space_pointer->tx_packet_struct.packet_header.packet_length += ram_space_pointer->tx_cmd_packet[i].length;
	}
	//заполнение остальных полей
	ram_space_pointer->tx_packet_struct.packet_header.header = ram_space_pointer->rx_packet_struct.packet_header.header;
	ram_space_pointer->tx_packet_struct.packet_header.receiver_addr = ram_space_pointer->rx_packet_struct.packet_header.sender_addr;
	ram_space_pointer->tx_packet_struct.packet_header.sender_addr = ram_space_pointer->rx_packet_struct.packet_header.receiver_addr;
	ram_space_pointer->tx_packet_struct.packet_header.packet_length += sizeof(ram_space_pointer->tx_packet_struct.packet_header) - sizeof(ram_space_pointer->tx_packet_struct.packet_header.header) + sizeof(ram_space_pointer->tx_packet_struct.packet_tail.checksum);
	//TODO:разобраться с полем сервисный байт
	ram_space_pointer->tx_packet_struct.packet_header.service_byte = 0x80;
	ram_space_pointer->tx_packet_struct.packet_header.cmd_number = ram_space_pointer->rx_packet_struct.packet_header.cmd_number;
	
	// вычисление контрольной суммы
	uint8_t buffer_crc[BUFFER_SIZE];
	uint32_t offset_crc;//смещение по buffer_crc
	memcpy(buffer_crc, &(ram_space_pointer->tx_packet_struct.packet_header.receiver_addr), sizeof(fields_packet_header)-sizeof(ram_space_pointer->tx_packet_struct.packet_header.header));
	offset_crc = sizeof(fields_packet_header)-sizeof(ram_space_pointer->tx_packet_struct.packet_header.header);
	for (uint32_t i = 0; i < ((ram_space_pointer->tx_packet_struct.packet_header).cmd_number); i++)
	{
		memcpy(buffer_crc + offset_crc  ,&(ram_space_pointer->tx_cmd_packet[i].cmd), sizeof(fields_cmd) - sizeof(ram_space_pointer->tx_cmd_packet[i].data));
		memcpy(buffer_crc + offset_crc + sizeof(fields_cmd) - sizeof(ram_space_pointer->tx_cmd_packet[i].data), (ram_space_pointer->tx_cmd_packet[i].data), (ram_space_pointer->tx_cmd_packet[i].length)-sizeof(ram_space_pointer->tx_cmd_packet[i].length)-sizeof(ram_space_pointer->tx_cmd_packet[i].cmd)-sizeof(ram_space_pointer->tx_cmd_packet[i].result));
		offset_crc += ram_space_pointer->tx_cmd_packet[i].length;
	}
	ram_space_pointer->tx_packet_struct.packet_tail.checksum = crc32(buffer_crc, ram_space_pointer->tx_packet_struct.packet_header.packet_length - sizeof(ram_space_pointer->tx_packet_struct.packet_tail.checksum));
	
	ram_space_pointer->tx_packet_struct.packet_tail.end = ram_space_pointer->rx_packet_struct.packet_tail.end;
	
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
/*
Функция для преобразования слова для передачи в сеть
*/
uint32_t* htonl(uint32_t *src) 
{
  uint8_t c;
  union {
    uint32_t v;
    uint8_t c[4];
  } x;
  x.v = *src;
  c = x.c[0]; x.c[0] = x.c[3]; x.c[3] = c;
  c = x.c[1]; x.c[1] = x.c[2]; x.c[2] = c;
	*src = x.v;
  return src;
}
/*
Функция для преобразования полуслова для передачи в сеть
*/
uint16_t* htons(uint16_t *src) 
{
  uint8_t c;
  union {
	uint16_t v;
    uint8_t c[2];
  } x;
  x.v = *src;
  c = x.c[0]; x.c[0] = x.c[1]; x.c[1] = c;
	*src = x.v;
  return src;
}


