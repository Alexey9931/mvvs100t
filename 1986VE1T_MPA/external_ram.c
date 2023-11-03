/*!
 \file
 \brief Файл с реализацией API для работы с областью памяти внешнего ОЗУ
*/

/*
 *	Внешняя память ОЗУ 1645Р4 подключается по внешней системной шине в 32-битном режиме данных.
 *	Сдвиг адресации делать нужно A2->A0, A3->A1 и тд (т.к. нормально работать с ОЗУ можно только в 32 битном режим EBC, а для этого нужен сдвиг)
*/

#include "external_ram.h"
#include "ebc.h"
#include <string.h>
#include <math.h>

extern ram_data *ram_space_pointer;
extern rom_data *rom_space_pointer;

/*!
	Функция инициализации области памяти внешнего ОЗУ
*/
void init_external_ram_space(void)
{
	common_rom_registers 	common_regs;
	mpa_rom_registers			mpa_regs;
	
	ram_space_pointer = (ram_data*)EXT_RAM_START_ADDR;
	rom_space_pointer = (rom_data*)EXT_ROM_START_ADDR;
	
	//первичная очистка используемого куска памяти ОЗУ
	memset(ram_space_pointer, 0, sizeof(ram_data));
	//инициализации структуры, которая лежит в начале ОЗУ
	ram_space_pointer->start_struct.length = 32;
	ram_space_pointer->start_struct.text_info = 332;
	ram_space_pointer->start_struct.flag_change_struct = 0;
	ram_space_pointer->start_struct.number_of_ranges = NUMBER_OF_RANGES;
	ram_space_pointer->start_struct.ranges_in_start_struct[0].range_type = 0x0100;
	ram_space_pointer->start_struct.ranges_in_start_struct[0].start_channel_num = 0x0000;
	ram_space_pointer->start_struct.ranges_in_start_struct[0].address = 1874;
	ram_space_pointer->start_struct.ranges_in_start_struct[0].size = 32;
	ram_space_pointer->start_struct.ranges_in_start_struct[1].range_type = 0x0400;
	ram_space_pointer->start_struct.ranges_in_start_struct[1].start_channel_num = 0x0001;
	ram_space_pointer->start_struct.ranges_in_start_struct[1].address = 254;
	ram_space_pointer->start_struct.ranges_in_start_struct[1].size = 10;
	ram_space_pointer->start_struct.ranges_in_start_struct[2].range_type = 0x0500;
	ram_space_pointer->start_struct.ranges_in_start_struct[2].start_channel_num = 0x0002;
	ram_space_pointer->start_struct.ranges_in_start_struct[2].address = 1906;
	ram_space_pointer->start_struct.ranges_in_start_struct[2].size = 8;
	
	//кладем карту регистров по адресу 200 во внешней ОЗУ и инциализируем ее
	//кладем регистры, которые инициализируются в ПО
	ram_space_pointer->common_ram_register_space.PLC_SoftVer.revision = 1;
	ram_space_pointer->common_ram_register_space.PLC_SoftVer.modification = 2;
	ram_space_pointer->common_ram_register_space.PLC_SoftVer.type = MPA;
	ram_space_pointer->common_ram_register_space.PLC_SoftVer.soft_ver = 1;
	ram_space_pointer->common_ram_register_space.PLC_SoftVer.add_info = 4;
	ram_space_pointer->common_ram_register_space.PLC_SoftVer.develop = 1;
	ram_space_pointer->common_ram_register_space.PLC_PMAddr.module_addr = PM_DEV_ADDR;
	ram_space_pointer->common_ram_register_space.PLC_PMAddr.chassis_addr = PM_CHASSIS_ADDR;
	ram_space_pointer->common_ram_register_space.PLC_Config.main_switch = 4;
	ram_space_pointer->common_ram_register_space.PLC_Config.add_switch_1 = 3;
	ram_space_pointer->common_ram_register_space.PLC_Config.add_switch_2 = 2;
	ram_space_pointer->common_ram_register_space.PLC_Config.reserv = 1;
	ram_space_pointer->common_ram_register_space.PLC_CM_State = 0x01;
	//кладем регистры, которые берутся из ПЗУ
	ebc_init(EBC_ROM);
	memcpy(&common_regs, &rom_space_pointer->common_rom_registers_space, sizeof(common_regs));
	memcpy(&mpa_regs, &rom_space_pointer->mpa_rom_registers_space, sizeof(mpa_regs));
	ebc_init(EBC_RAM);

	memcpy(&ram_space_pointer->common_ram_register_space.PLC_CommonRomRegs, &common_regs, sizeof(common_regs));
	memcpy(&ram_space_pointer->mpa_ram_register_space.AI_RomRegs, &mpa_regs, sizeof(mpa_regs));
	
	//Заполянем таблицу CRC32
	fill_crc32_table();
}

/*!
	Функция нахождения максимального элемента массива 
*/
uint16_t find_max_halfword(uint16_t *array, uint32_t array_size);
uint16_t find_max_halfword(uint16_t *array, uint32_t array_size)
{
	uint16_t result = 0;
	uint16_t next_item;
	
	for (int i = 0; i < array_size; i++)
	{
		memcpy(&next_item, (uint16_t*)(array + i*sizeof(result)), sizeof(result));
		if (next_item > result)
		{
			result = next_item;
		}
	}
	
	return result;
}
