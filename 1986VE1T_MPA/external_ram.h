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
#define CHANEL_NUMBER 8			
#define RX_DATA_SIZE 256
#define PM_ADDR 0

///��������� � �������� ������ ��� �������� ������������� ����
typedef struct bus_defect_struct
{
	unsigned many_fail_packet: 1;										///< ���������� ����� ������� ������ > ���. ��������
	unsigned fail_timeout: 1;												///< ������������� �� ��������
	unsigned reserv: 6;															///< ������
}__attribute__((packed)) bus_defect;
///���������, ������� ����� � ������ ��� ������ ������
typedef struct start_struct_ext_ram
{
	uint16_t	Length;
	uint16_t	TextInfo;
	uint16_t	FlagChangeStruct;
	uint16_t	NumberRanges;
	uint16_t	RangeType_0;
	uint16_t	StartChannelNumb_0;
	uint16_t	Address_0;
	uint16_t	Size_0;
	uint16_t	RangeType_1;
	uint16_t	StartChannelNumb_1;
	uint16_t	Address_1;
	uint16_t	Size_1;
	uint16_t	RangeType_2;
	uint16_t	StartChannelNumb_2;
	uint16_t	Address_2;
	uint16_t	Size_2;
}__attribute__((packed)) ram_start_struct;
///���������� ������������ ��������� �� ������� ���
typedef struct register_space_ext_ram
{
	uint32_t 		PLC_SoftVer;																	
	uint16_t		PLC_Config;
	uint16_t		PLC_PMAddr;
	uint32_t		PLC_Durat;
	uint32_t		PLC_CM_State;
	uint32_t		PLC_CorrPackFromDevice_B1;
	uint32_t		PLC_CorrPackToDevice_B1;
	uint32_t		PLC_ErrPackToDevice_B1;
	uint32_t		PLC_ErrPackFromDevice_B1;
	uint32_t		PLC_CorrPackFromDevice_B2;
	uint32_t		PLC_CorrPackToDevice_B2;
	uint32_t		PLC_ErrPackToDevice_B2;
	uint32_t		PLC_ErrPackFromDevice_B2;
	uint16_t		PLC_PowerDefect;
	bus_defect	PLC_BusDefect_B1;
	bus_defect  PLC_BusDefect_B2;
	uint8_t	  	PLC_SelfDiagDefect[10];
	uint8_t	  	Reserv_1[68];
	uint8_t			PLC_DeviceInfo[1024];
	uint32_t		PLC_DeviceType;
	uint32_t		PLC_SerialNumber;
	uint32_t		PLC_BusConfig_B1;
	uint32_t		PLC_BusConfig_B2;
	uint32_t		PLC_TimeoutForDefect_B1;
	uint32_t		PLC_TimeoutForDefect_B2;
	uint16_t		PLC_NumCrcErrorsForDefect_B1;
	uint16_t		PLC_NumCrcErrorsForDefect_B2;
	uint16_t		PLC_TimeToRepair;
	uint16_t		PLC_TimeSoloWork;
	uint16_t		PLC_DualControl;
	uint8_t	  	Reserv_2[64];
	uint16_t		AI_OperMode;
	uint16_t		AI_NumForAverag[CHANEL_NUMBER];
	uint16_t		AI_MinCodeADC[CHANEL_NUMBER];
	uint16_t		AI_MaxCodeADC[CHANEL_NUMBER];
	uint8_t			Reserv_3[32];
  float				AI_PolynConst0[CHANEL_NUMBER];
	float				AI_PolynConst1[CHANEL_NUMBER];
  float				AI_PolynConst2[CHANEL_NUMBER];
  float				AI_PolynConst3[CHANEL_NUMBER];
	float				AI_PolynConst4[CHANEL_NUMBER];
	float				AI_PolynConst5[CHANEL_NUMBER];
	float				AI_PolynConst6[CHANEL_NUMBER];
	uint8_t			AI_MetrologDat[32];
	uint8_t			Reserv_4[32];
	uint8_t			Reserv_5[32];
	uint16_t		AI_SignalChanged;
	uint16_t		AI_CodeADC[CHANEL_NUMBER];
	uint32_t		AI_PhysQuantFloat[CHANEL_NUMBER];
	uint8_t			AI_DiagnosticChannel[CHANEL_NUMBER];
	uint8_t			Reserv_6[2];

}__attribute__((packed)) ram_registers;
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
	ram_start_struct start_struct;																						///< ��������� �� ���������, ������� ������ ���������� � ������ ���
	uint8_t Reserv[REGISTER_SPACE_START_ADDR - sizeof(ram_start_struct)];			///< ����������������� �����
	ram_registers 			ram_register_space;																		///< ����� ��������� ������� � ������ REGISTER_SPACE_START_ADDR
	fields_packet 			rx_packet_struct;																			///< �������� ����� � ������������������� ������
	fields_packet 			tx_packet_struct;																			///< ������������ ����� � ������������������� ������
	fields_cmd 					tx_cmd_packet[NUMBER_CMDS_IN_PACKET];									///< ������ ������ ��� ���������� ������ � ������������ ������
	fields_cmd 					rx_cmd_packet[NUMBER_CMDS_IN_PACKET];									///< ������ ������ ��� ���������� ������ � �������� ������
	uint8_t 						tx_data[RX_DATA_SIZE];																///< ���������� ����� ��� ������ tx_packet (���� 1�����)
	uint8_t 						buffer_tx[BUFFER_SIZE];																///< ����� ��� ����������� UART
	service_struct_pm		service_byte_pm;																			///< ��������� ���������� ����� ��
	service_struct_um		service_byte_um;																			///< ��������� ���������� ����� ��
}__attribute__((packed)) ram_data;

/*!
 *	\brief ���������������� ������� ������ �������� ���
 *	\param *ram_space_pointer - ��������� �� ������� �������� ���
*/
void init_external_ram_space(void);
	
#endif /*__EXTERNAL_RAM_H */