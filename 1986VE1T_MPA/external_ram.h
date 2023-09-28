/*!
 \file
 \brief ������������ ���� � ��������� API ��� ������ � �������� ������ �������� ���
*/

#ifndef __EXTERNAL_RAM_H
#define __EXTERNAL_RAM_H

#include "MDR32_Drivers.h"
#include "rs422_protocol.h"

#define EXT_RAM_START_ADDR 0x50200000		///< ����� � ������ ��, � ������� ���������� ��������� � ������� ���
#define REGISTER_SPACE_START_ADDR 200		///< ��������� ����� ����� ��������� � ���
#define CHANEL_NUMBER 8									///< ���-�� ������� � ���
#define PM_DEV_ADDR 0										///< ����� ������
#define PM_CHASSIS_ADDR 0								///< ����� �����
#define NUMBER_OF_RANGES 3							///< ���-�� ���������� � ��������� ���������

///���� �������
typedef enum type_of_module
{
	MPD = 1,															
	MVD,														
	MVD_U,														
	MPT,												
	MVA,
	MPA,
	MPI_U,
	MSR,
	MPCH,
	MPPT,
	MPI
} module_type;

///��������� � �������� ������ ��� �������� ������ ��
typedef struct plc_sof_ver_struct
{
	unsigned revision: 4;														///< ������� ������
	unsigned modification: 4;												///< ����������� ������
	unsigned type: 9;																///< ��� ������
	unsigned soft_ver: 10;														///< ������ ��
	unsigned add_info: 4;																///< �������������� ����������
	unsigned develop: 1;														///< 1=�� � �������� ����������
}__attribute__((packed)) plc_soft_ver;

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

///��������� � �������� ������ ��� �������� ����� ����������
typedef struct device_address_struct
{
	unsigned module_addr: 	4;										///< ����� ������
	unsigned chassis_addr: 	4;										///< ����� �����
	unsigned reserv:				8;										///< ������
}__attribute__((packed)) device_address;

///��������� � �������� ������ ��� �������� ������������
typedef struct config_struct
{
	unsigned main_switch: 	4;										///< �������� �����
	unsigned add_switch_1: 	4;										///< ��� ����� 1
	unsigned add_switch_2: 	4;										///< ��� ����� 2
	unsigned reserv:				4;										///< ������
}__attribute__((packed)) device_config;

///��������� � �������� ������ ��� �������� ������������� �������
typedef struct power_failure_struct
{
	unsigned v2_3_3: 		1;										///< 2V3.3
	unsigned v2_5: 			1;										///< 2V5
	unsigned v1_3_3: 		1;										///< 1V3.3
	unsigned v1_5:			1;										///< 1V5
	unsigned reserv:		12;										///< ������
}__attribute__((packed)) power_failure;

///��������� � �������� ������ ��� �������� ������������� ����
typedef struct bus_defect_struct
{
	unsigned many_fail_packet: 1;										///< ���������� ����� ������� ������ > ���. ��������
	unsigned fail_timeout: 1;												///< ������������� �� ��������
	unsigned reserv: 14;														///< ������
}__attribute__((packed)) bus_defect;

