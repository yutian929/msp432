#include "msp.h"


/**
 * main.c
 * 2022/8/17 Zaki 张雨田
 * 2Y0A21红外测距原理(自己理解)：
 * 1.红黑线接Vcc,Gnd
 * 2.黄线输出电压模拟值，用ADC转换得数字电压值
 * 3.自己根据网上的图，找到对应段的电压值和距离的曲线公式，代入
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	//查数据手册得P5.5对应A0
    P5->SEL1 |= BIT5;
    P5->SEL0 |= BIT5;
    //Sampling time, S&H=16, ADC14 on
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    //Use sampling timer,12-bit conversion results
    ADC14->CTL1 = ADC14_CTL1_RES_2;
    //A1 ADC input select;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_0;

    int temp=0;
    int realDistance=0;
    while(1)
    {
        //开启转换
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
        //等待转换完成
        while((ADC14->IFGR0&ADC14_IFGR0_IFG0)==0);
        temp=ADC14->MEM[0];
        ADC14->CTL0 &=~ADC14_CTL0_ENC;
        //粗略计算,自己看着办吧,不想写了

    }
    //30cm->1250~1350
    //20cm->1550~1650
    //15cm->2450~2650
    //10cm->2750~2850
    //5cm->2850~2950
    //<5cm->adc>2850
}
