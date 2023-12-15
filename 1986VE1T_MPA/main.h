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
Функция для запроса данных
uart_n *uart_struct - выбор UART
возвращает сообщение с результатом 0 - пакет данных получен, обработан и ответный пакет отправлен; 1- нет принятого пакета, ошибка
*/
uint8_t request_data(uart_n *uart_struct);

/*
Функция для выполнения периферийной задачи МПА
*/
void do_mpa_task(adc_n *adc_struct);
/*
Функция синхронизации каналов АЦП (выполняется при срабатывании прерывания Timer2 по переполнению счетчика CNT)
*/
void sync_adc_chanels(void *data);
/*
Функция приема пакета с результатами измерений одного канала (выполняется при срабатывании прерывания Timer2 по захвату)
*/
void receive_adc_chanel_pack(void *data);

#endif /*__MAIN_H */
