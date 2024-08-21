#include "msp.h"
#include "xunji.h"
#include <math.h>
#include <driverlib.h>
#include "oled.h"
/**
 * main.c
 */

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
//    CS->KEY=0x695A;
//    CS->CTL1 =CS->CTL1 | CS_CTL1_SELM__HFXTCLK ;
//    CS->CTL2 |=
    //initial
    P4->DIR |= BIT6;
    P4->OUT |= BIT6;
    P1->DIR &=~BIT7;
    P1->REN |= BIT7;
    P1->DIR |= BIT5;
    //LED 2.0/1/2
    P2->DIR |= (BIT0+BIT1+BIT2);
    P2->OUT &=~(BIT0+BIT1+BIT2);
    //LED 1.0
    P1->DIR |=BIT0;
    P1->OUT&=~BIT0;
    //OLED
    init();
    OLED_Init();

    choose();

    OLED_Clear();
    OLED_ShowString(2,0,"WARD");
    if(count !=0)
    OLED_ShowNum(60, 0, count, 4, 16);
    else
    OLED_ShowNum(60, 0, rcount, 4, 16);

    OLED_ShowString(2,2,"  Current speed:");
    OLED_ShowString(2,4," MID!  ");
    start();
    while(1)
    {
      reload_shift_reg(); //刷新寄存器
      //ir_check();
      if(flag_back==0)
        {
            read_ir_values(); //刷新转弯角度
        }
        else if(flag_back==1)
        {
            rread_ir_values();
        }
      calculate_pid(); //PID计算
      motor_control(); //电机控制
      motor_stop();
      assisted_control();
      switcher_check();
    }
}

