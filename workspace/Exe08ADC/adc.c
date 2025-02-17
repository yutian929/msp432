#include "msp.h"
/*
 * adc.c
 *
 *  Created on: 2023年8月10日
 *      Author: Zaki
 */

void adc_b12_m0_ch23_query_init(){
    ADC14->CTL0 &=~ADC14_CTL0_ENC;  // disable convert
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;  // sampling hold time
    ADC14->CTL1 = ADC14_CTL1_RES_2 | ADC14_CTL1_BATMAP;  // 12bit, battery map
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_23; // A1 ADC input select, internal battery voltage
    // note: if not internal sorce, remember to set related pin mode

//    /*interrupt mode set*/
//    ADC14->IER0 |= ADC14_IER0_IE0;
//    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);  // Enable ADC interrupt in NVIC module
//    __enable_irq();

    ADC14->CTL0 |= ADC14_CTL0_ENC;
}

void adc_b12_m0_ch23_interrupt_init(){
    ADC14->CTL0 &=~ADC14_CTL0_ENC;  // disable convert
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;  // sampling hold time
    ADC14->CTL1 = ADC14_CTL1_RES_2 | ADC14_CTL1_BATMAP;  // 12bit, battery map
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_23; // A1 ADC input select, internal battery voltage
    // note: if not internal sorce, remember to set related pin mode

    /*interrupt mode set*/
    ADC14->IER0 |= ADC14_IER0_IE0;
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);  // Enable ADC interrupt in NVIC module
    __enable_irq();

    ADC14->CTL0 |= ADC14_CTL0_ENC;
}

void adc_b12_m0_batv_init(){
//    adc_b12_m0_ch23_query_init();
    adc_b12_m0_ch23_interrupt_init();
}


void adc_start(){
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;  // SC: start convertion
}

int adc_m0_query(){
    int m0;
    while((ADC14->IFGR0&ADC14_IFGR0_IFG0)==0);  // wait for convert finish
    m0 = ADC14->MEM[0];
    return m0;
}


