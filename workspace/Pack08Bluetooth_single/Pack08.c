#include "msp.h"


/**
 * main.c
 * 2022/8/16 Zaki ������
 * ʹ��UARTͨ��Э��,��EUSCI_A2ͨ��
 * �������ֲ��
 * P3.2��ӦUCA2RXD,Ӧ�ú�ʵ��������TX����
 * P3.3��ӦUCA2TXD��Ӧ�ú�ʵ��������RX����
 */
void bluetooth_init()
{
    //����ʱ�ӳ�ʼ��ʹ��Ĭ��SMCLK->DCO��3M
    //uart initial
    P3->SEL0 |= BIT2 | BIT3; // set 2-UART pin as secondary function
    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST/*Remain eUSCI in reset*/ | EUSCI_B_CTLW0_SSEL__SMCLK;  /*Configure eUSCI clock source for SMCLK*/
    //������9600
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
        if(EUSCI_A2->RXBUF=='0')
        {
            P2->OUT |= BIT0+BIT1+BIT2;
        }
    }
}
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	bluetooth_init();
	__sleep();
}
