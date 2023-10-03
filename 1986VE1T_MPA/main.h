#ifndef __MAIN_H
#define __MAIN_H

#include "1273pv19t.h"
#include "TIMER.h"
#include "SPI.h"
#include "UART.h"
#include "rs422_protocol.h"
#include "CLOCK.h"
#include "DMA.h"
#include "EBC.h"
#include "external_ram.h"

/*
������� ��� ������� ������
UARTn *UART_struct - ����� UART
���������� ��������� � ����������� 0 - ����� ������ �������, ��������� � �������� ����� ���������; 1- ��� ��������� ������, ������
*/
uint8_t request_data(UARTn *UART_struct);

#endif /*__MAIN_H */