#include "external_ram.h"
#include <string.h>

extern ram_data *ram_space_pointer;

void init_external_ram_space(void)
{
	ram_space_pointer = (ram_data*)EXT_RAM_START_ADDR;
	//первичная очистка используемого куска памяти ОЗУ
	memset(ram_space_pointer, 0, sizeof(ram_data));
	//инициализации структуры, которая лежит в начале ОЗУ
	ram_space_pointer->start_struct.Length = 32;
	ram_space_pointer->start_struct.TextInfo = 332;
	ram_space_pointer->start_struct.FlagChangeStruct = 0;
	ram_space_pointer->start_struct.NumberRanges = 3;
	ram_space_pointer->start_struct.RangeType_0 = 0x0100;
	ram_space_pointer->start_struct.StartChannelNumb_0 = 0x0000;
	ram_space_pointer->start_struct.Address_0 = 1874;
	ram_space_pointer->start_struct.Size_0 = 32;
	ram_space_pointer->start_struct.RangeType_1 = 0x0400;
	ram_space_pointer->start_struct.StartChannelNumb_1 = 0x0001;
	ram_space_pointer->start_struct.Address_1 = 254;
	ram_space_pointer->start_struct.Size_2 = 10;
	ram_space_pointer->start_struct.RangeType_2 = 0x0500;
	ram_space_pointer->start_struct.StartChannelNumb_2 = 0x0002;
	ram_space_pointer->start_struct.Address_2 = 1906;
	ram_space_pointer->start_struct.Size_2  = 8;
	
	//кладем карту регистров по адресу 200 во внешней ОЗУ и инциализируем ее
	strncpy(&(ram_space_pointer->ram_register_space.PLC_DeviceInfo),"MPA",sizeof("MPA"));
	ram_space_pointer->ram_register_space.PLC_PMAddr = PM_ADDR;
	ram_space_pointer->ram_register_space.PLC_CM_State = 0x01;
	ram_space_pointer->ram_register_space.PLC_DeviceType = 2684487201;
	ram_space_pointer->ram_register_space.PLC_SerialNumber = 1717986918;
	ram_space_pointer->ram_register_space.PLC_BusConfig_B1 = 66847485;
	ram_space_pointer->ram_register_space.PLC_BusConfig_B2 = 100271355;
	ram_space_pointer->ram_register_space.PLC_TimeoutForDefect_B1 = 200;
	ram_space_pointer->ram_register_space.PLC_TimeoutForDefect_B2 = 200;
	ram_space_pointer->ram_register_space.PLC_NumCrcErrorsForDefect_B1 = 6;
	ram_space_pointer->ram_register_space.PLC_NumCrcErrorsForDefect_B2 = 6;
	ram_space_pointer->ram_register_space.PLC_TimeToRepair = 65535;
	ram_space_pointer->ram_register_space.PLC_TimeSoloWork = 61166;
	ram_space_pointer->ram_register_space.PLC_DualControl = 56797;
	memset(&(ram_space_pointer->ram_register_space.Reserv_2), 0, sizeof(ram_space_pointer->ram_register_space.Reserv_2));
	ram_space_pointer->ram_register_space.AI_OperMode = 0;
	for (uint8_t i = 0; i < CHANEL_NUMBER; i++)
	{
		ram_space_pointer->ram_register_space.AI_NumForAverag[i] = 10;
		ram_space_pointer->ram_register_space.AI_MinCodeADC[i] = 0;
		ram_space_pointer->ram_register_space.AI_MaxCodeADC[i] = 65535;
		ram_space_pointer->ram_register_space.AI_PolynConst0[i] = 0;
		ram_space_pointer->ram_register_space.AI_PolynConst1[i] = 0;
		ram_space_pointer->ram_register_space.AI_PolynConst2[i] = 0;
		ram_space_pointer->ram_register_space.AI_PolynConst3[i] = 0;
		ram_space_pointer->ram_register_space.AI_PolynConst4[i] = 0;
		ram_space_pointer->ram_register_space.AI_PolynConst5[i] = 0;
		ram_space_pointer->ram_register_space.AI_PolynConst6[i] = 0;
		ram_space_pointer->ram_register_space.AI_MetrologDat[i] = 0;
	}
	memset(ram_space_pointer->ram_register_space.AI_MetrologDat, 0, sizeof(ram_space_pointer->ram_register_space.AI_MetrologDat));
	memset(ram_space_pointer->ram_register_space.Reserv_4, 0, sizeof(ram_space_pointer->ram_register_space.Reserv_4));
	memset(ram_space_pointer->ram_register_space.Reserv_5, 0, sizeof(ram_space_pointer->ram_register_space.Reserv_5));
}

