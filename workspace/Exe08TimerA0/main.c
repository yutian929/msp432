#include "msp.h"

/**
 * main.c
 * 实验1：
 * 按一下S1-1.1键，RGBLED_RED每1s闪烁一次，
 * 再按一下S1键，RGBLED_RED每0.5s闪烁一次，
 * 再按一次S1键， RGBLED_RED每0.25s闪烁一次，
 * 再按一次S键回到每1s闪烁一次，如此循环。
 * 请用Timer_A0做定时，并设计低功耗。
 *
 */
//#define D 19000

int count=-1;

void cs_initial()
{
    //ACLK设为LFXTCLK-32768Hz
    CS->KEY=0x695A;
    CS->CTL1=CS->CTL1&(~CS_CTL1_SELA_MASK)|CS_CTL1_SELA__LFXTCLK ;
    CS->KEY=0;
}

void PORT1_IRQHandler(void)
{
    int i=0;
    for(i=0;i<2000;i++)
        {}
    if(!(P1->IN&BIT1))
    {
        count++;
        if(count%3==0)
        {
            TIMER_A0->CCR[0]=32768;//1s
        }
        else if(count%3==1)
        {
            TIMER_A0->CCR[0]=16384;//0.5s
        }
        else if(count%3==2)
        {
            TIMER_A0->CCR[0]=8192;//0.25s
        }
    }

    P1->IFG &=~ BIT1;
}

void TA0_0_IRQHandler(void)
{
    P2->OUT ^= BIT0;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CTL &=~ BIT0;//我TM真服了
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	//CS
	cs_initial();

	//P2
	P2->DIR |= BIT0;
	P2->OUT &=~BIT0;
	//P1
	P1->DIR &=~BIT1;
	P1->REN |= BIT1;
	P1->OUT |= BIT1;
	P1->IE |= BIT1;
	P1->IES |= BIT1;
	P1->IFG = 0;
	//NVIC
	NVIC->ISER[1] = 1<<3;

	//TimerA0
	TIMER_A0->CTL= TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_MC__UP;//ACLK, up mode
	TIMER_A0->CTL &=~ BIT0;
	TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;//[0]IE
	//TIMER_A0->CCR[0]=100;//1s
	//TIMER_A0->CCR[1]=
	//NVIC
	NVIC->ISER[0]=1 << ((TA0_0_IRQn) & 31);

	//总
	//__enable_irq();
	__enable_interrupts();

	PCM->CTL1 = PCM_CTL1_KEY_VAL |PCM_CTL1_FORCE_LPM_ENTRY;


	//sleep
	SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
	//__DSB();
	while(1)
	{
	    __sleep();
	}


	__no_operation();
}
