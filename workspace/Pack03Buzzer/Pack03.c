#include "msp.h"


/**
 * main.c
 * 2022/8/13 Zaki ������
 * �õ������ŵ���Դ������
 * ������ͬʱ��Ĳ�ͣ�Ŀ���IO���ԴﵽƵ�ʺͽ���
 * �õľ���ϵͳĬ�ϵ�ʱ�ӣ�����Ҫ�޸�
 * ��Դ���磬����ΪP2.5
 */

//����������,n��Ƶ�ʣ�j�ǳ���ʱ�䣨ʱ��δ���Թ���֪��jȡ���٣�
void buzzer(int n,int j)
{
    P2DIR |= BIT5;
    ///��ʱÿ��������ʱ��
    for ( ;j>0;j--)
    {
        volatile unsigned int i;
        P2OUT ^= BIT5;
        i = n;
        do
        {
        i--;                                // �Լ�����
        __no_operation();                   // �ղ���
        }while (i != 0);                    // ��׼��do����while��䣬ע���©�������ź���ķֺ�
     }
}
//buzzer(95,50);   //do
//buzzer(85,50);   //re
//buzzer(75,50);   //mi
//buzzer(71,50);   //fa
//buzzer(63,50);   //so
//buzzer(56,50);   //la
//buzzer(50,50);   //xi
//buzzer(95,400);   //do
//buzzer(85,400);   //re
//buzzer(75,400);   //mi
//buzzer(71,400);   //fa
//buzzer(63,400);   //so
//buzzer(56,400);   //la
//buzzer(50,400);   //xi
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    buzzer(95,400);   //do
    buzzer(85,400);   //re
    buzzer(75,400);   //mi
    buzzer(95,400);   //do
    _delay_cycles(3000); //ÿ���������ĺ��һ����ʱ

    buzzer(95,400);   //do
    buzzer(85,400);   //re
    buzzer(75,400);   //mi
    buzzer(95,400);   //do
    _delay_cycles(3000); //ÿ���������ĺ��һ����ʱ

    buzzer(75,400);   //mi
    buzzer(71,400);   //fa
    buzzer(63,400);   //so
   _delay_cycles(3000);

   buzzer(75,400);   //mi
   buzzer(71,400);   //fa
   buzzer(63,400);   //so
    _delay_cycles(3000);

    buzzer(63,400);   //so
    buzzer(56,400);   //la
    buzzer(63,400);   //so
    buzzer(71,400);   //fa
    buzzer(75,400);   //mi
    buzzer(95,400);   //do
    _delay_cycles(3000);

    buzzer(63,400);   //so
    buzzer(56,400);   //la
    buzzer(63,400);   //so
    buzzer(71,400);   //fa
    buzzer(75,400);   //mi
    buzzer(95,400);   //do
    _delay_cycles(3000);

    buzzer(85,400);   //re
    buzzer(63,400);   //so
    buzzer(95,400);   //do
    _delay_cycles(3000);

    buzzer(85,400);   //re
    buzzer(63,400);   //so
    buzzer(95,400);   //do

}
