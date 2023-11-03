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
	fields_packet *tx_pack_ptr = &(ram_space_pointer->tx_packet_struct); //��������� �� ��������� � ������������ �������
	
	//���������� � ����� ��������� ������
	memcpy(&(ram_space_pointer->packet_tx), &(tx_pack_ptr->packet_header), sizeof(tx_pack_ptr->packet_header));
	//���������� � ����� ������ ������ (������� - ������)
	uint16_t buffer_offset = 0; //����������� �� ������
	for (uint32_t i = 0; i < ((tx_pack_ptr->packet_header).cmd_number); i++)
	{
		//���������� �������, ���������, ������
		memcpy((ram_space_pointer->packet_tx) + sizeof(tx_pack_ptr->packet_header) + buffer_offset, &(tx_pack_ptr->cmd_with_data[i].header), sizeof(tx_pack_ptr->cmd_with_data[i].header));
		memcpy((ram_space_pointer->packet_tx) + sizeof(tx_pack_ptr->packet_header) + buffer_offset + sizeof(fields_cmd_header), tx_pack_ptr->cmd_with_data[i].data, (tx_pack_ptr->cmd_with_data[i].header.length) - sizeof(fields_cmd_header));
		buffer_offset += tx_pack_ptr->cmd_with_data[i].header.length;
	}
	
	// ���������� ����������� �����
	tx_pack_ptr->packet_tail.checksum = crc32((ram_space_pointer->packet_tx) + sizeof(tx_pack_ptr->packet_header.header), tx_pack_ptr->packet_header.packet_length - sizeof(tx_pack_ptr->packet_tail.checksum));
	
	//���������� � ����� ����� ������
	memcpy((ram_space_pointer->packet_tx) + sizeof(tx_pack_ptr->packet_header) + buffer_offset, &(tx_pack_ptr->packet_tail), sizeof(tx_pack_ptr->packet_tail));
	
	//���������� ������ �� UART
	if (uart_write(uart_struct, ram_space_pointer->packet_tx, tx_pack_ptr->packet_header.packet_length + sizeof(tx_pack_ptr->packet_header.header) + sizeof(tx_pack_ptr->packet_tail.end)) != 0)
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
	fields_packet *rx_pack_ptr = &(ram_space_pointer->rx_packet_struct); //��������� �� ��������� � ����������� �������
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
		if (uart_struct->read_pos < UART_BUFFER_SIZE)
		{
			memcpy(&packet_head, (uint8_t*)(uart_struct->buffer + uart_struct->read_pos), sizeof(packet_head));
		}
		else
		{
			memcpy(&packet_head, (uint8_t*)(uart_struct->buffer), sizeof(packet_head));
		}
		
		///@todo
		///1. Add head (0x55) and tail (0xaaaa) definitions.
		//���� ��� ���� ��������� ����� �� ������, � ������ ���������� ������ (0x55) � ������ ���, ��� �������� ��� ������� ����������� ����� �������� ��������� � ������� �� �����
		if ((current_rx_packet != 0) && (packet_head != 0x55))
		{
			uart_clean(uart_struct);
			break;
		}
		
		//��������� ������ ���� � ��������� �� 0x55
		uart_error = uart_read(uart_struct, sizeof(rx_pack_ptr->packet_header.header),  ram_space_pointer->packet_rx);
		if (uart_error != 0)
		{
			error = UART_ERROR;
			error_handler(error, ext_bus);
			return error;
		}
		
		memcpy(&(rx_pack_ptr->packet_header.header), ram_space_pointer->packet_rx, sizeof(rx_pack_ptr->packet_header.header));
		if (ram_space_pointer->rx_packet_struct.packet_header.header != 0x55)
		{
			error = PACKET_ERROR;
			return error;
		}
		
		//��������� ��������� ����������
		uart_error = uart_read(uart_struct, sizeof(rx_pack_ptr->packet_header) - sizeof(rx_pack_ptr->packet_header.header),  (ram_space_pointer->packet_rx)+sizeof(rx_pack_ptr->packet_header.header));
		if (uart_error != 0)
		{
			error = UART_ERROR;
			error_handler(error, ext_bus);
			return error;
		}
		memcpy(&(rx_pack_ptr->packet_header), ram_space_pointer->packet_rx, sizeof(rx_pack_ptr->packet_header));
		
		//��������� ���� ����� ����������� �����
		uart_error = uart_read(uart_struct, (rx_pack_ptr->packet_header.packet_length)-sizeof(rx_pack_ptr->packet_header)+ sizeof(rx_pack_ptr->packet_header.header)+sizeof(rx_pack_ptr->packet_tail.end), 
			(ram_space_pointer->packet_rx) + sizeof(fields_packet_header));
		if (uart_error != 0)
		{
			error = UART_ERROR;
			error_handler(error, ext_bus);
			return error;
		}

		//��������� ����� ������
		memcpy(&(rx_pack_ptr->packet_tail), ram_space_pointer->packet_rx + (rx_pack_ptr->packet_header.packet_length) - sizeof(rx_pack_ptr->packet_tail.checksum) + sizeof(rx_pack_ptr->packet_header.header), sizeof(rx_pack_ptr->packet_tail));
		
		//������ ������������ �������� - ��������� 2 ����� ������ ���� 0xAA
		if (rx_pack_ptr->packet_tail.end != 0xAAAA)
		{
			error = PACKET_ERROR;
			return error;
		}
			
		current_rx_packet++;
	}
	
	///@todo
	///1. ���������� ��������� ��������� ���������� ����� (ready_to_get_control)
	///2. ������ ������� ������� � ��������� �������.
	//������������� ���������� ����� ��
	memcpy(&(ram_space_pointer->service_byte_um), &(rx_pack_ptr->packet_header.service_byte), sizeof(rx_pack_ptr->packet_header.service_byte));
	//��������� ������ ������ (������� - ������)
	uint16_t buffer_offset = 0; //����������� �� ������
	for (uint32_t i = 0; i < (rx_pack_ptr->packet_header.cmd_number); i++)
	{
		//��������� ��������� - �������, ���������, ������
		memcpy(&(rx_pack_ptr->cmd_with_data[i].header), ram_space_pointer->packet_rx + sizeof(rx_pack_ptr->packet_header) + buffer_offset, sizeof(rx_pack_ptr->cmd_with_data[i].header));
		//��������� ��������� �� ������
		rx_pack_ptr->cmd_with_data[i].data = ram_space_pointer->packet_rx + sizeof(rx_pack_ptr->packet_header) + buffer_offset + sizeof(rx_pack_ptr->cmd_with_data[i].header);
		buffer_offset += rx_pack_ptr->cmd_with_data[i].header.length;
	}
	
	// ���������� ����������� ����� � ��������� �� � ���, ��� � ����������	
	uint32_t real_checksum = crc32((ram_space_pointer->packet_rx) + sizeof(rx_pack_ptr->packet_header.header), rx_pack_ptr->packet_header.packet_length - sizeof(rx_pack_ptr->packet_tail.checksum));
	if (real_checksum != (rx_pack_ptr->packet_tail.checksum))
	{
		error = CRC_ERROR;
		error_handler(error, ext_bus);
		return error;
	}
	
	//�������� ���������
	if(rx_pack_ptr->packet_header.receiver_addr != ram_space_pointer->common_ram_register_space.PLC_PMAddr.module_addr)
	{
		error = PM_ADDR_ERROR;
		return error;
	}
	
	//���� ��� ������
	error = NO_ERROR;
	error_handler(error, ext_bus);	

	return error;
}
/*
������� ��� ���������� ��������� �������
*/
uint8_t protocol_do_cmds(uint8_t ext_bus)
{
	///@todo
	///1. ������� ������ �������� ����������.
	common_ram_registers *common_ram_reg_space_ptr = &ram_space_pointer->common_ram_register_space; //��������� �� ������� ������ �������� ��� � ������ ����������
	common_rom_registers 	common_regs;	//��������� ��������� � ������ ���������� ��� �������� � ���
	mpa_rom_registers			mpa_regs;	//��������� ��������� � ���������� ��� ��� �������� � ���
	uint32_t offset = 0; //��������� �� ������ ��� ������ �������
	uint16_t start_addr = 0; //��������� ����� ��� ������/������ ���������
	uint16_t size = 0;	//���-�� ���� ��� ������/������
			
	for (uint8_t i = 0; i < ram_space_pointer->rx_packet_struct.packet_header.cmd_number; i++)
	{
		switch (ram_space_pointer->rx_packet_struct.cmd_with_data[i].header.cmd)
		{
				case TYPE_CMD:
						//�� ������� TYPE ������ � ���� ���� �������� PLC_SoftVer, PLC_Config, PLC_DeviceType, PLC_SerialNumber
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].data = (ram_space_pointer->tx_data) + offset;
						
						memcpy((ram_space_pointer->tx_data) + offset, &(common_ram_reg_space_ptr->PLC_SoftVer), sizeof(common_ram_reg_space_ptr->PLC_SoftVer));
						offset += sizeof(common_ram_reg_space_ptr->PLC_SoftVer);
						memcpy((ram_space_pointer->tx_data) + offset, &(common_ram_reg_space_ptr->PLC_Config), sizeof(common_ram_reg_space_ptr->PLC_Config));
						offset += sizeof(common_ram_reg_space_ptr->PLC_Config);
						memcpy((ram_space_pointer->tx_data) + offset, &(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_DeviceType), sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_DeviceType));
						offset += sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_DeviceType);
						memcpy((ram_space_pointer->tx_data) + offset, &(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber), sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber));
						offset += sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber);
						
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.cmd = TYPE_CMD;
						//TODO:����������� ��� ���� ���� result
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.result = 0;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length = sizeof(fields_cmd_header) + sizeof(common_ram_reg_space_ptr->PLC_SoftVer) + 
							sizeof(ram_space_pointer->common_ram_register_space.PLC_Config) + sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_DeviceType) + sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber);
						break;
					
				case INIT_CMD:
						//�� ������� INIT ������ � ���� ���� ������� PLC_SerialNumber, ���� �������� ��� �������
						switch (ext_bus)
						{cmd_with_data
								case 1:
										if((ram_space_pointer->service_byte_um.ready_to_control == 1) && (ram_space_pointer->service_byte_pm.fail_bus_1 == 0) && (common_ram_reg_space_ptr->PLC_CM_State != 0x09))
										{
											//������� ���� � ��������� ���� ��
											ram_space_pointer->service_byte_pm.init = 1;
											ram_space_pointer->service_byte_pm.master = 0;
											//������� ���� � ��������
											common_ram_reg_space_ptr->PLC_CM_State = 0x04;
										}
										else
										{
											continue;
										}
										break;
										
								case 2:
										if((ram_space_pointer->service_byte_um.ready_to_control == 1) && (ram_space_pointer->service_byte_pm.fail_bus_2 == 0) && (common_ram_reg_space_ptr->PLC_CM_State != 0x04))
										{
											//������� ���� � ��������� ���� ��
											ram_space_pointer->service_byte_pm.init = 1;
											ram_space_pointer->service_byte_pm.master = 1;
											//������� ���� � ��������
											common_ram_reg_space_ptr->PLC_CM_State = 0x09;
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
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].data = (ram_space_pointer->tx_data) + offset;
						offset += sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber);
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.cmd = INIT_CMD;
						//TODO:����������� ��� ���� ���� result
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.result = 0;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length = sizeof(fields_cmd_header)+ sizeof(common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_SerialNumber);
						break;
					
				case READ_CMD:
						memcpy(&start_addr, ram_space_pointer->rx_packet_struct.cmd_with_data[i].data, sizeof(start_addr));
						memcpy(&size, ram_space_pointer->rx_packet_struct.cmd_with_data[i].data + sizeof(start_addr), sizeof(size));
						
						//�� ������� READ ������ � ���� ���� size ���� ������� � start_addr
						memcpy((ram_space_pointer->tx_data) + offset, (void*)(&(ram_space_pointer->start_struct)) + start_addr, size);
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].data = (ram_space_pointer->tx_data) + offset;
						offset += size;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.cmd = READ_CMD;
						//TODO:����������� ��� ���� ���� result
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.result = 0;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length = sizeof(fields_cmd_header) + size;
						break;
				
				case WRITE_CMD:
						//�������� ����, ��� ����������� ���������� �� ��������� ����
						///@todo
						///1. Definitions for 0x04, 0x09.
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
						//�� ������� WRITE ������ �� ������ start_addr size �������� ���� ��� ������ � � ���� ������ ������ ��� �������
						memcpy((void*)(&(ram_space_pointer->start_struct)) + start_addr, (ram_space_pointer->rx_packet_struct.cmd_with_data[i].data) + sizeof(start_addr) + sizeof(size), size);
						memset((ram_space_pointer->tx_data) + offset, WRITE_CMD, 1);
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].data = (ram_space_pointer->tx_data) + offset;
						offset++;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.cmd = WRITE_CMD;
						//TODO:����������� ��� ���� ���� result
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.result = 0;
						ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length = sizeof(fields_cmd_header) + 1;
						break;
					
			case RESET_CMD:
					//�������� ����, ��� ����������� ���������� �� ��������� ����
					///@todo
					///1. Definitions for 0x04, 0x09.
					switch (ext_bus)
					{
							case 1:
									if(common_ram_reg_space_ptr->PLC_CM_State != 0x04)
									{
										continue;
									}
									break;
									
							case 2:
									if(common_ram_reg_space_ptr->PLC_CM_State != 0x09)
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
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].data = (ram_space_pointer->tx_data) + offset;
					offset++;
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.cmd = RESET;
					//TODO:����������� ��� ���� ���� result
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.result = 0;
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length = sizeof(fields_cmd_header)  + 1;
					break;
				
			case CONFIG:
					//�������� ����, ��� ����������� ���������� �� ��������� ����
					///@todo
					///1. Definitions for 0x04, 0x09.
					switch (ext_bus)
					{
							case 1:
									if(common_ram_reg_space_ptr->PLC_CM_State != 0x04)
									{
										continue;
									}
									break;
									
							case 2:
									if(common_ram_reg_space_ptr->PLC_CM_State != 0x09)
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
					memcpy(&start_addr, ram_space_pointer->rx_packet_struct.cmd_with_data[i].data, sizeof(start_addr));
					memcpy(&size, ram_space_pointer->rx_packet_struct.cmd_with_data[i].data + sizeof(start_addr), sizeof(size));
					//������ ������ � ���
					memcpy(&common_regs, &ram_space_pointer->common_ram_register_space.PLC_CommonRomRegs, sizeof(common_regs));
					memcpy(&mpa_regs, &ram_space_pointer->mpa_ram_register_space.AI_RomRegs, sizeof(mpa_regs));
					ebc_init(EBC_ROM);
					memcpy_to_rom(ROM_REGISTER_SPACE_START_ADDR, &common_regs, sizeof(common_regs));
					memcpy_to_rom(ROM_REGISTER_SPACE_START_ADDR + sizeof(common_regs), &mpa_regs, sizeof(mpa_regs));
					ebc_init(EBC_RAM);
					
					memset((ram_space_pointer->tx_data) + offset, CONFIG, 1);
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].data = (ram_space_pointer->tx_data) + offset;
					offset++;
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.cmd = CONFIG;
					//TODO:����������� ��� ���� ���� result
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.result = 0;
					ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length = sizeof(fields_cmd_header) + 1;
					break;
				
			default:
					break;
		}
		ram_space_pointer->tx_packet_struct.packet_header.packet_length += ram_space_pointer->tx_packet_struct.cmd_with_data[i].header.length;
	}
	//���������� ��������� �����
	ram_space_pointer->tx_packet_struct.packet_header.header = ram_space_pointer->rx_packet_struct.packet_header.header;
	ram_space_pointer->tx_packet_struct.packet_header.receiver_addr = ram_space_pointer->rx_packet_struct.packet_header.sender_addr;
	ram_space_pointer->tx_packet_struct.packet_header.sender_addr = ram_space_pointer->rx_packet_struct.packet_header.receiver_addr;
	ram_space_pointer->tx_packet_struct.packet_header.packet_length += sizeof(ram_space_pointer->tx_packet_struct.packet_header) - sizeof(ram_space_pointer->tx_packet_struct.packet_header.header) + 
		sizeof(ram_space_pointer->tx_packet_struct.packet_tail.checksum);
	//TODO:����������� � ����� ��������� ����
	memcpy(&(ram_space_pointer->tx_packet_struct.packet_header.service_byte), &(ram_space_pointer->service_byte_pm), sizeof(ram_space_pointer->service_byte_pm));
	ram_space_pointer->tx_packet_struct.packet_header.cmd_number = ram_space_pointer->rx_packet_struct.packet_header.cmd_number;
	
	ram_space_pointer->tx_packet_struct.packet_tail.end = ram_space_pointer->rx_packet_struct.packet_tail.end;
	
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
������� ����������� ������
*/
void error_handler(protocol_error error, uint8_t ext_bus)
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
										common_ram_reg_space_ptr->PLC_CM_State = 0x05; // ������ �������������
										break;
								
								case 2:
										ram_space_pointer->service_byte_pm.fail_bus_2 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� �������� �������
										common_ram_reg_space_ptr->PLC_BusDefect_B2.fail_timeout = 1; //������ � ������� ������������� ���� ���� "������������� �� ��������"
										common_ram_reg_space_ptr->PLC_CM_State = 0x05; // ������ �������������
										break;
								
								default:
										break;
						}
						break;
						
				case CRC_ERROR:
						RESET_LED_OK_WORK()
						SET_LED_ERROR_WORK()
				
						switch (ext_bus)
						{
								case 1:
										common_ram_reg_space_ptr->PLC_ErrPackToDevice_B1++;//���-�� ������������ ������� ������ 
										if (common_ram_reg_space_ptr->PLC_ErrPackToDevice_B1 >=  common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_NumCrcErrorsForDefect_B1)
										{
											ram_space_pointer->service_byte_pm.fail_bus_1 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� ���-�� ������ ������������ ������� ������ ��������������
											common_ram_reg_space_ptr->PLC_BusDefect_B1.many_fail_packet = 1; //������ � ������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
											common_ram_reg_space_ptr->PLC_CM_State = 0x05; // ������ �������������
										}
										break;
										
								case 2:
										common_ram_reg_space_ptr->PLC_ErrPackToDevice_B2++;//���-�� ������������ ������� ������ 
										if (common_ram_reg_space_ptr->PLC_ErrPackToDevice_B2 >= common_ram_reg_space_ptr->PLC_CommonRomRegs.PLC_NumCrcErrorsForDefect_B2)
										{
											ram_space_pointer->service_byte_pm.fail_bus_2 = 1;  	//������ � ��������� ���� �� ���� ������������� ����, ���� ���-�� ������ ������������ ������� ������ ��������������
											common_ram_reg_space_ptr->PLC_BusDefect_B2.many_fail_packet = 1; //������ � ������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
											common_ram_reg_space_ptr->PLC_CM_State = 0x05; // ������ �������������
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


