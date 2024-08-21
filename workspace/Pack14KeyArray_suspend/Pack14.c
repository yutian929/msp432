#include "msp.h"


/**
 * main.c
 * 2022/8/29 Zaki ������
 * ���ƾ������
 * ����6.4 6.5 6.6 6.7
 * ����2.3 2.5 2.6 2.7
 */

void KeyArray_init()
{
    //KeyArray initial
    P6->DIR |= (BIT4+BIT5+BIT6+BIT7);
    P6->OUT &=~(BIT4+BIT5+BIT6+BIT7);
    P2->DIR &=~(BIT3+BIT5+BIT6+BIT7);
    P2->REN |= (BIT3+BIT5+BIT6+BIT7);
    P2->OUT &=~(BIT3+BIT5+BIT6+BIT7);//������Ϊ�����͵�ƽ
}
char Key()
{
    char KEYVALUE='\0';
    int i=0;
    //�����������Ϊ�ߵ�ƽ
    //����ɨ��:��1��
    P6->OUT |= BIT4;
    if(P2->IN&(BIT3+BIT5+BIT6+BIT7))
    {
        for(i=200;i>0;i--);//����
        if( (P2->IN&BIT3))
        {
            KEYVALUE='1';
        }
        else if( (P2->IN&BIT5))
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
    }
    P6->OUT &=~BIT4;
    //����ɨ��:��2��
    P6->OUT |= BIT5;
    if(P2->IN&(BIT3+BIT5+BIT6+BIT7))
    {
        for(i=200;i>0;i--);//����
        if( (P2->IN&BIT3))
        {
            KEYVALUE='4';
        }
        else if( (P2->IN&BIT5))
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
    }
    P6->OUT &=~BIT5;
    //����ɨ��:��3��
    P6->OUT |= BIT6;
    if(P2->IN&(BIT3+BIT5+BIT6+BIT7))
    {
        for(i=200;i>0;i--);//����
        if( (P2->IN&BIT3))
        {
            KEYVALUE='7';
        }
        else if( (P2->IN&BIT5))
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
    }
    P6->OUT &=~BIT6;
    //����ɨ��:��4��
    P6->OUT |= BIT7;
    if(P2->IN&(BIT3+BIT5+BIT6+BIT7))
    {
        for(i=200;i>0;i--);//����
        if( (P2->IN&BIT3))
        {
            KEYVALUE='#';
        }
        else if( (P2->IN&BIT5))
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
    }
    P6->OUT &=~BIT7;
    __no_operation();
    return KEYVALUE;
}
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	KeyArray_init();
    while(1)
    {
        char keyvalue=Key();
    }
}
