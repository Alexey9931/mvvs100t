#ifndef __TIMER_H
#define __TIMER_H

#include "MDR32_Drivers.h"

#include "UART.h"
#include "ADC.h"
#include <stdio.h>

void TIMER1_init(void);
void TIMER2_init(void);
void Delay_milli(uint32_t time_milli);
void Delay_micro(uint32_t time_micro);


#endif /*__TIMER_H */