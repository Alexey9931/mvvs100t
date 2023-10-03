#ifndef __PORTS_H
#define __PORTS_H

#include "MDR32_Drivers.h"

//управляющие ножки для активации на прием микросхемы RS-485
#define PORT_UART1_EN MDR_PORTD
#define PORT_UART2_EN MDR_PORTD
#define PIN_UART1_EN PORT_Pin_10
#define PIN_UART2_EN PORT_Pin_12
//управляющая ножка для аппаратного сброса микросхемы АЦП
#define PORT_ADC_RST MDR_PORTD
#define PIN_ADC_RST PORT_Pin_9
//ножка на вход прерывания SDIFS/SDOFS от АЦП (канал захвата для таймера)
#define PORT_ADC_SDIFS_IRQ MDR_PORTE
#define PIN_ADC_SDIFS_IRQ PORT_Pin_8

void ports_init(void);

#endif /*__PORTS_H */