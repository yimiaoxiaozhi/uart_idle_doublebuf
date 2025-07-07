//
// Created by wangbin on 25-6-25.
//

#ifndef IAP_H
#define IAP_H
#include "at32f403a_407.h"
#include "flash.h"
#include "boot_uart.h"
#include "boot_uart.h"
#include "wk_system.h"
#include "wk_tmr.h"


/* app starting address */
#define APP_START_ADDR          0x08020000  //128k+128k

#define EXCEL_START_ADDR        0x08080000 //80kb  to 08094000-1

/* the previous sector of app starting address is iap upgrade flag */
#define IAP_UPGRADE_FLAG_ADDR    (APP_START_ADDR - 0x800)   //2kb 读取app更新的标志
#define EXCEL_UPGRADE_FLAG_ADDR  (APP_START_ADDR - 0x1000)  //读取表的更新标志

/* when app received cmd 0x5aa5 from pc-tool, will set up the flag,
indicates that an app upgrade will follow, see iap application note for more details */
#define IAP_UPGRADE_FLAG         0x12345678   //app的标志位
#define EXCEL_UPGRADE_FLAG       0x11223344  //表的标志位


typedef void (*iapfun)(void);


void app_load(uint32_t appxaddr);




#endif //IAP_H

