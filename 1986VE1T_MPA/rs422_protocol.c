#include "rs422_protocol.h"
#include "external_ram.h"

extern ram_data *ram_space_pointer;
/*
������� ��� �������� ������ ������
*/
uint8_t transmit_packet(UARTn *UART_struct, uint8_t ext_bus)
{	
	data_exchange_errors error;
	
	//���������� � ����� ��������� ������
	memcpy(ram_space_pointer->packet_tx, &(ram_space_pointer->tx_packet_struct.packet_header), sizeof(ram_space_pointer->tx_packet_struct.packet_header));
	//���������� � ����� ������ ������ (������� - ������)
	uint16_t buffer_offset = 0; //����������� �� ������
	for (uint32_t i = 0; i < ((ram_space_pointer->tx_packet_struct.packet_header).cmd_number); i++)
	{
		//���������� �������, ���������, ������
		memcpy((ram_space_pointer->packet_tx) + sizeof(ram_space_pointer->tx_packet_struct.packet_header) + buffer_offset, &(ram_space_pointer->tx_cmd_packet[i]), sizeof(ram_space_pointer->tx_cmd_packet[i]) - sizeof(ram_space_pointer->tx_cmd_packet[i].data));
		memcpy((ram_space_pointer->packet_tx) + sizeof(ram_space_pointer->tx_packet_struct.packet_header) + buffer_offset + sizeof(ram_space_pointer->tx_cmd_packet[i]) - sizeof(ram_space_pointer->tx_cmd_packet[i].data), ram_space_pointer->tx_cmd_packet[i].data, (ram_space_pointer->tx_cmd_packet->length) - sizeof(ram_space_pointer->tx_cmd_packet[i]) + sizeof(ram_space_pointer->tx_cmd_packet[i].data));
		buffer_offset += ram_space_pointer->tx_cmd_packet[i].length;
	}
	
	// ���������� ����������� �����
	ram_space_pointer->tx_packet_struct.packet_tail.checksum = crc32((ram_space_pointer->packet_tx) + sizeof(ram_space_pointer->tx_packet_struct.packet_header.header), ram_space_pointer->tx_packet_struct.packet_header.packet_length - sizeof(ram_space_pointer->tx_packet_struct.packet_tail.checksum));
	
	//���������� � ����� ����� ������
	memcpy((ram_space_pointer->packet_tx) + sizeof(ram_space_pointer->tx_packet_struct.packet_header) + buffer_offset, &(ram_space_pointer->tx_packet_struct.packet_tail), sizeof(ram_space_pointer->tx_packet_struct.packet_tail));
	
	//���������� ������ �� UART
	if (uart_write(UART_struct, ram_space_pointer->packet_tx, ram_space_pointer->tx_packet_struct.packet_header.packet_length + sizeof(ram_space_pointer->tx_packet_struct.packet_header.header) + sizeof(ram_space_pointer->tx_packet_struct.packet_tail.end)) != 0)
	{
		error = UART_ERROR;
		return error;
	}
	
	return 0;
}
/*
������� ��� ������ ������ ������
*/
uint8_t receive_packet(UARTn *UART_struct, uint8_t ext_bus)
{
	data_exchange_errors error;
	uart_errors uart_error;
	
	//������� ���� �������� ������ ��������� � ����������� � ������������ �������
	memset(&(ram_space_pointer->rx_packet_struct), 0, sizeof(ram_space_pointer->rx_packet_struct) + sizeof(ram_space_pointer->tx_packet_struct) + sizeof(ram_space_pointer->tx_cmd_packet) + sizeof(ram_space_pointer->rx_cmd_packet) + sizeof(ram_space_pointer->tx_data) + sizeof(ram_space_pointer->packet_tx) + sizeof(ram_space_pointer->packet_rx));
	
	//����� ������ ������������ �������� - ������ ���� ������ ���� 0x55
	uart_error = uart_read(UART_struct, sizeof(ram_space_pointer->rx_packet_struct.packet_header.header),  ram_space_pointer->packet_rx);
	if (uart_error != 0)
	{
		if (uart_error == READ_TIMEOUT_ERROR)
		{
			switch(ext_bus)
			{
				case 1:
					ram_space_pointer->service_byte_pm.fail_bus_1 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� �������� �������
					ram_space_pointer->ram_register_space.PLC_BusDefect_B1.fail_timeout = 1; //������ � ������� ������������� ���� ���� "������������� �� ��������"
					ram_space_pointer->ram_register_space.PLC_CM_State = 0x05; // ������ �������������
					break;
				case 2:
					ram_space_pointer->service_byte_pm.fail_bus_2 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� �������� �������
					ram_space_pointer->ram_register_space.PLC_BusDefect_B2.fail_timeout = 1; //������ � ������� ������������� ���� ���� "������������� �� ��������"
					ram_space_pointer->ram_register_space.PLC_CM_State = 0x05; // ������ �������������
					break;
				default:
					break;
			}
		}
		error = UART_ERROR;
		return error;
	}
	memcpy(&(ram_space_pointer->rx_packet_struct.packet_header.header) , ram_space_pointer->packet_rx, sizeof(ram_space_pointer->rx_packet_struct.packet_header.header));
	if (ram_space_pointer->rx_packet_struct.packet_header.header != 0x55)
	{
		error = PACKET_ERROR;
		//����������� ���-�� ����� �������
		switch(ext_bus)
		{
			case 1:
				ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B1++;//���-�� ������������ ������� ������ 
				if (ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B1 >= ram_space_pointer->ram_register_space.PLC_NumCrcErrorsForDefect_B1)
				{
					ram_space_pointer->service_byte_pm.fail_bus_1 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� ���-�� ������ ������������ ������� ������ ��������������
					ram_space_pointer->ram_register_space.PLC_BusDefect_B1.many_fail_packet = 1; //������ � ������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
					ram_space_pointer->ram_register_space.PLC_CM_State = 0x05; // ������ �������������
				}
				break;
			case 2:
				ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B2++;//���-�� ������������ ������� ������ 
				if (ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B2 >= ram_space_pointer->ram_register_space.PLC_NumCrcErrorsForDefect_B2)
				{
					ram_space_pointer->service_byte_pm.fail_bus_2 = 1;  	//������ � ��������� ���� �� ���� ������������� ����, ���� ���-�� ������ ������������ ������� ������ ��������������
					ram_space_pointer->ram_register_space.PLC_BusDefect_B2.many_fail_packet = 1; //������ � ������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
					ram_space_pointer->ram_register_space.PLC_CM_State = 0x05; // ������ �������������
				}
				break;
			default:
				break;
		}
		return error;
	}
	uart_set_pos(UART_struct, uart_read_pos(UART_struct) - sizeof(ram_space_pointer->rx_packet_struct.packet_header.header)); //������� ������� ������ ������ �� �������� �������
	memset(ram_space_pointer->packet_rx,0,sizeof(ram_space_pointer->packet_rx));
	//��������� ����� ��������� ���������� ��� ����������� ����� ������
	uart_error = uart_read(UART_struct, sizeof(ram_space_pointer->rx_packet_struct.packet_header),  ram_space_pointer->packet_rx);
	if (uart_error != 0)
	{
		error = UART_ERROR;
		if (uart_error == READ_TIMEOUT_ERROR)
		{
			switch(ext_bus)
			{
				case 1:
					ram_space_pointer->service_byte_pm.fail_bus_1 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� �������� �������
					ram_space_pointer->ram_register_space.PLC_BusDefect_B1.fail_timeout = 1; //������ � ������� ������������� ���� ���� "������������� �� ��������"
					ram_space_pointer->ram_register_space.PLC_CM_State = 0x05; // ������ �������������
					break;
				case 2:
					ram_space_pointer->service_byte_pm.fail_bus_2 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� �������� �������
					ram_space_pointer->ram_register_space.PLC_BusDefect_B2.fail_timeout = 1; //������ � ������� ������������� ���� ���� "������������� �� ��������"
					ram_space_pointer->ram_register_space.PLC_CM_State = 0x05; // ������ �������������
					break;
				default:
					break;
			}
		}
		return error;
	}
	memcpy(&(ram_space_pointer->rx_packet_struct.packet_header) , ram_space_pointer->packet_rx, sizeof(ram_space_pointer->rx_packet_struct.packet_header));
	
	//��������� ���� ����� ����������� �����
	uart_set_pos(UART_struct, uart_read_pos(UART_struct) - sizeof(ram_space_pointer->rx_packet_struct.packet_header)); //������� ������� ������ ������ �� �������� �������
	memset(ram_space_pointer->packet_rx,0,sizeof(ram_space_pointer->packet_rx));
	uart_error = uart_read(UART_struct, ((ram_space_pointer->rx_packet_struct.packet_header).packet_length)+sizeof(ram_space_pointer->rx_packet_struct.packet_header.header)+sizeof(ram_space_pointer->rx_packet_struct.packet_tail.end), ram_space_pointer->packet_rx);
	if (uart_error != 0)
	{
		error = UART_ERROR;
		if (uart_error == READ_TIMEOUT_ERROR)
		{
			switch(ext_bus)
			{
				case 1:
					ram_space_pointer->service_byte_pm.fail_bus_1 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� �������� �������
					ram_space_pointer->ram_register_space.PLC_BusDefect_B1.fail_timeout = 1; //������ � ������� ������������� ���� ���� "������������� �� ��������"
					ram_space_pointer->ram_register_space.PLC_CM_State = 0x05; // ������ �������������
					break;
				case 2:
					ram_space_pointer->service_byte_pm.fail_bus_2 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� �������� �������
					ram_space_pointer->ram_register_space.PLC_BusDefect_B2.fail_timeout = 1; //������ � ������� ������������� ���� ���� "������������� �� ��������"
					ram_space_pointer->ram_register_space.PLC_CM_State = 0x05; // ������ �������������
					break;
				default:
					break;
			}
		}
		return error;
	}
	
	//��������� ��������� ������
	memcpy(&(ram_space_pointer->rx_packet_struct.packet_header) , ram_space_pointer->packet_rx, sizeof(ram_space_pointer->rx_packet_struct.packet_header));	
	
	//������������� ���������� ����� ��
	memcpy(&(ram_space_pointer->service_byte_um) , &(ram_space_pointer->rx_packet_struct.packet_header.service_byte), sizeof(ram_space_pointer->rx_packet_struct.packet_header.service_byte));	
	
	//��������� ����� ������
	memcpy(&(ram_space_pointer->rx_packet_struct.packet_tail), ram_space_pointer->packet_rx + (ram_space_pointer->rx_packet_struct.packet_header.packet_length) - sizeof(ram_space_pointer->rx_packet_struct.packet_tail.checksum) + sizeof(ram_space_pointer->rx_packet_struct.packet_header.header), sizeof(fields_packet_tail));
	
	//������ ������������ �������� - ��������� 2 ����� ������ ���� 0xAA
	if (ram_space_pointer->rx_packet_struct.packet_tail.end != 0xAAAA)
	{
		error = PACKET_ERROR;
		//����������� ���-�� ����� �������
		switch(ext_bus)
		{
			case 1:
				ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B1++;//���-�� ������������ ������� ������ 
				if (ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B1 >= ram_space_pointer->ram_register_space.PLC_NumCrcErrorsForDefect_B1)
				{
					ram_space_pointer->service_byte_pm.fail_bus_1 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� ���-�� ������ ������������ ������� ������ ��������������
					ram_space_pointer->ram_register_space.PLC_BusDefect_B1.many_fail_packet = 1; //������ � ������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
					ram_space_pointer->ram_register_space.PLC_CM_State = 0x05; // ������ �������������
				}
				break;
			case 2:
				ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B2++;//���-�� ������������ ������� ������ 
				if (ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B2 >= ram_space_pointer->ram_register_space.PLC_NumCrcErrorsForDefect_B2)
				{
					ram_space_pointer->service_byte_pm.fail_bus_2 = 1;  	//������ � ��������� ���� �� ���� ������������� ����, ���� ���-�� ������ ������������ ������� ������ ��������������
					ram_space_pointer->ram_register_space.PLC_BusDefect_B2.many_fail_packet = 1; //������ � ������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
					ram_space_pointer->ram_register_space.PLC_CM_State = 0x05; // ������ �������������
				}
				break;
			default:
				break;
		}
		return error;
	}
	
	//��������� ������ ������ (������� - ������)
	uint16_t buffer_offset = 0; //����������� �� ������
	for (uint32_t i = 0; i < ((ram_space_pointer->rx_packet_struct.packet_header).cmd_number); i++)
	{
		//��������� �������, ���������, ������
		memcpy(&(ram_space_pointer->rx_cmd_packet[i]), ram_space_pointer->packet_rx + sizeof(ram_space_pointer->rx_packet_struct.packet_header) + buffer_offset, sizeof(ram_space_pointer->rx_cmd_packet[i]) - sizeof(ram_space_pointer->rx_cmd_packet[i].data));
		//��������� ��������� �� ������
		ram_space_pointer->rx_cmd_packet[i].data = ram_space_pointer->packet_rx + sizeof(ram_space_pointer->rx_packet_struct.packet_header) + buffer_offset + sizeof(ram_space_pointer->rx_cmd_packet[i]) - sizeof(ram_space_pointer->rx_cmd_packet[i].data);
		buffer_offset += ram_space_pointer->rx_cmd_packet[i].length;
	}
	
	// ���������� ����������� ����� � ��������� �� � ���, ��� � ����������	
	uint32_t real_checksum = crc32((ram_space_pointer->packet_rx) + sizeof(ram_space_pointer->rx_packet_struct.packet_header.header), ram_space_pointer->rx_packet_struct.packet_header.packet_length - sizeof(ram_space_pointer->rx_packet_struct.packet_tail.checksum));
	//���� ����������� ����� �� �����, �.�. ����� ���������
	if (real_checksum != (ram_space_pointer->rx_packet_struct.packet_tail.checksum))
	{
		error = CRC_ERROR;
		//����������� ���-�� ����� �������
		switch(ext_bus)
		{
			case 1:
				ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B1++;//���-�� ������������ ������� ������ 
				if (ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B1 >= ram_space_pointer->ram_register_space.PLC_NumCrcErrorsForDefect_B1)
				{
					ram_space_pointer->service_byte_pm.fail_bus_1 = 1;	//������ � ��������� ���� �� ���� ������������� ����, ���� ���-�� ������ ������������ ������� ������ ��������������
					ram_space_pointer->ram_register_space.PLC_BusDefect_B1.many_fail_packet = 1; //������ � ������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
					ram_space_pointer->ram_register_space.PLC_CM_State = 0x05; // ������ �������������
				}
				break;
			case 2:
				ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B2++;//���-�� ������������ ������� ������ 
				if (ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B2 >= ram_space_pointer->ram_register_space.PLC_NumCrcErrorsForDefect_B2)
				{
					ram_space_pointer->service_byte_pm.fail_bus_2 = 1;  	//������ � ��������� ���� �� ���� ������������� ����, ���� ���-�� ������ ������������ ������� ������ ��������������
					ram_space_pointer->ram_register_space.PLC_BusDefect_B2.many_fail_packet = 1; //������ � ������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
					ram_space_pointer->ram_register_space.PLC_CM_State = 0x05; // ������ �������������
				}
				break;
			default:
				break;
		}
		
		return error;
	}
	//�������� ���������
	if(ram_space_pointer->rx_packet_struct.packet_header.receiver_addr != PM_ADDR)
	{
		error = PM_ADDR_ERROR;
		return error;
	}
	//���� ��� ������
	switch(ext_bus)
	{
		case 1:
			ram_space_pointer->ram_register_space.PLC_CorrPackToDevice_B1++; //����������� ������� ��������� �������� �������
			ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B1 = 0;//���-�� ������������ ������� ������ 
			ram_space_pointer->service_byte_pm.fail_bus_1 = 0; //������ � ��������� ����� �� ���� ������������� ����
			ram_space_pointer->ram_register_space.PLC_BusDefect_B1.many_fail_packet = 0; //������ � �������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
			ram_space_pointer->ram_register_space.PLC_BusDefect_B1.fail_timeout = 0; //������ � �������� ������������� ���� ���� "������������� �� ��������"
			break;
		case 2:
			ram_space_pointer->ram_register_space.PLC_CorrPackToDevice_B2++; //����������� ������� ��������� �������� �������
			ram_space_pointer->ram_register_space.PLC_ErrPackToDevice_B2 = 0;//���-�� ������������ ������� ������ 
			ram_space_pointer->service_byte_pm.fail_bus_2 = 0;	//������ � ��������� ����� �� ���� ������������� ����
			ram_space_pointer->ram_register_space.PLC_BusDefect_B2.many_fail_packet = 0; //������ � �������� ������������� ���� ���� "���-�� ����� ������� ������ ��������������"
			ram_space_pointer->ram_register_space.PLC_BusDefect_B2.fail_timeout = 0; //������ � �������� ������������� ���� ���� "������������� �� ��������"
			break;
		default:
			break;
	}		

	return 0;
}
/*
������� ��� ���������� ��������� �������
*/
uint8_t protocol_do_cmds(uint8_t ext_bus)
{
	uint32_t offset = 0; //��������� �� ������ ��� ������ �������
	uint16_t start_addr = 0; //��������� ����� ��� ������/������ ���������
	uint16_t size = 0;	//���-�� ���� ��� ������/������
			
	//���������� ���� ������
	for (uint8_t i = 0; i < ram_space_pointer->rx_packet_struct.packet_header.cmd_number; i++)
	{
		switch (ram_space_pointer->rx_cmd_packet[i].cmd)
		{
			case TYPE:
				//�� ������� TYPE ������ � ���� ���� �������� PLC_SoftVer, PLC_Config, PLC_DeviceType, PLC_SerialNumber
				memcpy((ram_space_pointer->tx_data) + offset, &(ram_space_pointer->ram_register_space.PLC_SoftVer), sizeof(ram_space_pointer->ram_register_space.PLC_SoftVer));
				memcpy((ram_space_pointer->tx_data) + offset + sizeof(ram_space_pointer->ram_register_space.PLC_SoftVer), &(ram_space_pointer->ram_register_space.PLC_Config), sizeof(ram_space_pointer->ram_register_space.PLC_Config));
				memcpy((ram_space_pointer->tx_data) + offset + sizeof(ram_space_pointer->ram_register_space.PLC_SoftVer) + sizeof(ram_space_pointer->ram_register_space.PLC_Config), &(ram_space_pointer->ram_register_space.PLC_DeviceType), sizeof(ram_space_pointer->ram_register_space.PLC_DeviceType));
				memcpy((ram_space_pointer->tx_data) + offset + sizeof(ram_space_pointer->ram_register_space.PLC_SoftVer) + sizeof(ram_space_pointer->ram_register_space.PLC_Config) + sizeof(ram_space_pointer->ram_register_space.PLC_DeviceType), &(ram_space_pointer->ram_register_space.PLC_SerialNumber), sizeof(ram_space_pointer->ram_register_space.PLC_SerialNumber));		
		    ram_space_pointer->tx_cmd_packet[i].data = (ram_space_pointer->tx_data) + offset;
				offset += sizeof(ram_space_pointer->ram_register_space.PLC_SoftVer) + sizeof(ram_space_pointer->ram_register_space.PLC_Config) + sizeof(ram_space_pointer->ram_register_space.PLC_DeviceType) + sizeof(ram_space_pointer->ram_register_space.PLC_SerialNumber);
				ram_space_pointer->tx_cmd_packet[i].cmd = TYPE;
				//TODO:����������� ��� ���� ���� result
				ram_space_pointer->tx_cmd_packet[i].result = 0;
				ram_space_pointer->tx_cmd_packet[i].length = sizeof(ram_space_pointer->tx_cmd_packet[i].cmd) + sizeof(ram_space_pointer->tx_cmd_packet[i].result) + sizeof(ram_space_pointer->tx_cmd_packet[i].length) + sizeof(ram_space_pointer->ram_register_space.PLC_SoftVer) + sizeof(ram_space_pointer->ram_register_space.PLC_Config) + sizeof(ram_space_pointer->ram_register_space.PLC_DeviceType) + sizeof(ram_space_pointer->ram_register_space.PLC_SerialNumber);
				break;
			case INIT:
				//�� ������� INIT ������ � ���� ���� ������� PLC_SerialNumber, ���� �������� ��� �������
				switch(ext_bus)
				{
					case 1:
						if((ram_space_pointer->service_byte_um.ready_to_control == 1) && (ram_space_pointer->service_byte_pm.fail_bus_1 == 0) && (ram_space_pointer->ram_register_space.PLC_CM_State != 0x09))
						{
							//������� ���� � ��������� ���� ��
							ram_space_pointer->service_byte_pm.init = 1;
							ram_space_pointer->service_byte_pm.master = 0;
							//������� ���� � ��������
							ram_space_pointer->ram_register_space.PLC_CM_State = 0x04;
						}
						else
						{
							continue;
						}
						break;
					case 2:
						if((ram_space_pointer->service_byte_um.ready_to_control == 1) && (ram_space_pointer->service_byte_pm.fail_bus_2 == 0) && (ram_space_pointer->ram_register_space.PLC_CM_State != 0x04))
						{
							//������� ���� � ��������� ���� ��
							ram_space_pointer->service_byte_pm.init = 1;
							ram_space_pointer->service_byte_pm.master = 1;
							//������� ���� � ��������
							ram_space_pointer->ram_register_space.PLC_CM_State = 0x09;
							continue;
						}	
						else
						{
							continue;
						}
						break;
					default:
						break;
				}		
				memcpy((ram_space_pointer->tx_data) + offset, &(ram_space_pointer->ram_register_space.PLC_SerialNumber), sizeof(ram_space_pointer->ram_register_space.PLC_SerialNumber));
				ram_space_pointer->tx_cmd_packet[i].data = (ram_space_pointer->tx_data) + offset;
				offset += sizeof(ram_space_pointer->ram_register_space.PLC_SerialNumber);
				ram_space_pointer->tx_cmd_packet[i].cmd = INIT;
				//TODO:����������� ��� ���� ���� result
				ram_space_pointer->tx_cmd_packet[i].result = 0;
				ram_space_pointer->tx_cmd_packet[i].length = sizeof(ram_space_pointer->tx_cmd_packet[i].cmd) + sizeof(ram_space_pointer->tx_cmd_packet[i].result) + sizeof(ram_space_pointer->tx_cmd_packet[i].length) + sizeof(ram_space_pointer->ram_register_space.PLC_SerialNumber);

				break;
			case READ:
				memcpy(&start_addr, ram_space_pointer->rx_cmd_packet[i].data, sizeof(start_addr));
				memcpy(&size, (ram_space_pointer->rx_cmd_packet[i].data) + sizeof(start_addr), sizeof(size));
				//�� ������� READ ������ � ���� ���� size ���� ������� � start_addr
				memcpy((ram_space_pointer->tx_data) + offset, &(ram_space_pointer->start_struct) + start_addr, size);
				ram_space_pointer->tx_cmd_packet[i].data = (ram_space_pointer->tx_data) + offset;
				offset += size;
				ram_space_pointer->tx_cmd_packet[i].cmd = READ;
				//TODO:����������� ��� ���� ���� result
				ram_space_pointer->tx_cmd_packet[i].result = 0;
				ram_space_pointer->tx_cmd_packet[i].length = sizeof(ram_space_pointer->tx_cmd_packet[i].cmd) + sizeof(ram_space_pointer->tx_cmd_packet[i].result) + sizeof(ram_space_pointer->tx_cmd_packet[i].length) + size;
				break;
			case WRITE:
				//�������� ����, ��� ����������� ���������� �� ��������� ����
				switch(ext_bus)
				{
					case 1:
						if(ram_space_pointer->ram_register_space.PLC_CM_State != 0x04)
						{
							continue;
						}
						break;
					case 2:
						if(ram_space_pointer->ram_register_space.PLC_CM_State != 0x09)
						{
							continue;
						}
						break;
					default:
						break;
				}
				memcpy(&start_addr, ram_space_pointer->rx_cmd_packet[i].data, sizeof(start_addr));
				memcpy(&size, (ram_space_pointer->rx_cmd_packet[i].data) + sizeof(start_addr), sizeof(size));
				//�� ������� WRITE ������ �� ������ start_addr size �������� ���� ��� ������ � � ���� ������ ������ ��� �������
				memcpy(&(ram_space_pointer->start_struct) + start_addr, (ram_space_pointer->rx_cmd_packet[i].data) + sizeof(start_addr) + sizeof(size), size);
				memset((ram_space_pointer->tx_data) + offset, WRITE, 1);
				ram_space_pointer->tx_cmd_packet[i].data = (ram_space_pointer->tx_data) + offset;
				offset++;
				ram_space_pointer->tx_cmd_packet[i].cmd = WRITE;
				//TODO:����������� ��� ���� ���� result
				ram_space_pointer->tx_cmd_packet[i].result = 0;
				ram_space_pointer->tx_cmd_packet[i].length = sizeof(ram_space_pointer->tx_cmd_packet[i].cmd) + sizeof(ram_space_pointer->tx_cmd_packet[i].result) + sizeof(ram_space_pointer->tx_cmd_packet[i].length) + 1;
				break;
			case RESET:
				//�������� ����, ��� ����������� ���������� �� ��������� ����
				switch(ext_bus)
				{
					case 1:
						if(ram_space_pointer->ram_register_space.PLC_CM_State != 0x04)
						{
							continue;
						}
						break;
					case 2:
						if(ram_space_pointer->ram_register_space.PLC_CM_State != 0x09)
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
				ram_space_pointer->tx_cmd_packet[i].data = (ram_space_pointer->tx_data) + offset;
				offset++;
				ram_space_pointer->tx_cmd_packet[i].cmd = RESET;
				//TODO:����������� ��� ���� ���� result
				ram_space_pointer->tx_cmd_packet[i].result = 0;
				ram_space_pointer->tx_cmd_packet[i].length = sizeof(ram_space_pointer->tx_cmd_packet[i].cmd) + sizeof(ram_space_pointer->tx_cmd_packet[i].result) + sizeof(ram_space_pointer->tx_cmd_packet[i].length) + 1;
				break;
			case CONFIG:
				//�������� ����, ��� ����������� ���������� �� ��������� ����
				switch(ext_bus)
				{
					case 1:
						if(ram_space_pointer->ram_register_space.PLC_CM_State != 0x04)
						{
							continue;
						}
						break;
					case 2:
						if(ram_space_pointer->ram_register_space.PLC_CM_State != 0x09)
						{
							continue;
						}
						break;
					default:
						break;
				}
				//�������� �� ��, ��� �� ��������� � ��������� ������
				if (ram_space_pointer->ram_register_space.PLC_PMAddr != 0x00)
				{
					break;
				}
				//�� ������� CONFIG �������� ��������������� � ��� � � ���� ������ ������ ��� �������
				memcpy(&start_addr, ram_space_pointer->rx_cmd_packet[i].data, sizeof(start_addr));
				memcpy(&size, (ram_space_pointer->rx_cmd_packet[i].data) + sizeof(start_addr), sizeof(size));
				//������ ������ � ���
				//TODO: �������� ������ � ���
				memset((ram_space_pointer->tx_data) + offset, CONFIG, 1);
				ram_space_pointer->tx_cmd_packet[i].data = (ram_space_pointer->tx_data) + offset;
				offset++;
				ram_space_pointer->tx_cmd_packet[i].cmd = CONFIG;
				//TODO:����������� ��� ���� ���� result
				ram_space_pointer->tx_cmd_packet[i].result = 0;
				ram_space_pointer->tx_cmd_packet[i].length = sizeof(ram_space_pointer->tx_cmd_packet[i].cmd) + sizeof(ram_space_pointer->tx_cmd_packet[i].result) + sizeof(ram_space_pointer->tx_cmd_packet[i].length) + 1;
				break;
			default:
				break;
		}
		ram_space_pointer->tx_packet_struct.packet_header.packet_length += ram_space_pointer->tx_cmd_packet[i].length;
	}
	//���������� ��������� �����
	ram_space_pointer->tx_packet_struct.packet_header.header = ram_space_pointer->rx_packet_struct.packet_header.header;
	ram_space_pointer->tx_packet_struct.packet_header.receiver_addr = ram_space_pointer->rx_packet_struct.packet_header.sender_addr;
	ram_space_pointer->tx_packet_struct.packet_header.sender_addr = ram_space_pointer->rx_packet_struct.packet_header.receiver_addr;
	ram_space_pointer->tx_packet_struct.packet_header.packet_length += sizeof(ram_space_pointer->tx_packet_struct.packet_header) - sizeof(ram_space_pointer->tx_packet_struct.packet_header.header) + sizeof(ram_space_pointer->tx_packet_struct.packet_tail.checksum);
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
	uint_least32_t crc_table[256];
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


