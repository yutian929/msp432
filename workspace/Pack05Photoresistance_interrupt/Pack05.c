#include "msp.h"


/**
 * main.c
 * 2022/8/14 Zaki ������
 * ��������,�Ľ�(һ��Ҫ��AO��)
 * ��Ƭ��ADCģ����ɹ���ǿ���ź�(ģ����)�Ĳɼ���ת��
 * ����ǿ��Խ�ߣ��õ��ķ���ֵԽС
 */
int SOUNDVAL;
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
    //ͨ��0ʹ��
    ADC14->IER0 = ADC14_IER0_IE0;
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    __enable_interrupts();
    //GPIO Setup
    P1->OUT &= ~BIT0; // Clear LED to start
    P1->DIR |= BIT0; // Set P1.0/LED to output
}
void ADC_single_start()
{
    //����ת��
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
}

void ADC14_IRQHandler(void)
{
    //ת�����
    SOUNDVAL=ADC14->MEM[0];
    ADC14->CTL0 &=~ADC14_CTL0_ENC;
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    ADC_init();
    while(1){
        ADC_single_start();
        if(SOUNDVAL > 4000){
            P1->OUT ^= BIT0;
        }

    }

}
