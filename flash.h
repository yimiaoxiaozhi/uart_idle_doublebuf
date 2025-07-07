//
// Created by wangbin on 25-6-25.
//
/**
 * flash table
 * flash size 1024 kb 2个扇区 每个扇区2kb  2kb=0x0800  2048byte   1kb=0x400
 *   1024kb = 64kb*16  = 0x0010 0000
 *     64kb = 0x0001 0000
 *
 *     1024kb=512kb*2  =    【64kb】     +  【256kb】  +  【256kb】 +      【64kb】      +  【 128kb】
 *                       【bootloader】 +   【app1】  +  【app2】  + 【device_info】    +    【 暂定】
 *
* Bank1  256扇区 0x0800 0000 – 0x0807 ffff
* Bank2  256扇区 0x0808 0000 – 0x080f ffff
 * System
 * --------------------------------------------------------------------------------------
 * |  0x0800 0000 - 0x0800 FFFF |   0x0801 0000 - 0x0804 FFFF |  0x0804 0000 - 0x0807 FFFF    |
 * |-------------------------------------------------------------------------------------
 * |     Boot_Loader(64KB)    |     app (256kb)  |     app2(256KB)          |
 * --------------------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------------------------------------
 * | 0x0808 0000 - 0x0808 FFFF | 0x0809 0000 - 0x080A FFFF |                            |                         |
 * ---------------------------------------------------------------------------------------------------------
 * |       device_info        |                             |                           |                         |
 * ---------------------------------------------------------------------------------------------------------
 * |                           |
 * -----------------------------
 * |           Backup          |
 * -----------------------------*/
#ifndef FLASH_H
#define FLASH_H

#include "at32f403a_407.h"
#include <stddef.h>
#include <string.h>

#define FLASH_SIZE    (*((uint32_t*)0x1FFFF7E0))  /* read from at32 flash capacity register(unit:kbyte) */
#define SRAM_SIZE     224                         /* sram size, unit:kbyte */
#define FLASH_SECTOR_SIZE  2048  // 1KB 扇区（或 2048 如果是 2KB）

void flash_2kb_write(uint32_t write_addr, uint8_t *pbuffer);
flash_status_type flash_4byte_write(uint32_t write_addr, unsigned int data);
flag_status flash_upgrade_flag_read(void);
uint32_t flash_read_data(uint32_t address, unsigned char* buffer, uint32_t length) ;
flash_status_type flash_multi_byte_write(uint32_t address, const uint8_t *data, uint32_t length);
flash_status_type flash_efficient_write(uint32_t write_addr, uint8_t *pbuffer, uint32_t length);

flash_status_type flash_precise_write(uint32_t write_addr, uint8_t *pbuffer, uint32_t length) ;
flash_status_type flash_efficient_write_v1(uint32_t write_addr, uint8_t *pbuffer, uint32_t length);

flag_status excel_upgrade_flag_read(void);



#endif //FLASH_H

