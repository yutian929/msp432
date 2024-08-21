//#include "msp.h"
//#include <driverlib.h>
//#include "adc14.c"
//#include "aes256.c"
//#include "comp_e.c"
//#include "cpu.c"
//#include "crc32.c"
//#include "dma.c"
//#include "eusci.h"
//#include "flash.c"
//#include "fpu.c"
//#include "gpio.c"
//#include "i2c.c"
//#include "interrupt.c"
//#include "mpu.c"
//#include "pcm.c"
//#include "pmap.c"
//#include "pss.c"
//#include "ref_a.c"
//#include "reset.c"
//#include "rtc_c.c"
//#include "spi.c"
//#include "sysctl.c"
//#include "systick.c"
//#include "timer32.c"
//#include "timer_a.c"
//#include "uart.c"
//#include "wdt_a.c"
//#include <cs.c>
//
///**
// * main.c
// */
//Timer_A_PWMConfig TIM0_PwmConfig =
//{
//        TIMER_A_CLOCKSOURCE_SMCLK,
//        TIMER_A_CLOCKSOURCE_DIVIDER_4,//12MHZ / 4 = 3MHz
//        60000,//3000 000 / 50 = 60000
//        TIMER_A_CAPTURECOMPARE_REGISTER_1,
//        TIMER_A_OUTPUTMODE_RESET_SET,
//        1500//舵机初始化角度
//};
//void TimerA0_Init(void)
//{
//    /* Configuring GPIO2.4 as peripheral output for PWM  */
//    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,
//                                                    GPIO_PIN4,
//                                                    GPIO_PRIMARY_MODULE_FUNCTION);
//    /* Configuring Timer_A to have a period of approximately 500ms and
//     * an initial duty cycle of 10% of that (3200 ticks)  */
//    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &TIM0_PwmConfig);
//}
///**
// *************************************************************************
// * @brief: Servo_TurnAngle
// * @param: uch_angle -- 舵机旋转角度
// * @return: void
// * @function: 舵机旋转角度
// * @author:
// * @version: V1.0
// * @note: 舵机需要50hz 3000000 / 60000= 50hz 60000
//         0度 0.5/20=0.025  60000*0.025= 1500
//         180度 2.5/20=0.125  60000*0.125= 7500
// *************************************************************************
//**/
//void Servo_TurnAngle(int uch_angle)
//{
//    float f_cycle;
//    f_cycle = (7500.0f - 1500.0f) / 180.0f * uch_angle + 1500.0f;//定时器计数与角度
//    TIM0_PwmConfig.dutyCycle = (int)f_cycle;//新的占空比
//    //TIM2_PwmConfig.dutyCycle = (int)f_cycle;//新的占空比
//    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &TIM0_PwmConfig);
//}
//
//
//void main(void)
//{
//	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
//	/* Configuring GPIO2.4 as peripheral output for PWM  */
//	    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,
//	                                                    GPIO_PIN4,
//	                                                    GPIO_PRIMARY_MODULE_FUNCTION);
//	while(1)
//	{
//	    /* Configuring Timer_A to have a period of approximately 500ms and
//	     * an initial duty cycle of 10% of that (3200 ticks)  */
//	    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &TIM0_PwmConfig);
////	    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &TIM0_PwmConfig);
//	}
//
//
//
//
//
//
//}
//
//
//
//
////void servo_init()
////{
////    //P7.4/PM_TA1.4/C0.5
////    TIMER_A1->CTL=TIMER_A1->CTL&(~TIMER_A_CTL_SSEL_MASK) | TIMER_A_CTL_SSEL__ACLK;
////    TIMER_A1->CTL=TIMER_A1->CTL&(~TIMER_A_CTL_MC_MASK) | TIMER_A_CTL_MC__UP | TACLR;//ACLK, UP
////}
////
////void servo(int per)
////{
////    //7.4
////    P7->DIR |= BIT4;
////    P6->SEL0 |= BIT4;
////    P6->SEL1 &=~BIT4;
////
////    TIMER_A1->CCR[0]=255;
////    TIMER_A1->CCR[3]=255*per/100;
////    TIMER_A1->CCTL[3]=OUTMOD_7;
////}
////Timer_A_PWMConfig _servo = {
//////                              uint_fast16_t clockSource;
//////                              uint_fast16_t clockSourceDivider;
//////                              uint_fast16_t timerPeriod;
//////                              uint_fast16_t compareRegister;
//////                              uint_fast16_t compareOutputMode;
//////                              uint_fast16_t dutyCycle;
////                            TIMER_A_CLOCKSOURCE_ACLK,
////                            TIMER_A_CLOCKSOURCE_DIVIDER_1,
////                            255,
////                            TIMER_A_CAPTURECOMPARE_REGISTER_4,
////                            TIMER_A_OUTPUTMODE_SET_RESET,
////                            0.5
////};







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
#include "wdt_a.c"

/* Timer_A PWM Configuration Parameter */
Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_24,//12M / 24 = 500K  1000clk = 0.002s 10000 = 0.02s 0.0015s = 750
        10000,//all duty = 20ms
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        750//now duty
};


//TA0 init
void Servo_Timer_Init(void)
{
    //配置 TA0 channel_1
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4,
            GPIO_PRIMARY_MODULE_FUNCTION);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
}

//TA0 CH1 控制角度
void Servo1_Angle(uint16_t x)
{
    uint32_t ta0ch1val = 0;
    //0.0015s = 750
    //0.0005s = 250
    //0.0025s = 1250
    //0.0005s ~ 0.0025s 250 ~ 1250
    ta0ch1val = x * 1000 / 180 + 250;
    x > 180 ? (x = 180):(x);
    Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1,(uint16_t)ta0ch1val);
}
