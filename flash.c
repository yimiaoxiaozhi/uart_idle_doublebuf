//
// Created by wangbin on 25-6-25.
//

#include "flash.h"
#include "boot_uart.h"
#include "iap.h"

/**
 *  Bootloader 	  128kb    0x0800 0000 to 0x0801 ffff
 *  App		 	  384kb	   0x0802 0000 to 0x0807 ffff
 *  表80kb  	  128kb	   0x0808 0000 to 0x0809 ffff
 *  device_data   128kb    0x080A 0000 to 0x080d ffff
 *  剩余		      256kb    0x080C 0000 to 0x080f ffff
*/


/**
  * @brief  flash erase/program operation.
  * @note   follow 2kb operation of ont time
  * @param  none
  * @retval none
  */
void flash_2kb_write(uint32_t write_addr, uint8_t *pbuffer)
{
  uint16_t index, write_data;
  flash_unlock();
  flash_sector_erase(write_addr);
  if(FLASH_SIZE < 0x100)  /* less than 256kb, 1kb/sector */
    flash_sector_erase(write_addr + 0x400);
  for(index = 0; index < 2048; index += 2)
  {
    write_data = (pbuffer[index+1] << 8) + pbuffer[index];
    flash_halfword_program(write_addr, write_data);
    write_addr += sizeof(uint16_t);
  }
  flash_lock();
}

/**
  * @brief  flash erase/program operation.
  * @note   follow 2kb operation of ont time
  * @param  none
  * @retval none
  */
flash_status_type flash_4byte_write(uint32_t write_addr, unsigned int data)
{
    flash_status_type status = FLASH_OPERATE_DONE;

    flash_unlock();

    flash_sector_erase(write_addr);

    status = flash_word_program(write_addr, data);

    flash_lock();

    return status;
}

/**
  * @brief  check flash upgrade flag.
  * @param  none
  * @retval none
  */
flag_status flash_upgrade_flag_read(void)
{
  if((*(uint32_t*)IAP_UPGRADE_FLAG_ADDR) == IAP_UPGRADE_FLAG)
    return SET;
  else
    return RESET;
}

/**
  * @brief  check excel upgrade flag.
  * @param  none
  * @retval none
  */
flag_status excel_upgrade_flag_read(void)
{
    if((*(uint32_t*)EXCEL_UPGRADE_FLAG_ADDR) == EXCEL_UPGRADE_FLAG)
        return SET;
    else
        return RESET;
}



/**
  * @brief  从Flash读取指定长度的数据到缓冲区
  * @param  address: 起始地址（必须是合法Flash地址）
  * @param  buffer: 目标缓冲区指针
  * @param  length: 要读取的字节数
  * @retval 实际读取的字节数（若地址越界返回0）
  */
uint32_t flash_read_data(uint32_t address, unsigned char* buffer, uint32_t length)
{
  /* 参数检查 */
  if (buffer == NULL || length == 0) {
    return 0;
  }

  /* 检查地址范围（根据芯片型号调整） */
#if defined(AT32F403A) || defined(AT32F407)
  if (address < FLASH_BANK1_START_ADDR ||
      address + length > FLASH_BANK2_END_ADDR + 1) {
    return 0;  // 地址越界
      }
#endif

  /* 直接内存拷贝（Flash区域可直接读取） */
  memcpy(buffer, (const void*)address, length);

  return length;
}
/**
  * @brief  Write multiple bytes to flash memory
  * @param  address: starting address to write (must be aligned according to memory type)
  * @param  data: pointer to the data to be written
  * @param  length: number of bytes to write
  * @retval flash_status_type: status of the operation
  */
