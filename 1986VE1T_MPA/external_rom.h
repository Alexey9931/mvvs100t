/*!
 \file
 \brief Заголовочный файл с описанием API для работы с областью памяти внешнего ПЗУ
*/

#ifndef __EXTERNAL_ROM_H
#define __EXTERNAL_ROM_H

#include "mdr32_drivers.h"

//#define ROM_IS_USED //макрос использования ПЗУ, т.е. регистры которые должны хранится в ПЗУ хранятся в ПЗУ, а не в ОЗУ

#define HWREG(x) (*(( unsigned char *)(x)))					///< Макрос чтения байта по адресу

#define EXT_ROM_START_ADDR					 		0x00100000	///< Адрес в памяти МК, с которой начинается обращение к внешней ПЗУ
#define ROM_REGISTER_SPACE_START_ADDR 	0						///< Стартовый адрес карты регистров в ПЗУ

#define DEV_TYPE MPA											///< Тип устройства (значение для инициализации по умолчанию)
#define DEV_INFO "MPA"										///< Текстовая информация о модуле (значение для инициализации по умолчанию)
#define REVISION 1												///< Ревизия модуля (значение для инициализации по умолчанию)
#define MODIFICATION 2										///< Модификация модуля (значение для инициализации по умолчанию)
#define BATCH 1														///< Партия (значение для инициализации по умолчанию)
#define DEV_TYPE_RESERV 20								///< Резерв (значение для инициализации по умолчанию)
#define SERIAL_NUMBER 1717986918					///< Серийный номер устройства (значение для инициализации по умолчанию)
#define TIMEOUT_FOR_DEFECT_B1 200					///< Время без связи до неиспр шины 1, мс (значение для инициализации по умолчанию)
#define TIMEOUT_FOR_DEFECT_B2 200					///< Время без связи до неиспр шины 2, мс (значение для инициализации по умолчанию)
#define NUM_CRC_ERRORS_FOR_DEFECT_B1 6		///< Количество ошибок приема пакета до неисправности шины 1 (значение для инициализации по умолчанию)
#define NUM_CRC_ERRORS_FOR_DEFECT_B2 6		///< Количество ошибок приема пакета до неисправности шины 2 (значение для инициализации по умолчанию)
#define TIME_TO_REPAIR 65535							///< Максимально время  переключения на резервный УМ, х10мс (значение для инициализации по умолчанию)
#define TIME_SOLO_WORK 61166							///< Время работы без резервирующего УМ, с (значение для инициализации по умолчанию)
#define DUAL_CONTROL 56797								///< Реакция при одновременном управлении  (значение для инициализации по умолчанию)

#define NUM_FOR_AVERAGE 10								///< Кол-во выборок для усреднения (значение для инициализации по умолчанию)
#define MIN_CODE_ADC 0										///< Минимальный код АЦП (значение для инициализации по умолчанию)
#define MAX_CODE_ADC 65535								///< Максимальный код АЦП (значение для инициализации по умолчанию)
#define METROLOG_DAT 0.0f									///< Сведения о метрологии (значение для инициализации по умолчанию)

/**
 * @brief Структура с битовыми полями для регистра тип устройства
 *
 */
