#ifndef __MDR32_Drivers_H
#define __MDR32_Drivers_H

#define K1986VE1T
//#define K1986VE3T

#ifdef K1986VE1T
	#include <MDR32F1QI.h>
	#include <MDR32FxQI_rst_clk.h>
	#include <MDR32FxQI_uart.h>
	#include <MDR32FxQI_ssp.h>
	#include <MDR32FxQI_port.h>
	#include <MDR32FxQI_dma.h>
	#include <MDR32FxQI_timer.h>
	#include <MDR32FxQI_ebc.h>  
	#include <MDR32FxQI_bkp.h> 
#endif

#ifdef K1986VE3T
	#include <MDR1986VE3.h>
	#include <MDR32F9Qx_rst_clk.h>
	#include <MDR32F9Qx_uart.h>
	#include <MDR32F9Qx_ssp.h>
	#include <MDR32F9Qx_port.h>
	#include <MDR32F9Qx_dma.h>
	#include <MDR32F9Qx_timer.h>
	#include <MDR32F9Qx_ebc.h>   
	#include <MDR32F9Qx_bkp.h>  	
#endif

#define HSE_OSC ((uint32_t)12000000) //������� �������� ��������� ���������� (!����� �������� � MDR32_config.h)
#define WORK_FREQ 144 //������� ������� � ���

#define CHANEL_NUMBER 6									///< ���-�� ������� � ���
#define MAX_CHANEL_NUMBER 8							///< ������������ ���-�� ������� � ���
#define PM_DEV_ADDR 0										///< ����� ������
#define PM_CHASSIS_ADDR 0								///< ����� �����
#define NUMBER_OF_RANGES 3							///< ���-�� ���������� � ��������� ���������


#endif /*__MDR32_Drivers_H */