flash_status_type flash_multi_byte_write(uint32_t address, const uint8_t *data, uint32_t length)
{
    flash_status_type status = FLASH_OPERATE_DONE;
    uint32_t i;
    uint32_t word_data;
    uint16_t halfword_data;

    /* Check for valid parameters */
    if (data == NULL || length == 0) {
        return FLASH_OPERATE_DONE;
    }

    /* Determine which memory region we're writing to */
    if ((address >= FLASH_BANK1_START_ADDR) && (address <= FLASH_BANK1_END_ADDR)) {
        /* Bank1 internal flash */
        flash_bank1_unlock();

        for (i = 0; i < length; ) {
            /* Try to write words when possible */
            if ((address % 4 == 0) && (length - i >= 4)) {
                /* Word write */
                word_data = *(uint32_t *)(data + i);
                status = flash_word_program(address + i, word_data);
                if (status != FLASH_OPERATE_DONE) break;
                i += 4;
            }
            /* Try to write halfwords when possible */
            else if ((address % 2 == 0) && (length - i >= 2)) {
                /* Halfword write */
                halfword_data = *(uint16_t *)(data + i);
                status = flash_halfword_program(address + i, halfword_data);
                if (status != FLASH_OPERATE_DONE) break;
                i += 2;
            }
            else {
                /* Byte write (only for internal flash) */
                status = flash_byte_program(address + i, data[i]);
                if (status != FLASH_OPERATE_DONE) break;
                i += 1;
            }
        }

        flash_bank1_lock();
    }
    else if ((address >= FLASH_BANK2_START_ADDR) && (address <= FLASH_BANK2_END_ADDR)) {
        /* Bank2 internal flash */
        flash_bank2_unlock();

        for (i = 0; i < length; ) {
            /* Try to write words when possible */
            if ((address % 4 == 0) && (length - i >= 4)) {
                /* Word write */
                word_data = *(uint32_t *)(data + i);
                status = flash_word_program(address + i, word_data);
                if (status != FLASH_OPERATE_DONE) break;
                i += 4;
            }
            /* Try to write halfwords when possible */
            else if ((address % 2 == 0) && (length - i >= 2)) {
                /* Halfword write */
                halfword_data = *(uint16_t *)(data + i);
                status = flash_halfword_program(address + i, halfword_data);
                if (status != FLASH_OPERATE_DONE) break;
                i += 2;
            }
            else {
                /* Byte write (only for internal flash) */
                status = flash_byte_program(address + i, data[i]);
                if (status != FLASH_OPERATE_DONE) break;
                i += 1;
            }
        }

        flash_bank2_lock();
    }
    else if (address >= FLASH_SPIM_START_ADDR) {
        /* External SPIM flash */
        flash_spim_unlock();

        /* SPIM only supports word and halfword writes */
        for (i = 0; i < length; ) {
            /* Try to write words when possible */
            if ((address % 4 == 0) && (length - i >= 4)) {
                /* Word write */
                word_data = *(uint32_t *)(data + i);
                status = flash_word_program(address + i, word_data);
                if (status != FLASH_OPERATE_DONE) break;
                i += 4;
            }
            /* Try to write halfwords when possible */
            else if ((address % 2 == 0) && (length - i >= 2)) {
                /* Halfword write */
                halfword_data = *(uint16_t *)(data + i);
                status = flash_halfword_program(address + i, halfword_data);
                if (status != FLASH_OPERATE_DONE) break;
                i += 2;
            }
            else {
                /* SPIM doesn't support byte writes, so we can't complete the operation */
                status = FLASH_PROGRAM_ERROR;
                break;
            }
        }

        flash_spim_lock();
    }
    else {
        /* Invalid address range */
        status = FLASH_PROGRAM_ERROR;
    }

    return status;
}

/**
  * @brief  Efficiently write data to flash (supports large files up to 100KB+)
  * @param  write_addr: Target flash address (must be 2-byte aligned)
  * @param  pbuffer: Pointer to data buffer
  * @param  length: Data length in bytes
  * @retval Flash operation status
  */
flash_status_type flash_efficient_write(uint32_t write_addr, uint8_t *pbuffer, uint32_t length)
{
    flash_status_type status = FLASH_OPERATE_DONE;

    // 1. 参数检查
    if ((write_addr & 0x1) != 0) {
        return FLASH_PROGRAM_ERROR; // 地址必须2字节对齐
    }

    if ((write_addr < FLASH_BANK1_START_ADDR) ||
        (write_addr + length > FLASH_BANK2_END_ADDR)) {
        return FLASH_PROGRAM_ERROR; // 地址超出范围
    }

    // 2. 解锁Flash
    flash_unlock();

    // 3. 擦除所有涉及的扇区 (2KB/扇区)
    uint32_t sector_addr = write_addr & ~(FLASH_SECTOR_SIZE - 1);
    uint32_t end_addr = write_addr + length;

    while (sector_addr < end_addr) {
        status = flash_sector_erase(sector_addr);
        if (status != FLASH_OPERATE_DONE) {
            flash_lock();
            return status;
        }
        sector_addr += FLASH_SECTOR_SIZE;

        // 添加延时确保擦除完成 (根据实际需要调整)
        wk_delay_ms(5);
    }

    // 4. 写入数据 (半字为单位)
    for (uint32_t i = 0; i < length; i += 2) {
        uint16_t write_data;

        // 处理奇数长度情况
        if ((i + 1) < length) {
            write_data = (pbuffer[i+1] << 8) | pbuffer[i];
        } else {
            write_data = pbuffer[i]; // 单字节写入
        }

        status = flash_halfword_program(write_addr, write_data);
        if (status != FLASH_OPERATE_DONE) {
            flash_lock();
            return status;
        }

        write_addr += 2; // 移动到下一个半字地址

        // 每写入1KB添加短暂延时 (根据实际需要调整)
        if ((i % 1024) == 0) {
            wk_delay_ms(1);
        }
    }

    // 5. 重新锁定Flash
    flash_lock();

    // 6. 可选：写入后校验
    #ifdef FLASH_WRITE_VERIFY
    for (uint32_t i = 0; i < length; i++) {
        if (*(uint8_t*)(write_addr + i) != pbuffer[i]) {
            return FLASH_PROGRAM_ERROR;
        }
    }
    #endif

    return FLASH_OPERATE_DONE;
}
/**
  * @brief  Write multiple bytes to flash efficiently
  * @note   This function will erase the affected sectors first, then write data
  * @param  write_addr: Starting address to write
  * @param  pbuffer: Pointer to data buffer
  * @param  length: Number of bytes to write
  * @retval flash_status_type: Status of the operation
  */
