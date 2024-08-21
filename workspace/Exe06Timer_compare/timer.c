#include "msp.h"
/*
 * timer.c
 *
 *  Created on: 2023年8月7日
 *      Author: Zaki
 */

void timer_A0_init(){
    // CS, mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_MC__UP;// ACLK = 32768 Hz, up mode
    // CCR0 -> target = 32768
    TIMER_A0->CCR[0] = 32768;
    // IE
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
     __enable_irq();  // Enable global interrupt
}


