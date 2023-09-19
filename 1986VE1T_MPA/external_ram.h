/*!
 \file
 \brief Заголовочный файл с описанием API для работы с областью памяти внешнего ОЗУ
*/

#ifndef __EXTERNAL_RAM_H
#define __EXTERNAL_RAM_H

#include "MDR32_Drivers.h"
#include "rs422_protocol.h"

#define EXT_RAM_START_ADDR 0x50200000		///< Адрес в памяти МК, с которой начинается обращение к внешней ОЗУ
#define REGISTER_SPACE_START_ADDR 200		///< Стартовый адрес карты регистров в ОЗУ
#define CHANEL_NUMBER 8			
#define RX_DATA_SIZE 256
#define PM_ADDR 0

///Структура с битовыми полями для регистра неисправность шины
typedef struct bus_defect_struct
{
	unsigned many_fail_packet: 1;										///< Количество битых пакетов подряд > уст. значения
	unsigned fail_timeout: 1;												///< Неисправность по таймауту
	unsigned reserv: 6;															///< Резерв
}__attribute__((packed)) bus_defect;
///структура, которая лежит в начале ОЗУ любого модуля
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
///органиация пространства регистров во внешней ОЗУ
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
///Структура с битовыми полями для сервисного байта ПМ
typedef struct service_byte_struct_pm
{
	unsigned init: 1;																	///< Инициализация (0 – не инициализирован, 1 – инициализирован)
	unsigned self_diagnostics_error: 1;								///< Неисправность самодиагностики
	unsigned reserv_1: 1;															///< Резерв
	unsigned reserv_2: 1;															///< Резерв
	unsigned fail_bus_1: 1;														///< Неисправность (1 шина)
	unsigned fail_bus_2: 1;														///< Неисправность (2 шина)
	unsigned both_control: 1;													///< Признак одновременного управления
	unsigned master: 1;																///< Ведущий мастер (0 – первый, 1 – второй)
}__attribute__((packed)) service_struct_pm;
///Структура с битовыми полями для сервисного байта УМ
typedef struct service_byte_struct_um
{
	unsigned last_answer: 1;													///< 1= не получен ответ на предыдущий пакет
	unsigned reserv: 6;																///< Резерв
	unsigned ready_to_control: 1;											///< Флаг «Готовность выполнять управление»
}__attribute__((packed)) service_struct_um;
///Структура организующая память во внешнем ОЗУ
typedef struct ram_data_struct
{
	ram_start_struct start_struct;																						///< Структура по умолчанию, которая должна находиться в начале ОЗУ
	uint8_t Reserv[REGISTER_SPACE_START_ADDR - sizeof(ram_start_struct)];			///< Зарезервированное место
	ram_registers 			ram_register_space;																		///< Карта регистров начиная с адреса REGISTER_SPACE_START_ADDR
	fields_packet 			rx_packet_struct;																			///< Принятый пакет с идентифицированными полями
	fields_packet 			tx_packet_struct;																			///< Отправляемый пакет с идентифицированными полями
	fields_cmd 					tx_cmd_packet[NUMBER_CMDS_IN_PACKET];									///< Массив данных для конкретных команд в отправляемом пакете
	fields_cmd 					rx_cmd_packet[NUMBER_CMDS_IN_PACKET];									///< Массив данных для конкретных команд в принятом пакете
	uint8_t 						tx_data[RX_DATA_SIZE];																///< Выделенное место для данных tx_packet (пока 1кбайт)
	uint8_t 						buffer_tx[BUFFER_SIZE];																///< Буфер для передатчика UART
	service_struct_pm		service_byte_pm;																			///< Структура сервисного байта ПМ
	service_struct_um		service_byte_um;																			///< Структура сервисного байта УМ
}__attribute__((packed)) ram_data;

/*!
 *	\brief Инициализацирует область памяти внешнего ОЗУ
 *	\param *ram_space_pointer - указатель на область внешнего ОЗУ
*/
void init_external_ram_space(void);
	
#endif /*__EXTERNAL_RAM_H */