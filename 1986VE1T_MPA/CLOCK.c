#include "CLOCK.h"


void CLOCK_Init(void) // ������������ 144 ���
{
	//��������� ������������ �� �������� ������ 16 ���
	// �������� ��������� �� ������� ������
	RST_CLK_HSEconfig (RST_CLK_HSE_ON);
	while (RST_CLK_HSEstatus () != SUCCESS);
	
	// ����������� �������� � ����������� ��������� PLL
	//(CPU_C1_SEL = HSE / 1 * 9 = 144 ��� )
	RST_CLK_CPU_PLLconfig ( RST_CLK_CPU_PLLsrcHSEdiv1,RST_CLK_CPU_PLLmul9);
	
	// �������� PLL, �� ��� �� ���������� � ��������� (PLL �������� ������� ������������)
	RST_CLK_CPU_PLLcmd (ENABLE);
	while (RST_CLK_CPU_PLLstatus () != SUCCESS);
	
	// �������� �3 ( CPU_C3_SEL = CPU_C2_SEL )
	RST_CLK_CPUclkPrescaler (RST_CLK_CPUclkDIV1);
	
	// �� �2 ���� � PLL, � �� �������� � �1 (CPU_C2_SEL = PLL)
	RST_CLK_CPU_PLLuse (ENABLE);
	// CPU ����� ������������ � ������ �3
	//(HCLK_SEL = CPU_C3_SEL = 144 ���)
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
	
}