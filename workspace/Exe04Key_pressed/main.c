#include "msp.h"


/**
 * main.c
 * ʵ�־��а���ȥ���ĳ��򣬰�S1(P1.1)����RGBLED_RED(P2.0)��ʾ����S2(P1.4)����RGBLED_GREEN(P2.1)��ʾ
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
	//initia
	P1->DIR &=~(BIT1+BIT4);
	P1->REN |= (BIT1+BIT4);
	P2->DIR |= (BIT0+BIT1+BIT2);
	P2->OUT &=~(BIT0+BIT1+BIT2);

	while(1)
	{
	    if((P1->IN&(BIT1+BIT4))==(BIT1+BIT4))
	    {
	    }
	    else
	    {
	        delay(5);
	        if((P1->IN&(BIT1+BIT4))==(BIT1+BIT4))
            {
            }
	        else
	        {
	            if((P1->IN&BIT1)!=BIT1)
	            {
	                P2->OUT |= BIT0;
	                P2->OUT &=~BIT1;
	            }
	            else if((P1->IN&BIT4)!=BIT4)
	            {
	                P2->OUT |= BIT1;
                    P2->OUT &=~BIT0;
	            }


	        }

	    }
	}




}
