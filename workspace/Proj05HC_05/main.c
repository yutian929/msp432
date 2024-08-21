#include "msp.h"
#include <string.h>
//#include "Uart.h"
/*
 * main.cPC端通过串口发送数据给MSP432 LaunchPad，
 * 其中，波特率设置为1200,8位数据位，1位奇校验位，1位停止位。
 * 若PC端发送的数据为0xAA010203，则MSP432端发0x01给PC端，且RGBLED_RED亮
 * 若PC端发送的数据为0xAA040506，则MSP432端发02给PC端，且RGBLED_GREEN亮
 * 若PC端发送其他数据，则MSP432端发03给PC端，RGBLED_BLUE亮。
 */

void initUART(void)
{
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
    EUSCI_A0->BRW = 625; // 12000000/16/9600
    EUSCI_A0->MCTLW = EUSCI_A_MCTLW_OS16;
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG; // Clear eUSCI RX interrupt flag
}

char code[10];
int i=0,j=0;
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    CS->KEY = CS_KEY_VAL; // Unlock CS module for register access
    CS->CTL0 = 0; // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3; // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 | // Select ACLK = REFO
    CS_CTL1_SELS_3 | // SMCLK = DCO
    CS_CTL1_SELM_3; // MCLK = DCO
    CS->KEY = 0; // Lock CS module from unintended accesses
    P2->DIR |= (BIT0+BIT1+BIT2);
    P2->OUT &=~(BIT0+BIT1+BIT2);
    initUART();
    EUSCI_A0->IE = EUSCI_A_IE_RXIE; // Enable USCI_A0 RX interrupt
    // Enable global interrupt
    __enable_irq();
    // Enable eUSCIA0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
    // Enable sleep on exit from ISR
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    // Ensures SLEEPONEXIT occurs immediately
    __DSB();
    // Enter LPM0
    __sleep();
    __no_operation(); // For debugger



}
// UART interrupt service routine
void EUSCIA0_IRQHandler(void)
{
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
    {
    // Check if the TX buffer is empty first
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    // Echo the received character back
    //EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
    code[i]=EUSCI_A0->RXBUF;
    i++;
    if(i==10)
    {
        if(strcmp(code,"0xAA010203")==0)
        {
            EUSCI_A0->TXBUF="0x01";
            P2->OUT |= BIT0;
            P2->OUT &=~(BIT1+BIT2);
        }
        else if(strcmp(code,"0xAA040506")==0)
        {
            EUSCI_A0->TXBUF="0x02";
            P2->OUT |= BIT1;
            P2->OUT &=~(BIT0+BIT2);
        }
        else
        {
            EUSCI_A0->TXBUF="0x03";
            P2->OUT |= BIT2;
            P2->OUT &=~(BIT1+BIT0);
        }
        i=0;
    }


    }
}

