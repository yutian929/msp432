#include "msp.h"
#include "timer.h"
#include "cs.h"
#include "lpm.h"

/**
 * main.c
 * date: 20230808
 * content: Lab6.2 cap mode
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	cs_init();
	timer_A2_cap_init();
	lpm_goto_lpm4();
	__no_operation();

}

void TA2_N_IRQHandler(void){
    TIMER_A2->CCTL[3] &= ~TIMER_A_CCTLN_CCIFG;
    int s = TIMER_A2->CCR[3];
    // other logic funcs
    __no_operation();
}
