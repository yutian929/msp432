#include "msp.h"


/**
 * main.c
 * date: 20230808
 * content: Lab7.1 WDT
 *
 */

int count=-1;
int T2=0;
void main(void)
{
    //WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW
                    | WDT_A_CTL_TMSEL  // Timer mode, not watch dog
                    | WDT_A_CTL_SSEL__ACLK  //
                    | WDT_A_CTL_IS_4   //interval=1s
                    | WDT_A_CTL_CNTCL;  // cnt clear
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
	NVIC->ISER[0] = 1 << ((WDT_A_IRQn) & 31);
	NVIC->ISER[1] = 1<<3;
	__enable_interrupts();
	//sleep
	SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
	__DSB();
	__sleep();


}
void WDT_A_IRQHandler(void)
{
    T2++;
    if(count%3==0||count%3==2)
    {
        P2->OUT ^= BIT0;
    }
    else if(count%3==1)
    {
        if(T2%2==0)
        {
            P2->OUT ^= BIT0;
        }
    }

}
void PORT1_IRQHandler()
{
    P1->IFG &=~BIT1;
    count++;
    if(count%3==0)
    {
        WDT_A->CTL = WDT_A_CTL_PW
                    | WDT_A_CTL_SSEL__ACLK
                    | WDT_A_CTL_TMSEL
                    | WDT_A_CTL_IS_4                //interval=1s
                    | WDT_A_CTL_CNTCL;
    }
    else if(count%3==1)
    {
        WDT_A->CTL = WDT_A_CTL_PW
                    | WDT_A_CTL_SSEL__ACLK
                    | WDT_A_CTL_TMSEL
                    | WDT_A_CTL_IS_5                //interval=250ms
                    | WDT_A_CTL_CNTCL;
        T2=0;

    }
    else if(count%3==2)
    {
        WDT_A->CTL = WDT_A_CTL_PW
                    | WDT_A_CTL_SSEL__ACLK
                    | WDT_A_CTL_TMSEL
                    | WDT_A_CTL_IS_5                //interval=250ms
                    | WDT_A_CTL_CNTCL;
    }
}