typedef struct device_type_struct
{
	unsigned revision: 4;											///< Ревизия модуля
	unsigned modification: 4;									///< Модификация модуля
	unsigned type: 9;													///< Тип модуля
	unsigned batch: 5;												///< Партия
	unsigned use_object: 5;										///< Объект применения
	unsigned reserv: 5;												///< Резерв
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
device_type;

/**
 * @brief Структура с битовыми полями для регистра режим работы канала
 *
 */
typedef struct ai_oper_mode_struct
{
	unsigned 	adc_chs_mode:						8;			///< Режим работы каналов
	unsigned 	reserv1:								8;			///< Резерв
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
ai_oper_mode;

/**
 * @brief Органиация пространства общих регистров для зеркализации во внешнем ПЗУ
 *
 */
typedef struct common_register_space_ext_rom
{
	uint8_t					PLC_DeviceInfo[1024];								///< Текстовая информация о модуле
	device_type			PLC_DeviceType;											///< Тип устройства
	uint32_t				PLC_SerialNumber;										///< Серийный номер устройства
	uint32_t				PLC_BusConfig_B1;										///< Конфигурация шины 1
	uint32_t				PLC_BusConfig_B2;										///< Конфигурация шины 2
	uint32_t				PLC_TimeoutForDefect_B1;						///< Время без связи до неиспр шины 1, мс
	uint32_t				PLC_TimeoutForDefect_B2;						///< Время без связи до неиспр шины 2, мс
	uint16_t				PLC_NumCrcErrorsForDefect_B1;				///< Количество ошибок приема пакета до неисправности шины 1
	uint16_t				PLC_NumCrcErrorsForDefect_B2;				///< Количество ошибок приема пакета до неисправности шины 2
	uint16_t				PLC_TimeToRepair;										///< Максимально время  переключения на резервный УМ, х10мс
	uint16_t				PLC_TimeSoloWork;										///< Время работы без резервирующего УМ, с
	uint16_t				PLC_DualControl;										///< Реакция при одновременном управлении 
	uint8_t	  			Reserv_2[64];												///< РЕЗЕРВ
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
common_rom_registers;

/**
 * @brief Органиация пространства регистров МПА для зеркализации во внешнем ПЗУ
 *
 */
typedef struct mpa_register_space_ext_rom
{
	ai_oper_mode	AI_OperMode;													///< Режим работы канала
	uint16_t			AI_NumForAverag[MAX_CHANEL_NUMBER];		///< Кол-во выборок для усреднения
	int16_t				AI_MinCodeADC[MAX_CHANEL_NUMBER];			///< Константа B - минимум кода АЦП
	int16_t				AI_MaxCodeADC[MAX_CHANEL_NUMBER];			///< Константа A - максимум кода АЦП
	uint8_t				Reserv_3[32];													///< Резерв
  float					AI_PolynConst0[MAX_CHANEL_NUMBER];		///< Константа А0 апрокс. полинома
	float					AI_PolynConst1[MAX_CHANEL_NUMBER];		///< Константа А1 апрокс. полинома
  float					AI_PolynConst2[MAX_CHANEL_NUMBER];		///< Константа А2 апрокс. полинома
  float					AI_PolynConst3[MAX_CHANEL_NUMBER];		///< Константа А3 апрокс. полинома
	float					AI_PolynConst4[MAX_CHANEL_NUMBER];		///< Константа А4 апрокс. полинома
	float					AI_PolynConst5[MAX_CHANEL_NUMBER];		///< Константа А5 апрокс. полинома
	float					AI_PolynConst6[MAX_CHANEL_NUMBER];		///< Константа А6 апрокс. полинома
	uint8_t				AI_MetrologDat[32];										///< Сведения о метрологии
	uint8_t				Reserv_4[32];													///< Резерв
	uint8_t				Reserv_5[32];													///< Резерв
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
mpa_rom_registers;						///<

/**
 * @brief Структура организующая память во внешнем ПЗУ
 *
 */
typedef struct rom_data_struct
{
	common_rom_registers 	common_rom_registers_space;		///< Общие регистры
	mpa_rom_registers			mpa_rom_registers_space;			///< Регистры МПА
}
#ifndef DOXYGEN
__attribute__((packed))
#endif
rom_data;	

/*!
 *	\brief Инициализацирует область памяти внешнего ПЗУ
*/
void init_external_rom_space(void);

/*!
 *	\brief Записывает байт в ПЗУ
 *	\param dest_addr - адрес назначения
 *	\param byte - записываемый байт
*/
uint8_t write_byte_rom(uint32_t dest_addr, uint8_t byte);

/*!
 *	\brief Читает байт из ПЗУ
 *	\param dest_addr - адрес чтения
 *  \return Байт
*/
uint8_t read_byte_rom(uint32_t dest_addr);

/*!
 *	\brief Копирует область памяти в ПЗУ
 *	\param dest_addr - адрес назначения
 *	\param src_addr - адрес источника
 *	\param size - размер данных
*/
void memcpy_to_rom(uint32_t dest_addr, void *src_addr, uint32_t size);

/*!
 *	\brief Очищает память в ПЗУ
*/
void erase_rom(void);

#endif /*__EXTERNAL_ROM_H */