///��������� � �������� ������ ��� �������� ������������� ���������������
typedef struct self_diag_struct
{
	unsigned fail_crc_firmware: 					1;										///< ������������ ��� ��������
	unsigned power_fail: 									1;										///< ������������� �������
	unsigned fail_download_rom: 					1;										///< ������ ��� �������� ������ �� ���
	unsigned fail_soft_ver:								1;										///< ������ �� � ��� ������ �� ���������
	unsigned fail_firmware_ram:						1;										///< �������������  � �� ������� ���
	unsigned fail_firmware_2_bus:					1;										///< �������������  � �� 2�� ����
	unsigned fail_firmware_1_bus:					1;										///< �������������  � �� 1�� ����
	unsigned reserv:											1;										///< ������
	uint8_t fail_chanels[8];																		///< ������������� � �������
	unsigned reserv1:											8;										///< ������
}__attribute__((packed)) self_diag;
///��������� ������ ��������� ��� ��������� ���������
typedef struct range_start_struct
{
	uint16_t	range_type;										///< ��� ��������� (h) � ��� �������� (l)
	uint16_t	start_channel_num;						///< ��������� ����� ������ (h) � ����� ��������� (l)
	uint16_t	address;											///< �����
	uint16_t	size;													///< ���������� ����
}__attribute__((packed)) range;
///���������, ������� ����� � ������ ��� ������ ������
typedef struct start_struct_ext_ram
{
	uint16_t	length;																			///< ����� ���������
	uint16_t	text_info;																	///< ������ �� ��������� ���������� � ������
	uint16_t	flag_change_struct;													///< ���� ��������� ���������
	uint16_t	number_of_ranges;														///< ���������� ����������
	range			ranges_in_start_struct[NUMBER_OF_RANGES];		///< ��������� � ��������� ���������
}__attribute__((packed)) ram_start_struct;
///���������� ������������ ����� ��������� �� ������� ���
typedef struct common_register_space_ext_ram
{
	plc_soft_ver 		PLC_SoftVer;																	
	device_config		PLC_Config;
	device_address	PLC_PMAddr;
	uint32_t				PLC_Durat;
	uint32_t				PLC_CM_State;
	uint32_t				PLC_CorrPackFromDevice_B1;
	uint32_t				PLC_CorrPackToDevice_B1;
	uint32_t				PLC_ErrPackToDevice_B1;
	uint32_t				PLC_ErrPackFromDevice_B1;
	uint32_t				PLC_CorrPackFromDevice_B2;
	uint32_t				PLC_CorrPackToDevice_B2;
	uint32_t				PLC_ErrPackToDevice_B2;
	uint32_t				PLC_ErrPackFromDevice_B2;
	power_failure		PLC_PowerDefect;
	bus_defect			PLC_BusDefect_B1;
	bus_defect  		PLC_BusDefect_B2;
	self_diag	  		PLC_SelfDiagDefect;
	uint8_t	  			Reserv_1[68];
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
}__attribute__((packed)) common_ram_registers;
///���������� ������������ ��������� ��� �� ������� ���
typedef struct mpa_register_space_ext_ram
{
	uint16_t			AI_OperMode;
	uint16_t			AI_NumForAverag[CHANEL_NUMBER];
	uint16_t			AI_MinCodeADC[CHANEL_NUMBER];
	uint16_t			AI_MaxCodeADC[CHANEL_NUMBER];
	uint8_t				Reserv_3[32];
  float					AI_PolynConst0[CHANEL_NUMBER];
	float					AI_PolynConst1[CHANEL_NUMBER];
  float					AI_PolynConst2[CHANEL_NUMBER];
  float					AI_PolynConst3[CHANEL_NUMBER];
	float					AI_PolynConst4[CHANEL_NUMBER];
	float					AI_PolynConst5[CHANEL_NUMBER];
	float					AI_PolynConst6[CHANEL_NUMBER];
	uint8_t				AI_MetrologDat[32];
	uint8_t				Reserv_4[32];
	uint8_t				Reserv_5[32];
	uint16_t			AI_SignalChanged;
	uint16_t			AI_CodeADC[CHANEL_NUMBER];
	uint32_t			AI_PhysQuantFloat[CHANEL_NUMBER];
	uint8_t				AI_DiagnosticChannel[CHANEL_NUMBER];
	uint8_t				Reserv_6[2];
}__attribute__((packed)) mpa_ram_registers;
///��������� � �������� ������ ��� ���������� ����� ��
typedef struct service_byte_struct_pm
{
	unsigned init: 1;																	///< ������������� (0 � �� ���������������, 1 � ���������������)
	unsigned self_diagnostics_error: 1;								///< ������������� ���������������
	unsigned reserv_1: 1;															///< ������
	unsigned reserv_2: 1;															///< ������
	unsigned fail_bus_1: 1;														///< ������������� (1 ����)
	unsigned fail_bus_2: 1;														///< ������������� (2 ����)
	unsigned both_control: 1;													///< ������� �������������� ����������
	unsigned master: 1;																///< ������� ������ (0 � ������, 1 � ������)
}__attribute__((packed)) service_struct_pm;
///��������� � �������� ������ ��� ���������� ����� ��
typedef struct service_byte_struct_um
{
	unsigned last_answer: 1;													///< 1= �� ������� ����� �� ���������� �����
	unsigned reserv: 6;																///< ������
	unsigned ready_to_control: 1;											///< ���� ����������� ��������� ����������
}__attribute__((packed)) service_struct_um;
///��������� ������������ ������ �� ������� ���
typedef struct ram_data_struct
{
	ram_start_struct 				start_struct;																											///< ��������� �� ���������, ������� ������ ���������� � ������ ���
	uint8_t 								Reserv[REGISTER_SPACE_START_ADDR - sizeof(ram_start_struct)];			///< ����������������� �����
	common_ram_registers		common_ram_register_space;																				///< ����� ����� ��������� ������� � ������ REGISTER_SPACE_START_ADDR
	mpa_ram_registers				mpa_ram_register_space;																						///< ����� ��������� ��� ������� � ������ REGISTER_SPACE_START_ADDR
	fields_packet 					rx_packet_struct;																									///< �������� ����� � ������������������� ������
	fields_packet 					tx_packet_struct;																									///< ������������ ����� � ������������������� ������
	uint8_t 								tx_data[BUFFER_SIZE];																							///< ���������� ����� ��� ������ tx_packet 
	uint8_t 								packet_rx[BUFFER_SIZE];																						///< ����� � �������� �������
	uint8_t 								packet_tx[BUFFER_SIZE];																						///< ����� � ������������ �������
	service_struct_pm				service_byte_pm;																									///< ��������� ���������� ����� ��
	service_struct_um				service_byte_um;																									///< ��������� ���������� ����� ��
	uint_least32_t 					crc_table[256];																										///< ������� ��� ���������� ����������� �����
	uint8_t 								uart1_rx_buffer[BUFFER_SIZE];																			///< ����� ��������� UART1
}__attribute__((packed)) ram_data;

/*!
 *	\brief ���������������� ������� ������ �������� ���
 *	\param *ram_space_pointer - ��������� �� ������� �������� ���
*/
void init_external_ram_space(void);
	
#endif /*__EXTERNAL_RAM_H */