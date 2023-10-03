#ifndef __PORTS_H
#define __PORTS_H

#include "MDR32_Drivers.h"

//����������� ����� ��� ��������� �� ����� ���������� RS-485
#define PORT_UART1_EN MDR_PORTD
#define PORT_UART2_EN MDR_PORTD
#define PIN_UART1_EN PORT_Pin_10
#define PIN_UART2_EN PORT_Pin_12
//����������� ����� ��� ����������� ������ ���������� ���
#define PORT_ADC_RST MDR_PORTD
#define PIN_ADC_RST PORT_Pin_9
//����� �� ���� ���������� SDIFS/SDOFS �� ��� (����� ������� ��� �������)
#define PORT_ADC_SDIFS_IRQ MDR_PORTE
#define PIN_ADC_SDIFS_IRQ PORT_Pin_8

void ports_init(void);

#endif /*__PORTS_H */