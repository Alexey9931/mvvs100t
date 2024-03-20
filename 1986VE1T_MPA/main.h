/*!
 \file
 \brief Заголовочный файл с реализацией основного функционала МПА
*/

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

/*!
 *	\brief Запрашивает данные на выбранной шине
 *	\param *uart_struct - Выбранный UART МК
 *	\return Код возврата: 0 - пакет данных получен, обработан и ответный пакет отправлен; 1- нет принятого пакета, ошибка
*/
uint8_t request_data(uart_n *uart_struct);

/*!
 *	\brief Выполняет периферийные задачи МПА
 *	\param *adc_struct - Структура с конфигурационными параметрами микросхемы АЦП
*/
void do_mpa_task(adc_n *adc_struct);

/*!
 *	\brief Синхронизирует каналы АЦП (выполняется при срабатывании прерывания Timer2 по переполнению счетчика CNT)
 *	\param *data - Указатель на передаваемые при необходимости данные (не используется)
*/
void sync_adc_chanels(void *data);

/*!
 *	\brief Принимает пакет с результатами измерений одного канала (выполняется при срабатывании прерывания Timer2 по захвату)
 *	\param *data - Указатель на передаваемые при необходимости данные (не используется)
*/
void receive_adc_chanel_pack(void *data);

#endif /*__MAIN_H */
