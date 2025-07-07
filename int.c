#include "boot_uart.h"
void USART1_IRQHandler(void)
{
  /* add user code begin USART1_IRQ 0 */
  // usart_send_string(USART1,"start update app\r\n");
  // usart_send_string(USART1,"please send app data\r\n");

  uint16_t reval;
  if(usart_interrupt_flag_get(USART1, USART_RDBF_FLAG) != RESET)
  {
    bootmode = 1 ;//收到字节表明进入boot模式  可能是更新app  也可能是更新表

    reval = usart_data_receive(USART1);

    if (update_app_flag == 0 && update_excel_flag == 0)  //未收到更新app的命令
    {
      if (usart_rx_cnt < 5)
      {
        app_update_cmd[usart_rx_cnt] = reval;
         usart_rx_cnt++;
      }
      // else
      // {
      //   usart_rx_cnt = 0;
      // }
    }
    else if (update_app_flag == 1 || update_excel_flag == 1)//接收到更新app的命令
    {
      if (usart_rx_cnt < USART_RX_MAX_LEN)
      {
          app_rx_buf[usart_rx_cnt] = reval;
          usart_rx_cnt++;
      }
    }

  }
  /* add user code end USART1_IRQ 0 */
  /* add user code begin USART1_IRQ 1 */

  /* add user code end USART1_IRQ 1 */
}

