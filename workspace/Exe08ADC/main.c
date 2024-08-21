#include "msp.h"
#include "adc.h"

/**
 * main.c
 * date: 20230813
 * content: Lab8: adc-query and interrupt
 */
int adc_val = 0;
void main(void) {
    int i;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;  // Stop WDT

    // Sampling time, S&H=16, ADC14 on
    adc_b12_m0_ch23_query_init();
    adc_b12_m0_ch23_interrupt_init();

     while (1)
    {

         for (i = 20000; i > 0; i--);        // Delay
         // Start sampling/conversion
         adc_start();
//         adc_val = adc_m0_query();
         __no_operation();
    }
}

void ADC14_IRQHandler(){
    adc_val = ADC14->MEM[0];
    __no_operation();
}
