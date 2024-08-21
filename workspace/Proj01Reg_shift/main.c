#include "msp.h"
#include "comm.h"


void main(void)
{

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	P4->DIR |= BIT6;
	P4->OUT |= BIT6;
	P1->DIR &=~BIT7;
    P1->REN |= BIT7;
    P1->DIR |= BIT5;
	//LED initial
	P2->DIR |= (BIT0+BIT1+BIT2);
	P2->OUT &=~(BIT0+BIT1+BIT2);
	//two register(8bit) used to store data
	//uint8_t reg0,reg1;

	while(1)
	{
	    reload_shift_reg(); //刷新传感器数据
	    //key_check();
	    //switcher_check();
	    ir_check();
	}
}
