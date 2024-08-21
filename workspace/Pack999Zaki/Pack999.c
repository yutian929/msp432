#include <zyt20221218.h>
#include "msp.h"

/**
 * main.c
 * 2022/9/1 Zaki ÕÅÓêÌï
 * For test
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
//	servo_init();
//	int ang=0;
//	while(1)
//	{
//	    ang=ang+10;
//	    servo1_angle(ang%655);
//	    servo2_angle(ang%655);
//	    servo3_angle(ang%655);
//	    servo4_angle(ang%655);
//	}
	led_init();
	bluetooth_init();
	__sleep();
	__no_operation();
}
void EUSCIA2_IRQHandler(void)//for bluetooth
{
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
    {
        char READ=EUSCI_A2->RXBUF;
        if(EUSCI_A2->RXBUF=='0')
        {
            P2->OUT ^=BIT0;
        }
    }
}
