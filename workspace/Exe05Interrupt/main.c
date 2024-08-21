#include "msp.h"
#include "gpio.h"
#include "lpm.h"
/**
 * main.c
 * date: 20230806
 * content: lab5 interrupt
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	gpio_init();
	interrupt_init();
	lpm_goto_lpm4();
	while(1){
	    __no_operation();
	}
}

void interrupt_init(){  // global interrupt init, needn't to save as .h/.c
    /*key onboard interrupt init*/
    // set key P1.1/4 as input with pull-up
    P1->DIR &=~ (BIT1|BIT4);
    P1->REN |= (BIT1|BIT4);  // pull-up
    P1->OUT |= (BIT1|BIT4);
    P1->IE |= (BIT1|BIT4);
    P1->IES |= (BIT1|BIT4);
    P1->IFG &=~ (BIT1|BIT4);
    P1->IFG = 0;

    NVIC->ISER[1] |= 1 << ((PORT1_IRQn) & 31);

    /*timer interrupt init*/


    __enable_interrupts();

}

void PORT1_IRQHandler(void){
    if (P1->IFG & BIT1){  // key s1
        if (!(P1->IN & BIT1)){
            int i = 10000;
            for(;i>0;i--){
                ;
            }
        }
        while(!(P1->IN & BIT1)){
            __no_operation();  // add other func
        }
        gpio_red_led_on();
    }
    else if(P1->IFG & BIT4){  // key s2
        if (!(P1->IN & BIT4)){
            int i = 10000;
            for(;i>0;i--){
                ;
            }
        }
        while(!(P1->IN & BIT1)){
            __no_operation();  // add other func
        }
        gpio_green_led_on();
    }
    P1->IFG = 0;
}
