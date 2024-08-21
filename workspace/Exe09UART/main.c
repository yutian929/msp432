#include "msp.h"
#include "cs.h"
#include "uart.h"
#include "lpm.h"
/**
 * main.c
 * date: 20230813
 * content: lab9.1 uart
 */

char code[10];
int i=0,j=0;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	cs_smclk_12m_init();
	uart_A0_init();
	lpm_goto_lpm4();
	__no_operation();
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
                EUSCI_A0->TXBUF= 0x01;

            }
            else if(strcmp(code,"0xAA040506")==0)
            {
                EUSCI_A0->TXBUF= 0x02;

            }
            else
            {
                EUSCI_A0->TXBUF= 0x03;

            }
            i=0;
        }
    }
}
