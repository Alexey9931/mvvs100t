#ifndef __MAIN_H
#define __MAIN_H

#include "1273pv19t.h"
#include "timers.h"
#include "spi.h"
#include "uart.h"
#include "rs422_protocol.h"
#include "clock.h"
#include "dma.h"
#include "ebc.h"
#include "internal_ram.h"
#include "external_ram.h"
#include "external_rom.h"
#include "leds.h"
#include "list.h"

/*
������� ��� ������� ������
uart_n *uart_struct - ����� UART
���������� ��������� � ����������� 0 - ����� ������ �������, ��������� � �������� ����� ���������; 1- ��� ��������� ������, ������
*/
uint8_t request_data(uart_n *uart_struct);

/*
������� ��� ���������� ������������ ������ ���
*/
void do_mpa_task(adc_n *adc_struct);
/*
������� ������������� ������� ��� (����������� ��� ������������ ���������� Timer2 �� ������������ �������� CNT)
*/
void sync_adc_chanels(void *data);
/*
������� ������ ������ � ������������ ��������� ������ ������ (����������� ��� ������������ ���������� Timer2 �� �������)
*/
void receive_adc_chanel_pack(void *data);

#endif /*__MAIN_H */
