#include "msp.h"
/**
 * main.c
 * 2022/8/16 Zaki ������
 * 1.����ģ��Ի�������ǿ�������У�һ�����������Χ����������ǿ�ȡ�
 * 2.ģ���ڻ�������ǿ�ȴﲻ���趨��ֵʱ��OUT����ߵ�ƽ������绷������ǿ�ȳ����趨��ֵʱ��ģ��OUT����͵�ƽ��
 * 3.С�����������OUT�����뵥Ƭ��ֱ��������ͨ����Ƭ�������ߵ͵�ƽ���ɴ�����⻷����������
 */
int CLAP=0;//���ִ���
int wait=0;
void soundDect_init()
{
    //����P3.7����Ϊ������
    P3->DIR &=~BIT7;
    P3->REN |= BIT7;
    P3->OUT &=~BIT7;
    //���ж���д�����������sleep
    P3->IE |= BIT7;
    P3->IES|= BIT7;/*ƽʱΪ�ߵ�ƽ�����Բ��ü���½���*/
    NVIC->ISER[1] = 1 << ((PORT3_IRQn) & 31);
    __enable_interrupts();
//    //��ʱ������TimerA0
//    TIMER_A0->CTL= TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_MC__STOP;/*ACLK,32768Hz, stop mode*/
//    //CCR0
//    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;/*[0]IE*/
//    NVIC->ISER[0]=1 << ((TA0_0_IRQn) & 31);
//    TIMER_A0->CCR[0]=32768;/*1s*/
//    //CCR1
//    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;/*[1]IE*/
//    NVIC->ISER[0]=1 << ((TA0_N_IRQn) & 31);
//    TIMER_A0->CCR[1]=32768/5;/*0.2s*/
//    //NVIC�ж����ȼ�
//    NVIC->IP[0]=TA0_0_IRQn;
//    NVIC->IP[4]=TA0_N_IRQn;
//    NVIC->IP[8]=PORT3_IRQn;
//    __enable_interrupts();
    //Systick����
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_TICKINT_Msk;//����systick,3MHz,���ж�
    SysTick->VAL=0x01;
    SysTick->LOAD=(0xB71B0 - 1)*4;//Ĭ��ʱ������£�1Hz
}
void SysTick_Handler(void)
{
    //1s�����㣬�ر�Systick
    CLAP=0;
    SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
}
//void TA0_0_IRQHandler()
//{
//    //1s
//    CLAP=0;
//    TIMER_A0->CCTL[0] &=~TIMER_A_CCTLN_CCIFG;
//}
//void TA0_N_IRQHandler()
//{
//    //0.2s
//    switch(TIMER_A0->IV &0x0E)
//    {
//    case 0: break; // No interrupt
//    case 2: wait=0;break; // CCR1 interrupt
//    case 4: break; // CCR2 interrupt
//    case 6: break; // CCR3 interrupt
//    case 8: break; // CCR4 interrupt
//    case 10: break; // CCR5 interrupt
//    case 12: break; // CCR6 interrupt
//    case 14: break; //overflow
//    default: break;
//    }
//}
void PORT3_IRQHandler()
{
//    if(wait==0)
//    {
//        wait=1;
        //һ�����������ο���(Ҫ��������Ҫ��ȥ֮ǰ��CLAP)
        if((P3->IFG)|BIT7==BIT7)
        {
            CLAP++;
            //����������ʱ,0.2s�ڲ���CLAP++
    //        TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
    //        while(wait);
    //        wait=1;
            //for��ʱ
            int i=0;
            for(i=50000;i>0;i--);
            if(CLAP==4)
            {
                P1->OUT ^=BIT0;/*���ص�*/
                CLAP=0;
    //            TIMER_A0->CTL = TIMER_A0->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__STOP;/*�ص���ʱ������*/
    //            TIMER_A0->CTL |= TIMER_A_CTL_CLR;
            }
            else if(CLAP>0&&CLAP<4)
            {
                //����Systick
                SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
            }
        }
//        wait=0;
//    }
    P3->IFG=0;
}
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	P1->DIR |= BIT0;
	P1->OUT &=~BIT0;
	soundDect_init();

    //����˯��
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
	__DSB();
	__sleep();
}
