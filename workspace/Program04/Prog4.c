#include "msp.h"
#include<pid.h>
#include<enc.h>
#include <zyt20221218.h>



/**
 * main.c 自动倒车入库，侧方停车小车
 */
/*****************************************************openmv*****************************************************************/
void openmv_servo_init()//TA1.1->P7.7   TA1.2->P7.6
{
    TIMER_A1->CTL = TIMER_A_CTL_TASSEL_1 | // ACLK
                         TIMER_A_CTL_MC_1|
                         TIMER_A_CTL_CLR; // up mode
    P7->DIR |= (BIT6+BIT7);
    P7->SEL0 |= (BIT6+BIT7);
    P7->SEL1 &=~(BIT6+BIT7);

    TIMER_A1->CCR[0]=655;
    int angle_up=55,angle_down=43;
    //servo_up and _down
    TIMER_A1->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A1->CCR[1]=angle_up;//7.7
    TIMER_A1->CCTL[2] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A1->CCR[2]=angle_down;//7.6
}

//a b c d e f g   x  根据发回的openmv的值修改error值
void EUSCIA2_IRQHandler(void)//for openmv
{
    uint8_t r;
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)  // 接收中断，是否为 1，接收完一位
    {
        r =MAP_UART_receiveData(EUSCI_A2_BASE);//读取接收到的数据
        if (r == 'x')//未知
        {
            error = 100;
        }
        else if (r == 'd')//贴着黑线走
        {
            error = 0;
        }
        else if (r == 'e')//偏右1
        {
            error = -1;
        }
        else if (r == 'f')//偏右2
        {
            error = -2;
        }
        else if (r == 'g')//偏右3
        {
            error = -3;
        }
        else if (r == 'c')//偏左1
        {
            error = 1;
        }
        else if (r == 'b')//偏左2
        {
            error = 2;
        }
        else if (r == 'a')//偏右3
        {
            error = 3;
        }
        else
        {
            error = 0;
        }
    }
}

//用P6.0、P6.1和openmv通讯，检测到横线就返回高电平进入中断
void link_init()
{
    P6->DIR &=~(BIT0+BIT1);
    P6->IE |=(BIT0+BIT1);
    P6->IES &=~(BIT0+BIT1);/*先上升沿*/
    NVIC->ISER[1] |= 1<<8;//port6
    __enable_interrupts();
}

//PORT6中断 P6.0判断第几条横线 （但不确定recline是否会重复计数！！）P6.1判断是否停车,及停车后的起步
void PORT6_IRQHandler()
{
    if(P6->IFG & BIT1)//检测到竖直黑线
    {
        int testangle=63;
        int testspeed=50;
        recline++;
        delay(20);
        if(recline==4)
        {
            motor_stop();
            delay(5);
            if(mode==0)
            {
            //大绕弯，找拐点
            testangle=63;
            testspeed=50;
            turn(testangle);
            rmotor(-1,testspeed);
            lmotor(-1,testspeed);
            while(!(P3->IN&BIT7))//wait for right
            {
                delay(1);
            }
            //找到拐点，右打死，找齐平
            motor_stop();
           // delay(30);
            testangle=91;
            testspeed=40;
            turn(testangle);
           // delay(10);
            lmotor(-1,0);
            rmotor(-1,testspeed);
            while(!(P3->IN&BIT5))//wait for left
            {
                delay(1);
            }
            delay(25);
            //已经齐平，倒库
            testspeed=40;
            turn(mid);
            rmotor(-1,testspeed);
            lmotor(-1,testspeed);
            delay(140);
            motor_stop();
            //倒库完成，等待出库
            delay(160);
            testspeed=50;
            turn(mid);
            rmotor(1,testspeed);
            lmotor(1,testspeed);
            delay(160);
            turn(right);
            delay(130);
            turn(mid);
            motor_stop();
            }
            if(mode==1)
            {
                turn(mid);
                rmotor(-1,testspeed);
                lmotor(-1,testspeed);
                delay(400);
                turn(right);
                rmotor(-1,testspeed);
                lmotor(-1,testspeed);
                delay(200);
                turn(left);
                rmotor(-1,testspeed);
                lmotor(-1,testspeed);
            }

        }

        //buzzer(95,50);
//        //可实现停车后出库（倒库）
//               motor_stop();
//               delay(200);
//               turn(mid);
//               if(mode==0)
//             {
//               rmotor(1,40);
//               lmotor(1,40);
//               delay(120);
//               turn(right);
//               delay(300);
//               turn(mid);
//               motor_stop();
//             }
//               if(mode==1)
//             {
//               turn(left);
//               rmotor(1,45);
//               lmotor(1,45);
//               delay(150);
//               turn(mid);
//               delay(50);
//               turn(right);
//               delay(100);
//               turn(mid);
//               motor_stop();
//             }
    }
    P6->IFG =0;
}

