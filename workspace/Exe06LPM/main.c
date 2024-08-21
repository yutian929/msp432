#include "msp.h"


/**
 * main.c
 * ʵ��1��ʵ�ְ�S1-1.1����RGBLED_RED��ʾ����S2-1.4����RGBLED_GREEN��ʾ��û�а�����ʱ�����LPM4ģʽ��
 *
 */

void PORT1_IRQHandler(void)
{
    int i=0;
    if((P1->IFG&(BIT1+BIT4)))
    {
        for(i=0;i<1000;i++){}
        if(!(P1->IN&BIT1))
        {
            P2->OUT |= BIT0;
            P2->OUT &=~BIT1;
            P1->IFG &=~BIT1;
        }
        else if(!(P1->IN&BIT4))
        {
            P2->OUT |= BIT1;
            P2->OUT &=~BIT0;
            P1->IFG &=~BIT4;
        }
    }
}



void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	//initial
	P2->DIR |= (BIT0+BIT1);
	P2->OUT &=~(BIT0+BIT1);
	P1->DIR &=~(BIT1+BIT4);
	P1->REN |= (BIT1+BIT4);
	P1->OUT |= (BIT1+BIT4);
	//���ö˿��ж�����
	P1->IE |= (BIT1+BIT4);
	P1->IES |= (BIT1+BIT4);
	P1->IFG=0;
	//�����ж�P1����
	NVIC->ISER[1]=1 << ((PORT1_IRQn) & 31);
	//�������ж�
	__enable_interrupts();
	// Enable PCM rude mode, which allows to device to enter LPM3 without
	//waiting for peripherals
	PCM->CTL1 = PCM_CTL1_KEY_VAL|PCM_CTL1_FORCE_LPM_ENTRY;
	//����˯�����Ϊ1
	SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
	//�ն����exit�����˯
	SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
	//����˯
	__DSB();
	//����LPM4
	__sleep();
}
