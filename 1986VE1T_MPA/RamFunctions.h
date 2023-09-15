#ifndef __RAM_FUNCTIONS_H
#define __RAM_FUNCTIONS_H

#include "rs422_protocol.h"

#define START_ADDR 0x50000000

///Структура с данными гарантированно расположенными последовательно во внешней ОЗУ
typedef struct ram_data_struct
{
	fields_packet rx_packet_struct;
	fields_packet tx_packet_struct;
	fields_cmd tx_cmd_packet[NUMBER_CMDS_IN_PACKET];
	fields_cmd rx_cmd_packet[NUMBER_CMDS_IN_PACKET];
}__attribute__((packed)) ram_data;


void WriteDataToRAM(uint32_t address, uint32_t inv);
uint32_t TestData(uint32_t address, uint32_t inv);
void WriteWordToRAM(uint32_t address, uint32_t value);
uint32_t ReadWordFromRAM(uint32_t address);
void enable_ram(void);
void disable_ram(void);
void write_ext_ram(uint32_t dest_ram_addr, uint8_t* source_addr, uint32_t size);

#endif /*__RAM_FUNCTIONS_H */