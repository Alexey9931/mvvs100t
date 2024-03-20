/*!
 \file
 \brief Файл с реализацией API для настройки тактирования МК
*/

#include "clock.h"

/*
	Функция настройки тактирования МК
*/
void clock_init(void)
{
	// Сброс настроек системы тактирования
  RST_CLK_DeInit();
	
	// Настройка тактирования от внешнего кварца HSE_OSC МГц
	// Включаем генератор на внешнем кварце
	RST_CLK_HSEconfig (RST_CLK_HSE_ON);
	while (RST_CLK_HSEstatus () != SUCCESS);
	
	// Настраиваем источник и коэффициент умножения PLL
	//(CPU_C1_SEL = HSE / 1 * 12 = 144 МГц )
	RST_CLK_CPU_PLLconfig ( RST_CLK_CPU_PLLsrcHSEdiv1,11);
	
	// Включаем PLL, но еще не подключаем к кристаллу (PLL умножает частоту тактирования)
	RST_CLK_CPU_PLLcmd (ENABLE);
	while (RST_CLK_CPU_PLLstatus () != SUCCESS);
	
	// Делитель С3 ( CPU_C3_SEL = CPU_C2_SEL )
	RST_CLK_CPUclkPrescaler (RST_CLK_CPUclkDIV1);
	
	// На С2 идет с PLL, а не напрямую с С1 (CPU_C2_SEL = PLL)
	RST_CLK_CPU_PLLuse (ENABLE);
	// CPU берет тактирование с выхода С3
	//( HCLK_SEL = CPU_C3_SEL = 128 МГц)
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
	
	SystemCoreClockUpdate();
}
