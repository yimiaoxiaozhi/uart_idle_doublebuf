#include "boot_uart.h"
#include "iap.h"



int main(void)
{
  /* add user code begin 1 */

  /* add user code end 1 */

  /* system clock config. */
  wk_system_clock_config();

  /* config periph clock. */
  wk_periph_clock_config();

  /* init debug function. */
  wk_debug_config();

  /* nvic config. */
  wk_nvic_config();

  /* timebase config. */
  wk_timebase_init();

  /* init gpio function. */
  wk_gpio_config();

  /* init usart1 function. */
  wk_usart1_init();

  /* init tmr9 function. */
  wk_tmr9_init();

  /* add user code begin 2 */

#if 0   //测试flash读写
   flash_4byte_write(IAP_UPGRADE_FLAG_ADDR,0x12345678);
#endif

  // usart_send_string(USART1,"start update app\r\n");wk_delay_ms(1000);
  // usart_send_string(USART1,"please send app data\r\n");wk_delay_ms(1000);
 // unsigned int a = 0x12345678;
 //  usart_transmit(USART1, (uint8_t *)&a, sizeof(a));
  wk_delay_ms(5000);   //2.5S  没收到字节直接跳转到app
  app_update();

  // wk_delay_ms(5000);
  // usart_data_transmit(USART1,bootmode);

  if (bootmode == 0)
    {//未进入boot模式  跳转
    if (flash_upgrade_flag_read() == SET) {  //这里好像没必要增加这个读app_flag标志位检测了
      /* check app starting address whether 0x08xxxxxx */
      if (((*(uint32_t *) (APP_START_ADDR + 4)) & 0xFF000000) == 0x08000000) {
        app_load(APP_START_ADDR);
      }
    }
  }

 /*if (flash_upgrade_flag_read() == RESET) {
    wk_delay_ms(5000);
    app_update();
  }*/

  if (excel_upgrade_flag_read() == SET)
  {
    //表的标志位置位了
  }

/**
 * bootloader靠命令进行是否更新app，
 * 如果是app收到更新的话直接写需要更新的flag的标志位，
 * boot里面可以通过检查标志位来检测是否是由app收到的更新命令。
 * 这样可以判断是app还是bootloader
*/




  /* add user code end 2 */

  while(1)
  {
    /* add user code begin 3 */
     my_app_load();
    /* add user code end 3 */
  }
}

