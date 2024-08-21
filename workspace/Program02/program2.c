#include "msp.h"
#include <math.h>
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
#include <cs.c>
#include <Program2.h>

#define clock_source 3    //时钟频率填写到这里（单位MHz）

int PART = 0;//全局变量表示题目号
int SPEED1=57;//60~70%的电->42   跟0.3m/s->SPEED1=57
//int SPEED2=80;
int CYCLE=0;
int warningenable=1;//启用预警
int lukouEnable=1;//遇到路口直接走
int pid_flag=0;   //****************************hs
int CROSS=0;
int DISTANCE;
int chaosheng_choose=2;
int chaosheng_flag=0;
int chaosheng_dflag=0;
int TRIG=0,ECHO=0;
int ddflag=0;
int GUAI=0;
int SysTickFlag=0;
int after_refresh=0;
int J=0,S=0;

void delay_us(unsigned long us){ //uS微秒级延时程序
    SysTick_disableInterrupt();   //禁用时钟中断
    SysTick_setPeriod(us*clock_source); //加载计数值
    SysTick->VAL=0x00;           //清空计数器
    SysTick_enableModule();   //使能系统时钟
    while(SysTick_getValue()&(us*clock_source));
    SysTick->VAL=0x00;        //清空计数器
    SysTick_disableModule();//关闭计数
}

void delay_mss(unsigned long ms){ //mS毫秒级延时程序
    while( ms-- != 0){
        delay_us(1000);
    }
}

void delay_s(unsigned long s){ //S秒级延时程序
    while( s-- != 0){
        delay_mss(1000);
    }
}

//蜂鸣器函数,n是频率，j是持续时间（时间未调试过不知道j取多少）
void Buzzer(int n,int j)
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
        __no_operation();                             // 空操作
        }while (i != 0);                    // 标准的do……while语句，注意别漏掉了括号后面的分号
     }
}

/******************************************************从机蓝牙初始化**************************************************************************/
void cong_bluetooth_init()
{
    //蓝牙时钟初始化使用默认SMCLK->DCO：3M
    //uart initial
    P3->SEL0 |= BIT2 | BIT3; // set 2-UART pin as secondary function
    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
            EUSCI_B_CTLW0_SSEL__SMCLK; // Configure eUSCI clock source for SMCLK
    // Baud Rate calculation
    // 12000000/(16*9600) = 78.125
    // Fractional portion = 0.125
    // User's Guide Table 21-4: UCBRSx = 0x00
    // UCBRFx = int ( (78.125-78)*16) = 2
    EUSCI_A2->BRW = 19; // 12000000/16/9600
    EUSCI_A2->MCTLW = (8 << EUSCI_A_MCTLW_BRF_OFS) |
            EUSCI_A_MCTLW_OS16;
    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG; // Clear eUSCI RX interrupt flag
}
void cong_bluetooth()
{
    char part;
    do
    {
        part=EUSCI_A2->RXBUF;
    }
    while(part=='\0');
    switch(part)
    {
    case '1':    P2OUT &=~(BIT1+BIT2);P2OUT |= BIT0;    PART=1;break;

    case '2':    P2OUT &=~(BIT0+BIT2);P2OUT |= BIT1;    PART=2;break;

    case '3':    P2OUT &=~(BIT1+BIT0);P2OUT |= BIT2;    PART=3;break;

    case '4':    P2OUT |= (BIT0+BIT1+BIT2);             PART=4;break;
    default:break;
    }
}
void parking_Bluetooth()
{
    //蓝牙初始化
    P3->SEL0 |= BIT2 | BIT3; // set 2-UART pin as secondary function
    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
            EUSCI_B_CTLW0_SSEL__SMCLK; // Configure eUSCI clock source for SMCLK
    // Baud Rate calculation
    // 12000000/(16*9600) = 78.125
    // Fractional portion = 0.125
    // User's Guide Table 21-4: UCBRSx = 0x00
    // UCBRFx = int ( (78.125-78)*16) = 2
    EUSCI_A2->BRW = 19; // 12000000/16/9600
    EUSCI_A2->MCTLW = (8 << EUSCI_A_MCTLW_BRF_OFS) |
            EUSCI_A_MCTLW_OS16;
    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG; // Clear eUSCI RX interrupt flag

    EUSCI_A2->IE |= EUSCI_A_IE_RXIE;
    NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);
    __enable_interrupts();
}
//蓝牙接收
void EUSCIA2_IRQHandler(void)
{
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
    {
//        P1->OUT ^= BIT0;
        /***************停车函数*****************/
        if(EUSCI_A2->RXBUF=='E')
        {
            cong_motor_stop();
        }
        else if(EUSCI_A2->RXBUF=='5')
        {
            RightMotor_Forward(0);
            LeftMotor_Forward(0);
            delay_s(4);
//            //继续走
//            RightMotor_Forward(200);
//            LeftMotor_Forward(100);
//            delay(10);
            refresh();
            CYCLE=1;
            warningenable=0;
////            trig();
//
////            reload_shift_reg(); //刷新寄存器
////            ir_check();
////            cong_1_read_ir_values(); //刷新转弯角度
////            calculate_pid(); //PID计算
////            motor_control(); //电机控制
////            cong_1_assisted_control();
////
        }
        else if(EUSCI_A2->RXBUF=='g')
        {
            GUAI=2;//开启WARNING就拐
            SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk;//启动systick,3MHz,开中断
//            __enable_interrupts();
            SysTick->VAL=0x01;
            SysTick->LOAD=(0xB71B0 - 1);//4Hz
            refresh();
        }
        else if(EUSCI_A2->RXBUF=='j')
        {
            J=1;
            P2->OUT &=~(BIT0+BIT1+BIT2);
        }
        else if(EUSCI_A2->RXBUF=='s')
        {
            S=1;
        }
    }
}

