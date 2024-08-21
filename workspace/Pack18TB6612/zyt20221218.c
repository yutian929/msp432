/*
 * zyt.c
 *
 *  Created on: 2022年9月1日
 *      Author: Zaki
 */
#include "msp.h"
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
#include <zyt20221218.h>


//KEY
void key_init()
{
    //initia,初始高电平，按下为低电平
    P1->DIR &=~(BIT1+BIT4);
    P1->REN |= (BIT1+BIT4);
}

//LED
void led_init()/*2.0,2.1,2.2,1.0的初始化*/
{
    P1->DIR |= BIT0;
    P2->DIR |= (BIT0|BIT1|BIT2);
    P1->OUT &=~BIT0;
    P2->OUT &=~(BIT0|BIT1|BIT2);
}

//LPM
void go_LPM()/*进入睡眠状态,sleepdeep=1*/
{
    //设置睡眠深度为1
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
    //终端完成exit后继续睡
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    //立即睡
    __DSB();
    //进入LPM4
    __sleep();
}

//delay 延时
void delay_us(unsigned long us){ //uS微秒级延时程序
    SysTick_disableInterrupt();   //禁用时钟中断
    SysTick_setPeriod(us*clock_source); //加载计数值
    SysTick->VAL=0x00;           //清空计数器
    SysTick_enableModule();   //使能系统时钟
    while(SysTick_getValue()&(us*clock_source));
    SysTick->VAL=0x00;        //清空计数器
    SysTick_disableModule();//关闭计数
}

void time_count_start(int i)/*间隔i*0.25s进入一次systick的中断,中断函数见zyt.c*/
{
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk;
    // Set SysTick period = 0xB71B0,4Hz/3MHz MCLK
    SysTick->LOAD = (0xB71B0 - 1)*i;
}
//void SysTick_Handler(void)
//{
//    P1->OUT ^= BIT0;
////    SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;//关闭
//}

//Buzzer
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
}

//Photoresistance
void Photoresistance_init_suspend()/*查询模式*/
{
    //查数据手册得P5.4口SEL设置
    P5->SEL1 |= BIT4;
    P5->SEL0 |= BIT4;
    //Sampling time, S&H=16, ADC14 on
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    //Use sampling timer,12-bit conversion results
    ADC14->CTL1 = ADC14_CTL1_RES_2;
    //A1 ADC input select;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1;
}
int light_value_suspend()/*返回光敏电阻值,越小,光强越强*/
{
    int mem0;
    //开启转换
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    //等待转换完成
    while((ADC14->IFGR0&ADC14_IFGR0_IFG0)==0);
    mem0=ADC14->MEM[0];
    ADC14->CTL0 &=~ADC14_CTL0_ENC;
    return mem0;
}
//int LIGHTVAL;/*中断模式*/
void Photoresistance_init_interrupt()
{
    //查数据手册得P5.4口SEL设置
    P5->SEL1 |= BIT4;
    P5->SEL0 |= BIT4;
    //Sampling time, S&H=16, ADC14 on
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    //Use sampling timer,12-bit conversion results
    ADC14->CTL1 = ADC14_CTL1_RES_2;
    //A1 ADC input select;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1;
    //通道0使能
    ADC14->IER0 = ADC14_IER0_IE0;
    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
    __enable_interrupts();
    //GPIO Setup
    P1->OUT &= ~BIT0; // Clear LED to start
    P1->DIR |= BIT0; // Set P1.0/LED to output
}
void light_value_start()/*start一次,改变一次全局变量*/
{
    //开启转换
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
}
void ADC14_IRQHandler(void)
{
    //转换完成
    LIGHTVAL=ADC14->MEM[0];
    ADC14->CTL0 &=~ADC14_CTL0_ENC;
}

//SoundIntensity
void soundIntensit_init_interrupt()/*3.7口的中断*/
{
    //采用P3.7口作为输入检测
    P3->DIR &=~BIT7;
    P3->REN |= BIT7;
    P3->OUT &=~BIT7;
    //用中断来写，主程序可以sleep
    P3->IE |= BIT7;
    P3->IES|= BIT7;/*平时为高电平，所以采用检测下降沿*/
    NVIC->ISER[1] = 1 << ((PORT3_IRQn) & 31);
    __enable_interrupts();
}
//void PORT3_IRQHandler()//for soundIntensit
//{
//    if((P3->IFG)|BIT7==BIT7)
//    {
//
//    }
//    P3->IFG=0;
//}

