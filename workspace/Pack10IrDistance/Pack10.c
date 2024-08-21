#include "msp.h"


/**
 * main.c
 * 2022/8/17 Zaki ������
 * 2Y0A21������ԭ��(�Լ����)��
 * 1.����߽�Vcc,Gnd
 * 2.���������ѹģ��ֵ����ADCת�������ֵ�ѹֵ
 * 3.�Լ��������ϵ�ͼ���ҵ���Ӧ�εĵ�ѹֵ�;�������߹�ʽ������
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	//�������ֲ��P5.5��ӦA0
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
        //����ת��
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
        //�ȴ�ת�����
        while((ADC14->IFGR0&ADC14_IFGR0_IFG0)==0);
        temp=ADC14->MEM[0];
        ADC14->CTL0 &=~ADC14_CTL0_ENC;
        //���Լ���,�Լ����Ű��,����д��

    }
    //30cm->1250~1350
    //20cm->1550~1650
    //15cm->2450~2650
    //10cm->2750~2850
    //5cm->2850~2950
    //<5cm->adc>2850
}
