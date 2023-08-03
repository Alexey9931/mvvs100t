#ifndef __MAIN_H
#define __MAIN_H

#include "ADC.h"
#include "TIMER.h"
#include "UART.h"
#include "PORTS.h"
#include "PROTOCOL.h"
#include "REGISTER_SPACE.h"
#include "CLOCK.h"
#include "DMA.h"

/*
������� ��� ������� ������
UARTn *UART_struct - ����� UART
���������� ��������� � ����������� 0 - ����� ������ �������, ��������� � �������� ����� ���������; 1- ��� ��������� ������, ������
*/
uint8_t request_data(UARTn *UART_struct);

#endif /*__MAIN_H */