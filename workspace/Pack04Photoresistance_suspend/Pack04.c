#include "msp.h"


/**
 * main.c
 * 2022/8/14 Zaki ������
 * ��������,�Ľ�(һ��Ҫ��AO��,DO�Ƚϼ򵥣���������)
 * ��Ƭ��ADCģ����ɹ���ǿ���ź�(ģ����)�Ĳɼ���ת��
 * ����ǿ��Խ�ߣ��õ��ķ���ֵԽС
 */

void ADC_init()
{
    //�������ֲ��P5.4��SEL����
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
    //����ת��
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    //�ȴ�ת�����
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
