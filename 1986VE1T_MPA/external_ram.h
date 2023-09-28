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
#define CHANEL_NUMBER 8									///< Кол-во каналов в МПА
#define PM_DEV_ADDR 0										///< Адрес модуля
#define PM_CHASSIS_ADDR 0								///< Адрес шасси
#define NUMBER_OF_RANGES 3							///< Кол-во диапазонов в стартовой структуре

///Типы модулей
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

///Структура с битовыми полями для регистра версия ПО
typedef struct plc_sof_ver_struct
{
	unsigned revision: 4;														///< Ревизия модуля
	unsigned modification: 4;												///< Модификация модуля
	unsigned type: 9;																///< Тип модуля
	unsigned soft_ver: 10;														///< Версия ПО
	unsigned add_info: 4;																///< Дополнительная информация
	unsigned develop: 1;														///< 1=ПО в процессе разработки
}__attribute__((packed)) plc_soft_ver;

///Структура с битовыми полями для регистра тип устройства
typedef struct device_type_struct
{
	unsigned revision: 4;														///< Ревизия модуля
	unsigned modification: 4;												///< Модификация модуля
	unsigned type: 9;																///< Тип модуля
	unsigned batch: 5;															///< Партия
	unsigned use_object: 5;													///< Объект применения
	unsigned reserv: 5;															///< Резерв
}__attribute__((packed)) device_type;

///Структура с битовыми полями для регистра адрес устройства
typedef struct device_address_struct
{
	unsigned module_addr: 	4;										///< Адрес модуля
	unsigned chassis_addr: 	4;										///< Адрес шасси
	unsigned reserv:				8;										///< Резерв
}__attribute__((packed)) device_address;

///Структура с битовыми полями для регистра конфигурация
typedef struct config_struct
{
	unsigned main_switch: 	4;										///< Основной свитч
	unsigned add_switch_1: 	4;										///< Доп свитч 1
	unsigned add_switch_2: 	4;										///< Доп свитч 2
	unsigned reserv:				4;										///< Резерв
}__attribute__((packed)) device_config;

///Структура с битовыми полями для регистра неисправность питания
typedef struct power_failure_struct
{
	unsigned v2_3_3: 		1;										///< 2V3.3
	unsigned v2_5: 			1;										///< 2V5
	unsigned v1_3_3: 		1;										///< 1V3.3
	unsigned v1_5:			1;										///< 1V5
	unsigned reserv:		12;										///< Резерв
}__attribute__((packed)) power_failure;

///Структура с битовыми полями для регистра неисправность шины
typedef struct bus_defect_struct
{
	unsigned many_fail_packet: 1;										///< Количество битых пакетов подряд > уст. значения
	unsigned fail_timeout: 1;												///< Неисправность по таймауту
	unsigned reserv: 14;														///< Резерв
}__attribute__((packed)) bus_defect;

///Структура с битовыми полями для регистра неисправность самодиагностики
typedef struct self_diag_struct
{
	unsigned fail_crc_firmware: 					1;										///< Некорректная ЦРЦ прошивки
	unsigned power_fail: 									1;										///< Неисправность питания
	unsigned fail_download_rom: 					1;										///< Ошибка при загрузке данных из ПЗУ
	unsigned fail_soft_ver:								1;										///< Версия ПО и тип модуля не совпадают
	unsigned fail_firmware_ram:						1;										///< Неисправность  в ПО арбитра ОЗУ
	unsigned fail_firmware_2_bus:					1;										///< Неисправность  в ПО 2ой шины
	unsigned fail_firmware_1_bus:					1;										///< Неисправность  в ПО 1ой шины
	unsigned reserv:											1;										///< Резерв
	uint8_t fail_chanels[8];																		///< Неисправность в каналах
	unsigned reserv1:											8;										///< Резерв
}__attribute__((packed)) self_diag;
///Структура одного диапазона для стартовой структуры
typedef struct range_start_struct
{
	uint16_t	range_type;										///< Тип диапазона (h) и тип операции (l)
	uint16_t	start_channel_num;						///< Стартовый номер канала (h) и номер диапазона (l)
	uint16_t	address;											///< Адрес
	uint16_t	size;													///< Количество байт
}__attribute__((packed)) range;
///структура, которая лежит в начале ОЗУ любого модуля
typedef struct start_struct_ext_ram
{
	uint16_t	length;																			///< Длина структуры
	uint16_t	text_info;																	///< Ссылка на текстувую информацию о модуле
	uint16_t	flag_change_struct;													///< Флаг изменения структуры
	uint16_t	number_of_ranges;														///< Количество диапазонов
	range			ranges_in_start_struct[NUMBER_OF_RANGES];		///< Диапазоны в стартовой структуре
}__attribute__((packed)) ram_start_struct;
///органиация пространства общих регистров во внешнем ОЗУ
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
///органиация пространства регистров МПА во внешнем ОЗУ
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
	ram_start_struct 				start_struct;																											///< Структура по умолчанию, которая должна находиться в начале ОЗУ
	uint8_t 								Reserv[REGISTER_SPACE_START_ADDR - sizeof(ram_start_struct)];			///< Зарезервированное место
	common_ram_registers		common_ram_register_space;																				///< Карта общих регистров начиная с адреса REGISTER_SPACE_START_ADDR
	mpa_ram_registers				mpa_ram_register_space;																						///< Карта регистров МПА начиная с адреса REGISTER_SPACE_START_ADDR
	fields_packet 					rx_packet_struct;																									///< Принятый пакет с идентифицированными полями
	fields_packet 					tx_packet_struct;																									///< Отправляемый пакет с идентифицированными полями
	uint8_t 								tx_data[BUFFER_SIZE];																							///< Выделенное место для данных tx_packet 
	uint8_t 								packet_rx[BUFFER_SIZE];																						///< Буфер с принятым пакетом
	uint8_t 								packet_tx[BUFFER_SIZE];																						///< Буфер с отправленным пакетом
	service_struct_pm				service_byte_pm;																									///< Структура сервисного байта ПМ
	service_struct_um				service_byte_um;																									///< Структура сервисного байта УМ
	uint_least32_t 					crc_table[256];																										///< Таблица для вычисления контрольной суммы
	uint8_t 								uart1_rx_buffer[BUFFER_SIZE];																			///< Буфер приемника UART1
}__attribute__((packed)) ram_data;

/*!
 *	\brief Инициализацирует область памяти внешнего ОЗУ
 *	\param *ram_space_pointer - указатель на область внешнего ОЗУ
*/
void init_external_ram_space(void);
	
#endif /*__EXTERNAL_RAM_H */