void SysTick_Handler(void)
{
    SysTickFlag++;
    if(SysTickFlag==38)
    {
        GUAI=0;//停止WARNING和ddflag置零
        P2->OUT &=~(BIT0+BIT1+BIT2);
        SPEED1=57;//恢复速度
        Buzzer(65,300);
        SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
    }
}
/**********************************************************从机超声测距*******************************************************************/

//void delay_ms(unsigned int ms)//在OLED.C中
//{
//    unsigned int a;
//    while(ms)
//    {
//        a=1800;
//        while(a--);
//        ms--;
//    }
//    return;
//}
const Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source->12MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_4,          // SMCLK/4 = 3MHz
        50000,                           // 5000 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

float distance(void)
{
    int count=0,i=0,sum=0,wait=0;
    float distance=0;
    while(i!=3)
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
        GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);//上拉
        delay_ms(10);
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
        TIMER_A_CMSIS(TIMER_A2_BASE)->R = 0;
        while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN5) == 0&&wait<200000)
        {wait++;}
        if(wait==10000)
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
            GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);//上拉
            delay_ms(20);
            GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
            TIMER_A_CMSIS(TIMER_A2_BASE)->R = 0;
            while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN5) == 0);
        }
//        Interrupt_enableInterrupt(INT_TA2_0);
        while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN5) == 1);  //
//        Interrupt_disableInterrupt(INT_TA2_0);
        count=TIMER_A_CMSIS(TIMER_A2_BASE)->R;
        //v = 340m/s = 34000cm/s = 34000cm/10^6us = 0.034cm/us
        //s = vt/2 = t*0.034/2 = t*0.017 ?t/58
        distance=((float)count / 58);
        i++;
        sum=sum+distance;
    }
        return sum/3-3;
}

void HCSR04Init(void)
{
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN4);//tring
    GPIO_setAsInputPin(GPIO_PORT_P6, GPIO_PIN5);

    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P6, GPIO_PIN5);

//    GPIO_disableInterrupt(GPIO_PORT_P6, GPIO_PIN4);
//    GPIO_disableInterrupt(GPIO_PORT_P6, GPIO_PIN5);
    P6->IE |= BIT5;
    P6->IES &=~ BIT5;
    P6->IFG =0;

//    TIMER_A2->CCTL[0] |=TIMER_A_CCTLN_CCIE;

    NVIC->ISER[1]=1<<8;
    //NVIC->ISER[0]= 1 << ((TA2_0_IRQn) & 31);
    __enable_interrupts();

    Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);
}

void trig()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
    GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);//上拉
    delay_ms(2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
    TIMER_A2->CTL =  TIMER_A2->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__STOP;//关闭A2
    __no_operation();
}

