#include "msp.h"
#include "comm.h"

/**
 * main.c
 * ʵ��2����S1-1.1������ȡ����Թܵ��źţ�����LED2��ָʾ���ĸ�����Թ��ں����ϡ�
 * ��S2-1.4������ȡ��ײ����״̬����LED1��ָʾ�Ƿ�����ײ��
 * û�а�����ʱ��ϵͳ����LPM4ģʽ��
 *
 */




int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    //LED��ʼ��
    P2DIR |= BIT1+BIT0+BIT2;
    P2OUT &=~ (BIT1+BIT0+BIT2);
    //���س�ʼ��
    P1DIR &=~ (BIT1+BIT4);
    P1REN |= BIT1+BIT4;
    P1OUT |= BIT1+BIT4;

    P1IES |= BIT1+BIT4;
    P1IFG = 0;
    P1IE |= BIT1+BIT4;

    //�����ж���͹���
    NVIC->ISER[1]=1<<((PORT1_IRQn)&31);  //P1�����ж�

    comm_init();

    PCM->CTL1 = PCM_CTL1_KEY_VAL|PCM_CTL1_FORCE_LPM_ENTRY;  //��Կһ���������˵
    __enable_interrupts();  //�����ж�
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk); // ����Ϊ���˯��ģʽ
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;  //�ж���Ϻ����˯��
    __DSB();  //ֹͣ����ж�
    __sleep();   //����LPM4˯����

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
    //����˯�����Ϊ1
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
    //�ն����exit�����˯
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    //����˯
    __DSB();
    //����LPM4
    __sleep();

    return 0;
}*/
