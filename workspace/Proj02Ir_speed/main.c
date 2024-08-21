#include "msp.h"
#include <math.h>

/**
 * main.c
 * 实验2：采用TA2的捕获模式读取小车左右轮的转速。
 *
 */


#define NUMBER_TIMER_CAPTURES        10

uint16_t timerAcaptureValues1[NUMBER_TIMER_CAPTURES] = {0};
uint16_t timerAcaptureCalcVal1[NUMBER_TIMER_CAPTURES - 1] = {0};
uint16_t timerAcaptureValues2[NUMBER_TIMER_CAPTURES] = {0};
uint16_t timerAcaptureCalcVal2[NUMBER_TIMER_CAPTURES - 1] = {0};
uint16_t timerAcapturePointer1 = 0;
uint16_t timerAcapturePointer2 = 0;
uint16_t S=0;
int v1=0,v2=0;
int i=0;
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	//CS initial->ACLK设为LFXTCLK-32768Hz
    CS->KEY=0x695A;
    CS->CTL1=CS->CTL1&(~CS_CTL1_SELA_MASK)|CS_CTL1_SELA__LFXTCLK ;
    CS->KEY=0;
    //LED initial
    P2->DIR |= (BIT0+BIT1+BIT2);
    P2->OUT &=~(BIT0+BIT1+BIT2);
	//P6.6,P6.7initial->数据手册
	P6->DIR &=~(BIT6+BIT7);
	P6->SEL0 |= (BIT6+BIT7);
	P6->SEL1 &=~(BIT6+BIT7);
//	P6->REN |= (BIT6+BIT7);
//	P6->OUT &=~(BIT6+BIT7);
	//TA2 initial->TA2.CCI3A  TA2.CCI4A
	TIMER_A2->CTL |= TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_MC__CONTINUOUS | TIMER_A_CTL_CLR /*| TIMER_A_CTL_IE*/;//ACLK, CONTINUOUS mode ,clear TAxR, TAIE
	TIMER_A2->CCTL[3] = TIMER_A_CCTLN_CM_1 |    // Capture rising edge,
	            TIMER_A_CCTLN_CCIS_0 |          // Use CCI3A as input,*******
	            TIMER_A_CCTLN_CCIE |            // Enable capture interrupt
	            TIMER_A_CCTLN_CAP |             // capture mode,
	            TIMER_A_CCTLN_SCS;              // Synchronous capture
	TIMER_A2->CCTL[4] = TIMER_A_CCTLN_CM_1 |    // Capture rising edge,
                TIMER_A_CCTLN_CCIS_0 |          // Use CCI4A as input,
                TIMER_A_CCTLN_CCIE |            // Enable capture interrupt
                TIMER_A_CCTLN_CAP |             // capture mode,
                TIMER_A_CCTLN_SCS;              // Synchronous capture
	TIMER_A2->CTL &=~ BIT0;//手动清IFG
	//sleep
	SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;    // Enable sleep on exit from ISR
    __DSB();// Ensures SLEEPONEXIT takes effect immediately
    //NVIC INTISR[13] Timer_A2 TA2CCTLx.CCIFG (x = 1 to 4), TA2CTL.TAIFG
    NVIC->ISER[0] |= 1 << 13;
    // Enable global interrupt
    __enable_irq();

    while(1)
    {
        __sleep();
        __no_operation();

        if(S==1)
        {
            v1=0;
            // Validate the captured clocks
            for (i = 0; i < (NUMBER_TIMER_CAPTURES - 1); i++)
            {
                timerAcaptureCalcVal1[i] = fabs(timerAcaptureValues1[i + 1] - timerAcaptureValues1[i]);
                v1 += timerAcaptureCalcVal1[i];
            }
            v1 = v1/NUMBER_TIMER_CAPTURES;

            P2->OUT ^= BIT0;
            //for(;;);

        }
        else if(S==2)
        {
            v2=0;
            // Validate the captured clocks
            for (i = 0; i < (NUMBER_TIMER_CAPTURES - 1); i++)
            {
                timerAcaptureCalcVal2[i] = fabs(timerAcaptureValues2[i + 1] - timerAcaptureValues2[i]);
                v2 += timerAcaptureCalcVal2[i];
            }
            v2 = v2/NUMBER_TIMER_CAPTURES;

            P2->OUT ^= BIT1;
            //for(;;);

        }
    }



}

void TA2_N_IRQHandler(void)
{
    TIMER_A2->CTL &=~ BIT0;
    //BITBAND_PERI(TIMER_A_CMSIS(TIMER_A0_BASE)->CCTL[3],TIMER_A_CCTLN_COV_OFS) = 0;//软件复位COV
    //BITBAND_PERI(TIMER_A_CMSIS(TIMER_A0_BASE)->CCTL[4],TIMER_A_CCTLN_COV_OFS) = 0;//软件复位COV
    P2->OUT ^= BIT2;
    switch(TIMER_A2->IV &0x0E)
    {
    case 0: break; // No interrupt
    case 2: break; // CCR1
    case 4: break; // CCR2 interrupt
    case 6:
        {
            if (timerAcapturePointer1 >= NUMBER_TIMER_CAPTURES)
            {
                // Disable capture timer
                TIMER_A2->CTL &= ~(TIMER_A_CTL_MC__CONTINUOUS);
                // Disable sleep on exit upon returning from ISR
                // to continue executing code in main
                SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
                // Ensures SLEEPONEXIT takes effect immediately
                __DSB();
                S=1;
            }
            else
            {
                // Capture the CCR register value into the array
                timerAcaptureValues1[timerAcapturePointer1++] = TIMER_A2->CCR[3];
            }
            // Clear the interrupt flag
            TIMER_A2->CCTL[3] &= ~(TIMER_A_CCTLN_CCIFG);
            TIMER_A2->CTL &=~ BIT0;
            break; // CCR3 interrupt->s1
        }
    case 8:
        {
            if (timerAcapturePointer2 >= NUMBER_TIMER_CAPTURES)
            {
                // Disable capture timer
                TIMER_A2->CTL &= ~(TIMER_A_CTL_MC__CONTINUOUS);
                // Disable sleep on exit upon returning from ISR
                // to continue executing code in main
                SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
                // Ensures SLEEPONEXIT takes effect immediately
                __DSB();
                S=2;
            }
            else
            {
                // Capture the CCR register value into the array
                timerAcaptureValues2[timerAcapturePointer2++] = TIMER_A2->CCR[4];
            }
            // Clear the interrupt flag
            TIMER_A2->CCTL[4] &= ~(TIMER_A_CCTLN_CCIFG);
            TIMER_A2->CTL &=~ BIT0;
            break; // CCR4 interrupt->s2
        }
    case 10: break; // CCR5 interrupt
    case 12: break; // CCR6 interrupt
    case 14: break; //overflow
    default: break;
    }

}

