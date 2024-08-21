#include "msp.h"
#include "timer.h"
#include "gpio.h"
#include "cs.h"
#include "lpm.h"
/**
 * main.c
 * date: 20230807
 * content: Lab6.1 Timer
 */

int cnt_key_pressed = 0;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	cs_init();  // ACLK = 32768Hz
	timer_A0_init();  // CCR0 -> 32768 = 1s, IE
	gpio_key_onboard_interrupt_init();  // both key interrupt enabled
	gpio_led_key_onboard_init();
	lpm_goto_lpm4();
}

void TA0_0_IRQHandler(void){
    // once CCR0 interrupt
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    P2->OUT ^= BIT0;
}

void PORT1_IRQHandler(void){
    int i = 3000;
    for(;i>0;i--){
        ;  //delay
    }
    if( (!(P1->IN & BIT1) || !(P1->IN & BIT4))){
        cnt_key_pressed++;
        if(cnt_key_pressed%3 == 1){  // 0.5s
            TIMER_A0->CCR[0] = 32768/2;
        }
        else if(cnt_key_pressed%3 == 2){  // 0.25s
            TIMER_A0->CCR[0] = 32768/4;
        }
        else{
            TIMER_A0->CCR[0] = 32768;
        }
    }
    P1->IFG = 0;
}
