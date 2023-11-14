/*!
 \file
 \brief ���� � ����������� API ��� ������ � �������� ������ �������� ���
*/

/*
 *	������� ������ ��� 1645�4 ������������ �� ������� ��������� ���� � 32-������ ������ ������.
 *	����� ��������� ������ ����� A2->A0, A3->A1 � �� (�.�. ��������� �������� � ��� ����� ������ � 32 ������ ����� EBC, � ��� ����� ����� �����)
*/

#include "external_ram.h"
#include "ebc.h"
#include <string.h>
#include <math.h>

extern ram_data *ram_space_pointer;
extern rom_data *rom_space_pointer;


/*!
	������� ������������� ������� ������ �������� ���
*/
void init_external_ram_space(void)
{
	ram_space_pointer = (ram_data*)EXT_RAM_START_ADDR;
	rom_space_pointer = (rom_data*)EXT_ROM_START_ADDR;
	
	ram_start_struct* start_struct_ptr = &ram_space_pointer->start_struct;
	common_ram_registers* common_ram_reg_ptr = &ram_space_pointer->common_ram_register_space;
	mpa_rom_registers* mpa_rom_reg_ptr = &ram_space_pointer->mpa_ram_register_space.AI_RomRegs;
	common_rom_registers* common_rom_reg_ptr = &ram_space_pointer->common_ram_register_space.PLC_CommonRomRegs;
	
	//��������� ������� ������������� ����� ������ ���
	memset(ram_space_pointer, 0, sizeof(ram_data));
	//������������� ���������, ������� ����� � ������ ���
	start_struct_ptr->length = START_STRUCT_LENGTH;
	start_struct_ptr->text_info = START_STRUCT_TEXT_INFO_ADDR;
	start_struct_ptr->flag_change_struct = START_STRUCT_CHANGE_FLAG;
	start_struct_ptr->number_of_ranges = START_STRUCT_NUMBER_OF_RANGES;
	start_struct_ptr->ranges_in_start_struct[0].range_type = START_STRUCT_RANGE0_TYPE;
	start_struct_ptr->ranges_in_start_struct[0].start_channel_num = START_STRUCT_RANGE0_START_CH_NUM;
	start_struct_ptr->ranges_in_start_struct[0].address = START_STRUCT_RANGE0_ADDR;
	start_struct_ptr->ranges_in_start_struct[0].size = START_STRUCT_RANGE0_SIZE;
	start_struct_ptr->ranges_in_start_struct[1].range_type = START_STRUCT_RANGE1_TYPE;
	start_struct_ptr->ranges_in_start_struct[1].start_channel_num = START_STRUCT_RANGE1_START_CH_NUM;
	start_struct_ptr->ranges_in_start_struct[1].address = START_STRUCT_RANGE1_ADDR;
	start_struct_ptr->ranges_in_start_struct[1].size = START_STRUCT_RANGE1_SIZE;
	start_struct_ptr->ranges_in_start_struct[2].range_type = START_STRUCT_RANGE2_TYPE;
	start_struct_ptr->ranges_in_start_struct[2].start_channel_num = START_STRUCT_RANGE2_START_CH_NUM;
	start_struct_ptr->ranges_in_start_struct[2].address = START_STRUCT_RANGE2_ADDR;
	start_struct_ptr->ranges_in_start_struct[2].size = START_STRUCT_RANGE2_SIZE;
	
	//������ ����� ��������� �� ������ 200 �� ������� ��� � ������������� ��
	//������ ��������, ������� ���������������� � ��
	common_ram_reg_ptr->PLC_SoftVer.revision = PLC_SOFT_VER_REVISION;
	common_ram_reg_ptr->PLC_SoftVer.modification = PLC_SOFT_VER_MODIFICATION;
	common_ram_reg_ptr->PLC_SoftVer.type = PLC_SOFT_VER_TYPE;
	common_ram_reg_ptr->PLC_SoftVer.soft_ver = PLC_SOFT_VER_SOFT_VER;
	common_ram_reg_ptr->PLC_SoftVer.add_info = PLC_SOFT_VER_ADD_INFO;
	common_ram_reg_ptr->PLC_SoftVer.develop = PLC_SOFT_VER_DEVELOP;
	common_ram_reg_ptr->PLC_PMAddr.module_addr = PLC_PM_MODULE_ADDR;
	common_ram_reg_ptr->PLC_PMAddr.chassis_addr = PLC_PM_CHASSIS_ADDR;
	common_ram_reg_ptr->PLC_Config.main_switch = PLC_CONFIG_MAIN_SWITCH;
	common_ram_reg_ptr->PLC_Config.add_switch_1 = PLC_CONFIG_ADD_SWITCH1;
	common_ram_reg_ptr->PLC_Config.add_switch_2 = PLC_CONFIG_ADD_SWITCH2;
	common_ram_reg_ptr->PLC_Config.reserv = PLC_CONFIG_RESERV;
	common_ram_reg_ptr->PLC_CM_State = PLC_CM_NOT_INIT;
	
	//������ ��������, ������� ������� �� ���
	#ifdef ROM_IS_USED
		common_rom_registers 	common_regs;
		mpa_rom_registers			mpa_regs;
		
		ebc_init(EBC_ROM);
		memcpy(&common_regs, &rom_space_pointer->common_rom_registers_space, sizeof(common_regs));
		memcpy(&mpa_regs, &rom_space_pointer->mpa_rom_registers_space, sizeof(mpa_regs));
		ebc_init(EBC_RAM);

		memcpy(&ram_space_pointer->common_ram_register_space.PLC_CommonRomRegs, &common_regs, sizeof(common_regs));
		memcpy(&ram_space_pointer->mpa_ram_register_space.AI_RomRegs, &mpa_regs, sizeof(mpa_regs));
	#endif
	#ifndef ROM_IS_USED
		//������������� ����� ���������
		strncpy(&common_rom_reg_ptr->PLC_DeviceInfo,DEV_INFO,sizeof(DEV_INFO));	
		common_rom_reg_ptr->PLC_DeviceType.revision = REVISION;
		common_rom_reg_ptr->PLC_DeviceType.modification = MODIFICATION;
		common_rom_reg_ptr->PLC_DeviceType.type = MPA;
		common_rom_reg_ptr->PLC_DeviceType.batch = DEV_TYPE;
		common_rom_reg_ptr->PLC_DeviceType.reserv = DEV_TYPE_RESERV;
		common_rom_reg_ptr->PLC_SerialNumber = SERIAL_NUMBER;
		common_rom_reg_ptr->PLC_TimeoutForDefect_B1 = TIMEOUT_FOR_DEFECT_B1;
		common_rom_reg_ptr->PLC_TimeoutForDefect_B2 = TIMEOUT_FOR_DEFECT_B2;
		common_rom_reg_ptr->PLC_NumCrcErrorsForDefect_B1 = NUM_CRC_ERRORS_FOR_DEFECT_B1;
		common_rom_reg_ptr->PLC_NumCrcErrorsForDefect_B2 = NUM_CRC_ERRORS_FOR_DEFECT_B2;
		common_rom_reg_ptr->PLC_TimeToRepair = TIME_TO_REPAIR;
		common_rom_reg_ptr->PLC_TimeSoloWork = TIME_SOLO_WORK;
		common_rom_reg_ptr->PLC_DualControl = DUAL_CONTROL;
		memset(&common_rom_reg_ptr->Reserv_2, 0, sizeof(common_rom_reg_ptr->Reserv_2));
		
		//������������ ��������� ���
		for (uint8_t i = 0; i < MAX_CHANEL_NUMBER; i++)
		{
			RESET_BIT(i, mpa_rom_reg_ptr->AI_OperMode.adc_chs_mode);
			mpa_rom_reg_ptr->AI_NumForAverag[i] = NUM_FOR_AVERAGE;
			mpa_rom_reg_ptr->AI_MinCodeADC[i] = MIN_CODE_ADC;
			mpa_rom_reg_ptr->AI_MaxCodeADC[i] = MAX_CODE_ADC;
			//����� �������� ����. ��������� ������ ��� ���������� 0-10�
			mpa_rom_reg_ptr->AI_PolynConst0[i] = POLYN_CONST_0;
			mpa_rom_reg_ptr->AI_PolynConst1[i] = POLYN_CONST_1;
			mpa_rom_reg_ptr->AI_PolynConst2[i] = POLYN_CONST_2;
			mpa_rom_reg_ptr->AI_PolynConst3[i] = POLYN_CONST_3;
			mpa_rom_reg_ptr->AI_PolynConst4[i] = POLYN_CONST_4; 
			mpa_rom_reg_ptr->AI_PolynConst5[i] = POLYN_CONST_5;
			mpa_rom_reg_ptr->AI_PolynConst6[i] = POLYN_CONST_6;
			mpa_rom_reg_ptr->AI_MetrologDat[i] = METROLOG_DAT;
		}
		memset(mpa_rom_reg_ptr->AI_MetrologDat, 0, sizeof(mpa_rom_reg_ptr->AI_MetrologDat));
		memset(mpa_rom_reg_ptr->Reserv_4, 0, sizeof(mpa_rom_reg_ptr->Reserv_4));
		memset(mpa_rom_reg_ptr->Reserv_5, 0, sizeof(mpa_rom_reg_ptr->Reserv_5));
	#endif
	
	//��������� ������� CRC32
	fill_crc32_table();
}

