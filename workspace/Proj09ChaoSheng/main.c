int PART=2;

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
#include "xunji.h"

#include <cs.c>


const Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/1 = 3MHz
        50000,                           // 5000 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};
void delay_ms(int DELAY)
{
    int i;
    for(i=0;i<DELAY*500;i++);
}
void HCSR04Init(void)
{

    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN4);//tring
    GPIO_setAsInputPin(GPIO_PORT_P6, GPIO_PIN5);

    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P6, GPIO_PIN5);

    GPIO_disableInterrupt(GPIO_PORT_P6, GPIO_PIN4);
    GPIO_disableInterrupt(GPIO_PORT_P6, GPIO_PIN5);

    Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);

    //Interrupt_enableSleepOnIsrExit();
//    Interrupt_enableInterrupt(INT_TA2_0);
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);

//    Interrupt_enableMaster();

}
float Distance(void)
{
    int count=0,i=0,sum=0;
    float distance=0;
    while(i!=5)
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
        GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);//上拉
        delay_ms(20);
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
        TIMER_A_CMSIS(TIMER_A2_BASE)->R = 0;
        while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN5) == 0);  //
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
        return sum/5;
}

int Distance012[3]={0};
int panduan=0;
int most=180;
void you_2()
{
    LeftMotor_Forward(250);
    RightMotor_Backward(250);
    delay(75);
    HCSR04Init();
}
void zuo_2()
{
    LeftMotor_Backward(250);
    RightMotor_Forward(250);
    delay(80);
}
void ting_2()
{
    LeftMotor_Forward(0);
    RightMotor_Forward(0);
    delay(10);
}

void part2_init()
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);//led灯
    CS_initClockSignal(CS_MCLK,CS_HFXTCLK_SELECT,CS_CLOCK_DIVIDER_1);//系统时钟初始化（系统主时钟）
    CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);//系统时钟初始化（子系统时钟）
    //UART_initModule(EUSCI_A0_BASE,&upConfig); //串口1 初始化
    HCSR04Init();
}
void part2()
{
    while(PART==2)//第一种判断情况下，超长距离要舍掉
    {
        //先停
        ting_2();
        //测中间
        Distance012[0]= Distance();
        while(Distance012[0]>30)
        {
            Distance012[0]= Distance();
            LeftMotor_Forward(250);
            RightMotor_Forward(250);
            delay(15);
//                reload_shift_reg(); //刷新寄存器
//                switcher_check();
        }
        //先停
        ting_2();
        //测左边
        zuo_2();
        ting_2();
        Distance012[1]= Distance();
        //测右边
        you_2();you_2();
        ting_2();
        Distance012[2]= Distance();
        //判断
        if(Distance012[2]>most&&Distance012[1]<most&&panduan==0)//距离超长
        {
            zuo_2();zuo_2();panduan=1;
        }
        else if(Distance012[1]>most&&Distance012[2]<most&&panduan==1)//第二次距离超长
        {
            zuo_2();zuo_2();panduan=2;
            LeftMotor_Forward(150);
            RightMotor_Forward(250);
            delay(100);//直走一段时间
            PART=3;
        }
        else if(Distance012[1]<most&&Distance012[2]<most&&Distance012[2]>=Distance012[1])
        {
        }
        else if(Distance012[1]<most&&Distance012[2]<most&&Distance012[2]<Distance012[1])
        {
            zuo_2();zuo_2();
        }
    }
}




int test;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    part2_init();
//    part2();
    HCSR04Init();
    while(1)
    {
        test=Distance();
        __no_operation();
    }

}

//
//void main()
//{
//    while(1)
//    {
//        ting_2();
//        delay(100);
//        zuo_2();
//        ting_2();
//        delay(100);
//        you_2();
//    }
//
//}



