#include "msp.h"


/**
 * main.c
 * 实验1：上电后，LED1闪烁，小车停止状态，
 * 按S1键小车开始前进，小车前进采用PWM波控制方式，
 * 实现小车40%的速度前进500ms,以80%的速度前进500ms,以60%的速度后退500ms
 *
 */

void delay(int mul)
{
    uint32_t count=mul*5000;
    uint32_t counter;
    for(counter=0;counter<count;counter++)
    {}
}
void LeftMotor_Forward(int rate)
{
    //左轮   P2.7 and P2.4
      P2->SEL0 |= BIT4;//左前
      P2->SEL1 &= ~BIT4;
      P2->DIR |=  BIT4;
      TIMER_A0->CCR[0] = 2048;
      TIMER_A0->CCTL[1] = OUTMOD_7;
      TIMER_A0->CCR[1] = rate;
      TIMER_A0->CTL = TASSEL_1+MC_1+TACLR;

      P2->SEL0 &= ~BIT7;
      P2->SEL1 &= ~BIT7;
      P2->DIR |= BIT7;
      P2->OUT &= ~BIT7;
}
void RightMotor_Forward(int rate)
{
    //右轮   P5.6 and P2.6
      P2->SEL0 |= BIT6;//左前
      P2->SEL1 &= ~BIT6;
      P2->DIR |=  BIT6;
      TIMER_A0->CCR[0] = 2048;
      TIMER_A0->CCTL[3] = OUTMOD_7;
      TIMER_A0->CCR[3] = rate;
      TIMER_A0->CTL = TASSEL_1+MC_1+TACLR;

      P5->SEL0 &= ~BIT6;
      P5->SEL1 &= ~BIT6;
      P5->DIR |= BIT6;
      P5->OUT &= ~BIT6;
}
void LeftMotor_Backward(int rate)
{
    //左轮   P2.7 and P2.4
      P2->SEL0 |= BIT7;//左前
      P2->SEL1 &= ~BIT7;
      P2->DIR |=  BIT7;
      TIMER_A0->CCR[0] = 255;
      TIMER_A0->CCTL[4] = OUTMOD_7;
      TIMER_A0->CCR[4] = rate;
      TIMER_A0->CTL = TASSEL_1+MC_1+TACLR;

      P2->SEL0 &= ~BIT4;
      P2->SEL1 &= ~BIT4;
      P2->DIR |= BIT4;
      P2->OUT &= ~BIT4;
}
void RightMotor_Backward(int rate)
{
    //右轮   P5.6 and P2.6
      P5->SEL0 |= BIT6;//左前
      P5->SEL1 &= ~BIT6;
      P5->DIR |=  BIT6;
      TIMER_A2->CCR[0] = 255;
      TIMER_A2->CCTL[1] = OUTMOD_7;
      TIMER_A2->CCR[1] = rate;
      TIMER_A2->CTL = TASSEL_1+MC_1+TACLR;

      P2->SEL0 &= ~BIT6;
      P2->SEL1 &= ~BIT6;
      P2->DIR |= BIT6;
      P2->OUT &= ~BIT6;
}
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	//systick
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk;
    // Set SysTick period = 0xB71B0,4Hz/3MHz MCLK
    SysTick->LOAD = (0xB71B0 - 1)*2;
	//P1.0
    P1->DIR |= BIT0;
    P1->OUT &=~BIT0;
	//TimerA0-A2
    TIMER_A0->CTL= TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_MC__UP;//ACLK, up mode
    TIMER_A2->CTL= TIMER_A_CTL_SSEL__ACLK | TIMER_A_CTL_MC__UP;//ACLK, up mode
    //P1-1
    P1->DIR &=~BIT1;
    P1->REN |= BIT1;
    P1->OUT |= BIT1;
    P1->IE |= BIT1;
    P1->IES |= BIT1;
    P1->IFG = 0;
    //NVIC
    NVIC->ISER[1] = 1<<3;
    __enable_interrupts();
    //sleep
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    __DSB();
    __sleep();

    __no_operation();
}
void SysTick_Handler(void)
{
    P1->OUT ^= BIT0; // Toggle P1.0 LED
}
void PORT1_IRQHandler(void)
{
    P1->IFG &=~BIT1;
    //P1->OUT &=~BIT0;
    SysTick->CTRL &=~SysTick_CTRL_ENABLE_Msk;
    LeftMotor_Forward(820);
    RightMotor_Forward(820);
    delay(50);
    LeftMotor_Forward(1640);
    RightMotor_Forward(1640);
    delay(50);
    LeftMotor_Backward(1230);
    RightMotor_Backward(1230);
    delay(50);
    LeftMotor_Forward(0);
    RightMotor_Forward(0);
    delay(50);
    P2->OUT &= ~BIT4;
    P2->OUT &= ~BIT6;
    P2->OUT &= ~BIT7;
    P5->OUT &= ~BIT6;
}

