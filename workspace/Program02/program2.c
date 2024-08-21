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

#define clock_source 3    //ʱ��Ƶ����д�������λMHz��

int PART = 0;//ȫ�ֱ�����ʾ��Ŀ��
int SPEED1=57;//60~70%�ĵ�->42   ��0.3m/s->SPEED1=57
//int SPEED2=80;
int CYCLE=0;
int warningenable=1;//����Ԥ��
int lukouEnable=1;//����·��ֱ����
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

void delay_us(unsigned long us){ //uS΢�뼶��ʱ����
    SysTick_disableInterrupt();   //����ʱ���ж�
    SysTick_setPeriod(us*clock_source); //���ؼ���ֵ
    SysTick->VAL=0x00;           //��ռ�����
    SysTick_enableModule();   //ʹ��ϵͳʱ��
    while(SysTick_getValue()&(us*clock_source));
    SysTick->VAL=0x00;        //��ռ�����
    SysTick_disableModule();//�رռ���
}

void delay_mss(unsigned long ms){ //mS���뼶��ʱ����
    while( ms-- != 0){
        delay_us(1000);
    }
}

void delay_s(unsigned long s){ //S�뼶��ʱ����
    while( s-- != 0){
        delay_mss(1000);
    }
}

//����������,n��Ƶ�ʣ�j�ǳ���ʱ�䣨ʱ��δ���Թ���֪��jȡ���٣�
void Buzzer(int n,int j)
{
    P2DIR |= BIT5;
    ///��ʱÿ��������ʱ��
    for ( ;j>0;j--)
    {
        volatile unsigned int i;
        P2OUT ^= BIT5;
        i = n;
        do
        {
        i--;                                // �Լ�����
        __no_operation();                             // �ղ���
        }while (i != 0);                    // ��׼��do����while��䣬ע���©�������ź���ķֺ�
     }
}

/******************************************************�ӻ�������ʼ��**************************************************************************/
void cong_bluetooth_init()
{
    //����ʱ�ӳ�ʼ��ʹ��Ĭ��SMCLK->DCO��3M
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
    //������ʼ��
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
//��������
void EUSCIA2_IRQHandler(void)
{
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG)
    {
//        P1->OUT ^= BIT0;
        /***************ͣ������*****************/
        if(EUSCI_A2->RXBUF=='E')
        {
            cong_motor_stop();
        }
        else if(EUSCI_A2->RXBUF=='5')
        {
            RightMotor_Forward(0);
            LeftMotor_Forward(0);
            delay_s(4);
//            //������
//            RightMotor_Forward(200);
//            LeftMotor_Forward(100);
//            delay(10);
            refresh();
            CYCLE=1;
            warningenable=0;
////            trig();
//
////            reload_shift_reg(); //ˢ�¼Ĵ���
////            ir_check();
////            cong_1_read_ir_values(); //ˢ��ת��Ƕ�
////            calculate_pid(); //PID����
////            motor_control(); //�������
////            cong_1_assisted_control();
////
        }
        else if(EUSCI_A2->RXBUF=='g')
        {
            GUAI=2;//����WARNING�͹�
            SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk;//����systick,3MHz,���ж�
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
        GUAI=0;//ֹͣWARNING��ddflag����
        P2->OUT &=~(BIT0+BIT1+BIT2);
        SPEED1=57;//�ָ��ٶ�
        Buzzer(65,300);
        SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
    }
}
/**********************************************************�ӻ��������*******************************************************************/

//void delay_ms(unsigned int ms)//��OLED.C��
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
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//����
        GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);//����
        delay_ms(10);
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//����
        TIMER_A_CMSIS(TIMER_A2_BASE)->R = 0;
        while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN5) == 0&&wait<200000)
        {wait++;}
        if(wait==10000)
        {
            GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//����
            GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);//����
            delay_ms(20);
            GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//����
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
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//����
    GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);//����
    delay_ms(2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//����
    TIMER_A2->CTL =  TIMER_A2->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__STOP;//�ر�A2
    __no_operation();
}

