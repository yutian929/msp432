#include "msp.h"


/**
 * main.c
 * date: 20230808
 * content: Lab7.2 Systick
 */
int count=-1;
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//P2.0
    P2->DIR |= BIT0;
    P2->OUT &=~BIT0;
    //P1.1-s1
    P1->DIR &=~BIT1;
    P1->REN |= BIT1;
    P1->OUT |= BIT1;
    P1->IE |= BIT1;
    P1->IES |= BIT1;
    P1->IFG =0;
    //NVIC
    NVIC->ISER[1] = 1<<3;
    __enable_interrupts();
    //sleep
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    __DSB();
    __sleep();


}
void PORT1_IRQHandler()
{
    P1->IFG &=~BIT1;
    count++;
    if(count%3==0)
    {
        SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk;
        // Set SysTick period = 0xB71B0,4Hz/3MHz MCLK
        SysTick->LOAD = (0xB71B0 - 1)*4;
    }
    else if(count%3==1)
    {
        SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk;
        // Set SysTick period = 0xB71B0,4Hz/3MHz MCLK
        SysTick->LOAD = (0xB71B0 - 1)*2;
    }
    else if(count%3==2)
    {
        SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk;
        // Set SysTick period = 0xB71B0,4Hz/3MHz MCLK
        SysTick->LOAD = (0xB71B0 - 1);
    }
}
void SysTick_Handler(void)
{
    P2->OUT ^= BIT0; // Toggle P2.0 LED
}