//Bluetooth
void bluetooth_init()/*3.2RX,3.3TX,接收到消息后进入EUSCIA2的中断,见zyt.c*/
{
    //蓝牙时钟初始化使用默认SMCLK->DCO：3M
    //uart initial
    P3->SEL0 |= BIT2 | BIT3; // set 2-UART pin as secondary function
    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST/*Remain eUSCI in reset*/ | EUSCI_B_CTLW0_SSEL__SMCLK;  /*Configure eUSCI clock source for SMCLK*/
    //波特率9600
    EUSCI_A2->BRW = 19;
    EUSCI_A2->MCTLW = (8 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;
    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // start eUSCI
    EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG; // Clear eUSCI RX interrupt flag
    //RX_IE
    EUSCI_A2->IE |= EUSCI_A_IE_RXIE;
    NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);
    __enable_interrupts();
}
//void EUSCIA2_IRQHandler(void)//for bluetooth
//{
//    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
//    {
//        if(EUSCI_A2->RXBUF=='0')
//        {
//
//        }
//    }
//}

//IrDistance
void Ir_distance_init()/*红外测距,返回电压模拟值，红黑黄接vcc,gnd,5.5*/
{
    //查数据手册得P5.5对应A0
    P5->SEL1 |= BIT5;
    P5->SEL0 |= BIT5;
    //Sampling time, S&H=16, ADC14 on
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    //Use sampling timer,12-bit conversion results
    ADC14->CTL1 = ADC14_CTL1_RES_2;
    //A1 ADC input select;
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_0;
}

int Ir_distance()
{
    int temp=0;
    //开启转换
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    //等待转换完成
    while((ADC14->IFGR0&ADC14_IFGR0_IFG0)==0);
    temp=ADC14->MEM[0];
    ADC14->CTL0 &=~ADC14_CTL0_ENC;
    //粗略计算,自己看着办吧,不想写函数拟合了
    return temp;
    //30cm->1250~1350
    //20cm->1550~1650
    //15cm->2450~2650
    //10cm->2750~2850
    //5cm->2850~2950
    //<5cm->adc>2850
}

//openmv
const eUSCI_UART_Config uartConfig2 =/*用EUSCI2作为uart，即3.2RX,3.3TX*/
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source->3MHz->9600 BRD
     19,                                      // BRDIV
     8,                                       // UCxBRF
     0,                                       // UCxBRS
     EUSCI_A_UART_NO_PARITY,                  // No Parity
     EUSCI_A_UART_LSB_FIRST,                  // MSB First
     EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
     EUSCI_A_UART_MODE,                       // UART mode
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
};
void openmv_init()/*初始化openmv,记得在openmv的main中也要初始化，配置相同比特率9600,之后接收到八位一字节信息后就会进入中断,见zyt.c*/
{
    //uart时钟初始化使用默认SMCLK->DCO：3M
    //uart initial
    P3->SEL0 |= (BIT2 | BIT3);// set 3-UART pin as secondary function
    P3->SEL1 &=~(BIT2 | BIT3);

    //![Simple UART Example]z
    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig2);
    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A2_BASE);
    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
    MAP_Interrupt_enableMaster();
}
//void EUSCIA2_IRQHandler(void)//for openmv
//{
//    uint8_t r;
//    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);
//    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)  // 接收中断，是否为 1，接收完一位
//    {
//        r =MAP_UART_receiveData(EUSCI_A2_BASE);    // 读取接收到的数据
//        if(r=='0')
//        {
//
//        }
//    }
//}

