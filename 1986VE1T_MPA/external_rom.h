/*!
 \file
 \brief Заголовочный файл с описанием API для работы с областью памяти внешнего ПЗУ
*/

#ifndef __EXTERNAL_ROM_H
#define __EXTERNAL_ROM_H

#include "MDR32_Drivers.h"
#include "external_ram.h"

#define HWREG(x) (*(( unsigned char *)(x)))

#define EXT_ROM_START_ADDR					 		0x00100000	///< Адрес в памяти МК, с которой начинается обращение к внешней ПЗУ
#define ROM_REGISTER_SPACE_START_ADDR 	0						///< Стартовый адрес карты регистров в ПЗУ

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

///Структура с битовыми полями для регистра режим работы канала
typedef struct ai_oper_mode_struct
{
	unsigned 	adc_chs_mode:						8;			///< Режим работы каналов
	unsigned 	reserv1:								8;			///< Резерв
}__attribute__((packed)) ai_oper_mode;

///органиация пространства общих регистров для зеркализации во внешнем ПЗУ
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

///органиация пространства регистров МПА для зеркализации во внешнем ПЗУ
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

///Структура организующая память во внешнем ПЗУ
typedef struct rom_data_struct
{
	common_rom_registers 	common_rom_registers_space;		///< Общие регистры
	mpa_rom_registers			mpa_rom_registers_space;			///< Регистры МПА
}__attribute__((packed)) rom_data;	

/*!
 *	\brief Инициализацирует область памяти внешнего ПЗУ
*/
void init_external_rom_space(void);

uint8_t write_byte_rom(uint32_t dest_addr, uint8_t byte);

uint8_t read_byte_rom(uint32_t dest_addr);

void memcpy_to_rom(uint32_t dest_addr, void *src_addr, uint32_t size);

void erase_rom(void);

#endif /*__EXTERNAL_ROM_H */