void PORT6_IRQHandler()
{
    P6->IFG =0;
    if((P6->IES&BIT5)==0)//如果上升沿
    {
        TIMER_A_CMSIS(TIMER_A2_BASE)->R = 0;
        TIMER_A2->CTL |= TIMER_A_CTL_CLR;
        TRIG=TIMER_A2->R;
        TIMER_A2->CTL =  TIMER_A2->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__UP;//关闭A2
        P6->IES |= BIT5;
        P6->IFG =0;
        __no_operation();
    }
    else if((P6->IES&BIT5)==BIT5)//如果是下降沿
    {
        //TIMER_A2->CTL =  TIMER_A2->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__STOP;//关闭A2
        ECHO=TIMER_A_CMSIS(TIMER_A2_BASE)->R;
         //v = 340m/s = 34000cm/s = 34000cm/10^6us = 0.034cm/us
         //s = vt/2 = t*0.034/2 = t*0.017 ?t/58
         DISTANCE=fabs(((float)(ECHO-TRIG) / 58));
         DISTANCE+=2;
         //  UPNUMBER=0;
         P6->IES &=~BIT5;
         P6->IFG =0;
         __no_operation();
         //根据DISTANCE改变速度
         P1->OUT ^= BIT0;
//         P1OUT ^= BIT0;


         if(PART==3)
         {
             if(DISTANCE==2)
               {

               }
               else if(DISTANCE<=10)
               {
                   RATE1=SPEED1-35;
                   Buzzer(45,200);
               }
               else if(DISTANCE<=17&&DISTANCE>10)
               {
                   RATE1=SPEED1-18;
               }
               else if(DISTANCE>=17&&DISTANCE<=25)
               {
                   RATE1=SPEED1;
               }
               else if(DISTANCE>25)
               {
                   RATE1=SPEED1+5;
               }
         }
         else
         {
             if(DISTANCE==2)
            {

            }
            else if(DISTANCE<=13)
            {
                RATE1=SPEED1-35;
                Buzzer(45,200);
            }
            else if(DISTANCE<=17&&DISTANCE>13)
            {
                RATE1=SPEED1-18;
            }
            else if(DISTANCE>=17&&DISTANCE<=25)
            {
                RATE1=SPEED1;
            }
            else if(DISTANCE>25)
            {
                RATE1=SPEED1+5;
            }

         }





          chaosheng_flag=0;
    }

}

/****************************************从车循迹****************************************************************************/


float cong_1_read_ir_values() //计算小车偏移量  --->遇到黑线为1
{
    //把停车口和分歧口看作一样的！！！不要WARNING!!!

    //路过停车+分歧口
    if( /*停车*/ ((sensor.ir_mid == 1) && (sensor.ir_left_1 == 1) && (sensor.ir_right_1 == 1))   || /*岔路口*/ ((sensor.ir_mid == 1) && (sensor.ir_right_2 == 1) && (sensor.ir_right_1 == 1))   ||   (sensor.ir_left_2&&sensor.ir_mid)  ||  (sensor.ir_right_2&&sensor.ir_mid)||(sensor.ir_left_3&&sensor.ir_mid)||(sensor.ir_left_3&&sensor.ir_right_2)||(sensor.ir_left_2&&sensor.ir_right_1)||(sensor.ir_left_2&&sensor.ir_right_2)||(sensor.ir_left_3&&sensor.ir_right_2))
     {
         error = 0;
         flag=0;
         dflag=0;
         ddflag=1;

         //
         Buzzer(85,100);
         //

     }
    //正常摇摆
    //左转小
    else if (sensor.ir_left_2 == 1&&sensor.ir_mid==0&&sensor.ir_left_1==0&&sensor.ir_right_1==0)
    {
        error = 2;flag=1;dflag=0;
    }
    //左转小
    else if (sensor.ir_left_1 == 1&&sensor.ir_mid==0&&sensor.ir_right_1==0&&sensor.ir_right_2==0)
    {
        error = 2;flag=0;dflag=0;
    }
    //左大转，只有左三
    else if(sensor.ir_left_3 == 1&&sensor.ir_left_2 == 0&&sensor.ir_mid==0&&sensor.ir_left_1==0&&sensor.ir_right_1==0&&sensor.ir_right_2==0&&sensor.ir_right_3==0)
    {
        error= -2;flag=1;dflag=0;//让其右转
//        error=4;flag=0;dflag=1;
    }
    //右转小
    else if (sensor.ir_right_2 == 1)
    {
        error = -2;flag=1;dflag=0;
    }
    //右转小
    else if (sensor.ir_right_1 == 1 && sensor.ir_right_2 == 1)
    {
        error = -2;flag=0;dflag=0;
    }
    //直行
    else if (((sensor.ir_mid == 1) && (sensor.ir_left_1 == 1)) || ((sensor.ir_mid == 1) && (sensor.ir_right_1 == 1))||((sensor.ir_mid == 1)&&(sensor.ir_left_1 == 0)&&(sensor.ir_right_1 == 0)))
    {
        error = 0;flag=1;dflag=0;
    }
    //右转大
    else if (sensor.ir_right_2 == 1 && sensor.ir_right_3 == 1)
    {
        error = -4;flag=1;dflag=0;
    }
    else
    {
        error=0;flag=0;dflag=0;
    }
    return error;
}
void cong_1_assisted_control()
{
    if(flag==0&&dflag==0)//左右小小转
       {
           delay(0);
       }
       else if(flag==1&&dflag==0)//普通左右转
       {
           delay(1);
       }
       else if(dflag==1)//遇到大转
       {
           delay(70);
       }
      if(ddflag)
       {
//           P1OUT ^= BIT0;
           delay(7);//--------------------------------------------------------------------------------------------------
           error=0;
           calculate_pid(); //PID计算
           motor_control(); //电机控制
           ddflag=0;
       }
}
void cong_motor_stop()
{

    RightMotor_Forward(5);
    LeftMotor_Forward(5);
    RightMotor_Forward(0);
    LeftMotor_Forward(0);
    //蜂鸣器
    Buzzer(95,400);
    delay(40);
    Buzzer(85,400);
    delay(40);
    Buzzer(75,400);
    delay(40);
    OLED_ShowString(0,2,"Arrived at");
    OLED_ShowString(0,4,"the end.");

    __disable_interrupts();

    int stop=0;
    while(1)
    {
        if(stop%2==0)
        {
            RightMotor_Forward(0);
            LeftMotor_Forward(0);
        }
        stop++;
    }
}

