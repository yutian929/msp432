#include "msp.h"
#include "cs.h"
#include "gpio.h"
/**
 * main.c
 * date: 20230807
 * content:
 */

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


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	cs_init();
	gpio_led_key_onboard_init();
	interrupt_init();

	while(1){
	    int i = 50000;
	    for(;i>0;i--){
	        ;  // delay
	    }
	    P2->OUT ^= BIT0;
	}
}

void PORT1_IRQHandler(void){
    if (P1->IFG & BIT1){  // key s1
        if (!(P1->IN & BIT1)){
            int i = 1000;
            for(;i>0;i--){
                ;
            }
        }
        while(!(P1->IN & BIT1)){
            __no_operation();  // add other func
            // unlock CS module for register access
            CS->KEY = 0x695A;
            //set MCLK source from hfxtclk
            CS->CTL1=CS->CTL1&(~CS_CTL1_SELM_MASK)|CS_CTL1_SELM__HFXTCLK;
            //set MCLK/4
            CS->CTL1=CS->CTL1&(~CS_CTL1_DIVM_MASK)|CS_CTL1_DIVM__8;  // now MCLK = 3MHz
            // lock CS module from unintended access
            CS->KEY = 0;


        }
    }
    else if(P1->IFG & BIT4){  // key s2
        if (!(P1->IN & BIT4)){
            int i = 1000;
            for(;i>0;i--){
                ;
            }
        }
        while(!(P1->IN & BIT4)){
            __no_operation();  // add other func
            CS->KEY = 0x695A;
            //set MCLK source from hfxtclk
            CS->CTL1=CS->CTL1&(~CS_CTL1_SELM_MASK)|CS_CTL1_SELM__HFXTCLK;
            //set MCLK/4
            CS->CTL1=CS->CTL1&(~CS_CTL1_DIVM_MASK)|CS_CTL1_DIVM__4;  // now MCLK = 3MHz
            // lock CS module from unintended access
            CS->KEY = 0;
        }
    }
    P1->IFG = 0;
}

