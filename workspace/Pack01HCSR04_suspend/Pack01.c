#include "msp.h"
/**
 * main.c
 * 2022/8/4 Zaki 张雨田
 * 超声测距模块HCSR04工作原理：
 * 1.使用单片机的一个引脚发送一个至少10us高电平的TTL脉冲信号到模块的Trig引脚，用于触发模块工作。
 * 2.模块检测到触发信号之后，会自动发送8个40khz的方波，然后自动切换至监测模式，监测是否有信号返回（超声波信号遇障碍物会返回）。
 * 3.如果有信号返回，通过模块的Echo引脚会输出一个高电平， 高电平持续的时间就是超声波从发射到返回的时间。
 * 4.因为声音在空气中的速度为340米/秒，即可计算出所测的距离。
 */
#include <driverlib.h>
#include "adc14.c"
#include "aes256.c"
#include "comp_e.c"
#include "cpu.c"
#include "crc32.c"
#include "dma.c"
#include "eusci.h"
#include "flash.c"
#include "fpu.c"
#include "gpio.c"
#include "i2c.c"
#include "interrupt.c"
#include "mpu.c"
#include "pcm.c"
#include "pmap.c"
#include "pss.c"
#include "ref_a.c"
#include "reset.c"
#include "rtc_c.c"
#include "spi.c"
#include "sysctl.c"
#include "systick.c"
#include "timer32.c"
#include "timer_a.c"
#include "uart.c"
#include "wdt_a.c"


void HCSR04Init()
{
    //TIMER_A2 setting
    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK/*选择SMCLK,默认3MHz*/
            | TIMER_A_CTL_MC__UP;/*增计数模式*/
    TIMER_A2->CCR[0]=50000;
//    TIMER_A2->CCTL[0]= TIMER_A_CCTLN_CCIE;/*若测距较远则需开启CCR[0]中断*/
    //GPIO setting
    P6->DIR |= BIT4;/*P6.4做Trig口*/
    P6->OUT &=~BIT4;
    P6->DIR &=~BIT5;/*P6.5做Echo口*/
    P6->REN |= BIT5;
    P6->OUT &=~BIT5;
}
float distance()
{
    int TRIG,ECHO;
    int count=0;
    float DISTANCE,sum=0;
    //测5次取平均
    while(count<5)
    {
        count++;
        //先给Trig一段高电平
        P6->OUT &=~BIT4;
        P6->OUT |= BIT4;
        int i;
        for(i=1000;i>0;i--);
        P6->OUT &=~BIT4;
        //TRIG为计数开始值
//        TIMER_A2->CTL |= TIMER_A_CTL_CLR;
        TIMER_A2->R=0;
        TRIG=TIMER_A2->R;
        //检测Echo什么时候结束高电平
        while((P6->IN & BIT5)==0);
        while((P6->IN & BIT5)==BIT5);
        ECHO=TIMER_A2->R;
        //计算
        DISTANCE=(float)(ECHO-TRIG)*0.0057-40;
        sum+=DISTANCE;
    }
    return sum/5;
}

float DISTANCE;
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	HCSR04Init();
	while(1)
	{
	    DISTANCE=distance();
	    __no_operation();
	}



}
