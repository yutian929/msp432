#include "msp.h"


/**
 * main.c
 */

int warning()
{
    if((P1->IN&BIT6)==BIT6)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	P1->DIR &=~BIT6;
	P1->REN |= BIT6;
	P1->OUT &=~BIT6;
	P1->DIR |= BIT0;
	P1->OUT &=~BIT0;
	//遇到黑线变低电平
	while(1)
	{
	    if((P1->IN&BIT6)==BIT6)
        {
            P1->OUT ^= BIT0;
        }
	}



}
