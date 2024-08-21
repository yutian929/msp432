#include "msp.h"
#include "comm.h"

/**
 * main.c
 * 实验2：按S1-1.1键，读取红外对管的信号，并用LED2灯指示是哪个红外对管在黑线上。
 * 按S2-1.4键，读取防撞开关状态，用LED1灯指示是否有碰撞。
 * 没有按键的时候，系统进入LPM4模式。
 *
 */




int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    //LED初始化
    P2DIR |= BIT1+BIT0+BIT2;
    P2OUT &=~ (BIT1+BIT0+BIT2);
    //开关初始化
    P1DIR &=~ (BIT1+BIT4);
    P1REN |= BIT1+BIT4;
    P1OUT |= BIT1+BIT4;

    P1IES |= BIT1+BIT4;
    P1IFG = 0;
    P1IE |= BIT1+BIT4;

    //配置中断与低功耗
    NVIC->ISER[1]=1<<((PORT1_IRQn)&31);  //P1允许被中断

    comm_init();

    PCM->CTL1 = PCM_CTL1_KEY_VAL|PCM_CTL1_FORCE_LPM_ENTRY;  //密钥一样，必须得说
    __enable_interrupts();  //开总中断
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk); // 设置为深度睡眠模式
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;  //中断完毕后继续睡觉
    __DSB();  //停止别的中断
    __sleep();   //进入LPM4睡觉觉

    return 0;
}


//hs
/*void PORT1_IRQHandler(void)
{
    uint32_t i;
    if(P1->IFG & BIT1)
    {
        for(i=0;i<1000;i++)
            if(!(P1IN & BIT1))
                {
                reload_shift_reg();
                ir_check();
                }
    }
    if(P1->IFG & BIT4)
        {
            for(i=0;i<1000;i++)
                if(!(P1IN & BIT4))
                {
                    reload_shift_reg();
                    switcher_check();
                }
        }
    P1IFG &=~ (BIT1+BIT4);
}*/
//yt
void PORT1_IRQHandler(void)
{
    if(P1->IFG&(BIT1+BIT4))
    {
        //delay(3);
        while(!(P1->IN&BIT1))
        {
            //P1->IFG &=~BIT1;
            reload_shift_reg();
            ir_check();
        }
        while(!(P1->IN&BIT4))
        {
            //P1->IFG &=~BIT4;
            reload_shift_reg();
            switcher_check();
        }
    }
    P1->IFG &=~ (BIT1+BIT4);
}






/*void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    smartCarInitial();
    //port ie
    P1->IE |= (BIT1+BIT4);
    P1->IES|= (BIT1+BIT4);
    P1->IFG=0;
    //i vector
    NVIC->ISER[1]=1<<3;//port1
    //sys
    __enable_interrupts();

    // Enable PCM rude mode, which allows to device to enter LPM3 without
    //waiting for peripherals
    PCM->CTL1 = PCM_CTL1_KEY_VAL|PCM_CTL1_FORCE_LPM_ENTRY;
    //设置睡眠深度为1
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
    //终端完成exit后继续睡
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    //立即睡
    __DSB();
    //进入LPM4
    __sleep();

    return 0;
}*/
