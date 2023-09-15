#include "CLOCK.h"


void CLOCK_Init(void) // тактирование WORK_FREQ МГц
{
	//настройка тактирования от внешнего кварца HSE_OSC МГц
	// Включаем генератор на внешнем кварце
	RST_CLK_HSEconfig (RST_CLK_HSE_ON);
	while (RST_CLK_HSEstatus () != SUCCESS);
	
	// Настраиваем источник и коэффициент умножения PLL
	//(CPU_C1_SEL = HSE / 1 * 9 = 128 МГц )
	RST_CLK_CPU_PLLconfig ( RST_CLK_CPU_PLLsrcHSEdiv1,17);
	
	// Включаем PLL, но еще не подключаем к кристаллу (PLL умножает частоту тактирования)
	RST_CLK_CPU_PLLcmd (ENABLE);
	while (RST_CLK_CPU_PLLstatus () != SUCCESS);
	
	// Делитель С3 ( CPU_C3_SEL = CPU_C2_SEL )
	RST_CLK_CPUclkPrescaler (RST_CLK_CPUclkDIV1);
	
	// На С2 идет с PLL, а не напрямую с С1 (CPU_C2_SEL = PLL)
	RST_CLK_CPU_PLLuse (ENABLE);
	// CPU берет тактирование с выхода С3
	//(HCLK_SEL = CPU_C3_SEL = 128 МГц)
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
	
	 
	/* Enables the RST_CLK_PCLK_BKP */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP, ENABLE);
  /* Setting the parameters of the voltage regulator SelectRI and LOW in the BKP controller (CPU_CLK = 128 MHz) */
  BKP_DUccMode(BKP_DUcc_over_80MHz);
	
	SystemCoreClockUpdate();
}