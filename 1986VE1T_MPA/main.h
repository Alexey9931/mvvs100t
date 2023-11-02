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
#include "external_ram.h"
#include "external_rom.h"
#include "leds.h"

/*
Функция для запроса данных
uart_n *uart_struct - выбор UART
возвращает сообщение с результатом 0 - пакет данных получен, обработан и ответный пакет отправлен; 1- нет принятого пакета, ошибка
*/
uint8_t request_data(uart_n *uart_struct);

/*
Функция для выполнения периферийной задачи МПА
*/
void do_mpa_task(adc_n *adc_struct);

#endif /*__MAIN_H */
