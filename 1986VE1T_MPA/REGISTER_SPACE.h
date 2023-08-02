#ifndef __REGISTER_SPACE_H
#define __REGISTER_SPACE_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#define COMMON 0
#define MPA 1

//Адресное пространство памяти регистров
static uint8_t Common_Registers[1258];//Общие регистры
static uint8_t MPA_Registers[1758];//Регистры для МПА

//функции для записи/чтения регистров по адресу
void write_register(uint8_t data[],uint16_t address, uint16_t data_size, uint8_t register_type);
uint8_t *read_register(uint16_t address, uint16_t data_size, uint8_t register_type);

//функция для первичной инициализации всех регистров
void registers_ini();

#endif /*__REGISTER_SPACE_H */