void PORT6_IRQHandler()
{
    P6->IFG =0;
    if((P6->IES&BIT5)==0)//���������
    {
        TIMER_A_CMSIS(TIMER_A2_BASE)->R = 0;
        TIMER_A2->CTL |= TIMER_A_CTL_CLR;
        TRIG=TIMER_A2->R;
        TIMER_A2->CTL =  TIMER_A2->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__UP;//�ر�A2
        P6->IES |= BIT5;
        P6->IFG =0;
        __no_operation();
    }
    else if((P6->IES&BIT5)==BIT5)//������½���
    {
        //TIMER_A2->CTL =  TIMER_A2->CTL&(~TIMER_A_CTL_MC_MASK)|TIMER_A_CTL_MC__STOP;//�ر�A2
        ECHO=TIMER_A_CMSIS(TIMER_A2_BASE)->R;
         //v = 340m/s = 34000cm/s = 34000cm/10^6us = 0.034cm/us
         //s = vt/2 = t*0.034/2 = t*0.017 ?t/58
         DISTANCE=fabs(((float)(ECHO-TRIG) / 58));
         DISTANCE+=2;
         //  UPNUMBER=0;
         P6->IES &=~BIT5;
         P6->IFG =0;
         __no_operation();
         //����DISTANCE�ı��ٶ�
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

/****************************************�ӳ�ѭ��****************************************************************************/


float cong_1_read_ir_values() //����С��ƫ����  --->��������Ϊ1
{
    //��ͣ���ںͷ���ڿ���һ���ģ�������ҪWARNING!!!

    //·��ͣ��+�����
    if( /*ͣ��*/ ((sensor.ir_mid == 1) && (sensor.ir_left_1 == 1) && (sensor.ir_right_1 == 1))   || /*��·��*/ ((sensor.ir_mid == 1) && (sensor.ir_right_2 == 1) && (sensor.ir_right_1 == 1))   ||   (sensor.ir_left_2&&sensor.ir_mid)  ||  (sensor.ir_right_2&&sensor.ir_mid)||(sensor.ir_left_3&&sensor.ir_mid)||(sensor.ir_left_3&&sensor.ir_right_2)||(sensor.ir_left_2&&sensor.ir_right_1)||(sensor.ir_left_2&&sensor.ir_right_2)||(sensor.ir_left_3&&sensor.ir_right_2))
     {
         error = 0;
         flag=0;
         dflag=0;
         ddflag=1;

         //
         Buzzer(85,100);
         //

     }
    //����ҡ��
    //��תС
    else if (sensor.ir_left_2 == 1&&sensor.ir_mid==0&&sensor.ir_left_1==0&&sensor.ir_right_1==0)
    {
        error = 2;flag=1;dflag=0;
    }
    //��תС
    else if (sensor.ir_left_1 == 1&&sensor.ir_mid==0&&sensor.ir_right_1==0&&sensor.ir_right_2==0)
    {
        error = 2;flag=0;dflag=0;
    }
    //���ת��ֻ������
    else if(sensor.ir_left_3 == 1&&sensor.ir_left_2 == 0&&sensor.ir_mid==0&&sensor.ir_left_1==0&&sensor.ir_right_1==0&&sensor.ir_right_2==0&&sensor.ir_right_3==0)
    {
        error= -2;flag=1;dflag=0;//������ת
//        error=4;flag=0;dflag=1;
    }
    //��תС
    else if (sensor.ir_right_2 == 1)
    {
        error = -2;flag=1;dflag=0;
    }
    //��תС
    else if (sensor.ir_right_1 == 1 && sensor.ir_right_2 == 1)
    {
        error = -2;flag=0;dflag=0;
    }
    //ֱ��
    else if (((sensor.ir_mid == 1) && (sensor.ir_left_1 == 1)) || ((sensor.ir_mid == 1) && (sensor.ir_right_1 == 1))||((sensor.ir_mid == 1)&&(sensor.ir_left_1 == 0)&&(sensor.ir_right_1 == 0)))
    {
        error = 0;flag=1;dflag=0;
    }
    //��ת��
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
    if(flag==0&&dflag==0)//����ССת
       {
           delay(0);
       }
       else if(flag==1&&dflag==0)//��ͨ����ת
       {
           delay(1);
       }
       else if(dflag==1)//������ת
       {
           delay(70);
       }
      if(ddflag)
       {
//           P1OUT ^= BIT0;
           delay(7);//--------------------------------------------------------------------------------------------------
           error=0;
           calculate_pid(); //PID����
           motor_control(); //�������
           ddflag=0;
       }
}
void cong_motor_stop()
{

    RightMotor_Forward(5);
    LeftMotor_Forward(5);
    RightMotor_Forward(0);
    LeftMotor_Forward(0);
    //������
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

void cong_3_read_ir_values() //ˢ��ת��Ƕ�
{
    //��ͣ���ںͷ���ڿ���һ���ģ�������ҪWARNING!!!
    static int jj=30,ss=30,cc=0;

    if(GUAI==2)//GUAI=2
    {
        if(sensor.ir_left_3)//��⵽����
        {
            RATE1=SPEED1;
            LeftMotor_Backward(200);
            RightMotor_Forward(200);
            delay(15);
            HCSR04Init();
            GUAI--;
            P2->OUT |= (BIT0+BIT1+BIT2);//����
            cc=17;//--->�����ٶ�
        }
//        else if(WARNING)//��⵽WARNING
//        {
//            LeftMotor_Forward(0);
//            RightMotor_Forward(200);
//            delay(140);
//            GUAI--;
//            P2->OUT |= (BIT0+BIT1+BIT2);//����
//            cc=20;//--->�����ٶ�
//        }

    }
    if(GUAI==1&&cc>0)//���ٳ���
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


    //·��ͣ��+�����
    if( /*ͣ��*/ ((sensor.ir_mid == 1) && (sensor.ir_left_1 == 1) && (sensor.ir_right_1 == 1))   || /*��·��*/ ((sensor.ir_mid == 1) && (sensor.ir_right_2 == 1) && (sensor.ir_right_1 == 1))   ||   (sensor.ir_left_2&&sensor.ir_mid)  ||  (sensor.ir_right_2&&sensor.ir_mid)||(sensor.ir_left_3&&sensor.ir_mid)||(sensor.ir_left_3&&sensor.ir_right_2)||(sensor.ir_left_2&&sensor.ir_right_1)||(sensor.ir_left_2&&sensor.ir_right_2)||(sensor.ir_left_3&&sensor.ir_right_2))
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
         if(GUAI==1)//������ʱ��С��
         {
//             error=4;
             LeftMotor_Forward(0);
             RightMotor_Forward(200);
             delay(25);
             GUAI--;
         }
     }
    //����ҡ��
    //��תС
    else if (sensor.ir_left_2 == 1&&sensor.ir_mid==0&&sensor.ir_left_1==0&&sensor.ir_right_1==0)
    {
        error = 2;flag=1;dflag=0;
    }
    //��תС
    else if (sensor.ir_left_1 == 1&&sensor.ir_mid==0&&sensor.ir_right_1==0&&sensor.ir_right_2==0)
    {
        error = 2;flag=0;dflag=0;
    }
    //���ת��ֻ������
    else if(sensor.ir_left_3 == 1&&sensor.ir_left_2 == 0&&sensor.ir_mid==0&&sensor.ir_left_1==0&&sensor.ir_right_1==0&&sensor.ir_right_2==0&&sensor.ir_right_3==0)
    {
        error= 0;flag=1;dflag=0;//������ת
//        error=4;flag=0;dflag=1;
    }
    //��תС
    else if (sensor.ir_right_2 == 1)
    {
        error = -2;flag=1;dflag=0;
    }
    //��תС
    else if (sensor.ir_right_1 == 1 && sensor.ir_right_2 == 1)
    {
        error = -2;flag=0;dflag=0;
    }
    //ֱ��
    else if (((sensor.ir_mid == 0) && (sensor.ir_left_1 == 0)) || ((sensor.ir_mid == 0) && (sensor.ir_right_1 == 0)))
    {
        error = 0;flag=1;dflag=0;
    }
//    else if (sensor.ir_right_3 == 1)   //��ת��
//    {
//      error = -4;flag=1;dflag=1;
//    }
    //��ת��
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
    if(flag==0&&dflag==0)//����ССת
       {
           delay(0);
       }
       else if(flag==1&&dflag==0)//��ͨ����ת
       {
           delay(5);
       }
       else if(dflag==1)//������ת
       {
           delay(50);
       }
      if(ddflag)
       {
//           P1OUT ^= BIT0;
           delay(10);
           error=0;
           calculate_pid(); //PID����
           motor_control(); //�������
           ddflag=0;
       }
}

void refresh()    //*******************************HS������ʼ��
{
    //PID�㷨����
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
    //OLED��ʼ��
    init();
    OLED_Init();
    //warning
    P1->DIR &=~BIT6;
    P1->REN |= BIT6;
    P1->OUT &=~BIT6;

    HCSR04Init();  //������ʼ��
    while(1)
    {
        trig();
        __no_operation();
    }

    //��ʼʱ�ȴ������ź�
    cong_bluetooth_init();
    while(!PART)
    {
        cong_bluetooth();
    }


//    PART=3;
//    GUAI=2;

    //PART�Ѿ��ı�
    OLED_Clear();
    OLED_ShowString(2,0,"Track");
    OLED_ShowNum(70, 0, PART, 4, 16);


//    cong_bluetooth_init();

    if((PART==1)||(PART==2)||(PART==4))//����1,2,4
    {
        RATE1=SPEED1;RATE2=100;//���ٵ���
        HCSR04Init();  //������ʼ��
        parking_Bluetooth();//ͣ������
        while((PART==1)||(PART==2)||(PART==4))
        {
          reload_shift_reg(); //ˢ�¼Ĵ���
//          ir_check();
          cong_1_read_ir_values(); //ˢ��ת��Ƕ�
          calculate_pid(); //PID����
          motor_control(); //�������
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
        RATE1=SPEED1;RATE2=100;//���ٵ�����
        HCSR04Init();  //������ʼ��
        parking_Bluetooth();//ͣ������
        PIAN1=10;PIAN2=10;//�ı�ƫ����
        while(PART==3)
        {
          reload_shift_reg(); //ˢ�¼Ĵ���
//          ir_check();
          cong_3_read_ir_values(); //ˢ��ת��Ƕ�
          calculate_pid(); //PID����
          motor_control(); //�������
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

