#include "msp.h"


/**
 * main.c
 * 2022/9/24 Zaki ������
 * �����������ʹ��:
 * �������pwm����A���������жϼ�������½��أ�ͬʱ���B��ĸߵ͵�ƽ���ж�����ת��+-���ʱ��������
 * A->P5.1  B->P4.0
 */
int v;
int SPEEDINDEX=300;
void MG310_init()/*MGR����������ĳ�ʼ����Ŀǰֻ��A�������½���*/
{
    //ָʾ��
    P1DIR |= BIT0;
    //A��
    P5->DIR &=~BIT1;
    P5->IE |= BIT1;
    P5->IES &=~BIT1;/*��������*/
    NVIC->ISER[1] |= 1<<7;
    __enable_interrupts();
    //B��
    P4->DIR &=~BIT0;
    //��ʱ��
    TA3CTL |= TASSEL_1+TACLR+MC_1;
    TA3CCR0=32768;
    TA3CCTL0=CCIE;
    NVIC->ISER[0]|= 1<<(TA3_0_IRQn & 31);

}
void TA3_0_IRQHandler()
{
    if(v>=SPEEDINDEX)
    {
        P1OUT |= BIT0;
    }
    else
    {
        P1OUT &=~BIT0;
    }
    printf("%d\n",v);
    v=0;
    TIMER_A3->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

}
void PORT5_IRQHandler()
{
    if(P5->IFG & BIT1)
    {
        if(!(P5->IES & BIT1))/*A�����ؽ���*/
        {
            P5->IES |= BIT1;
            if(P4->IN & BIT0)/*��ʱ���B��Ϊ�ߵ�ƽ*/
            {
                v++;
            }
            else
            {
                v--;
            }
        }
        else if((P5->IES & BIT1))/*A�½��ؽ���*/
        {
            P5->IES &=~BIT1;
            if(P4->IN & BIT0)/*��ʱ���B��Ϊ�ߵ�ƽ*/
            {
                v--;
            }
            else
            {
                v++;
            }
        }
    }
    P5->IFG =0;
}

//���ǰ��
void LeftMotor_Forward(int rate)
{
    //����   P2.7 and P2.4
      P2->SEL0 |= BIT4;//��ǰ
      P2->SEL1 &= ~BIT4;
      P2->DIR |=  BIT4;
      TIMER_A0->CCR[0] = 255;
      TIMER_A0->CCTL[1] = OUTMOD_7;
      TIMER_A0->CCR[1] = (rate);
      TIMER_A0->CTL = TASSEL_1+MC_1+TACLR;
      P2->SEL0 &= ~BIT7;
      P2->SEL1 &= ~BIT7;
      P2->DIR |= BIT7;
      P2->OUT &= ~BIT7;
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
//	MG310_init();
//    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_1 | // ACLK
//        TIMER_A_CTL_MC_1|TIMER_A_CTL_CLR; // up mode
//    P2->DIR |= BIT4; // P2.6-7 option selection TA0.3-4
//    P2->SEL0 |= BIT4;
//    P2->SEL1 &=~(BIT4);
//    TIMER_A0->CCR[0]=655;
//    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
//    TIMER_A0->CCR[1]=300;
	while(1)
	{
	    LeftMotor_Forward(128);
//	    __sleep();
	}



}


