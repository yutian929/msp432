#include "msp.h"


/**
 * main.c
 * 2022/8/6 Zaki ������
 * ʹ���ж�,���������غ��½�����ʵ�ֳ������,�����м�ȴ��ز���ʱ��������������
 * ������֪��Ϊʲôֻ�ܲ�19cm
 */

float DISTANCE;
int UPCOUNT=0;
void HCSR04Init(void)
{
    P6->DIR |= BIT4;/*TRIG*/
    P6->OUT &=~BIT4;
    P6->DIR &=~BIT5;/*ECHO*/
    P6->REN |= BIT5;
    P6->OUT &=~BIT5;
    //P6.5���ܿ��ж�ʹ��
    P6->IE |= BIT5;
    P6->IES &=~ BIT5;
    P6->IFG =0;
    NVIC->ISER[1]=1<<8;
    __enable_interrupts();
    //TIMER_A2
    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK/*ѡ��SMCLK,Ĭ��3MHz*/
            | TIMER_A_CTL_MC__UP;/*������ģʽ*/
    TIMER_A2->CCR[0]=50000;
    TIMER_A2->CCTL[0] |= TIMER_A_CCTLN_CCIE;
}

void trig()
{
    //�ȸ�Trigһ�θߵ�ƽ
    P6->OUT &=~BIT4;
    P6->OUT |= BIT4;
    int i;
    for(i=1000;i>0;i--);
    P6->OUT &=~BIT4;
    //�ر�TA2
    TIMER_A2->CTL =  TIMER_A2->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__STOP;
    __no_operation();
}
void TA2_0_IRQHandler()
{
    TIMER_A2->CCTL[0] &=~TIMER_A_CCTLN_CCIFG;
    UPCOUNT++;
}
void PORT6_IRQHandler()
{
    static int TRIG,ECHO;
    P6->IFG =0;
    //���������
    if((P6->IES&BIT5)==0)
    {
        //��¼��ʼֵ
        TIMER_A2->CTL |= TIMER_A_CTL_CLR;
        TIMER_A2->R=0;
        TRIG=TIMER_A2->R;
        //����A2
        TIMER_A2->CTL =  TIMER_A2->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__UP;
        //�ȴ��½���
        P6->IES |= BIT5;
        P6->IFG =0;
        __no_operation();
    }
    //����½���
    else if((P6->IES&BIT5)==BIT5)
    {
        //��¼��ֵֹ
        ECHO=TIMER_A2->R;
        //����
        DISTANCE=(float)(ECHO-TRIG)*0.0057+UPCOUNT*283.3;
        UPCOUNT=0;
        //��Ϊ������
        P6->IES &=~BIT5;
        P6->IFG =0;
        __no_operation();
    }

}
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	HCSR04Init();
	while(1)
	{
	    trig();
	    __no_operation();
	}
}
