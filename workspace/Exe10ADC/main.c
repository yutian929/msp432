#include "msp.h"


/**
 * main.c
 * ADC14的输入口A23，
 * A23通过ADC14BATMAP位置为1选择为电池电压监测，
 * 用ADC14采集电压值，
 * 并根据显示不同颜色的LED灯表示目前电池电量。
 */
void main(void)
{

    volatile uint32_t i;
    WDT_A->CTL = WDT_A_CTL_PW | // Stop WDT
    WDT_A_CTL_HOLD;
    ADC14->CTL0 &=~ADC14_CTL0_ENC;
    // GPIO Setup
    P1->OUT &= ~BIT0; // Clear LED to start
    P1->DIR |= BIT0; // Set P1.0/LED to output
//    P8->SEL1 |= BIT2; // Configure P8.2 for ADC
//    P8->SEL0 |= BIT2;
    // Sampling time, S&H=16, ADC14 on
    ADC14->CTL0 |= ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP |
    ADC14_CTL0_ON ;
    ADC14->CTL1 = ADC14_CTL1_RES_2 | ADC14_CTL1_BATMAP; // Use sampling timer, 12-bit conversion results,bat
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_23; // A1 ADC input select;

    ADC14->CTL0 |= ADC14_CTL0_ENC;
    while (1)
    {
        for (i = 20000; i > 0; i--); // Delay
        // Start sampling/conversion
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;//SC用于启动
        while((ADC14->IFGR0&ADC14_IFGR0_IFG0)==0);
        if (ADC14->MEM[0] >= 1986) // ADC12MEM0 = A1 > 1.6A?
        P1->OUT |= BIT0; // P1.0 = 1
        else
        P1->OUT &= ~BIT0; // P1.0 = 0
    }
}







