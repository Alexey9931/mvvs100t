#ifndef MT_12864_H
#define MT_12864_H

#include "MDR32F1QI.h"                  // Milandr::Device:Startup
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK
#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT 
#include "SystemFont5x7R.h"

#define LCD_E 	PORT_Pin_2
#define LCD_A0 	PORT_Pin_12
#define LCD_RW 	PORT_Pin_0
#define LCD_E2 	PORT_Pin_14
#define LCD_E1 	PORT_Pin_13

#define LCD_D0 	PORT_Pin_0
#define LCD_D1 	PORT_Pin_1
#define LCD_D2 	PORT_Pin_2
#define LCD_D3 	PORT_Pin_3
#define LCD_D4 	PORT_Pin_4
#define LCD_D5 	PORT_Pin_5
#define LCD_D6 	PORT_Pin_6
#define LCD_D7 	PORT_Pin_7

#define LCD_PORT_Data MDR_PORTA
#define LCD_PORT_RW_E_RES MDR_PORTC
#define LCD_PORT_A0_E1_E2 MDR_PORTE

void LCDInit(void);	
void LCDReInit(void);

void SetBusData(char byte);
char ReadBusData(void);

void WriteCodeL(char b);
void WriteCodeR(char b);
void WriteDataL(char b);
void WriteDataR(char b);
char ReadDataL(void);
char ReadDataR(void);
void WriteByte(char b, uint8_t cd, uint8_t l, uint8_t r);
char ReadByte(uint8_t cd, uint8_t l, uint8_t r);
void WaitReady(uint8_t l, uint8_t r);

void PrintLogo(void);
void LCD_Clear(void);
void LCD_ClearLine(uint8_t user_page);
void LCD_EndLine(void);
void LCD_PrintMessageNew(wchar_t *message, uint32_t length);
void LCD_PrintMessageContinue(wchar_t *message, uint32_t length);
void LCD_PrintMessageInPosition(wchar_t *message, uint32_t length, uint8_t user_page, uint8_t user_numChar);

#endif /*MT_12864_H*/
