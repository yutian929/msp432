#include "msp.h"


/**
 * main.c
 * 2022/8/13 Zaki 张雨田
 * 用的是两脚的无源蜂鸣器
 * 持续不同时间的不停的开关IO口以达到频率和节拍
 * 用的就是系统默认的时钟，不需要修改
 * 来源网络，设置为P2.5
 */

//蜂鸣器函数,n是频率，j是持续时间（时间未调试过不知道j取多少）
void buzzer(int n,int j)
{
    P2DIR |= BIT5;
    ///延时每个音调的时长
    for ( ;j>0;j--)
    {
        volatile unsigned int i;
        P2OUT ^= BIT5;
        i = n;
        do
        {
        i--;                                // 自减操作
        __no_operation();                   // 空操作
        }while (i != 0);                    // 标准的do……while语句，注意别漏掉了括号后面的分号
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
    _delay_cycles(3000); //每个完整的拍后接一个延时

    buzzer(95,400);   //do
    buzzer(85,400);   //re
    buzzer(75,400);   //mi
    buzzer(95,400);   //do
    _delay_cycles(3000); //每个完整的拍后接一个延时

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
