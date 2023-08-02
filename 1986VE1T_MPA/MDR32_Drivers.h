#ifndef __MDR32_Drivers_H
#define __MDR32_Drivers_H

#define K1986VE1T
//#define K1986VE3T

#ifdef K1986VE1T
	#include <MDR32F1QI.h>
	#include <MDR32FxQI_uart.h>
	#include <MDR32FxQI_port.h>
	#include <MDR32FxQI_rst_clk.h>
	#include <MDR32FxQI_dma.h>
	#include <MDR32FxQI_timer.h>
	#include <MDR32FxQI_adc.h>
#endif

#ifdef K1986VE3T
	#include <MDR1986VE3.h>
	#include <MDR32F9Qx_uart.h>
	#include <MDR32F9Qx_port.h>
	#include <MDR32F9Qx_rst_clk.h>
	#include <MDR32F9Qx_dma.h>
	#include <MDR32F9Qx_timer.h>
	#include <MDR32F9Qx_adc.h>
#endif


#endif /*__MDR32_Drivers_H */