void read_error_value()
{
    openmv_init();//开启openmv uart 通信
    delay(2);//用于中断
    MAP_UART_enableModule(EUSCI_A2_BASE);//关闭通信
}
/************************************************openmv********************************************************************8*/

/************************************************单片机********************************************************************8*/
//选择 P1.1进倒库 P1.4进侧方
void choose()
{
    P1DIR &=~ (BIT1+BIT4);
    P1REN |= BIT1+BIT4;
    P1OUT |= BIT1+BIT4;
    while(1)
    {
        if(!(P1->IN&BIT1))
        {
            delay(5);
            do{}while(!(P1IN & BIT1));   //检测S1松开
           mode=0;
        }
        else if(!(P1->IN&BIT4))
       {
            delay(5);
            do{}while(!(P1IN & BIT4));   //检测S1松开
          mode=1;
       }
    }
}
/************************************************单片机********************************************************************8*/

/**************************************************转动方向*****************************************************************************/
//转弯
void turn(int angle)
{
        //方向舵机 P7.5输出PWM波
        TIMER_A1->CTL = TASSEL_1|MC_1|TIMER_A_CTL_CLR;
        P7->DIR |=  BIT5;
        P7->SEL0 |= BIT5;
        P7->SEL1 &= ~BIT5;
        TIMER_A1->CCR[0] = 255;
        TIMER_A1->CCTL[3] = OUTMOD_7;
        TIMER_A1->CCR[3] = angle;
      //  TIMER_A0->CTL = TASSEL_1+MC_1;
}
/**************************************************转动方向*****************************************************************************/


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
//    openmv_servo_init();
//
//    //test
//    P2->DIR |= (BIT0+BIT1+BIT2);
//    P2->OUT &=~(BIT0+BIT1+BIT2);
//    //daoku ir
//    P3->DIR &=~(BIT5+BIT6+BIT7);
//    P3->REN |= (BIT5+BIT6+BIT7);
//    P3->OUT &=~(BIT5+BIT6+BIT7);
//
//    MG310_init();
//    link_init();
////    mode=0;
//
//    //choose();
//
//    turn(mid);
//    read_error_value();
//
//    rmotor(1,38);
//    lmotor(1,38);//43->35
//
//    while(1)
//    {
//        read_error_value();
//        switch(error)
//        {
//        case 0:P2->OUT |= (BIT0+BIT1+BIT2);turn(mid);break;//白，直行
//        case 1:P2->OUT |= BIT2;P2->OUT &=~(BIT0+BIT1);turn(mid-5);break;
//        case-1:P2->OUT |= BIT2;P2->OUT &=~(BIT0+BIT1);turn(mid+5);break;
//        case 2:P2->OUT |= BIT1;P2->OUT &=~(BIT0+BIT2);turn(mid-10);break;
//        case-2:P2->OUT |= BIT1;P2->OUT &=~(BIT0+BIT2);turn(mid+10);break;
//        case 3:P2->OUT |= BIT0;P2->OUT &=~(BIT1+BIT2);turn(mid-15);break;
//        case-3:P2->OUT |= BIT0;P2->OUT &=~(BIT1+BIT2);turn(mid+15);break;
//        case 100:P2->OUT &=~(BIT0+BIT1+BIT2);turn(mid-3);break;
//        }
//
//
//    }

    TB6612_init();
    while(1)
    {
        motor_control(1,1,250,250);
    }


}































//倒车入库
void daoku(void)
{
    mode=0;
    while(recline<4)
    {
     turn(mid);
     rmotor(1,rspeed);
     lmotor(1,lspeed);
     /*
      * 写好pid后放这里
      */

    }
    /*
     *调试时这里可能要增加往后退一段或前进一段
     */
    turn(right);
    rmotor(-1,40);
    lmotor(-1,40);

}

//侧方停车
void cefang(void)
{
    mode=1;
    while(recline<4)
    {
     turn(mid);
     rmotor(1,rspeed);
     lmotor(1,lspeed);
     /*
      * 写好pid后放这里
      */
    }
    /*
     *调试时这里可能要增加往后退一段或前进一段
     */
    rmotor(-1,40);
    lmotor(-1,40);
    delay(400);
    turn(right);
    rmotor(-1,40);
    lmotor(-1,40);
    delay(200);
    turn(left);
    rmotor(-1,40);
    lmotor(-1,40);
}




//void main(void)
//{
//    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
//    MG310_init();
//    link_init();
//    choose();
//}

