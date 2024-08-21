/*
 * enc.h
 *
 *  Created on: 2022年10月22日
 *      Author: Zaki
 */

#ifndef ENC_H_
#define ENC_H_

//左右轮驱动
void LeftMotor_Backward(int rate)
{
    //左轮   P2.5 and P2.4
      P2->SEL0 |= BIT4;//左前
      P2->SEL1 &= ~BIT4;
      P2->DIR |=  BIT4;
      TIMER_A0->CCR[0] = 255;
      TIMER_A0->CCTL[1] = OUTMOD_7;
      TIMER_A0->CCR[1] = rate;
      TIMER_A0->CTL = TASSEL_1+MC_1;
      P2->SEL0 &= ~BIT5;
      P2->SEL1 &= ~BIT5;
      P2->DIR |= BIT5;
      P2->OUT &= ~BIT5;
}
void RightMotor_Forward(int rate)
{
    //右轮   P2.7 and P2.6
      P2->SEL0 |= BIT6;//左前
      P2->SEL1 &= ~BIT6;
      P2->DIR |=  BIT6;
      TIMER_A0->CCR[0] = 255;
      TIMER_A0->CCTL[3] = OUTMOD_7;
      TIMER_A0->CCR[3] = rate;
      TIMER_A0->CTL = TASSEL_1+MC_1;
      P2->SEL0 &= ~BIT7;
      P2->SEL1 &= ~BIT7;
      P2->DIR |= BIT7;
      P2->OUT &= ~BIT7;
}
void LeftMotor_Forward(int rate)
{
    //左轮   P2.5 and P2.4
      P2->SEL0 |= BIT5;//左前
      P2->SEL1 &= ~BIT5;
      P2->DIR |=  BIT5;
      TIMER_A0->CCR[0] = 255;
      TIMER_A0->CCTL[2] = OUTMOD_7;
      TIMER_A0->CCR[2] = rate;
      TIMER_A0->CTL = TASSEL_1+MC_1;
      P2->SEL0 &= ~BIT4;
      P2->SEL1 &= ~BIT4;
      P2->DIR |= BIT4;
      P2->OUT &= ~BIT4;
}
void RightMotor_Backward(int rate)
{
    //右轮   P2.7 and P2.6
      P2->SEL0 |= BIT7;//左前
      P2->SEL1 &= ~BIT7;
      P2->DIR |=  BIT7;
      TIMER_A0->CCR[0] = 255;
      TIMER_A0->CCTL[4] = OUTMOD_7;
      TIMER_A0->CCR[4] = rate;
      TIMER_A0->CTL = TASSEL_1+MC_1;
      P2->SEL0 &= ~BIT6;
      P2->SEL1 &= ~BIT6;
      P2->DIR |= BIT6;
      P2->OUT &= ~BIT6;
}

//整合左右轮驱动，a=1前进，-1后退，rate为速度
void rmotor(int a,int rate)
{
  if(a==1)  RightMotor_Forward(rate) ;
  if(a==-1) RightMotor_Backward(rate);
}
void lmotor(int a,int rate)
{
  if(a==1)  LeftMotor_Forward(rate) ;
  if(a==-1) LeftMotor_Backward(rate);
}

//延时
void delay(int mul)
{
    uint32_t count=mul*2000;
    uint32_t counter;
    for(counter=0;counter<count;counter++)
    {}
}

//停车
void motor_stop()
{
  rmotor(1,0);
  lmotor(1,0);
}

//MGR编码器电机的初始化
void MG310_init()
{
    //指示灯
    P1DIR |= BIT0;
    //A相 左P4.0 右P4.4
    P4->DIR &=~(BIT0+BIT4);
    P4->IE |= (BIT0+BIT4);
    P4->IES &=~(BIT0+BIT4);/*先上升沿*/
    NVIC->ISER[1] |= 1<<7;
    __enable_interrupts();
    //B相 左P4.1 右P4.5
    P4->DIR &=~(BIT1+BIT5);
    //定时器，每隔1s读取速度
    TIMER_A3->CTL |= TASSEL_1+TACLR+MC_1;
    TIMER_A3->CCR[0]=600;
    TIMER_A3->CCR[1]=600;//修改这个来修改时间间隔,600/32768
    TIMER_A3->CCTL[0]=CCIE;
  //  TIMER_A3->CCTL[1]=CCIE;
    NVIC->ISER[0]|= 1<<(TA3_0_IRQn & 31);
   // NVIC->ISER[0]|= 1<<(TA3_N_IRQn & 31);
}

//TA3中断，速度控制,与PORT4中断组合使用，测速后反馈到TA3中断
void TA3_0_IRQHandler()
{
    if(lvelocity>rvelocity) lspeed=lspeed-2;
      if(lvelocity<rvelocity) rspeed=rspeed-2;
      if(lspeed<25||rspeed<25)
      {
          lspeed=lspeed+10;
          rspeed=rspeed+10;
      }
    TIMER_A3->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}

//PORT4中断 测速
void PORT4_IRQHandler()
{
    if(P4->IFG & BIT0)
    {
        if(!(P4->IES & BIT0))/*A上升沿进入*/
        {
            P4->IES |= BIT0;
            if(P4->IN & BIT1)/*此时如果B相为高电平*/
            {
                lvelocity++;
            }
            else
            {
                lvelocity--;
            }
        }
        else if((P4->IES & BIT0))/*A下降沿进入*/
        {
            P4->IES &=~BIT0;
            if(P4->IN & BIT1)/*此时如果B相为高电平*/
            {
                lvelocity--;
            }
            else
            {
                lvelocity++;
            }
        }
    }
    if(P4->IFG & BIT4)
        {
            if(!(P4->IES & BIT4))/*A上升沿进入*/
            {
                P4->IES |= BIT4;
                if(P4->IN & BIT5)/*此时如果B相为高电平*/
                {
                    rvelocity++;
                }
                else
                {
                    rvelocity--;
                }
            }
            else if((P4->IES & BIT4))/*A下降沿进入*/
            {
                P4->IES &=~BIT4;
                if(P4->IN & BIT5)/*此时如果B相为高电平*/
                {
                    rvelocity--;
                }
                else
                {
                    rvelocity++;
                }
            }
        }
   P4->IFG =0;
}




#endif /* ENC_H_ */
