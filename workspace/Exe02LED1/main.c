#include "msp.h"


/**
 * main.c
 * ����������0x20000000��ַ��ʼ��RAMд��һ���ַ�����Hi��micro computer��Enjoy MCU!��,
 * ���ǽ�����ַ���ͬʱ������0x20001000��ַ��ʼ��RAM�У�
 * д�����������������ַ����Ƿ�һ�£�
 * ���һ�£�����ʾLED1��˸������һ��LED1������
 */
void delay(int mul)
{
    uint32_t count=mul*5000;
    uint32_t counter;
    for(counter=0;counter<count;counter++)
    {}
}
void blink()
{
    P1DIR |= BIT0;
    while(1)
    {
        P1OUT ^=BIT0;
        delay(8);
    }
}

void LedOn()
{
    P1DIR |= BIT0;
    while(1)
    {
        P1OUT |=BIT0;
    }
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	int* ptr1=0x20000000;
	int* ptr2=0x20001000;
	char str[28]="Hi��micro computer��Enjoy MCU!";
	int i;
	for(i=0;i<28;i++)
	{
	    *ptr1++=str[i];
	    *ptr2++=str[i];
	}
	ptr1=0x20000000;
	ptr2=0x20001000;
	for(i=0;i<28;i++)
    {
        if(*ptr1==*ptr2)
        {
            ptr1++;ptr2++;
        }
        else
        {
            break;
        }
    }
	if(i==28)
	{
	    blink();
	}
	else
	{
	    LedOn();
	}


}
