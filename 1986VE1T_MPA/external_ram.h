/*!
 \file
 \brief Заголовочный файл с описанием API для работы с областью памяти внешнего ОЗУ
*/

#ifndef __EXTERNAL_RAM_H
#define __EXTERNAL_RAM_H

#include "mdr32_drivers.h"
#include "rs422_protocol.h"
#include "spi.h"
#include "external_rom.h"

#define EXT_RAM_START_ADDR 							0x50200000	///< Физический адрес в памяти МК, с которой начинается обращение к внешней ОЗУ
#define RAM_REGISTER_SPACE_START_ADDR 	200					///< Стартовый адрес карты регистров в ОЗУ

#define TIMER_NUM	4				///< Кол-во таймеров в МК

#define TEST_BIT(num, value) ((value>>num)&0x1)			///< Макрос проверки бита (0 или 1) в байте
#define SET_BIT(num, value) (value |= (1<<num))			///< Макрос установки бита в байте в 1
#define RESET_BIT(num, value) (value &= ~(1<<num))	///< Макрос сброса бита в байте в 0

#define START_STRUCT_LENGTH 							32			///< Длина структуры
#define START_STRUCT_TEXT_INFO_ADDR 			332			///< Ссылка на текстовую информацию о модуле
#define START_STRUCT_CHANGE_FLAG 					0				///< Флаг изменения структуры
#define START_STRUCT_NUMBER_OF_RANGES 		3				///< Кол-во диапазонов в стартовой структуре
#define START_STRUCT_RANGE0_TYPE 					0x0100	///< Тип диапазона (h) и тип операции (l)
#define START_STRUCT_RANGE0_START_CH_NUM 	0x0000	///< Стартовый номер канала (h) и номер диапазона (l)
#define START_STRUCT_RANGE0_ADDR 					1874		///< Адрес
#define START_STRUCT_RANGE0_SIZE 					32			///< Количество байт
#define START_STRUCT_RANGE1_TYPE 					0x0400	///< Тип диапазона (h) и тип операции (l)
#define START_STRUCT_RANGE1_START_CH_NUM 	0x0001	///< Стартовый номер канала (h) и номер диапазона (l)
#define START_STRUCT_RANGE1_ADDR 					254			///< Адрес
#define START_STRUCT_RANGE1_SIZE 					10			///< Количество байт
#define START_STRUCT_RANGE2_TYPE 					0x0500	///< Тип диапазона (h) и тип операции (l)
#define START_STRUCT_RANGE2_START_CH_NUM 	0x0002	///< Стартовый номер канала (h) и номер диапазона (l)
#define START_STRUCT_RANGE2_ADDR 					1906		///< Адрес
#define START_STRUCT_RANGE2_SIZE 					8				///< Количество байт

#define PLC_SOFT_VER_REVISION 			1								///< Ревизия модуля
#define PLC_SOFT_VER_MODIFICATION 	2								///< Модификация модуля
#define PLC_SOFT_VER_TYPE 					MPA							///< Тип модуля
#define PLC_SOFT_VER_SOFT_VER 			1								///< Версия ПО
#define PLC_SOFT_VER_ADD_INFO 			4								///< Доп.информация
#define PLC_SOFT_VER_DEVELOP 				1								///< 1=ПО в процессе разработки
#define PLC_PM_MODULE_ADDR					PM_DEV_ADDR			///< адрес модуля
#define PLC_PM_CHASSIS_ADDR 				PM_CHASSIS_ADDR	///< адрес шасси
#define PLC_CONFIG_MAIN_SWITCH 			4								///< Основной свитч
#define PLC_CONFIG_ADD_SWITCH1 			3								///< 1 Доп свитч (при наличии)
#define PLC_CONFIG_ADD_SWITCH2 			2								///< 2 Доп свитч (при наличии)
#define PLC_CONFIG_RESERV 					1								///< резерв

/**
 * @brief Типы модулей
 *
 */
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

/**
 * @brief Структура с битовыми полями для регистра версия ПО
 *
 */
