//
// Created by wangbin on 25-6-25.
//

#include "iap.h"

iapfun jump_to_app;


/**
  * @brief  app load.
  * @param  app_addr
  *         app code starting address
  * @retval none
  */
void app_load(uint32_t app_addr)
{
    /* check the address of stack */
    if(((*(uint32_t*)app_addr) - 0x20000000) <= (SRAM_SIZE * 1024))
    {
        /* disable periph clock */
        crm_periph_clock_enable(CRM_TMR9_PERIPH_CLOCK, FALSE);
        crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, FALSE);
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, FALSE);

        /* disable nvic irq and clear pending */
        nvic_irq_disable(USART1_IRQn);
        nvic_irq_disable(TMR1_BRK_TMR9_IRQn);
        __NVIC_ClearPendingIRQ(USART1_IRQn);
        __NVIC_ClearPendingIRQ(TMR1_BRK_TMR9_IRQn);

        jump_to_app = (iapfun)*(uint32_t*)(app_addr + 4);        /* code second word is reset address */
        __set_MSP(*(uint32_t*)app_addr);                        /* init app stack pointer(code first word is stack address) */
        jump_to_app();                                          /* jump to user app */
    }
}
























