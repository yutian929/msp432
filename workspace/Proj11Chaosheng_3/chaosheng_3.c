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
int PART = 0;//全局变量表示题目号


int DISTANCE;
int chaosheng_choose=2;
int chaosheng_flag=0;
int chaosheng_dflag=0;
int TRIG=0,ECHO=0;
int UPNUMBER=0;
void delay_ms(unsigned int ms)
{
    unsigned int a;
    while(ms)
    {
        a=1800;
        while(a--);
        ms--;
    }
    return;
}
const Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source->12MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_4,          // SMCLK/4 = 3MHz
        50000,                           // 5000 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

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
//
//void TA2_0_IRQHandler(void)
//{
//    UPNUMBER++;
//    TIMER_A_CMSIS(TIMER_A2_BASE)->R = 0;
//    TIMER_A2->CTL |= TIMER_A_CTL_CLR;
//    TIMER_A2->R=0;
//    TIMER_A2->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
//}

void trig()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
    GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);//上拉
    delay_ms(40);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
    TIMER_A2->CTL =  TIMER_A2->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__STOP;//关闭A2

    __no_operation();
}

void PORT6_IRQHandler()
{
//    TIMER_A_CMSIS(TIMER_A2_BASE)->R = 0;
//    TIMER_A2->CTL |= TIMER_A_CTL_CLR;
//    TIMER_A2->R=0;
//    TRIG=TIMER_A2->R;

    //while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN5) == 1);  //
//        Interrupt_disableInterrupt(INT_TA2_0);
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
         //  UPNUMBER=0;
         P6->IES &=~BIT5;
         P6->IFG =0;
         __no_operation();
    }

}

void main()
{
    HCSR04Init();
    while(1)
    {
        trig();
        __no_operation();
//        while(1)
//        {
//
//        }
    }
}


//float distance(void)
//{
//    int count=0,i=0,sum=0,wait=0;
//    float distance=0;
//    while(i!=2)
//    {
//        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
//        GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);//上拉
//        delay_ms(20);
//        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
//        TIMER_A_CMSIS(TIMER_A2_BASE)->R = 0;
//        while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN5) == 0&&wait<200000)
//        {wait++;}
//        if(wait==10000)
//        {
//            GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
//            GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);//上拉
//            delay_ms(20);
//            GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//下拉
//            TIMER_A_CMSIS(TIMER_A2_BASE)->R = 0;
//            while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN5) == 0);
//        }
////        Interrupt_enableInterrupt(INT_TA2_0);
//        while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN5) == 1);  //
////        Interrupt_disableInterrupt(INT_TA2_0);
//        count=TIMER_A_CMSIS(TIMER_A2_BASE)->R;
//        //v = 340m/s = 34000cm/s = 34000cm/10^6us = 0.034cm/us
//        //s = vt/2 = t*0.034/2 = t*0.017 ?t/58
//        distance=((float)count / 58);
//        i++;
//        sum=sum+distance;
//    }
//        return sum/2-3;
//}