typedef struct plc_sof_ver_struct
{
	unsigned revision: 4;											///< Ревизия модуля
	unsigned modification: 4;									///< Модификация модуля
	unsigned type: 9;													///< Тип модуля
	unsigned soft_ver: 10;										///< Версия ПО
	unsigned add_info: 4;											///< Дополнительная информация 
	unsigned develop: 1;											///< 1=ПО в процессе разработки 
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
plc_soft_ver;

/**
 * @brief Структура с битовыми полями для регистра адрес устройства
 *
 */
typedef struct device_address_struct
{
	unsigned module_addr: 	4;								///< Адрес модуля 
	unsigned chassis_addr: 	4;								///< Адрес шасси
	unsigned reserv:				8;								///< Резерв
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
device_address;

/**
 * @brief Структура с битовыми полями для регистра конфигурация
 *
 */
typedef struct config_struct
{
	unsigned main_switch: 	4;							  ///< Основной свитч
	unsigned add_switch_1: 	4;								///< Доп свитч 1
	unsigned add_switch_2: 	4;								///< Доп свитч 2
	unsigned reserv:				4;								///< Резерв
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
device_config;

/**
 * @brief Структура с битовыми полями для регистра неисправность питания 
 *
 */
typedef struct power_failure_struct
{
	unsigned v2_3_3: 		1;										///< 2V3.3
	unsigned v2_5: 			1;										///< 2V5
	unsigned v1_3_3: 		1;										///< 1V3.3
	unsigned v1_5:			1;										///< 1V5
	unsigned reserv:		12;										///< Резерв
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
power_failure;

/**
 * @brief Структура с битовыми полями для регистра неисправность шины
 *
 */
typedef struct bus_defect_struct
{
	unsigned many_fail_packet: 1;							///< Количество битых пакетов подряд > уст. значения
	unsigned fail_timeout: 1;									///< Неисправность по таймауту 
	unsigned reserv: 14;											///< Резерв
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
bus_defect;

/**
 * @brief Структура с битовыми полями для регистра неисправность самодиагностики
 *
 */
typedef struct self_diag_struct
{
	unsigned fail_crc_firmware: 				1;		///< Некорректная ЦРЦ прошивки
	unsigned power_fail: 								1;		///< Неисправность питания 
	unsigned fail_download_rom: 				1;		///< Ошибка при загрузке данных из ПЗУ 
	unsigned fail_soft_ver:							1;		///< Версия ПО и тип модуля не совпадают 
	unsigned fail_firmware_ram:					1;	  ///< Неисправность  в ПО арбитра ОЗУ  
	unsigned fail_firmware_2_bus:				1;	  ///< Неисправность  в ПО 2ой шины
	unsigned fail_firmware_1_bus:				1;		///< Неисправность  в ПО 1ой шины
	unsigned reserv:										1;		///< Резерв
	uint8_t fail_chanels[8];									///< Неисправность в каналах
	unsigned reserv1:										8;		///< Резерв
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
self_diag;

/**
 * @brief Структура одного диапазона для стартовой структуры
 *
 */
typedef struct range_start_struct
{
	uint16_t	range_type;										  ///< Тип диапазона (h) и тип операции (l)
	uint16_t	start_channel_num;						  ///< Стартовый номер канала (h) и номер диапазона (l)
	uint16_t	address;											  ///< Адрес
	uint16_t	size;													  ///< Количество байт
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
range;

/**
 * @brief Структура, которая лежит в начале ОЗУ любого модуля 
 *
 */
typedef struct start_struct_ext_ram
{
	uint16_t	length;																									///< Длина структуры 
	uint16_t	text_info;																							///< Ссылка на текстувую информацию о модуле 
	uint16_t	flag_change_struct;																			///< Флаг изменения структуры 
	uint16_t	number_of_ranges;																				///< Количество диапазонов 
	range			ranges_in_start_struct[START_STRUCT_NUMBER_OF_RANGES];	///< Диапазоны в стартовой структуре 
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
ram_start_struct;

/**
 * @brief Органиация пространства общих регистров во внешнем ОЗУ
 *
 */
typedef struct common_register_space_ext_ram
{
	plc_soft_ver 						PLC_SoftVer;									///< Версия ПО											
	device_config						PLC_Config;										///< Конфигурация устройства 
	device_address					PLC_PMAddr;										///< Адрес устройства
	uint32_t								PLC_Durat;										///< Время с момента запуска, с
	uint32_t								PLC_CM_State;									///< Состояние автомата выбора УМ 
	uint32_t								PLC_CorrPackFromDevice_B1;		///< Корректных пакетов по Ш1, от устройства
	uint32_t								PLC_CorrPackToDevice_B1;			///< Корректных пакетов по Ш1, к устройству
	uint32_t								PLC_ErrPackToDevice_B1;				///< Ошибок приема пакета по Ш1
	uint32_t								PLC_ErrPackFromDevice_B1;			///< Ошибок отправки пакета по Ш1
	uint32_t								PLC_CorrPackFromDevice_B2;		///< Корректных пакетов по Ш2, от устройства
	uint32_t								PLC_CorrPackToDevice_B2;			///< Корректных пакетов по Ш2, к устройству
	uint32_t								PLC_ErrPackToDevice_B2;				///< Ошибок приема пакета по Ш2
	uint32_t								PLC_ErrPackFromDevice_B2;			///< Ошибок отправки пакета по Ш2
	power_failure						PLC_PowerDefect;							///< Неиспр питания 
	bus_defect							PLC_BusDefect_B1;							///< Неиспр 1 шины
	bus_defect  						PLC_BusDefect_B2;							///< Неиспр 2 шины
	self_diag	  						PLC_SelfDiagDefect;						///< Неиспр самодиагностики 
	uint8_t	  							Reserv_1[68];									///< РЕЗЕРВ
	common_rom_registers	 	PLC_CommonRomRegs;						///< Общие регистры, которые храняться в ПЗУ 
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
common_ram_registers;

/**
 * @brief Органиация пространства регистров МПА во внешнем ОЗУ 
 *
 */
typedef struct mpa_register_space_ext_ram
{
	mpa_rom_registers 	AI_RomRegs;																	///< Регистры МПА, которые храняться в ПЗУ
	uint16_t						AI_SignalChanged;														///< Изменялся ли сигнал с последнего опроса   
	int16_t							AI_CodeADC[MAX_CHANEL_NUMBER];							///< Сырые данные, код АЦП
	float								AI_PhysQuantFloat[MAX_CHANEL_NUMBER];				///< Физическая величина (число с плавающей точкой)
	uint8_t							AI_DiagnosticChannel[MAX_CHANEL_NUMBER];		///< Самодиагностика каналов
	uint8_t							Reserv_6[2];																///< Резерв
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
mpa_ram_registers;	

/**
 * @brief Структура с битовыми полями для сервисного байта ПМ 
 *
 */
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
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
service_struct_pm;

/**
 * @brief Структура с битовыми полями для сервисного байта УМ
 *
 */
typedef struct service_byte_struct_um
{
	unsigned last_answer: 1;													///< 1= не получен ответ на предыдущий пакет
	unsigned reserv: 6;																///< Резерв
	unsigned ready_to_control: 1;											///< Флаг «Готовность выполнять управление»
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
service_struct_um;

/**
 * @brief Структура организующая память во внешнем ОЗУ
 *
 */
typedef struct ram_data_struct
{
	ram_start_struct 				start_struct;																											///< Структура по умолчанию, которая должна находиться в начале ОЗУ 
	uint8_t 								Reserv[RAM_REGISTER_SPACE_START_ADDR - sizeof(ram_start_struct)];	///< Зарезервированное место 
	common_ram_registers		common_ram_register_space;																				///< Карта общих регистров начиная с адреса REGISTER_SPACE_START_ADDR
	mpa_ram_registers				mpa_ram_register_space;																						///< Карта регистров МПА начиная с адреса REGISTER_SPACE_START_ADDR
	fields_packet 					rx_packet_struct;																									///< Принятый пакет с идентифицированными полями 
	fields_packet 					tx_packet_struct;																									///< Отправляемый пакет с идентифицированными полями 
	uint8_t 								tx_data[UART_BUFFER_SIZE];																				///< Выделенное место для данных tx_packet 
	uint8_t 								packet_rx[UART_BUFFER_SIZE];																			///< Буфер с принятым пакетом 
	uint8_t 								packet_tx[UART_BUFFER_SIZE];																		  ///< Буфер с отправленным пакетом
	service_struct_pm				service_byte_pm;																									///< Структура сервисного байта ПМ
	service_struct_um				service_byte_um;																									///< Структура сервисного байта УМ
	uint_least32_t 					crc_table[256];																										///< Таблица для вычисления контрольной суммы 
	uint8_t 								uart1_rx_buffer[UART_BUFFER_SIZE];																///< Буфер приемника UART1
	uint8_t 								uart2_rx_buffer[UART_BUFFER_SIZE];																///< Буфер приемника UART2
	uint16_t 								spi_1_rx_buffer[SPI_BUFFER_SIZE];																	///< Буфер приемника SPI1
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
ram_data;

/*!
 *	\brief Инициализацирует область памяти внешнего ОЗУ 
*/
void init_external_ram_space(void);

	
#endif /*__EXTERNAL_RAM_H */
