#include "msp.h"


/**
 * main.c
 * 2022/8/14 Zaki 张雨田
 * 光敏电阻,四脚(一定要有AO口,DO比较简单，不做介绍)
 * 用片内ADC模块完成光照强度信号(模拟量)的采集与转换
 * 光照强度越高，得到的返回值越小
 */

void ADC_init()
{
    //查数据手册得P5.4口SEL设置
    P5->SEL1 |= BIT4;
    P5->SEL0 |= BIT4;
    //Sampling time, S&H=16, ADC14 on
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    //Use sampling timer,12-bit conversion results
    ADC14->CTL1 = ADC14_CTL1_RES_2;
    //A1 ADC input select;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1;
}
int light_value()
{
    int mem0;
    //开启转换
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    //等待转换完成
    while((ADC14->IFGR0&ADC14_IFGR0_IFG0)==0);
    mem0=ADC14->MEM[0];
    ADC14->CTL0 &=~ADC14_CTL0_ENC;
    return mem0;
}

int LIGHTVAL;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	ADC_init();
	while(1)
	{
	    LIGHTVAL=light_value();
	}

}
