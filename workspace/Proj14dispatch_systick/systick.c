#include "msp.h"
/*
 * systick.c
 *
 *  Created on: 2023年8月9日
 *      Author: Zaki
 */

void systick_init(){
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;  // core clock = 3MHz, so 1ms = 24kHz->0x5DC0;
    SysTick->LOAD = 0xBB8;
}
void systick_start(){
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
    SysTick->VAL = 0x01;
}


