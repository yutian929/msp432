#include "msp.h"
/**
 * main.c
 * 2022/8/16 Zaki 张雨田
 * 1.声音模块对环境声音强度最敏感，一般用来检测周围环境的声音强度。
 * 2.模块在环境声音强度达不到设定阈值时，OUT输出高电平，当外界环境声音强度超过设定阈值时，模块OUT输出低电平；
 * 3.小板数字量输出OUT可以与单片机直接相连，通过单片机来检测高低电平，由此来检测环境的声音；
 */
int CLAP=0;//拍手次数
int wait=0;
void soundDect_init()
{
    //采用P3.7口作为输入检测
    P3->DIR &=~BIT7;
    P3->REN |= BIT7;
    P3->OUT &=~BIT7;
    //用中断来写，主程序可以sleep
    P3->IE |= BIT7;
    P3->IES|= BIT7;/*平时为高电平，所以采用检测下降沿*/
    NVIC->ISER[1] = 1 << ((PORT3_IRQn) & 31);
    __enable_interrupts();
//    //计时器设置TimerA0
//    TIMER_A0->CTL= TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_MC__STOP;/*ACLK,32768Hz, stop mode*/
//    //CCR0
//    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;/*[0]IE*/
//    NVIC->ISER[0]=1 << ((TA0_0_IRQn) & 31);
//    TIMER_A0->CCR[0]=32768;/*1s*/
//    //CCR1
//    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;/*[1]IE*/
//    NVIC->ISER[0]=1 << ((TA0_N_IRQn) & 31);
//    TIMER_A0->CCR[1]=32768/5;/*0.2s*/
//    //NVIC中断优先级
//    NVIC->IP[0]=TA0_0_IRQn;
//    NVIC->IP[4]=TA0_N_IRQn;
//    NVIC->IP[8]=PORT3_IRQn;
//    __enable_interrupts();
    //Systick设置
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_TICKINT_Msk;//启动systick,3MHz,开中断
    SysTick->VAL=0x01;
    SysTick->LOAD=(0xB71B0 - 1)*4;//默认时钟情况下，1Hz
}
void SysTick_Handler(void)
{
    //1s后清零，关闭Systick
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
        //一秒内拍手两次开灯(要消抖，还要除去之前的CLAP)
        if((P3->IFG)|BIT7==BIT7)
        {
            CLAP++;
            //开启消抖计时,0.2s内不再CLAP++
    //        TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
    //        while(wait);
    //        wait=1;
            //for延时
            int i=0;
            for(i=50000;i>0;i--);
            if(CLAP==4)
            {
                P1->OUT ^=BIT0;/*开关灯*/
                CLAP=0;
    //            TIMER_A0->CTL = TIMER_A0->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__STOP;/*关掉计时并清零*/
    //            TIMER_A0->CTL |= TIMER_A_CTL_CLR;
            }
            else if(CLAP>0&&CLAP<4)
            {
                //启动Systick
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

    //设置睡眠
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
	__DSB();
	__sleep();
}
