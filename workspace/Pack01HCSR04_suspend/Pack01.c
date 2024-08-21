#include "msp.h"
/**
 * main.c
 * 2022/8/4 Zaki ������
 * �������ģ��HCSR04����ԭ��
 * 1.ʹ�õ�Ƭ����һ�����ŷ���һ������10us�ߵ�ƽ��TTL�����źŵ�ģ���Trig���ţ����ڴ���ģ�鹤����
 * 2.ģ���⵽�����ź�֮�󣬻��Զ�����8��40khz�ķ�����Ȼ���Զ��л������ģʽ������Ƿ����źŷ��أ��������ź����ϰ���᷵�أ���
 * 3.������źŷ��أ�ͨ��ģ���Echo���Ż����һ���ߵ�ƽ�� �ߵ�ƽ������ʱ����ǳ������ӷ��䵽���ص�ʱ�䡣
 * 4.��Ϊ�����ڿ����е��ٶ�Ϊ340��/�룬���ɼ��������ľ��롣
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
    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK/*ѡ��SMCLK,Ĭ��3MHz*/
            | TIMER_A_CTL_MC__UP;/*������ģʽ*/
    TIMER_A2->CCR[0]=50000;
//    TIMER_A2->CCTL[0]= TIMER_A_CCTLN_CCIE;/*������Զ���迪��CCR[0]�ж�*/
    //GPIO setting
    P6->DIR |= BIT4;/*P6.4��Trig��*/
    P6->OUT &=~BIT4;
    P6->DIR &=~BIT5;/*P6.5��Echo��*/
    P6->REN |= BIT5;
    P6->OUT &=~BIT5;
}
float distance()
{
    int TRIG,ECHO;
    int count=0;
    float DISTANCE,sum=0;
    //��5��ȡƽ��
    while(count<5)
    {
        count++;
        //�ȸ�Trigһ�θߵ�ƽ
        P6->OUT &=~BIT4;
        P6->OUT |= BIT4;
        int i;
        for(i=1000;i>0;i--);
        P6->OUT &=~BIT4;
        //TRIGΪ������ʼֵ
//        TIMER_A2->CTL |= TIMER_A_CTL_CLR;
        TIMER_A2->R=0;
        TRIG=TIMER_A2->R;
        //���Echoʲôʱ������ߵ�ƽ
        while((P6->IN & BIT5)==0);
        while((P6->IN & BIT5)==BIT5);
        ECHO=TIMER_A2->R;
        //����
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
