#include "msp.h"
/*
 * uart.c
 *
 *  Created on: 2023年8月13日
 *      Author: Zaki
 */

void uart_A0_init(){
    // Configure UART pins
    P1->SEL0 |= (BIT2+BIT3); // set 2-UART pin as secondary function
    P1->SEL1 &=~(BIT2+BIT3);
    //P1->SEL1 &=~(BIT2 | BIT3);
    // Configure UART
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
    EUSCI_A_CTLW0_SSEL__SMCLK | EUSCI_A_CTLW0_PEN; // Configure eUSCI clock source for SMCLK
    // Baud Rate calculation
    // 12000000/(16*9600) = 78.125
    // Fractional portion = 0.125
    // User's Guide Table 21-4: UCBRSx = 0x00
    // UCBRFx = int ( (78.125-78)*16) = 2
    EUSCI_A0->BRW = 78; // 12000000/16/9600
    EUSCI_A0->MCTLW =  (2 << EUSCI_A_MCTLW_BRF_OFS) |EUSCI_A_MCTLW_OS16;
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG; // Clear eUSCI RX interrupt flag

    EUSCI_A0->IE = EUSCI_A_IE_RXIE; // Enable USCI_A0 RX interrupt
    // Enable eUSCIA0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
    // Enable global interrupt
    __enable_irq();
}



