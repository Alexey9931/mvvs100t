#include "REGISTER_SPACE.h"


void registers_ini()
{
	//PLC_SoftVer
	uint8_t PLC_SoftVer[4] = {0x01,0x00,0x00,0x00};
	write_register(PLC_SoftVer,0, 4, COMMON);
	//PLC_Config
	uint8_t PLC_Config[2] = {0x01,0x02};
	write_register(PLC_Config,4, 2, COMMON);
	//PLC_PMAddr
	uint8_t PLC_PMAddr[2] = {0x20,0x00};//адрес модуля 2, шасси 0
	write_register(PLC_PMAddr,6, 2, COMMON);
	//PLC_DeviceType
	uint8_t PLC_DeviceType[4] = {0x01,0x02,0x03,0x04};
	write_register(PLC_DeviceType,1160, 4, COMMON);
	//PLC_SerialNumber
	uint8_t PLC_SerialNumber[4] = {0x01,0x01,0x01,0x01};
	write_register(PLC_SerialNumber,1164, 4, COMMON);
}
void write_register(uint8_t data[],uint16_t address, uint16_t data_size, uint8_t register_type)
{
	if(register_type == COMMON)
	{
		for (uint16_t i = 0; i < data_size; i++)
		{
			Common_Registers[address+i] = data[i];
		}
	}
	else if(register_type == MPA)
	{
		for (uint16_t i = 0; i < data_size; i++)
		{
			MPA_Registers[address+i] = data[i];
		}
	}
}
/*
data[] - байты данных
adress - начальный адрес регистра
data_size - кол-во байт для записи
register_type - тип(общий или для МПА)
*/
uint8_t *read_register(uint16_t address, uint16_t data_size, uint8_t register_type)
{
	uint8_t result[data_size];
	
	if(register_type == COMMON)
	{
		for (uint16_t i = 0; i < data_size; i++)
		{
			result[i] = Common_Registers[address+i];	
		}
	}
	else if(register_type == MPA)
	{
		for (uint16_t i = 0; i < data_size; i++)
		{
			result[i] = MPA_Registers[address+i];	
		}
	}
	
	return result;
}