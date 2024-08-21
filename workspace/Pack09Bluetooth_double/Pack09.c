#include "msp.h"


/**
 * main.c
 * 2022/8/16 Zaki 张雨田
 * 双蓝牙，即主从机蓝牙模式，需要先进行配对(USB转TTL,蓝牙调试助手)
 * 下面主要写从机代码
 */
void bluetooth_init()
{
    //蓝牙时钟初始化使用默认SMCLK->DCO：3M
    //uart initial
    P3->SEL0 |= BIT2 | BIT3; // set 2-UART pin as secondary function
    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST/*Remain eUSCI in reset*/ | EUSCI_B_CTLW0_SSEL__SMCLK;  /*Configure eUSCI clock source for SMCLK*/
    //波特率9600
    EUSCI_A2->BRW = 19;
    EUSCI_A2->MCTLW = (8 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;
    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // start eUSCI
    EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG; // Clear eUSCI RX interrupt flag
    //RX_IE
    EUSCI_A2->IE |= EUSCI_A_IE_RXIE;
    NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);
    __enable_interrupts();
    //LED init
    P2->DIR |= (BIT0+BIT1+BIT2);
    P2->OUT &=~(BIT0+BIT1+BIT2);
}
void EUSCIA2_IRQHandler(void)
{
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
    {
        if(EUSCI_A2->RXBUF=='A')
        {
            P2->OUT ^= BIT0;
        }
        else if(EUSCI_A2->RXBUF=='B')
        {
            P2->OUT ^= BIT1;
        }
        else
        {
            P2->OUT ^= BIT2;
        }
    }
}
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	bluetooth_init();
	__sleep();
}
