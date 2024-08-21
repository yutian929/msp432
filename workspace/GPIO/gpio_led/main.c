#include "msp.h"


/**
 * main.c
 */
void main(void)
{
    int  i;
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    P1->SEL0 &= ~0x01;
    P1->SEL1 &= ~0x01;
    P1->DIR |= 0x01;     //  make P1.0 out
    P1->OUT &= ~0x01;      //    P1.0 output low  Ãð
    while(1){
        P1->OUT ^= 0x01;
        for(i=20000;i>0;i--);
     }
}
