//
// Created by wangbin on 25-6-25.
//

#ifndef BOOT_UART_H
#define BOOT_UART_H

#include "at32f403a_407_usart.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "flash.h"
#include "iap.h"
#include "wk_system.h"

#include <stddef.h>
#include "stdarg.h"




#define USART_RX_MAX_LEN      1024*124
extern unsigned char  bootmode;//是否进入boot模式
extern unsigned char update_app_flag;//更显app的flag
extern unsigned int usart_rx_cnt;    //接收到的字节数
extern unsigned char write_state;
extern unsigned int oldcount;
extern unsigned int applenth;
extern unsigned int excelenth;

extern unsigned char update_excel_flag;


extern uint8_t app_rx_buf[USART_RX_MAX_LEN];
extern unsigned char  app_update_cmd[5];


void usart_send_string(usart_type* usart_x, const char* str);
void usart_transmit(usart_type* usart_x, unsigned char* data, uint32_t length);
void app_update(void);
void my_app_load(void);



#endif //BOOT_UART_H



















