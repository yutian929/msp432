#include "msp.h"
#include <zyt20221218.h>

/**
 * main.c 2022/12/19 Zaki
 * 改用新驱动TB6612驱动电机
 */

char wait_for_key()
{
    while(1)
    {
        int i;
        if(P1->IN&(BIT1+BIT4)==(BIT1+BIT4))
        {
            //未按下
        }
        else
        {
            for(i=50000;i>=0;i--);
            if((P1->IN&BIT1)!=BIT1)//P11
            {
                return '1';
            }
            else if((P1->IN&BIT4)!=BIT4)//P14
            {
                return '4';
            }
            else
            {
                //消抖
            }
        }
    }
}
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	led_init();
	key_init();
	TB6612_init();
	int apwm=10;
	int a=0;
	int cnt=0;
	while(1)
	{
	    char btn = wait_for_key();
	    switch(btn)
	    {
	    case '1':apwm+=10;if(apwm>=255){apwm=0;}P2->OUT ^=BIT0;break;
	    case '4':cnt+=1;P2->OUT ^=BIT1;
	        if(cnt%4==0){a=0;}
	        else if(cnt%4==1){a=1;}
	        else if(cnt%4==2){a=2;}
	        else {a=3;}
	        break;
	    default:break;
	    }
	    motor_control(a,0,apwm,0);
	}
}
