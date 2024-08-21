#include "msp.h"
int PART = 0;//全局变量表示题目号
char part='\0';
/*************从机初始化****************/
void cong_bluetooth_init()
{
    //    //led initial
        CS->KEY =CS_KEY_VAL;
        CS->CTL0 = 0;
        CS->CTL0 = CS_CTL0_DCORSEL_3; // Set DCO to 12MHz (nominal, center of 8-16MHz range)
        CS->CTL1 = CS_CTL1_SELS_3;    // sMCLK = LFXT
        CS->KEY = 0; // Lock CS module from unintended accesses

        //uart initial
        P3->SEL0 |= BIT2 | BIT3; // set 2-UART pin as secondary function
        EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
        EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
                EUSCI_B_CTLW0_SSEL__SMCLK; // Configure eUSCI clock source for SMCLK
    // Baud Rate calculation
    // 12000000/(16*9600) = 78.125
    // Fractional portion = 0.125
    // User's Guide Table 21-4: UCBRSx = 0x00
    // UCBRFx = int ( (78.125-78)*16) = 2
        EUSCI_A2->BRW = 78; // 12000000/16/9600
        EUSCI_A2->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
                EUSCI_A_MCTLW_OS16;
        EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
        EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG; // Clear eUSCI RX interrupt flag



}
/***************从机******************/
int LANYA=0;
void cong_bluetooth()
{
    char part;
    do
    {
        part=EUSCI_A2->RXBUF;
    }
    while(part=='\0');

    switch(part)
    {
    case '1':    P2OUT &=~(BIT1+BIT2);P2OUT |= BIT0;PART=1;LANYA=1;break;

    case '2':    P2OUT &=~(BIT0+BIT2);P2OUT |= BIT1;PART=2;LANYA=2;break;

    case '3':    P2OUT &=~(BIT1+BIT0);P2OUT |= BIT2;PART=3;LANYA=3;break;
    case '4':    P2OUT |= (BIT0+BIT1+BIT2);PART=4;LANYA=4;break;
    default:break;
    }


}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    P1DIR |= BIT0;
    P1OUT &=~ BIT0;
    P2->DIR |= BIT0+BIT1+BIT2;

    cong_bluetooth_init();
    while(!LANYA)
    {
        cong_bluetooth();
    }


    CS->KEY =CS_KEY_VAL;
    CS->CTL0 = 0;
    CS->CTL0 = CS_CTL0_DCORSEL_1; // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELS_0;    // sMCLK = LFXT
    CS->KEY = 0; // Lock CS module from unintended accesses
}
