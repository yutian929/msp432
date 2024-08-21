#include "msp.h"
/*
 * timer.c
 *
 *  Created on: 2023年8月7日
 *      Author: Zaki
 */

void timer_A0_com_init(){
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

void timer_A2_cap_init(){
    // A2: CS = ACLK = 32768 Hz, continuous mode
    TIMER_A2->CTL |= TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_MC__CONTINUOUS | TIMER_A_CTL_CLR;
    // A2->CCR[3]: Cap mode, Capture rising edge, CCI3A, IE, Synchronous capture
    TIMER_A2->CCTL[3] = TIMER_A_CCTLN_CAP | TIMER_A_CCTLN_CM__RISING | TIMER_A_CCTLN_CCIS__CCIA | TIMER_A_CCTLN_CCIE | TIMER_A_CCTLN_SCS;
    // NVIC, enable_interrupts
    NVIC->ISER[0] = 1 << ((TA2_N_IRQn) & 31);
    __enable_irq();
    // datasheet: P6.6->CCR[3], CCI3A
    P6->DIR &=~ BIT6;
    P6->SEL0 |= BIT6;
    P6->SEL1 &=~ BIT6;

}

void timer_A0_1_4_pwm_init(int ccr0){
    TIMER_A0->CTL= TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;  // ACLK
    TIMER_A0->CCR[0] = ccr0;
}

void timer_A0_1_pwm_on(int ccr1){  // datasheet: P2.4 - A0CCR1
    TIMER_A0->CCTL[1] = OUTMOD_7;
    TIMER_A0->CCR[1] = ccr1;
    P2->SEL0 |= BIT4;  // datasheet: pin mode
    P2->SEL1 &= ~BIT4;
    P2->DIR |=  BIT4;
}

void timer_A0_4_pwm_on(int ccr4){  // datasheet: P2.7 - A0CCR4
    TIMER_A0->CCTL[4] = OUTMOD_7;
    TIMER_A0->CCR[4] = ccr4;
    P2->SEL0 |= BIT7;
    P2->SEL1 &= ~BIT7;
    P2->DIR |=  BIT7;
}

void timer_A0_1_pwm_off(){
    P2->SEL0 &= ~BIT4;
    P2->SEL1 &= ~BIT4;
    P2->DIR |= BIT4;
    P2->OUT &= ~BIT4;
}

void timer_A0_4_pwm_off(){
    P2->SEL0 &= ~BIT7;
    P2->SEL1 &= ~BIT7;
    P2->DIR |= BIT7;
    P2->OUT &= ~BIT7;
}
