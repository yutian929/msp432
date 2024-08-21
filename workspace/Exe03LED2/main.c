#include "msp.h"


/**
 * main.c
 * date:    20230806
 * content: lab3.1,3.2
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
	P2->DIR |= (BIT0|BIT1|BIT2);
	P2->OUT &=~(BIT0|BIT1|BIT2);
	int i=0;
	while(1)
	{
	    P2->OUT |= BIT0;
	    delay(50);
	    P2->OUT &=~(BIT0|BIT1|BIT2);
	    //P2->OUT &=~BIT0;
	    P2->OUT |= BIT1;
	    delay(50);
	    P2->OUT &=~(BIT0|BIT1|BIT2);
	    //P2->OUT &=~BIT1;
	    P2->OUT |= BIT2;
	    delay(50);
	    P2->OUT &=~(BIT0|BIT1|BIT2);
	    //P2->OUT &=~BIT2;
	}
}
