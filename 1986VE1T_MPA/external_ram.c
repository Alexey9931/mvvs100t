#include "external_ram.h"
#include <string.h>
#include <math.h>

extern ram_data *ram_space_pointer;

void init_external_ram_space(void)
{
	ram_space_pointer = (ram_data*)EXT_RAM_START_ADDR;
	//первичная очистка используемого куска памяти ОЗУ
	memset(ram_space_pointer, 0, sizeof(ram_data));
	//инициализации структуры, которая лежит в начале ОЗУ
	ram_space_pointer->start_struct.length = 32;
	ram_space_pointer->start_struct.text_info = 332;
	ram_space_pointer->start_struct.flag_change_struct = 0;
	ram_space_pointer->start_struct.number_of_ranges = 3;
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
	strncpy(&(ram_space_pointer->common_ram_register_space.PLC_DeviceInfo),"MPA",sizeof("MPA"));
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
	ram_space_pointer->common_ram_register_space.PLC_DeviceType.revision = 1;
	ram_space_pointer->common_ram_register_space.PLC_DeviceType.modification = 2;
	ram_space_pointer->common_ram_register_space.PLC_DeviceType.type = MPA;
	ram_space_pointer->common_ram_register_space.PLC_DeviceType.batch = 1;
	ram_space_pointer->common_ram_register_space.PLC_DeviceType.reserv = 20;
	
	ram_space_pointer->common_ram_register_space.PLC_SerialNumber = 1717986918;
	//ram_space_pointer->ram_register_space.PLC_BusConfig_B1 = 0x66847485;
	//ram_space_pointer->ram_register_space.PLC_BusConfig_B2 = 0x100271355;
	ram_space_pointer->common_ram_register_space.PLC_TimeoutForDefect_B1 = 200;
	ram_space_pointer->common_ram_register_space.PLC_TimeoutForDefect_B2 = 200;
	ram_space_pointer->common_ram_register_space.PLC_NumCrcErrorsForDefect_B1 = 6;
	ram_space_pointer->common_ram_register_space.PLC_NumCrcErrorsForDefect_B2 = 6;
	ram_space_pointer->common_ram_register_space.PLC_TimeToRepair = 65535;
	ram_space_pointer->common_ram_register_space.PLC_TimeSoloWork = 61166;
	ram_space_pointer->common_ram_register_space.PLC_DualControl = 56797;
	memset(&(ram_space_pointer->common_ram_register_space.Reserv_2), 0, sizeof(ram_space_pointer->common_ram_register_space.Reserv_2));
	
	//инциализация регистров МПА
	for (uint8_t i = 0; i < MAX_CHANEL_NUMBER; i++)
	{
		RESET_BIT(i, ram_space_pointer->mpa_ram_register_space.AI_OperMode.adc_chs_mode);
		ram_space_pointer->mpa_ram_register_space.AI_NumForAverag[i] = 10;
		ram_space_pointer->mpa_ram_register_space.AI_MinCodeADC[i] = 0;
		ram_space_pointer->mpa_ram_register_space.AI_MaxCodeADC[i] = 65535;
		//такие значения коэф. полиномов только для напряжения 0-10В
		ram_space_pointer->mpa_ram_register_space.AI_PolynConst0[i] = 4.972769f;
		ram_space_pointer->mpa_ram_register_space.AI_PolynConst1[i] = 0.000161f;
		ram_space_pointer->mpa_ram_register_space.AI_PolynConst2[i] = 0.0f;
		ram_space_pointer->mpa_ram_register_space.AI_PolynConst3[i] = 0.0f;
		ram_space_pointer->mpa_ram_register_space.AI_PolynConst4[i] = 0.0f; 
		ram_space_pointer->mpa_ram_register_space.AI_PolynConst5[i] = 0.0f;
		ram_space_pointer->mpa_ram_register_space.AI_PolynConst6[i] = 0.0f;
		ram_space_pointer->mpa_ram_register_space.AI_MetrologDat[i] = 0.0f;
	}
	memset(ram_space_pointer->mpa_ram_register_space.AI_MetrologDat, 0, sizeof(ram_space_pointer->mpa_ram_register_space.AI_MetrologDat));
	memset(ram_space_pointer->mpa_ram_register_space.Reserv_4, 0, sizeof(ram_space_pointer->mpa_ram_register_space.Reserv_4));
	memset(ram_space_pointer->mpa_ram_register_space.Reserv_5, 0, sizeof(ram_space_pointer->mpa_ram_register_space.Reserv_5));
	
	//Заполянем таблицу CRC32
	fill_crc32_table();
}

