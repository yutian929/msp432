#include "msp.h"
#include "timer.h"
#include "cs.h"
/**
 * main.c
 * date: 20230809
 * content: Lab7.1 PWM control motor
 */

void delay__lab7(){
    int i =2000000;
    for(;i>0;i--){
        ;  // delay
    }
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	// hardware construct -> P2.4/7 (TIMERA0.1/4)
	cs_init();
	timer_A0_1_4_pwm_init(100);
	while(1){

	    timer_A0_1_pwm_on(80);
	    timer_A0_4_pwm_off();

	    delay__lab7();

	    timer_A0_4_pwm_on(80);
        timer_A0_1_pwm_off();

        delay__lab7();

        timer_A0_1_pwm_on(40);
        timer_A0_4_pwm_off();

        delay__lab7();

        timer_A0_4_pwm_on(40);
        timer_A0_1_pwm_off();

        delay__lab7();
	}
}
