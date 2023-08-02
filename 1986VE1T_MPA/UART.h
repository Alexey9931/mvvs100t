#ifndef __UART_H
#define __UART_H

#include "MDR32_Drivers.h"
#include "string.h"
#include "math.h"

#define BUFFER_SIZE 64
#define BUFFER_MASK (BUFFER_SIZE-1)
/*
��������� � ����������������� ����������� UART � ������� ������:
	������������ ��������� � ������������� ���������� UART
	����� ������ DMA ��� UARTn
	����� ����������� ���������� UARTn
	��������� ������������ ��� UARTn
	������������ ��������� � ����������������� ����������� UART
	�������� �������� ������� ��� UART
	
	����� � ��������� �������
	������� ��������� ������
	������� ������� ������� ������ � ������
*/
typedef struct UART_ConfigData
{
	MDR_UART_TypeDef* UARTx;
	uint8_t DMA_Channel;
	IRQn_Type IRQn;
	uint32_t RST_CLK_PCLK_UARTn;
	UART_InitTypeDef UART;
	uint32_t UART_HCLKdiv;
	
	uint8_t buffer[BUFFER_SIZE];
	uint32_t buffer_count;
	uint32_t read_pos;
} UARTn ;

/*
������� ������������� UART
UARTn *UART_struct - ����� UART
���������� ��������� � ����������� 0 - �����, 1- ������
*/
uint8_t uart_init(UARTn *UART_struct);
/*
������� �������� ������ �� UART
UARTn *UART_struct - ����� UART
uint8_t data[] - ������ ������������ ������
uint32_t data_size - ������ ������������� ������� ������
���������� ��������� � ����������� 0 - �����, 1- ������
*/
uint8_t uart_write(UARTn *UART_struct, uint8_t data[], uint32_t data_size);
/*
������� ������ ������ �� ������ UART
UARTn *UART_struct - ����� UART
uint32_t len - ���-�� ���� ��� ������
uint8_t *data - ��������� �� ������, ���� ����������� ����� �� ������ UART
���������� ��������� � ����������� 0 - �����, 1- ������
*/
uint8_t uart_read(UARTn *UART_struct, uint32_t len, uint8_t *data);
/*
������� ������� ������ UART
UARTn *UART_struct - ����� UART
*/
void uart_clean(UARTn *UART_struct);
/*
������� ��������� ������� ������ � ������
UARTn *UART_struct - ����� UART
uint32_t pos - ������� ������� � ������
���������� ��������� � ����������� 0 - �����, 1- ������
*/
uint8_t uart_set_pos(UARTn *UART_struct, uint32_t pos);
/*
������� ������ ������� ������� ������� ������
UARTn *UART_struct - ����� UART
���������� ������� �������
*/
uint32_t uart_read_pos(UARTn *UART_struct);
/*
������� ������ ���-�� ���� � ������ UART
UARTn *UART_struct - ����� UART
���������� ���-�� ���� � ������ UART
*/
uint32_t uart_get_buf_counter(UARTn *UART_struct);
/*
������� ������������� n-�� ������ DMA  �� ������ �� ��������� UARTn
UARTn *UART_struct - ����� UART
*/
void DMA_UART_RX_init(UARTn *UART_struct);

#endif /*__UART_H */



