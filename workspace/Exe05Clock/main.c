#include "msp.h"


/**
 * main.c
 * 实验1：ACLK=LFXTCLK，SMCLK=REFOCLK，
 * 初始化MCLK=HFXTCLK/16=3MHz,其中HFXTCLK=48MHz， RGBLED_RED(P2.0)每2s闪烁一次；
 * 按一下S1(P1,1)键，MCLK= MCLK/8=6MHz，LED(P2.0)每1s闪烁一次；
 * 按一下S2(P1,4)键，MCLK=MCLK/4=12MHz，LED(P2.0)每0.5s闪烁一次；
 *
 */
 void delay(int mul)
{
    uint32_t count=mul*5000;
    uint32_t counter;
    for(counter=0;counter<count;counter++)
    {}
}
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	//open clock system
	CS->KEY=0x695A;
	//设置ACLK,SMCLK的时钟源
	CS->CTL1=CS->CTL1&(~CS_CTL1_SELA_MASK)|CS_CTL1_SELA_0;
	CS->CTL1=CS->CTL1&(~CS_CTL1_SELS_MASK)|CS_CTL1_SELS_2;
	//设置MCLK时钟源为HFXTCLK，并16分频
	CS->CTL1=CS->CTL1&(~CS_CTL1_SELM_MASK)|CS_CTL1_SELM_5;
	CS->CTL1=CS->CTL1&(~CS_CTL1_DIVM_MASK)|CS_CTL1_DIVM_4;
	//设置HFXTCLK为48MHz
	CS->CTL2=CS->CTL2&(~CS_CTL2_HFXTFREQ_MASK)|CS_CTL2_HFXTFREQ_6|CS_CTL2_HFXTDRIVE;
	CS->KEY=0;//lock CS
	//initial LED
	P2->DIR |= BIT0;
	//initial Btn
	P1->DIR &=~(BIT1+BIT4);
	P1->REN |= (BIT1+BIT4);
	P1->OUT |= (BIT1+BIT4);
	int flag=3;
	while(1)
	{
	    int i=0;
	    while(1)
	    {
	        P2->OUT ^=BIT0;
	        for(i=0;i<40000&&flag==3;i++)
            {                           }
	        for(i=0;i<40000&&flag==1;i++)
            {	                        }
	        for(i=0;i<30000&&flag==2;i++)
	        {                           }
	        if((P1->IN&(BIT1+BIT4))!=(BIT1+BIT4))
            {
               delay(1);
               if((P1->IN&(BIT1+BIT4))!=(BIT1+BIT4))
               {
                   if((P1->IN&BIT1)!=BIT1)//按1.1->1s
                   {
                       CS->KEY=0x695A;
                       CS->CTL1=CS->CTL1&(~CS_CTL1_DIVM_MASK)|CS_CTL1_DIVM__8;
                       CS->KEY=0;
                       flag=1;
                   }
                   else if((P1->IN&BIT4)!=BIT4)//按1.4->0.5s
                   {
                       CS->KEY=0x695A;
                       CS->CTL1=CS->CTL1&(~CS_CTL1_DIVM_MASK)|CS_CTL1_DIVM__4;
                       CS->KEY=0;
                       flag=2;
                   }

               }
            }
	    }

	}

}
