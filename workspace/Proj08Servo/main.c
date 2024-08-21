#include "msp.h"

void delay(int mul)
{
    uint32_t count=mul*2000;
    uint32_t counter;
    for(counter=0;counter<count;counter++)
    {}
}



void servo_init()
{   //P7.4/PM_TA1.4/C0.5 2
    TIMER_A1->CTL = TIMER_A_CTL_TASSEL_1 | // ACLK
        TIMER_A_CTL_MC_1|TIMER_A_CTL_CLR; // up mode
    P7->DIR |= BIT4; // P2.6-7 option selection TA0.3-4
    P7->SEL0 |= BIT4;
    P7->SEL1 &=~(BIT4);
}
//servo_angle(21);
//servo_angle(54);
//servo_angle(86);
void servo_angle(int ang)
{
    // Configure GPIO
    TIMER_A1->CCR[0]=655;
    TIMER_A1->CCTL[4] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A1->CCR[4]=ang;
}

void main()
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop WDT
    servo_init();
    int i;
    while(1)
    {
        servo_angle(21);
        delay(30);
        servo_angle(54);
        delay(30);
        servo_angle(86);
        delay(30);
    }



}

