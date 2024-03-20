/*!
 \file
 \brief Файл с реализацией API для работы с областью памяти внешнего ПЗУ
*/

/*
 *	Внешняя память ПЗУ 1636Р1 подключается по внешней системной шине в 8-битном режиме данных.
 *	Сдвиг адресации делать не нужно A0->A0, A1->A1 и тд (т.к. 8 битный режим EBC)
*/

#include "external_ram.h"
#include "ebc.h"

#define FIRST_TIME_INIT ///< Макрос для записи данных (по умолчанию) в ПЗУ в первый раз

/// Константы полиномов
extern float polyn_ch_consts[MAX_CHANEL_NUMBER][7];

/*!
	Функция инициализации области памяти внешнего ПЗУ
*/
void init_external_rom_space(void)
{
	#ifdef FIRST_TIME_INIT
		common_rom_registers 	common_regs;
		mpa_rom_registers			mpa_regs;
		
		// Очистка ПЗУ
		erase_rom();
		
		// Инициализация общих регистров
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
		
		// Инциализация регистров МПА
		for (uint8_t i = 0; i < MAX_CHANEL_NUMBER; i++)
		{
			RESET_BIT(i, mpa_regs.AI_OperMode.adc_chs_mode);
			mpa_regs.AI_NumForAverag[i] = NUM_FOR_AVERAGE;
			mpa_regs.AI_MinCodeADC[i] = MIN_CODE_ADC;
			mpa_regs.AI_MaxCodeADC[i] = MAX_CODE_ADC;
			// Такие значения коэф. полиномов только для напряжения 0-10В
			mpa_regs.AI_PolynConst0[i] = polyn_ch_consts[i][0];
			mpa_regs.AI_PolynConst1[i] = polyn_ch_consts[i][1];
			mpa_regs.AI_PolynConst2[i] = polyn_ch_consts[i][2];
			mpa_regs.AI_PolynConst3[i] = polyn_ch_consts[i][3];
			mpa_regs.AI_PolynConst4[i] = polyn_ch_consts[i][4];
			mpa_regs.AI_PolynConst5[i] = polyn_ch_consts[i][5];
			mpa_regs.AI_PolynConst6[i] = polyn_ch_consts[i][6];
			mpa_regs.AI_MetrologDat[i] = METROLOG_DAT;
		}
		memset(mpa_regs.AI_MetrologDat, 0, sizeof(mpa_regs.AI_MetrologDat));
		memset(mpa_regs.Reserv_4, 0, sizeof(mpa_regs.Reserv_4));
		memset(mpa_regs.Reserv_5, 0, sizeof(mpa_regs.Reserv_5));
		
		// Копирование данных в ПЗУ
		memcpy_to_rom(ROM_REGISTER_SPACE_START_ADDR, &common_regs, sizeof(common_regs));
		memcpy_to_rom(ROM_REGISTER_SPACE_START_ADDR + sizeof(common_regs), &mpa_regs, sizeof(mpa_regs));
		
	#endif
}

/*!
	Функция записи байта в ПЗУ
*/
uint8_t write_byte_rom(uint32_t dest_addr, uint8_t byte)
{		  
	uint8_t status;
	
	// Конфигурирование выводов EBC на выход
	MDR_PORTA->OE = 0x0000FFFF;
	
	HWREG(EXT_ROM_START_ADDR + 0x555) = 0xAA;
	HWREG(EXT_ROM_START_ADDR + 0x2AA) = 0x55;
	HWREG(EXT_ROM_START_ADDR + 0x555) = 0xA0;
	HWREG(dest_addr + EXT_ROM_START_ADDR) = byte;

	// Конфигурирование выводов EBC на вход
	MDR_PORTA->OE = 0x00000000;
	
	// Теперь проверяем, ывполнилась ли запись успешно (успех = бит D7 == data[7])
	do status = HWREG(dest_addr + EXT_ROM_START_ADDR);
	while(((status & 0x80) != (byte & 0x80)) & ((status & 0x20) == 0));
	
	if((status & 0x20) != 0) 
	{
		status = HWREG(dest_addr + EXT_ROM_START_ADDR);
		if((status & 0x80) == (byte & 0x80)) //Успех
		{
			return 0;
		}
		else 
		{
			return 1; // Ошибка записи
		}
	}
	else if((status & 0x80) == (byte & 0x80)) // Успех
	{
		return 0;
	}
	return 2; 
}

/*!
	Функция чтения байта из ПЗУ
*/
uint8_t read_byte_rom(uint32_t dest_addr)
{ 
	// Конфигурирование выводов EBC на вход
	MDR_PORTA->OE = 0x00000000;	
	return (HWREG(dest_addr));
}

/*!
	Функция копирования области памяти в ПЗУ
*/
void memcpy_to_rom(uint32_t dest_addr, void *src_addr, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		write_byte_rom(dest_addr + i, *((uint8_t*)src_addr+i));
	}
}

/*!
	Функция очистки памяти ПЗУ
*/
void erase_rom(void)
{
	uint8_t status;
	
	// Команда Reset
	HWREG(EXT_ROM_START_ADDR) = 0xF0;
	
	// Конфигурирование выводов EBC на выход
	MDR_PORTA->OE = 0x0000FFFF;
	
	// Отправка соответствующей команды
	HWREG(EXT_ROM_START_ADDR + 0x555) = 0xAA;
	HWREG(EXT_ROM_START_ADDR + 0x2AA) = 0x55;
	HWREG(EXT_ROM_START_ADDR + 0x555) = 0x80;
	HWREG(EXT_ROM_START_ADDR + 0x555) = 0xAA;
	HWREG(EXT_ROM_START_ADDR + 0x2AA) = 0x55;
	HWREG(EXT_ROM_START_ADDR + 0x555) = 0x10;
	
	// Конфигурирование выводов EBC на вход
	MDR_PORTA->OE = 0x00000000;
	
	do status = HWREG(EXT_ROM_START_ADDR);
	while ((status & 0x80) != 0x80);
}
