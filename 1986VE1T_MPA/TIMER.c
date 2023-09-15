#include "TIMER.h"


//Структуры для 1 таймера
TIMER_CntInitTypeDef TIMER1InitStruct;
TIMER_ChnInitTypeDef sTIM1_ChnInit;
void TIMER1_init(void)//период 1сек
{
	 RST_CLK_PCLKcmd (RST_CLK_PCLK_TIMER1, ENABLE);
	 /* Initializes the TIMERx Counter */
	 TIMER_CntStructInit(&TIMER1InitStruct);
	 TIMER1InitStruct.TIMER_Period = 0xC34F;//50000-1
	 TIMER1InitStruct.TIMER_Prescaler = WORK_FREQ*20 - 1;//2560-1
	 
	 TIMER_CntInit(MDR_TIMER1, &TIMER1InitStruct);
	
   /* Enable TIMER1 clock */
   TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv1);

	 /* Enable TIMER1 */
   TIMER_Cmd(MDR_TIMER1,ENABLE);
}
//Структуры для 2 таймера
TIMER_CntInitTypeDef TIMER2InitStruct;
TIMER_ChnInitTypeDef sTIM2_ChnInit;
void TIMER2_init(void)//период 10мс
{
	 RST_CLK_PCLKcmd (RST_CLK_PCLK_TIMER2, ENABLE);
	 /* Initializes the TIMERx Counter */
	 TIMER_CntStructInit(&TIMER2InitStruct);
	 TIMER2InitStruct.TIMER_Period = 0x270F;//10000-1
	 TIMER2InitStruct.TIMER_Prescaler = WORK_FREQ - 1;//144-1
	 
	 TIMER_CntInit(MDR_TIMER2, &TIMER2InitStruct);
	
   /* Enable TIMER2 clock */
   TIMER_BRGInit(MDR_TIMER2,TIMER_HCLKdiv1);

	 /* Enable TIMER2 */
   TIMER_Cmd(MDR_TIMER2,ENABLE);
}

void Delay_milli(uint32_t time_milli)//задержка максимум 1сек (time_milli=999)
{ 
	MDR_TIMER1->CNT = 0;
	while(TIMER_GetCounter(MDR_TIMER1)!=(time_milli*50));
}
void Delay_micro(uint32_t time_micro)//задержка в мкс
{ 
	MDR_TIMER2->CNT = 0;
	while(TIMER_GetCounter(MDR_TIMER2)!=time_micro);
}