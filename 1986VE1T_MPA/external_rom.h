/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � �������� ������ �������� ���
*/

#ifndef __EXTERNAL_ROM_H
#define __EXTERNAL_ROM_H

#include "MDR32_Drivers.h"
#include "external_ram.h"

#define HWREG(x) (*(( unsigned char *)(x)))

#define EXT_ROM_START_ADDR					 		0x00100000	///< ����� � ������ ��, � ������� ���������� ��������� � ������� ���
#define ROM_REGISTER_SPACE_START_ADDR 	0						///< ��������� ����� ����� ��������� � ���

///��������� � �������� ������ ��� �������� ��� ����������
typedef struct device_type_struct
{
	unsigned revision: 4;														///< ������� ������
	unsigned modification: 4;												///< ����������� ������
	unsigned type: 9;																///< ��� ������
	unsigned batch: 5;															///< ������
	unsigned use_object: 5;													///< ������ ����������
	unsigned reserv: 5;															///< ������
}__attribute__((packed)) device_type;

///��������� � �������� ������ ��� �������� ����� ������ ������
typedef struct ai_oper_mode_struct
{
	unsigned 	adc_chs_mode:						8;			///< ����� ������ �������
	unsigned 	reserv1:								8;			///< ������
}__attribute__((packed)) ai_oper_mode;

///���������� ������������ ����� ��������� ��� ������������ �� ������� ���
typedef struct common_register_space_ext_rom
{
	uint8_t					PLC_DeviceInfo[1024];
	device_type			PLC_DeviceType;
	uint32_t				PLC_SerialNumber;
	uint32_t				PLC_BusConfig_B1;
	uint32_t				PLC_BusConfig_B2;
	uint32_t				PLC_TimeoutForDefect_B1;
	uint32_t				PLC_TimeoutForDefect_B2;
	uint16_t				PLC_NumCrcErrorsForDefect_B1;
	uint16_t				PLC_NumCrcErrorsForDefect_B2;
	uint16_t				PLC_TimeToRepair;
	uint16_t				PLC_TimeSoloWork;
	uint16_t				PLC_DualControl;
	uint8_t	  			Reserv_2[64];
}__attribute__((packed)) common_rom_registers;

///���������� ������������ ��������� ��� ��� ������������ �� ������� ���
typedef struct mpa_register_space_ext_rom
{
	ai_oper_mode	AI_OperMode;
	uint16_t			AI_NumForAverag[MAX_CHANEL_NUMBER];
	int16_t				AI_MinCodeADC[MAX_CHANEL_NUMBER];
	int16_t				AI_MaxCodeADC[MAX_CHANEL_NUMBER];
	uint8_t				Reserv_3[32];
  float					AI_PolynConst0[MAX_CHANEL_NUMBER];
	float					AI_PolynConst1[MAX_CHANEL_NUMBER];
  float					AI_PolynConst2[MAX_CHANEL_NUMBER];
  float					AI_PolynConst3[MAX_CHANEL_NUMBER];
	float					AI_PolynConst4[MAX_CHANEL_NUMBER];
	float					AI_PolynConst5[MAX_CHANEL_NUMBER];
	float					AI_PolynConst6[MAX_CHANEL_NUMBER];
	uint8_t				AI_MetrologDat[32];
	uint8_t				Reserv_4[32];
	uint8_t				Reserv_5[32];
}__attribute__((packed)) mpa_rom_registers;

///��������� ������������ ������ �� ������� ���
typedef struct rom_data_struct
{
	common_rom_registers 	common_rom_registers_space;		///< ����� ��������
	mpa_rom_registers			mpa_rom_registers_space;			///< �������� ���
}__attribute__((packed)) rom_data;	

/*!
 *	\brief ���������������� ������� ������ �������� ���
*/
void init_external_rom_space(void);

uint8_t write_byte_rom(uint32_t dest_addr, uint8_t byte);

uint8_t read_byte_rom(uint32_t dest_addr);

void memcpy_to_rom(uint32_t dest_addr, void *src_addr, uint32_t size);

void erase_rom(void);

#endif /*__EXTERNAL_ROM_H */
