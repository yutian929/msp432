#include "msp.h"


/**
 * main.c
 * 2022/8/6 Zaki 张雨田
 * 使用中断,捕获上升沿和下降沿来实现超声测距,消除中间等待回波的时间对主程序的阻塞
 * ？但不知道为什么只能测19cm
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
    //P6.5接受口中断使能
    P6->IE |= BIT5;
    P6->IES &=~ BIT5;
    P6->IFG =0;
    NVIC->ISER[1]=1<<8;
    __enable_interrupts();
    //TIMER_A2
    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK/*选择SMCLK,默认3MHz*/
            | TIMER_A_CTL_MC__UP;/*增计数模式*/
    TIMER_A2->CCR[0]=50000;
    TIMER_A2->CCTL[0] |= TIMER_A_CCTLN_CCIE;
}

void trig()
{
    //先给Trig一段高电平
    P6->OUT &=~BIT4;
    P6->OUT |= BIT4;
    int i;
    for(i=1000;i>0;i--);
    P6->OUT &=~BIT4;
    //关闭TA2
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
    //如果上升沿
    if((P6->IES&BIT5)==0)
    {
        //记录开始值
        TIMER_A2->CTL |= TIMER_A_CTL_CLR;
        TIMER_A2->R=0;
        TRIG=TIMER_A2->R;
        //开启A2
        TIMER_A2->CTL =  TIMER_A2->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__UP;
        //等待下降沿
        P6->IES |= BIT5;
        P6->IFG =0;
        __no_operation();
    }
    //如果下降沿
    else if((P6->IES&BIT5)==BIT5)
    {
        //记录终止值
        ECHO=TIMER_A2->R;
        //计算
        DISTANCE=(float)(ECHO-TRIG)*0.0057+UPCOUNT*283.3;
        UPCOUNT=0;
        //改为上升沿
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
