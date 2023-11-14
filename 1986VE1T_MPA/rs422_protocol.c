/*!
 \file
 \brief ���� � ����������� API ��������� ������ ������� �� ���������� RS-422
*/
#include "rs422_protocol.h"
#include "external_ram.h"
#include "ebc.h"
#include "leds.h"

extern uart_n uart_1;

extern ram_data *ram_space_pointer;
extern rom_data *rom_space_pointer;
/*
������� ��� �������� ������ ������
*/
protocol_error transmit_packet(uart_n *uart_struct, uint8_t ext_bus)
{	
	protocol_error error;
	fields_packet *tx_pack_ptr = &ram_space_pointer->tx_packet_struct; //��������� �� ��������� � ������������ �������
	fields_cmd *tx_pack_cmd_with_data_ptr =	&ram_space_pointer->tx_packet_struct.cmd_with_data[0]; //��������� �� ������ ������ � ������� ��� ������������ ������
	fields_packet_header *tx_pack_header_ptr = &ram_space_pointer->tx_packet_struct.packet_header; //��������� �� ��������� ������������ ������
	fields_packet_tail *tx_pack_tail_ptr = &ram_space_pointer->tx_packet_struct.packet_tail; //��������� �� ����� ������������ ������
	
	//���������� � ����� ��������� ������
	memcpy(&(ram_space_pointer->packet_tx), &(tx_pack_ptr->packet_header), sizeof(tx_pack_ptr->packet_header));
	//���������� � ����� ������ ������ (������� - ������)
	uint16_t buffer_offset = 0; //����������� �� ������
	for (uint32_t i = 0; i < (tx_pack_header_ptr->cmd_number); i++)
	{
		//���������� �������, ���������, ������
		memcpy((ram_space_pointer->packet_tx) + sizeof(tx_pack_ptr->packet_header) + buffer_offset, &((tx_pack_cmd_with_data_ptr + i)->header), sizeof((tx_pack_cmd_with_data_ptr + i)->header));
		memcpy((ram_space_pointer->packet_tx) + sizeof(tx_pack_ptr->packet_header) + buffer_offset + sizeof(fields_cmd_header), (tx_pack_cmd_with_data_ptr + i)->data, 
			((tx_pack_cmd_with_data_ptr + i)->header.length) - sizeof(fields_cmd_header));
		buffer_offset += (tx_pack_cmd_with_data_ptr + i)->header.length;
	}
	
	// ���������� ����������� �����
	tx_pack_tail_ptr->checksum = crc32((ram_space_pointer->packet_tx) + sizeof(tx_pack_header_ptr->header), tx_pack_header_ptr->packet_length - sizeof(tx_pack_tail_ptr->checksum));
	
	//���������� � ����� ����� ������
	memcpy((ram_space_pointer->packet_tx) + sizeof(tx_pack_ptr->packet_header) + buffer_offset, &(tx_pack_ptr->packet_tail), sizeof(tx_pack_ptr->packet_tail));
	
	//���������� ������ �� UART
	if (uart_write(uart_struct, ram_space_pointer->packet_tx, tx_pack_header_ptr->packet_length + sizeof(tx_pack_header_ptr->header) + sizeof(tx_pack_tail_ptr->end)) != 0)
	{
		error = UART_ERROR;
		return error;
	}
	
	//���� ��� ������
	error = NO_ERROR;
	
	return error;
}
/*
������� ��� ������ ������ ������
*/
protocol_error receive_packet(uart_n *uart_struct, uint8_t ext_bus)
{
	fields_packet *rx_pack_ptr = &ram_space_pointer->rx_packet_struct; //��������� �� ��������� � ����������� �������
	fields_packet_header *rx_pack_header_ptr = &ram_space_pointer->rx_packet_struct.packet_header; //��������� �� ��������� ������������ ������
	fields_packet_tail *rx_pack_tail_ptr = &ram_space_pointer->rx_packet_struct.packet_tail; //��������� �� ��������� ������������ ������
	fields_cmd *rx_pack_cmd_with_data_ptr =	&ram_space_pointer->rx_packet_struct.cmd_with_data[0]; //��������� �� ������ ������ � ������� ��� ������������ ������
	uint32_t current_rx_packet = 0;
	
	protocol_error error;
	uart_errors uart_error;
	uint8_t packet_head;
	
	//������� ���� �������� ������ ��������� � ����������� � ������������ �������
	memset(rx_pack_ptr, 0, sizeof(ram_space_pointer->rx_packet_struct) + sizeof(ram_space_pointer->tx_packet_struct) + 
		sizeof(ram_space_pointer->tx_data) + sizeof(ram_space_pointer->packet_tx) + sizeof(ram_space_pointer->packet_rx));
	
	//����� � ����� ����� ������� � ���������� ��������� ����� � ������ ��������� UART
	while (1)
	{
		if (uart_read_pos(uart_struct) < UART_BUFFER_SIZE)
		{
			memcpy(&packet_head, (uint8_t*)(GET_UART_BUF_PTR + uart_read_pos(uart_struct)), sizeof(packet_head));
		}
		else
		{
			memcpy(&packet_head, (uint8_t*)(GET_UART_BUF_PTR), sizeof(packet_head));
		}
		
		//���� ��� ���� ��������� ����� �� ������, � ������ ���������� ������ (0x55) � ������ ���, ��� �������� ��� ������� ����������� ����� �������� ��������� � ������� �� �����
		if ((current_rx_packet != 0) && (packet_head != PACKET_HEAD))
		{
			uart_clean(uart_struct);
			break;
		}
		
		//��������� ������ ���� � ��������� �� 0x55
		uart_error = uart_read(uart_struct, sizeof(rx_pack_header_ptr->header), ram_space_pointer->packet_rx);
		if (uart_error != 0)
		{
			error = UART_ERROR;
			rx_error_handler(error, ext_bus);
			return error;
		}
		
		memcpy(&(rx_pack_header_ptr->header), ram_space_pointer->packet_rx, sizeof(rx_pack_header_ptr->header));
		if (rx_pack_header_ptr->header != PACKET_HEAD)
		{
			error = PACKET_ERROR;
			rx_error_handler(error, ext_bus);
			return error;
		}
		
		//��������� ��������� ����������
		uart_error = uart_read(uart_struct, sizeof(rx_pack_ptr->packet_header) - sizeof(rx_pack_header_ptr->header),  (ram_space_pointer->packet_rx) + sizeof(rx_pack_header_ptr->header));
		if (uart_error != 0)
		{
			error = UART_ERROR;
			rx_error_handler(error, ext_bus);
			return error;
		}
		memcpy(rx_pack_header_ptr, ram_space_pointer->packet_rx, sizeof(rx_pack_ptr->packet_header));
		
		//������ ����� ������
		if (rx_pack_header_ptr->packet_length > UART_BUFFER_SIZE)
		{
			error = PACKET_ERROR;
			rx_error_handler(error, ext_bus);
			return error;
		}
		
		//��������� ���� ����� ����������� �����
		uart_error = uart_read(uart_struct, (rx_pack_header_ptr->packet_length) - sizeof(rx_pack_ptr->packet_header) + sizeof(rx_pack_header_ptr->header) + sizeof(rx_pack_tail_ptr->end), 
			(ram_space_pointer->packet_rx) + sizeof(fields_packet_header));
		if (uart_error != 0)
		{
			error = UART_ERROR;
			rx_error_handler(error, ext_bus);
			return error;
		}

		//��������� ����� ������
		memcpy(&(rx_pack_ptr->packet_tail), ram_space_pointer->packet_rx + (rx_pack_header_ptr->packet_length) - sizeof(rx_pack_tail_ptr->checksum) + sizeof(rx_pack_header_ptr->header), sizeof(rx_pack_ptr->packet_tail));
		
		//������ ������������ �������� - ��������� 2 ����� ������ ���� 0xAA
		if (rx_pack_tail_ptr->end != PACKET_TAIL)
		{
			error = PACKET_ERROR;
			rx_error_handler(error, ext_bus);
			return error;
		}
			
		current_rx_packet++;
	}
	
	//������������� ���������� ����� ��
	memcpy(&(ram_space_pointer->service_byte_um), &(rx_pack_header_ptr->service_byte), sizeof(rx_pack_header_ptr->service_byte));
	//��������� ���������� ����� ��
	um_service_byte_handler(ext_bus);
	//��������� ������ ������ (������� - ������)
	uint16_t buffer_offset = 0; //����������� �� ������
	for (uint32_t i = 0; i < (rx_pack_header_ptr->cmd_number); i++)
	{
		//��������� ��������� - �������, ���������, ������
		memcpy(&((rx_pack_cmd_with_data_ptr + i)->header), ram_space_pointer->packet_rx + sizeof(rx_pack_ptr->packet_header) + buffer_offset, sizeof((rx_pack_cmd_with_data_ptr + i)->header));
		//������ ����� �������
		if ((rx_pack_cmd_with_data_ptr + i)->header.length > UART_BUFFER_SIZE)
		{
			error = PACKET_ERROR;
			return error;
		}
		//��������� ��������� �� ������
		(rx_pack_cmd_with_data_ptr + i)->data = ram_space_pointer->packet_rx + sizeof(rx_pack_ptr->packet_header) + buffer_offset + sizeof((rx_pack_cmd_with_data_ptr + i)->header);
		buffer_offset += (rx_pack_cmd_with_data_ptr + i)->header.length;
	}
	
	// ���������� ����������� ����� � ��������� �� � ���, ��� � ����������	
	uint32_t real_checksum = crc32((ram_space_pointer->packet_rx) + sizeof(rx_pack_header_ptr->header), rx_pack_header_ptr->packet_length - sizeof(rx_pack_tail_ptr->checksum));
	if (real_checksum != (rx_pack_tail_ptr->checksum))
	{
		error = CRC_ERROR;
		rx_error_handler(error, ext_bus);
		return error;
	}
	
	//�������� ���������
	if(rx_pack_header_ptr->receiver_addr != ram_space_pointer->common_ram_register_space.PLC_PMAddr.module_addr)
	{
		error = PM_ADDR_ERROR;
		return error;
	}
	
	//���� ��� ������
	error = NO_ERROR;
	rx_error_handler(error, ext_bus);	

	return error;
}
/*
������� ��� ���������� ��������� �������
*/
uint8_t protocol_do_cmds(uint8_t ext_bus)
{
	common_ram_registers *common_ram_reg_space_ptr = &ram_space_pointer->common_ram_register_space; //��������� �� ������� ������ �������� ��� � ������ ����������
	fields_cmd *tx_pack_cmd_with_data_ptr =	&ram_space_pointer->tx_packet_struct.cmd_with_data[0]; //��������� �� ������ ������ � ������� ��� ������������� ������
	fields_cmd *rx_pack_cmd_with_data_ptr =	&ram_space_pointer->rx_packet_struct.cmd_with_data[0]; //��������� �� ������ ������ � ������� ��� ������������ ������
	fields_packet_header *tx_pack_header = &ram_space_pointer->tx_packet_struct.packet_header; //��������� �� ��������� ������������� ������
	fields_packet_header *rx_pack_header = &ram_space_pointer->rx_packet_struct.packet_header; //��������� �� ��������� ������������ ������
	fields_packet_tail *tx_pack_tail = &ram_space_pointer->tx_packet_struct.packet_tail; //��������� �� ����� ������������� ������
	fields_packet_tail *rx_pack_tail = &ram_space_pointer->rx_packet_struct.packet_tail; //��������� �� ����� ��������� ������
	common_rom_registers 	common_regs;	//��������� ��������� � ������ ���������� ��� �������� � ���
	mpa_rom_registers			mpa_regs;	//��������� ��������� � ���������� ��� ��� �������� � ���
	uint32_t offset = 0; //��������� �� ������ ��� ������ �������
	uint16_t start_addr = 0; //��������� ����� ��� ������/������ ���������
	uint16_t size = 0;	//���-�� ���� ��� ������/������
			
	for (uint8_t i = 0; i < rx_pack_header->cmd_number; i++)
	{
		switch ((rx_pack_cmd_with_data_ptr + i)->header.cmd)
		{
				case TYPE_CMD:
						//�� ������� TYPE ������ � ���� ���� �������� PLC_SoftVer, PLC_Config, PLC_DeviceType, PLC_SerialNumber
						(tx_pack_cmd_with_data_ptr + i)->data = (ram_space_pointer->tx_data) + offset;
						
						memcpy((ram_space_pointer->tx_data) + offset, &(common_ram_reg_space_ptr->PLC_SoftVer), sizeof(common_ram_reg_space_ptr->PLC_SoftVer));
						offset += sizeof(common_ram_reg_space_ptr->PLC_SoftVer);
						memcpy((ram_space_pointer->tx_data) + offset, &(common_ram_reg_space_ptr->PLC_Config), sizeof(common_ram_reg_space_ptr->PLC_Config));
						offset += sizeof(common_ram_reg_space_ptr->PLC_Config);
						memcpy((ram_space_pointer->tx_data) + offset, &(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_DeviceType), sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_DeviceType));
						offset += sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_DeviceType);
						memcpy((ram_space_pointer->tx_data) + offset, &(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber), sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber));
						offset += sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber);
						
						(tx_pack_cmd_with_data_ptr + i)->header.cmd = TYPE_CMD;
						//TODO:����������� ��� ���� ���� result
						(tx_pack_cmd_with_data_ptr + i)->header.result = 0;
						(tx_pack_cmd_with_data_ptr + i)->header.length = sizeof(fields_cmd_header) + sizeof(common_ram_reg_space_ptr->PLC_SoftVer) + 
							sizeof(ram_space_pointer->common_ram_register_space.PLC_Config) + sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_DeviceType) + sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber);
						break;
					
				case INIT_CMD:
						//�� ������� INIT ������ � ���� ���� ������� PLC_SerialNumber, ���� �������� ��� �������
						switch (ext_bus)
						{
								case 1:
										if((ram_space_pointer->service_byte_um.ready_to_control == 1) && (ram_space_pointer->service_byte_pm.fail_bus_1 == 0) && (common_ram_reg_space_ptr->PLC_CM_State != PLC_CM_INIT_2_BUS))
										{
											//������� ���� � ��������� ���� ��
											ram_space_pointer->service_byte_pm.init = 1;
											ram_space_pointer->service_byte_pm.master = 0;
											//������� ���� � ��������
											common_ram_reg_space_ptr->PLC_CM_State = PLC_CM_INIT_1_BUS;
										}
										else
										{
											continue;
										}
										break;
										
								case 2:
										if((ram_space_pointer->service_byte_um.ready_to_control == 1) && (ram_space_pointer->service_byte_pm.fail_bus_2 == 0) && (common_ram_reg_space_ptr->PLC_CM_State != PLC_CM_INIT_1_BUS))
										{
											//������� ���� � ��������� ���� ��
											ram_space_pointer->service_byte_pm.init = 1;
											ram_space_pointer->service_byte_pm.master = 1;
											//������� ���� � ��������
											common_ram_reg_space_ptr->PLC_CM_State = PLC_CM_INIT_2_BUS;
										}	
										else
										{
											continue;
										}
										break;
										
								default:
										break;
						}		
						memcpy((ram_space_pointer->tx_data) + offset, &(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber), sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber));
						(tx_pack_cmd_with_data_ptr + i)->data = (ram_space_pointer->tx_data) + offset;
						offset += sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber);
						(tx_pack_cmd_with_data_ptr + i)->header.cmd = INIT_CMD;
						//TODO:����������� ��� ���� ���� result
						(tx_pack_cmd_with_data_ptr + i)->header.result = 0;
						(tx_pack_cmd_with_data_ptr + i)->header.length = sizeof(fields_cmd_header)+ sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber);
						break;
					
				case READ_CMD:
						memcpy(&start_addr, (rx_pack_cmd_with_data_ptr + i)->data, sizeof(start_addr));
						memcpy(&size, (rx_pack_cmd_with_data_ptr + i)->data + sizeof(start_addr), sizeof(size));
						
						//�� ������� READ ������ � ���� ���� size ���� ������� � start_addr
						memcpy((ram_space_pointer->tx_data) + offset, (void*)(&(ram_space_pointer->start_struct)) + start_addr, size);
						(tx_pack_cmd_with_data_ptr + i)->data = (ram_space_pointer->tx_data) + offset;
						offset += size;
						(tx_pack_cmd_with_data_ptr + i)->header.cmd = READ_CMD;
						//TODO:����������� ��� ���� ���� result
						(tx_pack_cmd_with_data_ptr + i)->header.result = 0;
						(tx_pack_cmd_with_data_ptr + i)->header.length = sizeof(fields_cmd_header) + size;
						break;
				
				case WRITE_CMD:
						//�������� ����, ��� ����������� ���������� �� ��������� ����
						switch (ext_bus)
						{
								case 1:
										if(ram_space_pointer->common_ram_register_space.PLC_CM_State != PLC_CM_INIT_1_BUS)
										{
											continue;
										}
										break;
										
								case 2:
										if(ram_space_pointer->common_ram_register_space.PLC_CM_State != PLC_CM_INIT_2_BUS)
										{
											continue;
										}
										break;
										
								default:
										break;
						}
						memcpy(&start_addr, (rx_pack_cmd_with_data_ptr + i)->data, sizeof(start_addr));
						memcpy(&size, ((rx_pack_cmd_with_data_ptr + i)->data) + sizeof(start_addr), sizeof(size));
						//�� ������� WRITE ������ �� ������ start_addr size �������� ���� ��� ������ � � ���� ������ ������ ��� �������
						memcpy((void*)(&(ram_space_pointer->start_struct)) + start_addr, ((rx_pack_cmd_with_data_ptr + i)->data) + sizeof(start_addr) + sizeof(size), size);
						memset((ram_space_pointer->tx_data) + offset, WRITE_CMD, 1);
						(tx_pack_cmd_with_data_ptr + i)->data = (ram_space_pointer->tx_data) + offset;
						offset++;
						(tx_pack_cmd_with_data_ptr + i)->header.cmd = WRITE_CMD;
						//TODO:����������� ��� ���� ���� result
						(tx_pack_cmd_with_data_ptr + i)->header.result = 0;
						(tx_pack_cmd_with_data_ptr + i)->header.length = sizeof(fields_cmd_header) + 1;
						break;
					
			case RESET_CMD:
					//�������� ����, ��� ����������� ���������� �� ��������� ����
					switch (ext_bus)
					{
							case 1:
									if(common_ram_reg_space_ptr->PLC_CM_State != PLC_CM_INIT_1_BUS)
									{
										continue;
									}
									break;
									
							case 2:
									if(common_ram_reg_space_ptr->PLC_CM_State != PLC_CM_INIT_2_BUS)
									{
										continue;
									}
									break;
									
							default:
									break;
					}
					//�� ������� RESET ���������� �������� � � ���� ������ ������ ��� �������
					init_external_ram_space();
					memset((ram_space_pointer->tx_data) + offset, RESET, 1);
					(tx_pack_cmd_with_data_ptr + i)->data = (ram_space_pointer->tx_data) + offset;
					offset++;
					(tx_pack_cmd_with_data_ptr + i)->header.cmd = RESET;
					//TODO:����������� ��� ���� ���� result
					(tx_pack_cmd_with_data_ptr + i)->header.result = 0;
					(tx_pack_cmd_with_data_ptr + i)->header.length = sizeof(fields_cmd_header)  + 1;
					break;
				
			case CONFIG:
					//�������� ����, ��� ����������� ���������� �� ��������� ����
					switch (ext_bus)
					{
							case 1:
									if(common_ram_reg_space_ptr->PLC_CM_State != PLC_CM_INIT_1_BUS)
									{
										continue;
									}
									break;
									
							case 2:
									if(common_ram_reg_space_ptr->PLC_CM_State != PLC_CM_INIT_2_BUS)
									{
										continue;
									}
									break;
									
							default:
									break;
					}
					//�������� �� ��, ��� �� ��������� � ��������� ������
					if (common_ram_reg_space_ptr->PLC_PMAddr.module_addr != 0x00)
					{
						break;
					}
					//�� ������� CONFIG �������� ��������������� � ��� � � ���� ������ ������ ��� �������
					memcpy(&start_addr, (rx_pack_cmd_with_data_ptr + i)->data, sizeof(start_addr));
					memcpy(&size, (rx_pack_cmd_with_data_ptr + i)->data + sizeof(start_addr), sizeof(size));
					#ifdef ROM_IS_USED
						//������ ������ � ���
						memcpy(&common_regs, &common_ram_reg_space_ptr->PLC_CommonRomRegs, sizeof(common_regs));
						memcpy(&mpa_regs, &ram_space_pointer->mpa_ram_register_space.AI_RomRegs, sizeof(mpa_regs));
						ebc_init(EBC_ROM);
						memcpy_to_rom(ROM_REGISTER_SPACE_START_ADDR, &common_regs, sizeof(common_regs));
						memcpy_to_rom(ROM_REGISTER_SPACE_START_ADDR + sizeof(common_regs), &mpa_regs, sizeof(mpa_regs));
						ebc_init(EBC_RAM);
					#endif
					
					memset((ram_space_pointer->tx_data) + offset, CONFIG, 1);
					(tx_pack_cmd_with_data_ptr + i)->data = (ram_space_pointer->tx_data) + offset;
					offset++;
					(tx_pack_cmd_with_data_ptr + i)->header.cmd = CONFIG;
					//TODO:����������� ��� ���� ���� result
					(tx_pack_cmd_with_data_ptr + i)->header.result = 0;
					(tx_pack_cmd_with_data_ptr + i)->header.length = sizeof(fields_cmd_header) + 1;
					break;
				
			default:
					break;
		}
		tx_pack_header->packet_length += (tx_pack_cmd_with_data_ptr + i)->header.length;
	}
	//���������� ��������� �����
	tx_pack_header->header = rx_pack_header->header;
	tx_pack_header->receiver_addr = rx_pack_header->sender_addr;
	tx_pack_header->sender_addr = rx_pack_header->receiver_addr;
	tx_pack_header->packet_length += sizeof(ram_space_pointer->tx_packet_struct.packet_header) - sizeof(tx_pack_header->header) + 
		sizeof(tx_pack_tail->checksum);
	//TODO:����������� � ����� ��������� ����
	memcpy(&(tx_pack_header->service_byte), &(ram_space_pointer->service_byte_pm), sizeof(ram_space_pointer->service_byte_pm));
	tx_pack_header->cmd_number = rx_pack_header->cmd_number;
	
	tx_pack_tail->end = rx_pack_tail->end;
	
	return 0;
}
/*
������� ���������� ����������� ����� ������ �� ��������� CRC32
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
������� ��� ���������� ������� CRC32
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
������� ����������� ������ ������ �������
*/
void rx_error_handler(protocol_error error, uint8_t ext_bus)
{
	common_ram_registers *common_ram_reg_space_ptr = &ram_space_pointer->common_ram_register_space; //��������� �� ������� ������ �������� ��� � ������ ����������
	
	switch ((uint8_t)error)
	{
			case NO_ERROR:
					SET_LED_OK_WORK()
					RESET_LED_ERROR_WORK()
					switch (ext_bus)
					{
							case 1:
									common_ram_reg_space_ptr->PLC_CorrPackToDevice_B1++; //����������� ������� ��������� �������� �������
									common_ram_reg_space_ptr->PLC_ErrPackToDevice_B1 = 0;//���-�� ������������ ������� ������ 
									ram_space_pointer->service_byte_pm.fail_bus_1 = 0; //������ � ��������� ����� �� ���� ������������� ����
									common_ram_reg_space_ptr->PLC_BusDefect_B1.many_fail_packet = 0; //������ � �������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
									common_ram_reg_space_ptr->PLC_BusDefect_B1.fail_timeout = 0; //������ � �������� ������������� ���� ���� "������������� �� ��������"
									break;
								
							case 2:
									common_ram_reg_space_ptr->PLC_CorrPackToDevice_B2++; //����������� ������� ��������� �������� �������
									common_ram_reg_space_ptr->PLC_ErrPackToDevice_B2 = 0;//���-�� ������������ ������� ������ 
									ram_space_pointer->service_byte_pm.fail_bus_2 = 0;	//������ � ��������� ����� �� ���� ������������� ����
									common_ram_reg_space_ptr->PLC_BusDefect_B2.many_fail_packet = 0; //������ � �������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
									common_ram_reg_space_ptr->PLC_BusDefect_B2.fail_timeout = 0; //������ � �������� ������������� ���� ���� "������������� �� ��������"
									break;
								
							default:
									break;
					}	
					break;
					
				case UART_ERROR:
						RESET_LED_OK_WORK()
						SET_LED_ERROR_WORK()
				
						switch (ext_bus)
						{
								case 1:
										ram_space_pointer->service_byte_pm.fail_bus_1 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� �������� �������
										common_ram_reg_space_ptr->PLC_BusDefect_B1.fail_timeout = 1; //������ � ������� ������������� ���� ���� "������������� �� ��������"
										common_ram_reg_space_ptr->PLC_CM_State = PLC_CM_REMOVE_INIT; // ������ �������������
										break;
								
								case 2:
										ram_space_pointer->service_byte_pm.fail_bus_2 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� �������� �������
										common_ram_reg_space_ptr->PLC_BusDefect_B2.fail_timeout = 1; //������ � ������� ������������� ���� ���� "������������� �� ��������"
										common_ram_reg_space_ptr->PLC_CM_State = PLC_CM_REMOVE_INIT; // ������ �������������
										break;
								
								default:
										break;
						}
						break;
						
				case CRC_ERROR: case PACKET_ERROR:
						RESET_LED_OK_WORK()
						SET_LED_ERROR_WORK()
				
						switch (ext_bus)
						{
								case 1:
										common_ram_reg_space_ptr->PLC_ErrPackToDevice_B1++;//���-�� ������������ ������� ������ 
										if (common_ram_reg_space_ptr->PLC_ErrPackToDevice_B1 >= common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_NumCrcErrorsForDefect_B1)
										{
											ram_space_pointer->service_byte_pm.fail_bus_1 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� ���-�� ������ ������������ ������� ������ ��������������
											common_ram_reg_space_ptr->PLC_BusDefect_B1.many_fail_packet = 1; //������ � ������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
											common_ram_reg_space_ptr->PLC_CM_State = PLC_CM_REMOVE_INIT; // ������ �������������
										}
										break;
										
								case 2:
										common_ram_reg_space_ptr->PLC_ErrPackToDevice_B2++;//���-�� ������������ ������� ������ 
										if (common_ram_reg_space_ptr->PLC_ErrPackToDevice_B2 >= common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_NumCrcErrorsForDefect_B2)
										{
											ram_space_pointer->service_byte_pm.fail_bus_2 = 1;  	//������ � ��������� ���� �� ���� ������������� ����, ���� ���-�� ������ ������������ ������� ������ ��������������
											common_ram_reg_space_ptr->PLC_BusDefect_B2.many_fail_packet = 1; //������ � ������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
											common_ram_reg_space_ptr->PLC_CM_State = PLC_CM_REMOVE_INIT; // ������ �������������
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
������� ����������� ���������� ����� ��
*/
void um_service_byte_handler(uint8_t ext_bus)
{
	common_ram_registers *common_ram_reg_space_ptr = &ram_space_pointer->common_ram_register_space; //��������� �� ������� ������ �������� ��� � ������ ����������

	switch (ram_space_pointer->service_byte_um.last_answer)
	{
			case 0:
					switch (ext_bus)
					{
							case 1:
									if((ram_space_pointer->service_byte_um.ready_to_control == 0) && (common_ram_reg_space_ptr->PLC_CM_State == PLC_CM_INIT_1_BUS))
									{
										//����� �������������
										common_ram_reg_space_ptr->PLC_CM_State = PLC_CM_REMOVE_INIT;
										ram_space_pointer->service_byte_pm.init = 0;
									}
									common_ram_reg_space_ptr->PLC_CorrPackFromDevice_B1++; //����������� ������� ��������� ������������ �������
									break;
								
							case 2:
									if((ram_space_pointer->service_byte_um.ready_to_control == 0) && (common_ram_reg_space_ptr->PLC_CM_State == PLC_CM_INIT_2_BUS))
									{
										//����� �������������
										common_ram_reg_space_ptr->PLC_CM_State = PLC_CM_REMOVE_INIT;
										ram_space_pointer->service_byte_pm.init = 0;
									}	
									common_ram_reg_space_ptr->PLC_CorrPackFromDevice_B2++; //����������� ������� ��������� ������������ �������
									break;
								
							default:
									break;
					}	
					break;
					
				case 1:
						switch (ext_bus)
						{
								case 1:
										if((ram_space_pointer->service_byte_um.ready_to_control == 0) && (common_ram_reg_space_ptr->PLC_CM_State == PLC_CM_INIT_1_BUS))
										{
											//����� �������������
											common_ram_reg_space_ptr->PLC_CM_State = PLC_CM_REMOVE_INIT;
											ram_space_pointer->service_byte_pm.init = 0;
										}
										common_ram_reg_space_ptr->PLC_ErrPackFromDevice_B1++; //����������� ������� �������� ������������ �������
										break;
								
								case 2:
										if((ram_space_pointer->service_byte_um.ready_to_control == 0) && (common_ram_reg_space_ptr->PLC_CM_State == PLC_CM_INIT_2_BUS))
										{
											//����� �������������
											common_ram_reg_space_ptr->PLC_CM_State = PLC_CM_REMOVE_INIT;
											ram_space_pointer->service_byte_pm.init = 0;
										}	
										common_ram_reg_space_ptr->PLC_ErrPackFromDevice_B2++; //����������� ������� �������� ������������ �������
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
������� ��� �������������� ����� ��� �������� � ����
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
������� ��� �������������� ��������� ��� �������� � ����
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
