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
Функция для запроса данных
UARTn *UART_struct - выбор UART
возвращает сообщение с результатом 0 - пакет данных получен, обработан и ответный пакет отправлен; 1- нет принятого пакета, ошибка
*/
uint8_t request_data(UARTn *UART_struct);

#endif /*__MAIN_H */