/*!
	������� ���������� ������������� �������� ������� 
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
/*!
	������� ��������� ��������� ������ �� ������� ��� (������� ������)
*/
uint8_t* malloc_ram_pages(uint32_t size)
{
	uint32_t page_num = size/PAGE_SIZE + 1; //���-�� ���������� �������
	uint32_t page_cnt; //������� �������
	uint32_t consecutive_page_cnt; //������� ������ ������ ������ �������
	 
	for (page_cnt = 0; (page_cnt + page_num) < PAGE_SIZE; page_cnt++)
	{
		//��������� ��� ������ ���� ������ �������� ������
		for (consecutive_page_cnt = 0; consecutive_page_cnt < page_num; consecutive_page_cnt++)
		{
			if (ram_space_pointer->memory_page_status[page_cnt + consecutive_page_cnt] != 0)
			{
				page_cnt += consecutive_page_cnt;
				break;
			}
		}
		//���� ������� ����� ��������� ���-�� ������ ������ ������ ��������, �� ����������� ���������
		if (consecutive_page_cnt == page_num)	
		{
			//��������� ������ �������
			for (consecutive_page_cnt = 0; consecutive_page_cnt < page_num; consecutive_page_cnt++)
			{
				memset(&ram_space_pointer->memory_page_status[page_cnt + consecutive_page_cnt], 1, sizeof(ram_space_pointer->memory_page_status[page_cnt + consecutive_page_cnt]));
			}
			return &ram_space_pointer->memory_page_space[page_cnt][0];
		}
	}
	
	return NULL;
}
/*!
	������� ������������ ������ �� ������� ��� (������� ������)
*/
void free_ram_pages(uint8_t *page_addr, uint32_t size)
{
	uint32_t page_num = size/PAGE_SIZE + 1; //���-�� ������������� �������
	uint32_t page_cnt; //������� �������
	 
	for (page_cnt = 0; (page_cnt + page_num) < PAGE_SIZE; page_cnt++)
	{
		if (page_addr == &ram_space_pointer->memory_page_space[page_cnt][PAGE_SIZE])
		{
			//��������� ������ �������
			for (uint32_t i = 0; i < page_num; i++)
			{
				memset(&ram_space_pointer->memory_page_status[page_cnt + i], 0, sizeof(ram_space_pointer->memory_page_status[page_cnt + i]));
			}
		}
	}
}

