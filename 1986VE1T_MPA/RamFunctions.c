#ifndef	__RAM_FUNCTIONS_C
#define __RAM_FUNCTIONS_C

#include <MDR32F1QI.h>
#include <MDR32FxQI_timer.h>
#include <MDR32FxQI_port.h>
#include <MDR32FxQI_rst_clk.h>
#include "RamFunctions.h"



//Функция для записи данных от 0 до 0x3FFFF в ОЗУ
//Параметры address - адрес, с которого необходимо записывать данные в ОЗУ
//			inv - флаг записи инверсного значения в память ОЗУ
//Возвращаемых значений нет
void WriteDataToRAM(uint32_t address, uint32_t inv)
{
	uint32_t*	ptr;
	uint32_t	tmp;
	
	ptr=(uint32_t*)address;

	MDR_PORTE->CLRTX=0x40;

	if(!inv)
		for(tmp=0;tmp<0x40000;tmp++)	*ptr++ =tmp;
	else
		for(tmp=0;tmp<0x40000;tmp++)	*ptr++ =(~tmp);
				
	MDR_PORTE->SETTX=0x40;
}

//Функция для проверки записанных данных в ОЗУ
//Параметры address - адрес, с которого необходимо тестировать данные в ОЗУ
//			inv - флаг проверки инверсных значений в памяти ОЗУ
//Возвращаемое значение:	0 - данные в памяти отличаются от записанных
//							1 - все данные в памяти совпадают с записанными
uint32_t TestData(uint32_t address, uint32_t inv)
{
	uint32_t *ptr, tmp, data, res;
	
	ptr=(uint32_t*)address;

	MDR_PORTE->CLRTX=0x40;
	res=1;
		
	if(!inv)
	{
		for(tmp=0;tmp<0x40000;tmp++)
		{
			data=*ptr++;
			if(data!=tmp)
			{
				res=0;
				break;
			}
		}
	}
	else
	{
		for(tmp=0;tmp<0x40000;tmp++)
		{
			data=*ptr++;
			if(data!=(~tmp))
			{
				res=0;
				break;
			}
		}
	}

	MDR_PORTE->SETTX=0x40;
	return res;
}

void WriteWordToRAM(uint32_t address, uint32_t value)
{
	uint32_t*	ptr;
	
	ptr=(uint32_t*)(START_ADDR + address);

	MDR_PORTE->CLRTX=0x40;
	*ptr++ =value;
	MDR_PORTE->SETTX=0x40;
}

uint32_t ReadWordFromRAM(uint32_t address)
{
	uint32_t *ptr, data;
	
	ptr=(uint32_t*)(START_ADDR + address);
	MDR_PORTE->CLRTX=0x40;	
	data=*ptr;
	MDR_PORTE->SETTX=0x40;
	
	return data;
}

void enable_ram(void)
{
	MDR_PORTE->CLRTX=0x40;
}

void disable_ram(void)
{
	MDR_PORTE->SETTX=0x40;
}

void write_ext_ram(uint32_t dest_ram_addr, uint8_t* source_addr, uint32_t size)
{
	uint32_t *ptr = (uint32_t*)(START_ADDR + (dest_ram_addr<<2));	
	for (uint32_t i = 0; i < size; i++)
	{
		*ptr++ = *source_addr++;
	}
}

#endif	//__RAM_FUNCTIONS_C

