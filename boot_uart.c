//
// Created by wangbin on 25-6-25.
//

#include "boot_uart.h"

unsigned char  bootmode  = 0; //是否进入boot模式  1：是         0：否



unsigned char update_app_flag = 0; // 更新app的标志
unsigned int usart_rx_cnt = 0;    //接收到的字节数
unsigned char write_state = 0;
unsigned int oldcount = 0;
unsigned int applenth = 0;
unsigned int excelenth = 0;

unsigned char update_excel_flag = 0;//更显ecxel的标志


/*在RAM中定义缓冲区(不占用Flash)*/
uint8_t app_rx_buf[USART_RX_MAX_LEN] __attribute__((section(".bss.ARM.__at_0x20000000")));
unsigned char  app_update_cmd[5];

/**
  * @brief  通过USART发送字符串（阻塞式）
  * @param  usart_x: 选择的USART/UART外设（如USART1, USART2等）
  * @param  str: 要发送的字符串（以'\0'结尾）
  * @retval 无
  */
void usart_send_string(usart_type* usart_x, const char* str)
{
    /* 参数检查 */
    if (usart_x == NULL || str == NULL) {
        return;
    }

    /* 逐个字符发送，直到遇到字符串结束符'\0' */
    while (*str != '\0')
    {
        /* 等待发送缓冲区为空 */
        while (usart_flag_get(usart_x, USART_TDBE_FLAG) == RESET);

        /* 发送当前字符 */
        usart_data_transmit(usart_x, (uint16_t)(*str));
        str++;
    }
    /* 可选：等待发送完成（确保最后一个字节已发出） */
    while (usart_flag_get(usart_x, USART_TDC_FLAG) == RESET);
}
void usart_transmit(usart_type* usart_x, unsigned char* data, uint32_t length)
{
    /* Check parameters */
    if(usart_x == NULL || data == NULL || length == 0)
    {
        return;
    }

    /* Wait until transmitter is ready */
    while(usart_flag_get(usart_x, USART_TDBE_FLAG) == RESET)
    {
        /* You may want to add a timeout here for robust operation */
    }

    /* Transmit data byte by byte */
    for(uint32_t i = 0; i < length; i++)
    {
        /* Wait until transmit data buffer is empty */
        while(usart_flag_get(usart_x, USART_TDBE_FLAG) == RESET)
        {
            /* You may want to add a timeout here for robust operation */
        }

        /* Send current byte */
        usart_data_transmit(usart_x, (uint16_t)data[i]);
    }

    /* Wait until transmission is complete */
    while(usart_flag_get(usart_x, USART_TDC_FLAG) == RESET)
    {
        /* You may want to add a timeout here for robust operation */
    }
}


void app_update(void)
{
    if (app_update_cmd[0]==0x12 && \
        app_update_cmd[1]==0x34 && \
        app_update_cmd[2]==0x56 && \
        app_update_cmd[3]==0x78 && \
        app_update_cmd[4]==0x9A)
    {
        update_app_flag = 1 ;//开始接收app
        usart_send_string(USART1,"start update app\r\n");wk_delay_ms(1000);
        usart_send_string(USART1,"please send app data\r\n");wk_delay_ms(1000);
        usart_rx_cnt = 0;
    }
    if (app_update_cmd[0] == 0x11 &&
        app_update_cmd[1] == 0x22 &&
        app_update_cmd[2] == 0x33 &&
        app_update_cmd[3] == 0x44 &&
        app_update_cmd[4] == 0x55)
        {
            update_excel_flag = 1 ;//开始接收excel 80kb
              // update_app_flag = 1; //因为我的uart中断里只置了这个标志位
            usart_send_string(USART1,"start update excel\r\n");wk_delay_ms(1000);
            usart_send_string(USART1,"please send excel data\r\n");wk_delay_ms(1000);
            usart_rx_cnt = 0;
        }
}

