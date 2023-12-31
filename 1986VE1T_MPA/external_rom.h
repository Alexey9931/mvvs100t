/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � �������� ������ �������� ���
*/

#ifndef __EXTERNAL_ROM_H
#define __EXTERNAL_ROM_H

#include "mdr32_drivers.h"

//#define ROM_IS_USED //������ ������������� ���, �.�. �������� ������� ������ �������� � ��� �������� � ���, � �� � ���

#define HWREG(x) (*(( unsigned char *)(x)))

#define EXT_ROM_START_ADDR					 		0x00100000	///< ����� � ������ ��, � ������� ���������� ��������� � ������� ���
#define ROM_REGISTER_SPACE_START_ADDR 	0						///< ��������� ����� ����� ��������� � ���

#define DEV_TYPE MPA
#define DEV_INFO "MPA"
#define REVISION 1
#define MODIFICATION 2
#define BATCH 1
#define DEV_TYPE_RESERV 20
#define SERIAL_NUMBER 1717986918
#define TIMEOUT_FOR_DEFECT_B1 200
#define TIMEOUT_FOR_DEFECT_B2 200
#define NUM_CRC_ERRORS_FOR_DEFECT_B1 6
#define NUM_CRC_ERRORS_FOR_DEFECT_B2 6
#define TIME_TO_REPAIR 65535
#define TIME_SOLO_WORK 61166
#define DUAL_CONTROL 56797

#define NUM_FOR_AVERAGE 10
#define MIN_CODE_ADC 0
#define MAX_CODE_ADC 65535
#define METROLOG_DAT 0.0f


///��������� � �������� ������ ��� �������� ��� ����������
typedef struct device_type_struct
{
	unsigned revision: 4;											///< ������� ������
	unsigned modification: 4;									///< ����������� ������
	unsigned type: 9;													///< ��� ������
	unsigned batch: 5;												///< ������
	unsigned use_object: 5;										///< ������ ����������
	unsigned reserv: 5;												///< ������
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
	uint8_t					PLC_DeviceInfo[1024];								///< ��������� ���������� � ������
	device_type			PLC_DeviceType;											///< ��� ����������
	uint32_t				PLC_SerialNumber;										///< �������� ����� ����������
	uint32_t				PLC_BusConfig_B1;										///< ������������ ���� 1
	uint32_t				PLC_BusConfig_B2;										///< ������������ ���� 2
	uint32_t				PLC_TimeoutForDefect_B1;						///< ����� ��� ����� �� ������ ���� 1, ��
	uint32_t				PLC_TimeoutForDefect_B2;						///< ����� ��� ����� �� ������ ���� 2, ��
	uint16_t				PLC_NumCrcErrorsForDefect_B1;				///< ���������� ������ ������ ������ �� ������������� ���� 1
	uint16_t				PLC_NumCrcErrorsForDefect_B2;				///< ���������� ������ ������ ������ �� ������������� ���� 2
	uint16_t				PLC_TimeToRepair;										///< ����������� �����  ������������ �� ��������� ��, �10��
	uint16_t				PLC_TimeSoloWork;										///< ����� ������ ��� �������������� ��, �
	uint16_t				PLC_DualControl;										///< ������� ��� ������������� ���������� 
	uint8_t	  			Reserv_2[64];												///< ������
}__attribute__((packed)) common_rom_registers;

///���������� ������������ ��������� ��� ��� ������������ �� ������� ���
typedef struct mpa_register_space_ext_rom
{
	ai_oper_mode	AI_OperMode;													///< ����� ������ ������
	uint16_t			AI_NumForAverag[MAX_CHANEL_NUMBER];		///< ���-�� ������� ��� ����������
	int16_t				AI_MinCodeADC[MAX_CHANEL_NUMBER];			///< ��������� B - ������� ���� ���
	int16_t				AI_MaxCodeADC[MAX_CHANEL_NUMBER];			///< ��������� A - �������� ���� ���
	uint8_t				Reserv_3[32];													///< ������
  float					AI_PolynConst0[MAX_CHANEL_NUMBER];		///< ��������� �0 ������. ��������
	float					AI_PolynConst1[MAX_CHANEL_NUMBER];		///< ��������� �1 ������. ��������
  float					AI_PolynConst2[MAX_CHANEL_NUMBER];		///< ��������� �2 ������. ��������
  float					AI_PolynConst3[MAX_CHANEL_NUMBER];		///< ��������� �3 ������. ��������
	float					AI_PolynConst4[MAX_CHANEL_NUMBER];		///< ��������� �4 ������. ��������
	float					AI_PolynConst5[MAX_CHANEL_NUMBER];		///< ��������� �5 ������. ��������
	float					AI_PolynConst6[MAX_CHANEL_NUMBER];		///< ��������� �6 ������. ��������
	uint8_t				AI_MetrologDat[32];										///< �������� � ����������
	uint8_t				Reserv_4[32];													///< ������
	uint8_t				Reserv_5[32];													///< ������
}__attribute__((packed)) mpa_rom_registers;						///<
	
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

/*!
 *	\brief ���������� ���� � ���
 *	\param dest_addr - ����� ����������
 *	\param byte - ������������ ����
*/
uint8_t write_byte_rom(uint32_t dest_addr, uint8_t byte);

/*!
 *	\brief ������ ���� �� ���
 *	\param dest_addr - ����� ������
 *  \return ����
*/
uint8_t read_byte_rom(uint32_t dest_addr);

/*!
 *	\brief �������� ������� ������ � ���
 *	\param dest_addr - ����� ����������
 *	\param src_addr - ����� ���������
 *	\param size - ������ ������
*/
void memcpy_to_rom(uint32_t dest_addr, void *src_addr, uint32_t size);

/*!
 *	\brief ������� ������ � ���
*/
void erase_rom(void);

#endif /*__EXTERNAL_ROM_H */