void cong_3_read_ir_values() //刷新转弯角度
{
    //把停车口和分歧口看作一样的！！！不要WARNING!!!
    static int jj=30,ss=30,cc=0;

    if(GUAI==2)//GUAI=2
    {
        if(sensor.ir_left_3)//检测到左三
        {
            RATE1=SPEED1;
            LeftMotor_Backward(200);
            RightMotor_Forward(200);
            delay(15);
            HCSR04Init();
            GUAI--;
            P2->OUT |= (BIT0+BIT1+BIT2);//闪光
            cc=17;//--->超车速度
        }
//        else if(WARNING)//检测到WARNING
//        {
//            LeftMotor_Forward(0);
//            RightMotor_Forward(200);
//            delay(140);
//            GUAI--;
//            P2->OUT |= (BIT0+BIT1+BIT2);//闪光
//            cc=20;//--->超车速度
//        }

    }
    if(GUAI==1&&cc>0)//加速超车
    {
        SPEED1+=1;
        cc--;
    }

    if(J>0&&jj>0)
    {
        SPEED1-=1;
        jj--;
    }
    if(S>0&&ss>0)
    {
        SPEED1+=1;
        ss--;
    }


    //路过停车+分歧口
    if( /*停车*/ ((sensor.ir_mid == 1) && (sensor.ir_left_1 == 1) && (sensor.ir_right_1 == 1))   || /*岔路口*/ ((sensor.ir_mid == 1) && (sensor.ir_right_2 == 1) && (sensor.ir_right_1 == 1))   ||   (sensor.ir_left_2&&sensor.ir_mid)  ||  (sensor.ir_right_2&&sensor.ir_mid)||(sensor.ir_left_3&&sensor.ir_mid)||(sensor.ir_left_3&&sensor.ir_right_2)||(sensor.ir_left_2&&sensor.ir_right_1)||(sensor.ir_left_2&&sensor.ir_right_2)||(sensor.ir_left_3&&sensor.ir_right_2))
     {
         error = -2;
         flag=0;
         dflag=0;
         ddflag=1;

         //
         Buzzer(85,100);
         //
         if(GUAI>0)
         {
             ddflag=0;
         }
         if(GUAI==1)//回来的时候小拐
         {
//             error=4;
             LeftMotor_Forward(0);
             RightMotor_Forward(200);
             delay(25);
             GUAI--;
         }
     }
    //正常摇摆
    //左转小
    else if (sensor.ir_left_2 == 1&&sensor.ir_mid==0&&sensor.ir_left_1==0&&sensor.ir_right_1==0)
    {
        error = 2;flag=1;dflag=0;
    }
    //左转小
    else if (sensor.ir_left_1 == 1&&sensor.ir_mid==0&&sensor.ir_right_1==0&&sensor.ir_right_2==0)
    {
        error = 2;flag=0;dflag=0;
    }
    //左大转，只有左三
    else if(sensor.ir_left_3 == 1&&sensor.ir_left_2 == 0&&sensor.ir_mid==0&&sensor.ir_left_1==0&&sensor.ir_right_1==0&&sensor.ir_right_2==0&&sensor.ir_right_3==0)
    {
        error= 0;flag=1;dflag=0;//让其右转
//        error=4;flag=0;dflag=1;
    }
    //右转小
    else if (sensor.ir_right_2 == 1)
    {
        error = -2;flag=1;dflag=0;
    }
    //右转小
    else if (sensor.ir_right_1 == 1 && sensor.ir_right_2 == 1)
    {
        error = -2;flag=0;dflag=0;
    }
    //直行
    else if (((sensor.ir_mid == 0) && (sensor.ir_left_1 == 0)) || ((sensor.ir_mid == 0) && (sensor.ir_right_1 == 0)))
    {
        error = 0;flag=1;dflag=0;
    }
//    else if (sensor.ir_right_3 == 1)   //右转大
//    {
//      error = -4;flag=1;dflag=1;
//    }
    //右转大
    else if (sensor.ir_right_2 == 1 && sensor.ir_right_3 == 1)
    {
        error = -4;flag=1;dflag=0;
    }
    else
    {
        error=0;
    }



    return error;

}

