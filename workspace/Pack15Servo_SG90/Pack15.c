#include "msp.h"


/**
 * main.c
 * 2022/8/28 Zaki 张雨田
 * SG90伺服电机，TIMERA1输出PWM波控制，信号口P2.4
 *
 */
void servo_init()
{   //P2.4/PM_TA0.1
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_1 | // ACLK
        TIMER_A_CTL_MC_1|TIMER_A_CTL_CLR; // up mode
    P2->DIR |= BIT4; // P2.6-7 option selection TA0.3-4
    P2->SEL0 |= BIT4;
    P2->SEL1 &=~(BIT4);
}
//angle from 12~13~44~76~87
void servo_angle(int ang)
{
    // Configure GPIO
    TIMER_A0->CCR[0]=655;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCR[1]=ang;
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	servo_init();
	int i=0;
	while(1)
	{
//	    servo_angle(13);
	    servo_angle(44);
//	    servo_angle(76);
	}
}