//KeyArray
void KeyArray_init()
{
    //KeyArray initial
    P6->DIR |= (BIT4+BIT5+BIT6+BIT7);
    P6->OUT &=~(BIT4+BIT5+BIT6+BIT7);
    P2->DIR &=~(BIT3+BIT5+BIT6+BIT7);
    P2->REN |= (BIT3+BIT5+BIT6+BIT7);
    P2->OUT &=~(BIT3+BIT5+BIT6+BIT7);//都先置为下拉低电平
}
char Key()/*没键按下,默认返回'\0'*/
{
    char KEYVALUE='\0';
    int i=0;
    //输出端依次置为高电平
    //逐行扫描:第1行
    P6->OUT |= BIT4;
    if(P2->IN&(BIT3+BIT5+BIT6+BIT7))
    {
        for(i=200;i>0;i--);//消抖
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
    //逐行扫描:第2行
    P6->OUT |= BIT5;
    if(P2->IN&(BIT3+BIT5+BIT6+BIT7))
    {
        for(i=200;i>0;i--);//消抖
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
    //逐行扫描:第3行
    P6->OUT |= BIT6;
    if(P2->IN&(BIT3+BIT5+BIT6+BIT7))
    {
        for(i=200;i>0;i--);//消抖
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
    //逐行扫描:第4行
    P6->OUT |= BIT7;
    if(P2->IN&(BIT3+BIT5+BIT6+BIT7))
    {
        for(i=200;i>0;i--);//消抖
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

//SG90
void servo_init()//TIMERA1输出PWM波控制，信号口P2.4
{   //P2.4/PM_TA0.1
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_1 | // ACLK
                         TIMER_A_CTL_MC_1|
                         TIMER_A_CTL_CLR; // up mode
    P2->DIR |= BIT4; // P2.6-7 option selection TA0.3-4
    P2->SEL0 |= BIT4;
    P2->SEL1 &=~(BIT4);
}
void servo_angle(int ang)//angle from 12~13~44~76~87
{
    // Configure GPIO
    TIMER_A0->CCR[0]=655;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCR[1]=ang;
}
//HCSR04超声
void HCSR04_Init_suspend()/*查询模式,6.4Trig 6.5Echo*/
{
    //TIMER_A2 setting
    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK/*选择SMCLK,默认3MHz*/
            | TIMER_A_CTL_MC__UP;/*增计数模式*/
    TIMER_A2->CCR[0]=50000;
//    TIMER_A2->CCTL[0]= TIMER_A_CCTLN_CCIE;/*若测距较远则需开启CCR[0]中断*/
    //GPIO setting
    P6->DIR |= BIT4;/*P6.4做Trig口*/
    P6->OUT &=~BIT4;
    P6->DIR &=~BIT5;/*P6.5做Echo口*/
    P6->REN |= BIT5;
    P6->OUT &=~BIT5;
}
float distance_suspend()/*返回距离，但一定距离长之后失效*/
{
    int TRIG,ECHO;
    int count=0;
    float DISTANCE,sum=0;
    //测5次取平均
    while(count<5)
    {
        count++;
        //先给Trig一段高电平
        P6->OUT &=~BIT4;
        P6->OUT |= BIT4;
        int i;
        for(i=1000;i>0;i--);
        P6->OUT &=~BIT4;
        //TRIG为计数开始值
//        TIMER_A2->CTL |= TIMER_A_CTL_CLR;
        TIMER_A2->R=0;
        TRIG=TIMER_A2->R;
        //检测Echo什么时候结束高电平
        while((P6->IN & BIT5)==0);
        while((P6->IN & BIT5)==BIT5);
        ECHO=TIMER_A2->R;
        //计算
        DISTANCE=(float)(ECHO-TRIG)*0.0057-40;
        sum+=DISTANCE;
    }
    return sum/5;
}

float DISTANCE;
int UPCOUNT=0;

void HCSR04_Init_interrupt(void)/*中断模式,可有效避免主程序的阻塞,6.4Trig 6.5Echo,且可测长距离,但也存在小问题*/
{
    P6->DIR |= BIT4;/*TRIG*/
    P6->OUT &=~BIT4;
    P6->DIR &=~BIT5;/*ECHO*/
    P6->REN |= BIT5;
    P6->OUT &=~BIT5;
    //P6.5接受口中断使能
    P6->IE |= BIT5;
    P6->IES &=~ BIT5;
    P6->IFG =0;
    NVIC->ISER[1]=1<<8;
    __enable_interrupts();
    //TIMER_A2
    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK/*选择SMCLK,默认3MHz*/
            | TIMER_A_CTL_MC__UP;/*增计数模式*/
    TIMER_A2->CCR[0]=50000;
    TIMER_A2->CCTL[0] |= TIMER_A_CCTLN_CCIE;
}

void trig()/*产生一段高电平,之后便会进入中断,见zyt.c*/
{
    //先给Trig一段高电平
    P6->OUT &=~BIT4;
    P6->OUT |= BIT4;
    int i;
    for(i=1000;i>0;i--);
    P6->OUT &=~BIT4;
    //关闭TA2
    TIMER_A2->CTL =  TIMER_A2->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__STOP;
    __no_operation();
}
//void TA2_0_IRQHandler()//for hcsr04 long distance
//{
//    TIMER_A2->CCTL[0] &=~TIMER_A_CCTLN_CCIFG;
//    UPCOUNT++;
//}
//void PORT6_IRQHandler()//for hcsr04
//{
//    static int TRIG,ECHO;
//    P6->IFG =0;
//    //如果上升沿
//    if((P6->IES&BIT5)==0)
//    {
//        //记录开始值
//        TIMER_A2->CTL |= TIMER_A_CTL_CLR;
//        TIMER_A2->R=0;
//        TRIG=TIMER_A2->R;
//        //开启A2
//        TIMER_A2->CTL =  TIMER_A2->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__UP;
//        //等待下降沿
//        P6->IES |= BIT5;
//        P6->IFG =0;
//        __no_operation();
//    }
//    //如果下降沿
//    else if((P6->IES&BIT5)==BIT5)
//    {
//        //记录终止值
//        ECHO=TIMER_A2->R;
//        //计算
//        DISTANCE=(float)(ECHO-TRIG)*0.0057+UPCOUNT*283.3;
//        UPCOUNT=0;
//        //改为上升沿
//        P6->IES &=~BIT5;
//        P6->IFG =0;
//        __no_operation();
//    }
//
//}

//OLED
//char PICTURE[64][128];
//void OLED_Print()//自己写的一个函数，根据数组画图,需要引用oled文件，并在main.c中写一遍
//{
//    int x=0,y=0;
//    for(x=0;x<128;x++)//逐列打印
//    {
//        for(y=0;y<8;y++)//每列八个操作点
//        {
//            unsigned char temp=0,index=0;
//            OLED_Set_Pos(x,y);
//            for(index=0;index<8;index++)//每个操作点包含8个像素点
//            {
//                if(PICTURE[y*8+index][x]=='1')
//                {
//                    temp |= (1<<index);//移位操作，共8位，最终temp属于0~127
//                }
//            }
//            OLED_WR_Byte(temp,1);//temp的数值决定8个像素点的亮法(共2的8次方种)
//        }
//    }
//}

//TB6612
void TB6612_init()
{
    P2->SEL0 |= BIT4;//PWMA
    P2->SEL1 &= ~BIT4;
    P2->DIR |=  BIT4;
    TIMER_A0->CCR[0] = 255;
    TIMER_A0->CCTL[1] = OUTMOD_7;
    TIMER_A0->CCR[1] = 0;
    TIMER_A0->CTL = TASSEL_1+MC_1;
    P2->SEL0 |= BIT6;//PWMB
    P2->SEL1 &= ~BIT6;
    P2->DIR |=  BIT6;
    TIMER_A0->CCR[0] = 255;
    TIMER_A0->CCTL[3] = OUTMOD_7;
    TIMER_A0->CCR[3] = 0;
    TIMER_A0->CTL = TASSEL_1+MC_1;
    P8->DIR |= (BIT2+BIT4+BIT5+BIT6);//P82,P84,P85,P86
    P8->OUT &=~(BIT2+BIT4+BIT5+BIT6);
}
void motor_control(int a,int b,int apwm,int bpwm)
{
    //对于A
    switch(a)
    {
    case 0:P8->OUT &=~(BIT2+BIT4);break;//停止
    case 3:P8->OUT |= (BIT2+BIT4);break;//制动
    case 1:P8->OUT |= BIT2;P8->OUT &=~BIT4;//前进
    P2->SEL0 |= BIT4;//PWMA
    P2->SEL1 &= ~BIT4;
    P2->DIR |=  BIT4;
    TIMER_A0->CCR[0] = 255;
    TIMER_A0->CCTL[1] = OUTMOD_7;
    TIMER_A0->CCR[1] = apwm;
    TIMER_A0->CTL = TASSEL_1+MC_1;
    break;
    case 2:P8->OUT |= BIT4;P8->OUT &=~BIT2;//后退
    P2->SEL0 |= BIT4;//PWMA
    P2->SEL1 &= ~BIT4;
    P2->DIR |=  BIT4;
    TIMER_A0->CCR[0] = 255;
    TIMER_A0->CCTL[1] = OUTMOD_7;
    TIMER_A0->CCR[1] = apwm;
    TIMER_A0->CTL = TASSEL_1+MC_1;
    break;
    default:P8->OUT &=~(BIT2+BIT4);break;
    }
    //对于B
    switch(b)
    {
    case 0:P8->OUT &=~(BIT5+BIT6);break;//停止
    case 3:P8->OUT |= (BIT5+BIT6);break;//制动
    case 1:P8->OUT |= BIT5;P8->OUT &=~BIT6;//前进
    P2->SEL0 |= BIT6;//PWMA
    P2->SEL1 &= ~BIT6;
    P2->DIR |=  BIT6;
    TIMER_A0->CCR[0] = 255;
    TIMER_A0->CCTL[1] = OUTMOD_7;
    TIMER_A0->CCR[1] = bpwm;
    TIMER_A0->CTL = TASSEL_1+MC_1;
    break;
    case 2:P8->OUT |= BIT6;P8->OUT &=~BIT5;//后退
    P2->SEL0 |= BIT6;//PWMA
    P2->SEL1 &= ~BIT6;
    P2->DIR |=  BIT6;
    TIMER_A0->CCR[0] = 255;
    TIMER_A0->CCTL[1] = OUTMOD_7;
    TIMER_A0->CCR[1] = bpwm;
    TIMER_A0->CTL = TASSEL_1+MC_1;
    break;
    default:P8->OUT &=~(BIT5+BIT6);break;
    }






}