flash_status_type flash_efficient_write_v1(uint32_t write_addr, uint8_t *pbuffer, uint32_t length)
{
    flash_status_type status = FLASH_OPERATE_DONE;
    uint16_t write_data;
    uint32_t i;
    uint32_t sector_addr;

    // Check if address is in valid range
    if((write_addr < FLASH_BANK1_START_ADDR) || (write_addr > FLASH_BANK2_END_ADDR)) {
        return FLASH_PROGRAM_ERROR;
    }

    // Unlock the flash
    flash_unlock();

    // Calculate sector address (align down to sector boundary)
    sector_addr = write_addr & ~(FLASH_SECTOR_SIZE - 1);

    // Erase the sector(s) that will be written to
    status = flash_sector_erase(sector_addr);
    if(status != FLASH_OPERATE_DONE) {
        flash_lock();
        return status;
    }

    // If data crosses sector boundary, erase next sector too
    if((write_addr + length) > (sector_addr + FLASH_SECTOR_SIZE)) {
        status = flash_sector_erase(sector_addr + FLASH_SECTOR_SIZE);
        if(status != FLASH_OPERATE_DONE) {
            flash_lock();
            return status;
        }
    }

    // Write data in halfword units
    for(i = 0; i < length; i += 2) {
        // Handle odd length case
        if((i + 1) < length) {
            write_data = (pbuffer[i+1] << 8) | pbuffer[i];
        } else {
            write_data = pbuffer[i];  // Only one byte left
        }

        // Program halfword
        status = flash_halfword_program(write_addr, write_data);
        if(status != FLASH_OPERATE_DONE) {
            break;
        }

        write_addr += 2;  // Move to next halfword address
    }

    // Lock the flash
    flash_lock();

    return status;
}

/**
  * @brief  精准Flash写入函数（支持任意位置起始写入）
  * @param  write_addr : 写入起始地址（需2字节对齐）
  * @param  pbuffer    : 数据指针
  * @param  length     : 数据长度（字节）
  * @retval 操作状态
  */
flash_status_type flash_precise_write(uint32_t write_addr, uint8_t *pbuffer, uint32_t length)
{
    // 1. 基础检查
    if(write_addr & 0x1)
        return FLASH_PROGRAM_ERROR; // 必须2字节对齐

    if((write_addr < FLASH_BANK1_START_ADDR) ||
       (write_addr + length > FLASH_BANK2_END_ADDR))
        return FLASH_PROGRAM_ERROR;

    // 2. 计算涉及的扇区
    uint32_t first_sector = write_addr / FLASH_SECTOR_SIZE;
    uint32_t last_sector = (write_addr + length - 1) / FLASH_SECTOR_SIZE;

    // 3. 擦除必要扇区（仅擦除需要修改的扇区）
    for(uint32_t sector = first_sector; sector <= last_sector; sector++) {
        uint32_t sector_addr = sector * FLASH_SECTOR_SIZE;

        // 3.1 检查该扇区是否需要擦除（是否已有数据需要修改）
        uint8_t need_erase = 0;
        for(uint32_t i = 0; i < FLASH_SECTOR_SIZE; i++) {
            uint32_t addr = sector_addr + i;
            if(addr >= write_addr && addr < write_addr + length) {
                uint8_t flash_val = *(uint8_t*)addr;
                uint8_t new_val = pbuffer[addr - write_addr];
                if((flash_val & new_val) != new_val) { // 需要从1->0的位
                    need_erase = 1;
                    break;
                }
            }
        }

        // 3.2 执行擦除
        if(need_erase) {
            flash_status_type status = flash_sector_erase(sector_addr);
            if(status != FLASH_OPERATE_DONE)
                return status;
        }
    }

    // 4. 按字节写入（实际以半字操作）
    for(uint32_t i = 0; i < length; i += 2) {
        uint16_t write_data;

        // 处理奇数长度
        if(i + 1 < length) {
            write_data = (pbuffer[i+1] << 8) | pbuffer[i];
        } else {
            write_data = pbuffer[i]; // 单字节写入（高位补0xFF）
        }

        flash_status_type status = flash_halfword_program(write_addr + i, write_data);
        if(status != FLASH_OPERATE_DONE)
            return status;
    }

    return FLASH_OPERATE_DONE;
}
