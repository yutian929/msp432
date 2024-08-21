#include "msp.h"


/**
 * main.c
 * 2022/8/28 Zaki 张雨田
 * 自制矩阵键盘
 * 引发6.4 6.5 6.6 6.7
 * 接收2.3 2.4 2.6 2.7
 * 大概率是我PCB板设计不太好
 * 要用就用查询模式吧，中断真的搞不出来
 */

char KEYVALUE='\0';

void KeyArray_init()
{
    //KeyArray initial
    P6->DIR |= (BIT4+BIT5+BIT6+BIT7);
    P6->OUT |= (BIT4+BIT5+BIT6+BIT7);//都先发出高电平
    P2->DIR &=~(BIT3+BIT4+BIT6+BIT7);
    P2->REN |= (BIT3+BIT4+BIT6+BIT7);
    P2->OUT &=~(BIT3+BIT4+BIT6+BIT7);//都先置为下拉低电平
    //加入中断
    P2->IE |= (BIT3+BIT4+BIT6+BIT7);
    P2->IES &=~(BIT3+BIT4+BIT6+BIT7);//上升沿中断
    NVIC->ISER[1]=(1<<4);
    __enable_interrupts();
    P2->IFG =0;
}

void PORT2_IRQHandler()
{
    P2->IFG =0;
    int i=0;
    for(i=200;i>0;i--);//消抖
    //先全部置低
    P6->OUT &=~(BIT4+BIT5+BIT6+BIT7);
    //输出端依次置为高电平
    //逐行扫描:第1行
    P6->OUT |= BIT4;
    if( (P2->IN&BIT3))
    {
        KEYVALUE='1';
    }
    else if( (P2->IN&BIT4))
    {
        KEYVALUE='2';
    }
    else if( (P2->IN&BIT6))
    {
        KEYVALUE='3';
    }
    else if( (P2->IN&BIT7))
    {
        KEYVALUE='A';
    }
    P6->OUT &=~BIT4;
    //逐行扫描:第2行
    P6->OUT |= BIT5;
    if( (P2->IN&BIT3))
    {
        KEYVALUE='4';
    }
    else if( (P2->IN&BIT4))
    {
        KEYVALUE='5';
    }
    else if( (P2->IN&BIT6))
    {
        KEYVALUE='6';
    }
    else if( (P2->IN&BIT7))
    {
        KEYVALUE='B';
    }
    P6->OUT &=~BIT5;
    //逐行扫描:第3行
    P6->OUT |= BIT6;
    if( (P2->IN&BIT3))
    {
        KEYVALUE='7';
    }
    else if( (P2->IN&BIT4))
    {
        KEYVALUE='8';
    }
    else if( (P2->IN&BIT6))
    {
        KEYVALUE='9';
    }
    else if( (P2->IN&BIT7))
    {
        KEYVALUE='C';
    }
    P6->OUT &=~BIT6;
    //逐行扫描:第4行
    P6->OUT |= BIT7;
    if( (P2->IN&BIT3))
    {
        KEYVALUE='#';
    }
    else if( (P2->IN&BIT4))
    {
        KEYVALUE='0';
    }
    else if( (P2->IN&BIT6))
    {
        KEYVALUE='*';
    }
    else if( (P2->IN&BIT7))
    {
        KEYVALUE='D';
    }
    P6->OUT &=~BIT7;
    __no_operation();
    //恢复全部置高
    P6->OUT |= (BIT4+BIT5+BIT6+BIT7);
    P2IFG = 0;
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	KeyArray_init();

	while(1)
	{

	    __sleep();
	}


}
