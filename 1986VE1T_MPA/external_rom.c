/*!
 \file
 \brief ���� � ����������� API ��� ������ � �������� ������ �������� ���
*/

/*
 *	������� ������ ��� 1636�1 ������������ �� ������� ��������� ���� � 8-������ ������ ������.
 *	����� ��������� ������ �� ����� A0->A0, A1->A1 � �� (�.�. 8 ������ ����� EBC)
*/

#include "external_ram.h"
#include "ebc.h"

#define FIRST_TIME_INIT //������ ��� ������ ������ (�� ���������) � ��� � ������ ���

/*!
	������� ������������� ������� ������ �������� ���
*/
void init_external_rom_space(void)
{
	#ifdef FIRST_TIME_INIT
		common_rom_registers 	common_regs;
		mpa_rom_registers			mpa_regs;
		
		erase_rom();
		
		//������������� ����� ���������
		strncpy(&common_regs.PLC_DeviceInfo,DEV_INFO,sizeof(DEV_INFO));	
		common_regs.PLC_DeviceType.revision = REVISION;
		common_regs.PLC_DeviceType.modification = MODIFICATION;
		common_regs.PLC_DeviceType.type = DEV_TYPE;
		common_regs.PLC_DeviceType.batch = BATCH;
		common_regs.PLC_DeviceType.reserv = DEV_TYPE_RESERV;
		common_regs.PLC_SerialNumber = SERIAL_NUMBER;
		common_regs.PLC_TimeoutForDefect_B1 = TIMEOUT_FOR_DEFECT_B1;
		common_regs.PLC_TimeoutForDefect_B2 = TIMEOUT_FOR_DEFECT_B2;
		common_regs.PLC_NumCrcErrorsForDefect_B1 = NUM_CRC_ERRORS_FOR_DEFECT_B1;
		common_regs.PLC_NumCrcErrorsForDefect_B2 = NUM_CRC_ERRORS_FOR_DEFECT_B2;
		common_regs.PLC_TimeToRepair = TIME_TO_REPAIR;
		common_regs.PLC_TimeSoloWork = TIME_SOLO_WORK;
		common_regs.PLC_DualControl = DUAL_CONTROL;
		memset(&common_regs.Reserv_2, 0, sizeof(common_regs.Reserv_2));
		
		//������������ ��������� ���
		for (uint8_t i = 0; i < MAX_CHANEL_NUMBER; i++)
		{
			RESET_BIT(i, mpa_regs.AI_OperMode.adc_chs_mode);
			mpa_regs.AI_NumForAverag[i] = NUM_FOR_AVERAGE;
			mpa_regs.AI_MinCodeADC[i] = MIN_CODE_ADC;
			mpa_regs.AI_MaxCodeADC[i] = MAX_CODE_ADC;
			//����� �������� ����. ��������� ������ ��� ���������� 0-10�
			mpa_regs.AI_PolynConst0[i] = POLYN_CONST_0;
			mpa_regs.AI_PolynConst1[i] = POLYN_CONST_1;
			mpa_regs.AI_PolynConst2[i] = POLYN_CONST_2;
			mpa_regs.AI_PolynConst3[i] = POLYN_CONST_3;
			mpa_regs.AI_PolynConst4[i] = POLYN_CONST_4; 
			mpa_regs.AI_PolynConst5[i] = POLYN_CONST_5;
			mpa_regs.AI_PolynConst6[i] = POLYN_CONST_6;
			mpa_regs.AI_MetrologDat[i] = METROLOG_DAT;
		}
		memset(mpa_regs.AI_MetrologDat, 0, sizeof(mpa_regs.AI_MetrologDat));
		memset(mpa_regs.Reserv_4, 0, sizeof(mpa_regs.Reserv_4));
		memset(mpa_regs.Reserv_5, 0, sizeof(mpa_regs.Reserv_5));
		
		//����������� � ���
		memcpy_to_rom(ROM_REGISTER_SPACE_START_ADDR, &common_regs, sizeof(common_regs));
		memcpy_to_rom(ROM_REGISTER_SPACE_START_ADDR + sizeof(common_regs), &mpa_regs, sizeof(mpa_regs));
		
	#endif
}

/*!
	������� ������ ����� � ���
*/
uint8_t write_byte_rom(uint32_t dest_addr, uint8_t byte)
{		  
	uint8_t status;
	
  /* Configure PORTA pins 0..15 for inout ExtBus data  */
	MDR_PORTA->OE = 0x0000FFFF;
	
	HWREG(EXT_ROM_START_ADDR + 0x555) = 0xAA;
	HWREG(EXT_ROM_START_ADDR + 0x2AA) = 0x55;
	HWREG(EXT_ROM_START_ADDR + 0x555) = 0xA0;
	HWREG(dest_addr + EXT_ROM_START_ADDR) = byte;

	MDR_PORTA->OE = 0x00000000;
	
	// ������ ���������, ����������� �� ������ ������� (����� = ��� D7 == data[7])
	do status = HWREG(dest_addr + EXT_ROM_START_ADDR);
	while(((status & 0x80) != (byte & 0x80)) & ((status & 0x20) == 0));
	
	if((status & 0x20) != 0) 
	{
		status = HWREG(dest_addr + EXT_ROM_START_ADDR);
		if((status & 0x80) == (byte & 0x80)) //�����
		{
			return 0;
		}
		else 
		{
			return 1; // Error write
		}
	}
	else if((status & 0x80) == (byte & 0x80)) //�����
	{
		return 0;
	}
	return 2; // Unknow state
}

/*!
	������� ������ ����� �� ���
*/
uint8_t read_byte_rom(uint32_t dest_addr)
{ 
	MDR_PORTA->OE = 0x00000000;	
	return (HWREG(dest_addr));
}

/*!
	������� ����������� ������� ������ � ���
*/
void memcpy_to_rom(uint32_t dest_addr, void *src_addr, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		write_byte_rom(dest_addr + i, *((uint8_t*)src_addr+i));
	}
}

/*!
	������� ������� ������ ���
*/
void erase_rom(void)
{
	uint8_t status;
	
	//������� Reset
	HWREG(EXT_ROM_START_ADDR) = 0xF0;
	
	MDR_PORTA->OE = 0x0000FFFF;
	
	//CHIP erase
	HWREG(EXT_ROM_START_ADDR + 0x555) = 0xAA;
	HWREG(EXT_ROM_START_ADDR + 0x2AA) = 0x55;
	HWREG(EXT_ROM_START_ADDR + 0x555) = 0x80;
	HWREG(EXT_ROM_START_ADDR + 0x555) = 0xAA;
	HWREG(EXT_ROM_START_ADDR + 0x2AA) = 0x55;
	HWREG(EXT_ROM_START_ADDR + 0x555) = 0x10;
	
	
	MDR_PORTA->OE = 0x00000000;
	
	do status = HWREG(EXT_ROM_START_ADDR);
	while ((status & 0x80) != 0x80);
}
