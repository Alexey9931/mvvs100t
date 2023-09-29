#include "rs422_protocol.h"
#include "external_ram.h"

extern ram_data *ram_space_pointer;
/*
Функция для отправки пакета данных
*/
protocol_error transmit_packet(UARTn *UART_struct, uint8_t ext_bus)
{	
	protocol_error error;
	fields_packet *tx_pack_ptr = &(ram_space_pointer->tx_packet_struct); //указатель на структуру с отправляемым пакетом
	
	//записываем в буфер заголовок пакета
	memcpy(&(ram_space_pointer->packet_tx), &(tx_pack_ptr->packet_header), sizeof(tx_pack_ptr->packet_header));
	//*(fields_packet_header*)(ram_space_pointer->packet_tx) = tx_pack_ptr->packet_header;
	//записываем в буфер массив команд (команда - данные)
	uint16_t buffer_offset = 0; //перемешение по буферу
	for (uint32_t i = 0; i < ((tx_pack_ptr->packet_header).cmd_number); i++)
	{
		//записываем команда, результат, данные
		//*(fields_cmd_header*)((ram_space_pointer->packet_tx) + sizeof(tx_pack_ptr->packet_header) + buffer_offset) = tx_pack_ptr->cmd_with_data[i].header;
		memcpy((ram_space_pointer->packet_tx) + sizeof(tx_pack_ptr->packet_header) + buffer_offset, &(tx_pack_ptr->cmd_with_data[i].header), sizeof(tx_pack_ptr->cmd_with_data[i].header));
		memcpy((ram_space_pointer->packet_tx) + sizeof(tx_pack_ptr->packet_header) + buffer_offset + sizeof(fields_cmd_header), tx_pack_ptr->cmd_with_data[i].data, (tx_pack_ptr->cmd_with_data[i].header.length) - sizeof(fields_cmd_header));
		buffer_offset += tx_pack_ptr->cmd_with_data[i].header.length;
	}
	
	// вычисление контрольной суммы
	tx_pack_ptr->packet_tail.checksum = crc32((ram_space_pointer->packet_tx) + sizeof(tx_pack_ptr->packet_header.header), tx_pack_ptr->packet_header.packet_length - sizeof(tx_pack_ptr->packet_tail.checksum));
	
	//записываем в буфер хвост пакета
	memcpy((ram_space_pointer->packet_tx) + sizeof(tx_pack_ptr->packet_header) + buffer_offset, &(tx_pack_ptr->packet_tail), sizeof(tx_pack_ptr->packet_tail));
	//*(fields_packet_tail*)((ram_space_pointer->packet_tx) + sizeof(tx_pack_ptr->packet_header) + buffer_offset) = tx_pack_ptr->packet_tail;
	
	//отправляем данные по UART
	if (uart_write(UART_struct, ram_space_pointer->packet_tx, tx_pack_ptr->packet_header.packet_length + sizeof(tx_pack_ptr->packet_header.header) + sizeof(tx_pack_ptr->packet_tail.end)) != 0)
	{
		error = UART_ERROR;
		return error;
	}
	
	//если нет ошибок
	error = NO_ERROR;
	
	return error;
}
/*
Функция для чтения пакета данных
*/
protocol_error receive_packet(UARTn *UART_struct, uint8_t ext_bus)
{
	fields_packet *rx_pack_ptr = &(ram_space_pointer->rx_packet_struct); //указатель на структуру с принимаемым пакетом
	
	protocol_error error;
	uart_errors uart_error;
	
	//очистка всех структур данных связанных с принимаемым и отправляемым пакетом
	memset(rx_pack_ptr, 0, sizeof(ram_space_pointer->rx_packet_struct) + sizeof(ram_space_pointer->tx_packet_struct) + 
	+ sizeof(ram_space_pointer->tx_data) + sizeof(ram_space_pointer->packet_tx) + sizeof(ram_space_pointer->packet_rx));
	
	//считываем первый байт и проверяем на 0x55
	uart_error = uart_read(UART_struct, sizeof(rx_pack_ptr->packet_header.header),  ram_space_pointer->packet_rx);
	if (uart_error != 0)
	{
		error = UART_ERROR;
		error_handler(error, ext_bus);
		return error;
	}
	
	//rx_pack_ptr->packet_header = *(fields_packet_header*)(ram_space_pointer->packet_rx);
	memcpy(&(rx_pack_ptr->packet_header.header), ram_space_pointer->packet_rx, sizeof(rx_pack_ptr->packet_header.header));
	if (ram_space_pointer->rx_packet_struct.packet_header.header != 0x55)
	{
		error = PACKET_ERROR;
		return error;
	}
	
	//считываем заголовок телеграммы
	uart_error = uart_read(UART_struct, sizeof(rx_pack_ptr->packet_header) - sizeof(rx_pack_ptr->packet_header.header),  (ram_space_pointer->packet_rx)+sizeof(rx_pack_ptr->packet_header.header));
	if (uart_error != 0)
	{
		error = UART_ERROR;
		error_handler(error, ext_bus);
		return error;
	}
	memcpy(&(rx_pack_ptr->packet_header), ram_space_pointer->packet_rx, sizeof(rx_pack_ptr->packet_header));
	//rx_pack_ptr->packet_header = *(fields_packet_header*)(ram_space_pointer->packet_rx);
	
	//считываем весь пакет вычисленной длины
	uart_error = uart_read(UART_struct, (rx_pack_ptr->packet_header.packet_length)-sizeof(rx_pack_ptr->packet_header)+ sizeof(rx_pack_ptr->packet_header.header)+sizeof(rx_pack_ptr->packet_tail.end), 
		(ram_space_pointer->packet_rx) + sizeof(fields_packet_header));
	if (uart_error != 0)
	{
		error = UART_ERROR;
		error_handler(error, ext_bus);
		return error;
	}
	
	//распознавание сервисного байта УМ
	//ram_space_pointer->service_byte_um = *(service_struct_um*)(rx_pack_ptr->packet_header.service_byte);
	memcpy(&(ram_space_pointer->service_byte_um), &(rx_pack_ptr->packet_header.service_byte), sizeof(rx_pack_ptr->packet_header.service_byte));
	
	//считываем хвост пакета
//	rx_pack_ptr->packet_tail = *(fields_packet_tail*)(ram_space_pointer->packet_rx + (rx_pack_ptr->packet_header.packet_length)
//		- sizeof(rx_pack_ptr->packet_tail.checksum) + sizeof(rx_pack_ptr->packet_header.header));
	memcpy(&(rx_pack_ptr->packet_tail), ram_space_pointer->packet_rx + (rx_pack_ptr->packet_header.packet_length) - sizeof(rx_pack_ptr->packet_tail.checksum) + sizeof(rx_pack_ptr->packet_header.header), sizeof(rx_pack_ptr->packet_tail));
	
	//вторая обязательная проверка - последние 2 байта должны быть 0xAA
	if (rx_pack_ptr->packet_tail.end != 0xAAAA)
	{
		error = PACKET_ERROR;
		return error;
	}
	//считываем массив команд (команда - данные)
	uint16_t buffer_offset = 0; //перемешение по буферу
	for (uint32_t i = 0; i < (rx_pack_ptr->packet_header.cmd_number); i++)
	{
		//считываем заголовок - команда, результат, данные
		memcpy(&(rx_pack_ptr->cmd_with_data[i].header), ram_space_pointer->packet_rx + sizeof(rx_pack_ptr->packet_header) + buffer_offset, sizeof(rx_pack_ptr->cmd_with_data[i].header));
		//rx_pack_ptr->cmd_with_data[i].header = *(fields_cmd_header*)(ram_space_pointer->packet_rx + sizeof(rx_pack_ptr->packet_header) + buffer_offset);
		//считываем указатель на данные
		rx_pack_ptr->cmd_with_data[i].data = ram_space_pointer->packet_rx + sizeof(rx_pack_ptr->packet_header) + buffer_offset + sizeof(rx_pack_ptr->cmd_with_data[i].header);
		buffer_offset += rx_pack_ptr->cmd_with_data[i].header.length;
	}
	
	// вычисление контрольной суммы и сравнение ее с той, что в телеграмме	
	uint32_t real_checksum = crc32((ram_space_pointer->packet_rx) + sizeof(rx_pack_ptr->packet_header.header), rx_pack_ptr->packet_header.packet_length - sizeof(rx_pack_ptr->packet_tail.checksum));
	if (real_checksum != (rx_pack_ptr->packet_tail.checksum))
	{
		error = CRC_ERROR;
		error_handler(error, ext_bus);
		return error;
	}
	
	//проверка адресации
	if(rx_pack_ptr->packet_header.receiver_addr != ram_space_pointer->common_ram_register_space.PLC_PMAddr.module_addr)
	{
		error = PM_ADDR_ERROR;
		return error;
	}
	
	//если нет ошибок
	error = NO_ERROR;
	error_handler(error, ext_bus);	

	return error;
}
/*
Функция для выполнения требуемой команды
*/
uint8_t protocol_do_cmds(uint8_t ext_bus)
{
	uint32_t offset = 0; //указатель на данные для каждой команды
	uint16_t start_addr = 0; //начальный адрес для чтения/записи регистров
	uint16_t size = 0;	//кол-во байт для чтения/записи
			
	for (uint8_t i = 0; i < ram_space_pointer->rx_packet_struct.packet_header.cmd_number; i++)
	{
		switch (ram_space_pointer->rx_packet_struct.cmd_with_data[i].header.cmd)
		{
				case TYPE:
						//по команде TYPE кладем в поле дата регистры PLC_SoftVer, PLC_Config, PLC_DeviceType, PLC_SerialNumber
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].data = (ram_space_pointer->tx_data) + offset;
						
						//*(plc_soft_ver*)((ram_space_pointer->tx_data) + offset) = ram_space_pointer->common_ram_register_space.PLC_SoftVer;
						memcpy((ram_space_pointer->tx_data) + offset, &(ram_space_pointer->common_ram_register_space.PLC_SoftVer), sizeof(ram_space_pointer->common_ram_register_space.PLC_SoftVer));
						offset += sizeof(ram_space_pointer->common_ram_register_space.PLC_SoftVer);
						memcpy((ram_space_pointer->tx_data) + offset, &(ram_space_pointer->common_ram_register_space.PLC_Config), sizeof(ram_space_pointer->common_ram_register_space.PLC_Config));
						//*(device_config*)((ram_space_pointer->tx_data) + offset) = ram_space_pointer->common_ram_register_space.PLC_Config;
						offset += sizeof(ram_space_pointer->common_ram_register_space.PLC_Config);
						memcpy((ram_space_pointer->tx_data) + offset, &(ram_space_pointer->common_ram_register_space.PLC_DeviceType), sizeof(ram_space_pointer->common_ram_register_space.PLC_DeviceType));
						//*(device_type*)((ram_space_pointer->tx_data) + offset) = ram_space_pointer->common_ram_register_space.PLC_DeviceType;
						offset += sizeof(ram_space_pointer->common_ram_register_space.PLC_DeviceType);
						memcpy((ram_space_pointer->tx_data) + offset, &(ram_space_pointer->common_ram_register_space.PLC_SerialNumber), sizeof(ram_space_pointer->common_ram_register_space.PLC_SerialNumber)  );
						//*(uint32_t*)((ram_space_pointer->tx_data) + offset) = ram_space_pointer->common_ram_register_space.PLC_SerialNumber;
						offset += sizeof(ram_space_pointer->common_ram_register_space.PLC_SerialNumber);
						
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.cmd = TYPE;
						//TODO:разобраться для чего поле result
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.result = 0;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length = sizeof(fields_cmd_header) + sizeof(ram_space_pointer->common_ram_register_space.PLC_SoftVer) + 
							sizeof(ram_space_pointer->common_ram_register_space.PLC_Config) + sizeof(ram_space_pointer->common_ram_register_space.PLC_DeviceType) + sizeof(ram_space_pointer->common_ram_register_space.PLC_SerialNumber);
						break;
					
				case INIT:
						//по команде INIT кладем в поле дата регистр PLC_SerialNumber, если выполнен ряд условий
						switch (ext_bus)
						{
								case 1:
										if((ram_space_pointer->service_byte_um.ready_to_control == 1) && (ram_space_pointer->service_byte_pm.fail_bus_1 == 0) && (ram_space_pointer->common_ram_register_space.PLC_CM_State != 0x09))
										{
											//заносим инфу в сервисный байт ПМ
											ram_space_pointer->service_byte_pm.init = 1;
											ram_space_pointer->service_byte_pm.master = 0;
											//заносим инфу в регистры
											ram_space_pointer->common_ram_register_space.PLC_CM_State = 0x04;
										}
										else
										{
											continue;
										}
										break;
										
								case 2:
										if((ram_space_pointer->service_byte_um.ready_to_control == 1) && (ram_space_pointer->service_byte_pm.fail_bus_2 == 0) && (ram_space_pointer->common_ram_register_space.PLC_CM_State != 0x04))
										{
											//заносим инфу в сервисный байт ПМ
											ram_space_pointer->service_byte_pm.init = 1;
											ram_space_pointer->service_byte_pm.master = 1;
											//заносим инфу в регистры
											ram_space_pointer->common_ram_register_space.PLC_CM_State = 0x09;
										}	
										else
										{
											continue;
										}
										break;
										
								default:
										break;
						}		
						memcpy((ram_space_pointer->tx_data) + offset, &(ram_space_pointer->common_ram_register_space.PLC_SerialNumber), sizeof(ram_space_pointer->common_ram_register_space.PLC_SerialNumber));
						//*(uint32_t*)((ram_space_pointer->tx_data) + offset) = ram_space_pointer->common_ram_register_space.PLC_SerialNumber;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].data = (ram_space_pointer->tx_data) + offset;
						offset += sizeof(ram_space_pointer->common_ram_register_space.PLC_SerialNumber);
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.cmd = INIT;
						//TODO:разобраться для чего поле result
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.result = 0;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length = sizeof(fields_cmd_header)+ sizeof(ram_space_pointer->common_ram_register_space.PLC_SerialNumber);
						break;
					
				case READ:
						memcpy(&start_addr, ram_space_pointer->rx_packet_struct.cmd_with_data[i].data, sizeof(start_addr));
						memcpy(&size, ram_space_pointer->rx_packet_struct.cmd_with_data[i].data + sizeof(start_addr), sizeof(size));
						
						//по команде READ кладем в поле дата size байт начиная с start_addr
						memcpy((ram_space_pointer->tx_data) + offset, (void*)(&(ram_space_pointer->start_struct)) + start_addr, size);
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].data = (ram_space_pointer->tx_data) + offset;
						offset += size;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.cmd = READ;
						//TODO:разобраться для чего поле result
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.result = 0;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length = sizeof(fields_cmd_header) + size;
						break;
				
				case WRITE:
						//проверка того, что установлено соединение по выбранной шине
						switch (ext_bus)
						{
								case 1:
										if(ram_space_pointer->common_ram_register_space.PLC_CM_State != 0x04)
										{
											continue;
										}
										break;
										
								case 2:
										if(ram_space_pointer->common_ram_register_space.PLC_CM_State != 0x09)
										{
											continue;
										}
										break;
										
								default:
										break;
						}
						memcpy(&start_addr, ram_space_pointer->rx_packet_struct.cmd_with_data[i].data, sizeof(start_addr));
						memcpy(&size, (ram_space_pointer->rx_packet_struct.cmd_with_data[i].data) + sizeof(start_addr), sizeof(size));
						//start_addr = *(ram_space_pointer->rx_packet_struct.cmd_with_data[i].data);
						//size = *((ram_space_pointer->rx_packet_struct.cmd_with_data[i].data) + sizeof(start_addr));
						//по команде WRITE кладем по адресу start_addr size принятых байт для записи и в поле данных кладем код команды
						memcpy((void*)(&(ram_space_pointer->start_struct)) + start_addr, (ram_space_pointer->rx_packet_struct.cmd_with_data[i].data) + sizeof(start_addr) + sizeof(size), size);
						memset((ram_space_pointer->tx_data) + offset, WRITE, 1);
						//*(uint8_t*)((ram_space_pointer->tx_data) + offset) = WRITE;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].data = (ram_space_pointer->tx_data) + offset;
						offset++;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.cmd = WRITE;
						//TODO:разобраться для чего поле result
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.result = 0;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length = sizeof(fields_cmd_header) + 1;
						break;
					
			case RESET:
					//проверка того, что установлено соединение по выбранной шине
					switch (ext_bus)
					{
							case 1:
									if(ram_space_pointer->common_ram_register_space.PLC_CM_State != 0x04)
									{
										continue;
									}
									break;
									
							case 2:
									if(ram_space_pointer->common_ram_register_space.PLC_CM_State != 0x09)
									{
										continue;
									}
									break;
									
							default:
									break;
					}
					//по команде RESET сбрасываем регистры и в поле данных кладем код команды
					init_external_ram_space();
					memset((ram_space_pointer->tx_data) + offset, RESET, 1);
					//*(uint8_t*)((ram_space_pointer->tx_data) + offset) = RESET;
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].data = (ram_space_pointer->tx_data) + offset;
					offset++;
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.cmd = RESET;
					//TODO:разобраться для чего поле result
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.result = 0;
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length = sizeof(fields_cmd_header)  + 1;
					break;
				
			case CONFIG:
					//проверка того, что установлено соединение по выбранной шине
					switch (ext_bus)
					{
							case 1:
									if(ram_space_pointer->common_ram_register_space.PLC_CM_State != 0x04)
									{
										continue;
									}
									break;
									
							case 2:
									if(ram_space_pointer->common_ram_register_space.PLC_CM_State != 0x09)
									{
										continue;
									}
									break;
									
							default:
									break;
					}
					//проверка на то, что ПМ находится в сервисном режиме
					if (ram_space_pointer->common_ram_register_space.PLC_PMAddr.module_addr != 0x00)
					{
						break;
					}
					//по команде CONFIG регистры зеркализируются в ПЗУ и в поле данных кладем код команды
					memcpy(&start_addr, ram_space_pointer->rx_packet_struct.cmd_with_data[i].data, sizeof(start_addr));
					memcpy(&size, ram_space_pointer->rx_packet_struct.cmd_with_data[i].data + sizeof(start_addr), sizeof(size));
					//start_addr = *(ram_space_pointer->rx_packet_struct.cmd_with_data[i].data);
					//size = *((ram_space_pointer->rx_packet_struct.cmd_with_data[i].data) + sizeof(start_addr));
					//запись данных в ПЗУ
					//TODO: добавить работу с ПЗУ
					memset((ram_space_pointer->tx_data) + offset, CONFIG, 1);
					//*(uint8_t*)((ram_space_pointer->tx_data) + offset) = CONFIG;
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].data = (ram_space_pointer->tx_data) + offset;
					offset++;
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.cmd = CONFIG;
					//TODO:разобраться для чего поле result
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.result = 0;
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length = sizeof(fields_cmd_header) + 1;
					break;
				
			default:
					break;
		}
		ram_space_pointer->tx_packet_struct.packet_header.packet_length += ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length;
	}
	//заполнение остальных полей
	ram_space_pointer->tx_packet_struct.packet_header.header = ram_space_pointer->rx_packet_struct.packet_header.header;
	ram_space_pointer->tx_packet_struct.packet_header.receiver_addr = ram_space_pointer->rx_packet_struct.packet_header.sender_addr;
	ram_space_pointer->tx_packet_struct.packet_header.sender_addr = ram_space_pointer->rx_packet_struct.packet_header.receiver_addr;
	ram_space_pointer->tx_packet_struct.packet_header.packet_length += sizeof(ram_space_pointer->tx_packet_struct.packet_header) - sizeof(ram_space_pointer->tx_packet_struct.packet_header.header) + 
		sizeof(ram_space_pointer->tx_packet_struct.packet_tail.checksum);
	//TODO:разобраться с полем сервисный байт
	memcpy(&(ram_space_pointer->tx_packet_struct.packet_header.service_byte), &(ram_space_pointer->service_byte_pm), sizeof(ram_space_pointer->service_byte_pm));
	ram_space_pointer->tx_packet_struct.packet_header.cmd_number = ram_space_pointer->rx_packet_struct.packet_header.cmd_number;
	
	ram_space_pointer->tx_packet_struct.packet_tail.end = ram_space_pointer->rx_packet_struct.packet_tail.end;
	
	return 0;
}
/*
Функция вычисления контрольной суммы буфера по алгоритму CRC32
*/
uint_least32_t crc32(unsigned char *buf, size_t len)
{
	uint_least32_t crc;

	crc = 0xFFFFFFFFUL;

	while (len--)
		crc = ram_space_pointer->crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

	return crc ^ 0xFFFFFFFFUL;
}
/*
Функция для заполнения таблицы CRC32
*/
void fill_crc32_table(void)
{
	uint_least32_t crc; int i, j;
	for (i = 0; i < 256; i++)
	{
		crc = i;
		for (j = 0; j < 8; j++)
			crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

		ram_space_pointer->crc_table[i] = crc;
	}
}
/*
Функция обработчика ошибок
*/
void error_handler(protocol_error error, uint8_t ext_bus)
{
	switch ((uint8_t)error)
	{
			case NO_ERROR:
					switch (ext_bus)
					{
							case 1:
									(ram_space_pointer->common_ram_register_space.PLC_CorrPackToDevice_B1)++; //увеличиваем счетчик корректно принятых пакетов
									ram_space_pointer->common_ram_register_space.PLC_ErrPackToDevice_B1 = 0;//кол-во поврежденных пакетов подряд 
									ram_space_pointer->service_byte_pm.fail_bus_1 = 0; //снятие в сервисном байте ПМ бита несиправности шины
									ram_space_pointer->common_ram_register_space.PLC_BusDefect_B1.many_fail_packet = 0; //снятие в регистре неисправности шины бита "кол-во битых пакетов больше установленного"
									ram_space_pointer->common_ram_register_space.PLC_BusDefect_B1.fail_timeout = 0; //снятие в регистре неисправности шины бита "неисправность по таймауту"
									break;
								
							case 2:
									(ram_space_pointer->common_ram_register_space.PLC_CorrPackToDevice_B2)++; //увеличиваем счетчик корректно принятых пакетов
									ram_space_pointer->common_ram_register_space.PLC_ErrPackToDevice_B2 = 0;//кол-во поврежденных пакетов подряд 
									ram_space_pointer->service_byte_pm.fail_bus_2 = 0;	//снятие в сервисном байте ПМ бита несиправности шины
									ram_space_pointer->common_ram_register_space.PLC_BusDefect_B2.many_fail_packet = 0; //снятие в регистре неисправности шины бита "кол-во битых пакетов больше установленного"
									ram_space_pointer->common_ram_register_space.PLC_BusDefect_B2.fail_timeout = 0; //снятие в регистре неисправности шины бита "неисправность по таймауту"
									break;
								
							default:
									break;
					}	
					break;
					
				case UART_ERROR:
						switch (ext_bus)
						{
								case 1:
										ram_space_pointer->service_byte_pm.fail_bus_1 = 1;	//запись в сервисный байт ПМ бита несиправности шины, если превышен таймаут
										ram_space_pointer->common_ram_register_space.PLC_BusDefect_B1.fail_timeout = 1; //запись в регистр неисправности шины бита "неисправность по таймауту"
										ram_space_pointer->common_ram_register_space.PLC_CM_State = 0x05; // снятие инициализации
										break;
								
								case 2:
										ram_space_pointer->service_byte_pm.fail_bus_2 = 1;	//запись в сервисный байт ПМ бита несиправности шины, если превышен таймаут
										ram_space_pointer->common_ram_register_space.PLC_BusDefect_B2.fail_timeout = 1; //запись в регистр неисправности шины бита "неисправность по таймауту"
										ram_space_pointer->common_ram_register_space.PLC_CM_State = 0x05; // снятие инициализации
										break;
								
								default:
										break;
						}
						break;
						
				case CRC_ERROR:
						switch (ext_bus)
						{
								case 1:
										(ram_space_pointer->common_ram_register_space.PLC_ErrPackToDevice_B1)++;//кол-во поврежденных пакетов подряд 
										if ((ram_space_pointer->common_ram_register_space.PLC_ErrPackToDevice_B1) >= (ram_space_pointer->common_ram_register_space.PLC_NumCrcErrorsForDefect_B1))
										{
											ram_space_pointer->service_byte_pm.fail_bus_1 = 1;	//запись в сервисный байт ПМ бита несиправности шины, если кол-во подряд поврежденных пакетов больше установленного
											ram_space_pointer->common_ram_register_space.PLC_BusDefect_B1.many_fail_packet = 1; //запись в регистр неисправности шины бита "кол-во битых пакетов больше установленного"
											ram_space_pointer->common_ram_register_space.PLC_CM_State = 0x05; // снятие инициализации
										}
										break;
										
								case 2:
										(ram_space_pointer->common_ram_register_space.PLC_ErrPackToDevice_B2)++;//кол-во поврежденных пакетов подряд 
										if (ram_space_pointer->common_ram_register_space.PLC_ErrPackToDevice_B2 >= ram_space_pointer->common_ram_register_space.PLC_NumCrcErrorsForDefect_B2)
										{
											ram_space_pointer->service_byte_pm.fail_bus_2 = 1;  	//запись в сервисный байт ПМ бита несиправности шины, если кол-во подряд поврежденных пакетов больше установленного
											ram_space_pointer->common_ram_register_space.PLC_BusDefect_B2.many_fail_packet = 1; //запись в регистр неисправности шины бита "кол-во битых пакетов больше установленного"
											ram_space_pointer->common_ram_register_space.PLC_CM_State = 0x05; // снятие инициализации
										}
										break;
										
								default:
										break;
						}
						break;
						
				default:
						break;
	}
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