void cong_3_assisted_control()
{
    if(flag==0&&dflag==0)//左右小小转
       {
           delay(0);
       }
       else if(flag==1&&dflag==0)//普通左右转
       {
           delay(5);
       }
       else if(dflag==1)//遇到大转
       {
           delay(50);
       }
      if(ddflag)
       {
//           P1OUT ^= BIT0;
           delay(10);
           error=0;
           calculate_pid(); //PID计算
           motor_control(); //电机控制
           ddflag=0;
       }
}

void refresh()    //*******************************HS参数初始化
{
    //PID算法参数
    Kp = 15, Ki = 0.01, Kd = 8;

    error = 0, P = 0, I = 0, D = 0, PID_value = 0;
    previous_error = 0, previous_I = 0;
    left_motor_speed = 0;
    right_motor_speed = 0;
    motor_flag = 0;
    left_pulse = 0;
    right_pulse = 0;
    a=0,b=0,c=0;
    left_velocity = 0;
    right_velocity = 0;
    left_update = 0;
    right_update = 0;
    after_refresh=1;
}





/*********************************************main*****************************************************************************/



void main(void)
        {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

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
    //OLED初始化
    init();
    OLED_Init();
    //warning
    P1->DIR &=~BIT6;
    P1->REN |= BIT6;
    P1->OUT &=~BIT6;

    HCSR04Init();  //超声初始化
    while(1)
    {
        trig();
        __no_operation();
    }

    //开始时等待蓝牙信号
    cong_bluetooth_init();
    while(!PART)
    {
        cong_bluetooth();
    }


//    PART=3;
//    GUAI=2;

    //PART已经改变
    OLED_Clear();
    OLED_ShowString(2,0,"Track");
    OLED_ShowNum(70, 0, PART, 4, 16);


//    cong_bluetooth_init();

    if((PART==1)||(PART==2)||(PART==4))//赛道1,2,4
    {
        RATE1=SPEED1;RATE2=100;//车速调整
        HCSR04Init();  //超声初始化
        parking_Bluetooth();//停车蓝牙
        while((PART==1)||(PART==2)||(PART==4))
        {
          reload_shift_reg(); //刷新寄存器
//          ir_check();
          cong_1_read_ir_values(); //刷新转弯角度
          calculate_pid(); //PID计算
          motor_control(); //电机控制
          cong_1_assisted_control();

          pid_flag++;     /*************************HS*/
          if(pid_flag>1000)
          {
              refresh();
              pid_flag=0;
//              P1->OUT ^= BIT0;
          }

          chaosheng_flag++;
          if(chaosheng_flag%20==0)
          {
              trig();
          }
        }
    }
    else if(PART==3)
    {
        RATE1=SPEED1;RATE2=100;//车速调成整
        HCSR04Init();  //超声初始化
        parking_Bluetooth();//停车蓝牙
        PIAN1=10;PIAN2=10;//改变偏移率
        while(PART==3)
        {
          reload_shift_reg(); //刷新寄存器
//          ir_check();
          cong_3_read_ir_values(); //刷新转弯角度
          calculate_pid(); //PID计算
          motor_control(); //电机控制
          cong_3_assisted_control();

          pid_flag++;     /*************************HS*/
          if(pid_flag>1000)
          {
              refresh();
              pid_flag=0;
          }

          chaosheng_flag++;
          if(chaosheng_flag%20==0)////////
          {
              trig();
          }
        }
    }
    else if(PART==0)
    {
        RightMotor_Forward(0);
        LeftMotor_Forward(0);
    }

}