void my_app_load(void)
{
    if (update_app_flag == 1) //更新app
    {
        if (write_state == 0)
        {
            if (usart_rx_cnt)
            {
                wk_delay_ms(1000);/* 新周期内,没有收到任何数据,认为本次数据接收完成 */
                if (oldcount == usart_rx_cnt)
                {
                    applenth = usart_rx_cnt;
                    usart_transmit(USART1, (uint8_t *)&applenth, sizeof(applenth));
                    oldcount = 0;
                    usart_rx_cnt = 0;
                    usart_send_string(USART1,"APP received!\r\n");
                    write_state = 1;
                }
                else
                {
                    oldcount = usart_rx_cnt;
                }
            }
        }
        else if (write_state == 1)
        {

#if 0
            flash_status_type flash_status = flash_efficient_write(APP_START_ADDR, app_rx_buf,applenth);
            if (flash_status == FLASH_OPERATE_DONE) {
                // 校验 Flash 数据是否和接收的数据一致
                uint8_t verify_ok = 1;
                for (uint32_t i = 0; i < applenth; i++) {
                    uint8_t flash_data = *(uint8_t*)(APP_START_ADDR + i);
                    if (flash_data != app_rx_buf[i]) {
                        verify_ok = 0;
                        break;
                    }
                }
                if (verify_ok) {
                    usart_send_string(USART1, "Flash verify OK!\r\n");
                } else {
                    usart_send_string(USART1, "Flash verify FAILED!\r\n");
                    return;  // 校验失败，不跳转
                }
            }
#endif

#if 1
            usart_transmit(USART1, (uint8_t *)&applenth, sizeof(applenth));
             // flash_status_type flash_status = flash_efficient_write(APP_START_ADDR, app_rx_buf,applenth);
             // flash_status_type flash_status = flash_precise_write(APP_START_ADDR, app_rx_buf,applenth);
            flash_status_type flash_status = flash_efficient_write_v1(APP_START_ADDR, app_rx_buf,applenth);
            wk_delay_ms(1000);
             // usart_data_transmit(USART1,flash_status);
             // wk_delay_ms(1000);
            if (flash_status == FLASH_OPERATE_DONE )
            {
                //这里好像没必要写这个flag了因为到这里肯定是在更新app了，应该留这个标志位在app里去检测才对
                flash_status_type  app_write_flag_status  = flash_4byte_write(IAP_UPGRADE_FLAG_ADDR,IAP_UPGRADE_FLAG);
                if (app_write_flag_status == FLASH_OPERATE_DONE)
                {
                    usart_send_string(USART1,"APP FLAG SET !\r\n");
                }
                wk_delay_ms(1000);
                usart_send_string(USART1,"APP operation done!\r\n");
                wk_delay_ms(1000);


                write_state = 0;

                /* check app starting address whether 0x08xxxxxx */
                if(((*(uint32_t*)(APP_START_ADDR + 4)) & 0xFF000000) == 0x08000000)
                {
                    app_load(APP_START_ADDR);
                }
            }
#endif

        }
    }
    if (update_excel_flag == 1)//更新excel
    {
        if(write_state == 0)
       {
           if(usart_rx_cnt)
           {
                wk_delay_ms(1000);/* 新周期内,没有收到任何数据,认为本次数据接收完成 80kb实测要7s发送完毕 */
                if(oldcount == usart_rx_cnt)
                {
                     excelenth = usart_rx_cnt;
                     oldcount = 0;
                     usart_rx_cnt = 0;
                      usart_send_string(USART1,"excel data received!\r\n");
                     write_state = 1;
                }
                else
                {
                    oldcount = usart_rx_cnt;
                }
           }
       }
        else if (write_state == 1)
        {
            wk_delay_ms(1000);
            usart_transmit(USART1,(unsigned char*)&excelenth,4);//验证长度
            wk_delay_ms(1000);
            // flash_status_type flash_status = flash_efficient_write(EXCEL_START_ADDR, app_rx_buf,excelenth);
            // flash_status_type flash_status = flash_precise_write(EXCEL_START_ADDR, app_rx_buf,excelenth);
            flash_status_type flash_status = flash_efficient_write_v1(EXCEL_START_ADDR, app_rx_buf,excelenth);

            wk_delay_ms(1000);
            // usart_data_transmit(USART1,flash_status);
            // wk_delay_ms(1000);

            if (flash_status == FLASH_OPERATE_DONE )
            {
                //这里好像没必要写这个flag了因为到这里肯定是在更新app了，应该留这个标志位在app里去检测才对
                flash_status_type  excel_write_flag_status  = flash_4byte_write(EXCEL_UPGRADE_FLAG_ADDR,EXCEL_UPGRADE_FLAG);
                if (excel_write_flag_status == FLASH_OPERATE_DONE)
                {
                     usart_send_string(USART1,"EXCEL FLAG SET !\r\n");
                }
                wk_delay_ms(1000);
                 usart_send_string(USART1,"EXCEL operation done!\r\n");
                wk_delay_ms(1000);

                write_state = 0;
                // app_load(APP_START_ADDR);//更新完了跳转到app
                nvic_system_reset();  //复位
            }
        }
    }
}






