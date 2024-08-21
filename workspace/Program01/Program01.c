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
#include "xunji.h"
#include <cs.c>

int PART=1;
/***************************************************PART0************************************************************/
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


/***************************************************PART1************************************************************/

void part1_init(void)
{
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
}

float part1_read_ir_values() //����С��ƫ����  --->��������Ϊ1
{
    //ͣ��
    if ((sensor.ir_mid == 1) && (sensor.ir_left_1 == 1) && (sensor.ir_left_2 == 1) && (sensor.ir_left_3 == 1) && (sensor.ir_right_1 == 1) && (sensor.ir_right_2 == 1) && (sensor.ir_right_3 == 1))
    {
        error = 5;flag=1;dflag=0;
        part1_stop();
    }
    //ʮ��·��

    else if (sensor.ir_right_3 == 1)   //��ת��
       {
         error = -4;flag=1;dflag=1;
         RightMotor_Backward(200);
         LeftMotor_Forward(200);
         delay(40);
       }
    else if (sensor.ir_left_2 == 1)  //��תС
    {
      error = 2;flag=1;dflag=0;
    }

    else if (sensor.ir_left_1 == 1 && sensor.ir_left_2 == 1)
    {
      error = 2;flag=0;dflag=0;           //��תС
    }

    else if (sensor.ir_right_2 == 1)  //��תС
    {
      error = -2;flag=1;dflag=0;
    }

    else if (sensor.ir_right_1 == 1 && sensor.ir_right_2 == 1)
    {
      error = -2;flag=0;dflag=0;           //��תС
    }

    else if (((sensor.ir_mid == 0) && (sensor.ir_left_1 == 0)) || ((sensor.ir_mid == 0) && (sensor.ir_right_1 == 0))) //ֱ��
    {
      error = 0;flag=1;dflag=0;
    }

    else if (sensor.ir_right_2 == 1 && sensor.ir_right_3 == 1) //��ת��
    {
      error = -4;flag=1;dflag=0;
    }
    else if (sensor.ir_left_2 == 1 && sensor.ir_left_3 == 1) //��ת��
    {
      error = 4;flag=1;dflag=0;
    }
    return error;
}
void part1_stop()//�жϺ�ʱֹͣ
{
  if(error==5)//��һ�γ���error=5
  {
      RightMotor_Forward(0);
      LeftMotor_Forward(0);
      P1OUT |= BIT0;
      delay(200);
      P1OUT &=~ BIT0;
      OLED_ShowString(0,2,"Arrived at");
      OLED_ShowString(0,4,"the entrance.");
      Buzzer(85,50);   //re
      Buzzer(75,50);   //mi
      Buzzer(71,50);   //fa
      PART=2;
  }
}
void part1()
{
    while(PART==1)
    {
      reload_shift_reg();//ˢ�¼Ĵ���
      part1_read_ir_values(); //ˢ��ת��Ƕ�
      calculate_pid();  //PID����
      motor_control();  //�������
      part1_stop();
      //assisted_control();  //��������
    }
}




/***************************************************PART2************************************************************/

int Distance012[3]={0};
int c2=0,c1=0;
int afterhit=0;
int most=130;
const Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/1 = 3MHz
        50000,                           // 5000 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};
/*void delay_ms (int a)
{
    int i;
    for(i=0;i<a*500;i++);
}*/
void servo_init()
{   //P7.4/PM_TA1.4/C0.5 2
    TIMER_A1->CTL = TIMER_A_CTL_TASSEL_1 | // ACLK
        TIMER_A_CTL_MC_1|TIMER_A_CTL_CLR; // up mode
    P7->DIR |= BIT4; // P2.6-7 option selection TA0.3-4
    P7->SEL0 |= BIT4;
    P7->SEL1 &=~(BIT4);
}
//servo_angle(21);//->you
//servo_angle(54);//->qian
//servo_angle(86);//->zou
void servo_angle(int ang)
{
    // Configure GPIO
    TIMER_A1->CCR[0]=655;
    TIMER_A1->CCTL[4] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A1->CCR[4]=ang;
}
//void delay_ms(unsigned int ms)
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

float Distance(void)
{
    int count=0,i=0,sum=0,wait=0;
    float distance=0;
    while(i!=5)
    {
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//����
        GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN4);//����
        delay_ms(20);
        GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN4);//����
        TIMER_A_CMSIS(TIMER_A2_BASE)->R = 0;
        while(GPIO_getInputPinValue(GPIO_PORT_P6, GPIO_PIN5) == 0&&wait<200000)
        {wait++;}
        if(wait==200000)
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
        return sum/5;
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

void part2_hit_def()
{
   switch(hitcase)
   {
   case 1:{LeftMotor_Backward(100);RightMotor_Backward(130);delay(250);LeftMotor_Forward(0);RightMotor_Forward(0);delay(10);afterhit=1;break;}
   case 4:{LeftMotor_Backward(130);RightMotor_Backward(100);delay(250);LeftMotor_Forward(0);RightMotor_Forward(0);delay(10);afterhit=1;break;}
//   case 2:{LeftMotor_Backward(180);RightMotor_Backward(180);delay(70);LeftMotor_Forward(left_motor_speed);RightMotor_Forward(right_motor_speed);break;}
//   case 3:{LeftMotor_Backward(180);RightMotor_Backward(180);delay(70);LeftMotor_Forward(left_motor_speed);RightMotor_Forward(right_motor_speed);break;}
//   case 5:{LeftMotor_Forward(180);RightMotor_Forward(180);delay(70);LeftMotor_Forward(left_motor_speed);RightMotor_Forward(right_motor_speed);break;}
//   case 6:{LeftMotor_Forward(180);RightMotor_Forward(180);delay(70);LeftMotor_Forward(left_motor_speed);RightMotor_Forward(right_motor_speed);break;}
   default:hitcase=0;break;
   }
   hitcase=0;
//   LeftMotor_Forward(0);
//   RightMotor_Forward(0);
//   delay(10);
}

void part2_switcher_check()
{

    if(sensor.switcher_front_left_2||sensor.switcher_front_left_1||sensor.switcher_front_right_1||sensor.switcher_front_right_2||sensor.switcher_back_left||sensor.switcher_back_right)
    {
        delay(3);
        if(sensor.switcher_front_left_2||sensor.switcher_front_right_2)//��2��2Ϊ��
        {
            P2->OUT |= BIT0;
            P2->OUT &=~BIT1;
            P2->OUT &=~BIT2;
            if(sensor.switcher_front_left_2) hitcase=1;
                       else hitcase=4;

        }
//        else if(sensor.switcher_front_left_1||sensor.switcher_front_right_1)//��1��1Ϊ��
//        {
//            P2->OUT |= BIT1;
//            P2->OUT &=~BIT0;
//            P2->OUT &=~BIT2;
//            if(sensor.switcher_front_left_1) hitcase=2;
//            else hitcase=3;
//        }
        else if(sensor.switcher_back_left||sensor.switcher_back_right)//������Ϊ��
        {
            P2->OUT |= BIT2;
            P2->OUT &=~BIT0;
            P2->OUT &=~BIT1;
            if(sensor.switcher_front_left_1) hitcase=5;
            else hitcase=6;
        }
    }
    part2_hit_def();
    afterhit=1;
}

void you_2()    ///////////////����ֱ��
{
    LeftMotor_Forward(250);
    RightMotor_Backward(245);
    delay(72);//ԭ����73
    HCSR04Init();
}
void zuo_2()
{
    LeftMotor_Backward(250);
    RightMotor_Forward(245);
    delay(78);//ԭ����82
}
void ting_2()
{
    LeftMotor_Forward(0);
    RightMotor_Forward(0);
    delay(10);
}

void part2_init()
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);//led��
    CS_initClockSignal(CS_MCLK,CS_HFXTCLK_SELECT,CS_CLOCK_DIVIDER_1);//ϵͳʱ�ӳ�ʼ����ϵͳ��ʱ�ӣ�
    CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);//ϵͳʱ�ӳ�ʼ������ϵͳʱ�ӣ�
    //UART_initModule(EUSCI_A0_BASE,&upConfig); //����1 ��ʼ��
    HCSR04Init();
}
void part2()
{
    while(PART==2)//��һ���ж�����£���������Ҫ���
    {
        //��ͣ
        ting_2();
        //���м�
        servo_init();
        servo_angle(54);  //ת��ԭ�Ƕ�
        delay(50);
        part2_init();
        Distance012[0]= Distance();
        while(Distance012[0]>35)
        {
            Distance012[0]= Distance();
            LeftMotor_Forward(300);
            RightMotor_Forward(328); //��������һ��
            delay(15);
            reload_shift_reg();
            part2_switcher_check();

            if(afterhit==1)
            {
                LeftMotor_Forward(150);
                RightMotor_Forward(145); //��������һ��
                delay(50);
                afterhit=0;
            }
            HCSR04Init();
            Distance012[0]= Distance();
//                reload_shift_reg(); //ˢ�¼Ĵ���
//                switcher_check();
        }
        //��ͣ
        ting_2();
        //�����
        servo_init();
        servo_angle(86);//->zou
        delay(50);
        HCSR04Init();//
        Distance012[1]= Distance();
        //���ұ�
        servo_init();
        servo_angle(21);//->you
        delay(100);
        HCSR04Init();
        Distance012[2]= Distance();
        //�ж�
        if(Distance012[2]>=Distance012[1])//�ұ߱���ߴ�
        {
            if(c2==1)
            {
                zuo_2();
            }
            else
            {
                you_2();
            }
            c2++;
        }
        else if(Distance012[1]>=Distance012[2]&&Distance012[1])//��ߴ����ұ�
        {
            if(Distance012[1]>=most)
            {
                zuo_2();
                LeftMotor_Forward(200);
                RightMotor_Forward(240);
                delay(666);//ֱ��һ��ʱ��
                PART=3;
                OLED_ShowString(0,2,"Arrived at");
                OLED_ShowString(0,4,"the registration.");
                Buzzer(63,50);   //so
                Buzzer(56,50);   //la
                Buzzer(50,50);   //xi
            }
            else
            {
                zuo_2();
            }
        }
        //
//        else if(Distance012[1]>=Distance012[2]<=most&&Distance012[2]>=Distance012[1])
//        {
//            you_2();
//        }
//        else if(Distance012[1]<=most&&Distance012[2]<=most&&Distance012[2]<Distance012[1])
//        {
//            zuo_2();
//        }
//        else if(Distance012[1]>=most&&Distance012[2]>=most)
//        {
//            zuo_2();
//        }
//        else
//        {
//            LeftMotor_Forward(0);
//            RightMotor_Forward(250);
//            delay(100);
//        }
    }
}




/***************************************************PART3************************************************************/

int light[5]={0};
int standard=1000;
int light_value()
{
    int mem0;
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;
    while((ADC14->IFGR0&ADC14_IFGR0_IFG0)==0);
    mem0=ADC14->MEM[0];
    ADC14->CTL0 &=~ADC14_CTL0_ENC;
    return mem0;
}
void zuo_1()
{
    RightMotor_Forward(254);
    LeftMotor_Backward(254);
    delay(30);
}
void you_1()
{
    LeftMotor_Forward(254);
    RightMotor_Backward(254);
    delay(30);
}

void part3_init()
{
    P5->SEL1 |= BIT4; // Configure P5.4 for ADC
    P5->SEL0 |= BIT4;
    // Sampling time, S&H=16, ADC14 on
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP |
    ADC14_CTL0_ON;
    ADC14->CTL1 = ADC14_CTL1_RES_2; // Use sampling timer,12-bit conversion results
    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1; // A1 ADC input select;
    // GPIO Setup
    P1->OUT &= ~BIT0; // Clear LED to start
    P1->DIR |= BIT0; // Set P1.0/LED to output
}
void part3()
{
    while (PART==3)
    {//��ת����Ƕȣ�ѡ��light_value()��С�ģ�ת��÷���ǰ��
        part3_init();
        you_1();
        you_1();
        light[0]=light_value();

        zuo_1();
        light[1]=light_value();

        zuo_1();
        light[2]=light_value();

        zuo_1();
        light[3]=light_value();

        zuo_1();
        light[4]=light_value();
        //ͣһ�� debug
        LeftMotor_Forward(0);
        RightMotor_Forward(0);
        delay(20);

        //�ж���Сֵ����
        int min=0;
        int i=0;
        for(i=1;i<5;i++)
        {
            if(light[min]>light[i])
            {
                min=i;
            }
        }
        switch(min)
        {
        case 0:you_1();you_1();you_1();you_1();break;
        case 1:you_1();you_1();you_1();break;
        case 2:you_1();you_1();break;
        case 3:you_1();break;
        case 4:break;
        }

        if(light[1]<standard&&light[2]<standard&&light[3]<standard)
        {
            LeftMotor_Forward(0);//ͣ��
            RightMotor_Forward(0);
            delay(100);

            P1->OUT |= BIT0;//��˸����
            delay(20);
            P1->OUT &=~BIT0;
            delay(20);
            P1->OUT |= BIT0;
            delay(20);
            P1->OUT &=~BIT0;

            PART=4;

            OLED_ShowString(0,0,"Arrived at");
            OLED_ShowString(0,2,"the warehouse");
            OLED_ShowNum(80, 2, count, 4, 16);
            OLED_ShowString(0,4,"successfully!");//ִ����һ������
        }
        else
        {
            LeftMotor_Forward(254);
            RightMotor_Forward(254);
            delay(90);
        }

    }
}




/***************************************************PART4************************************************************/

//part4�ж�ֹͣ
void part4_motor_stop()//�жϺ�ʱֹͣ
{
  if(error==5)//��һ�γ���error=5
  {
     RightMotor_Forward(0);
     LeftMotor_Forward(0);

     int i=0;
     for(i=0;i<6;i++)
     {
         delay(20);
         P1->OUT ^= BIT0;
     }
     Buzzer(95,50);   //do
     Buzzer(85,50);   //re
     Buzzer(75,50);   //mi
     Buzzer(71,50);   //fa
     Buzzer(63,50);   //so
     Buzzer(56,50);   //la
     Buzzer(50,50);   //xi

     Buzzer(95,400);   //do
     Buzzer(85,400);   //re
     Buzzer(75,400);   //mi
     Buzzer(71,400);   //fa
     Buzzer(63,400);   //so
     Buzzer(56,400);   //la
     Buzzer(50,400);   //xi
     PART=5;
     //���У׼ʱ�Ӳ�������ֻ�ϻ�
        /*if(CALBC1_1MHZ==0xFF||CALDCO_1MHZ==0xFF)
             {
                 while(1);
             }*/
            // BCSCTL1=CALBC1_1MHZ;
           //  DCOCTL=CALDCO_1MHZ;   //У׼ʱ��Ƶ��1MHZ
             Buzzer(95,400);   //do
             Buzzer(85,400);   //re
             Buzzer(75,400);   //mi
             Buzzer(95,400);   //do
             _delay_cycles(3000); //ÿ���������ĺ��һ����ʱ

             Buzzer(95,400);   //do
             Buzzer(85,400);   //re
             Buzzer(75,400);   //mi
             Buzzer(95,400);   //do
             _delay_cycles(3000); //ÿ���������ĺ��һ����ʱ

             Buzzer(75,400);   //mi
             Buzzer(71,400);   //fa
             Buzzer(63,400);   //so
            _delay_cycles(3000);

            Buzzer(75,400);   //mi
            Buzzer(71,400);   //fa
            Buzzer(63,400);   //so
             _delay_cycles(3000);

             Buzzer(63,400);   //so
             Buzzer(56,400);   //la
             Buzzer(63,400);   //so
             Buzzer(71,400);   //fa
             Buzzer(75,400);   //mi
             Buzzer(95,400);   //do
             _delay_cycles(3000);

             Buzzer(63,400);   //so
             Buzzer(56,400);   //la
             Buzzer(63,400);   //so
             Buzzer(71,400);   //fa
             Buzzer(75,400);   //mi
             Buzzer(95,400);   //do
             _delay_cycles(3000);

             Buzzer(85,400);   //re
             Buzzer(63,400);   //so
             Buzzer(95,400);   //do
             _delay_cycles(3000);

             Buzzer(85,400);   //re
             Buzzer(63,400);   //so
             Buzzer(95,400);   //do


  }
}

void part4_init()
{
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
}
float part4_read_ir_values() //����С��ƫ����  --->��������Ϊ1
{
    //ͣ��
    if ((sensor.ir_left_3)&&(sensor.ir_left_2)&&(sensor.ir_left_1)&&(sensor.ir_mid)&&(sensor.ir_right_1)&&(sensor.ir_right_2)&&(sensor.ir_right_3))
    {
        error = 5;flag=1;dflag=0;
        part4_motor_stop();
    }
    //ʮ��·��
    else if (((sensor.ir_left_2 == 1)&&(sensor.ir_right_2))||(sensor.ir_left_3&&sensor.ir_right_3))
    {
      cross++;
//      delay(12);//��ֹ���---------------------------------------------------------------
      if(cross==1&&count==1)
      {
          zuo();
      }
      if(cross==1&&count==2)
      {
          you();
      }
      if(cross==1&&count!=1&&count!=2)//��ֹ���cross->��ֹ��ǰת
      {
          RightMotor_Forward(200);
          LeftMotor_Forward(200);
          delay(25);
      }
      if(cross>=2&&count==3)
      {
          zuo();
      }
      if(cross>=2&&count==4)
      {
          you();
      }

    }
    //����ҡ��
    else if (sensor.ir_left_2 == 1)  //��תС
    {
      error = 2;flag=1;dflag=0;
    }

    else if (sensor.ir_left_1 == 1 && sensor.ir_left_2 == 1)
    {
      error = 2;flag=0;dflag=0;           //��תС
    }

    else if (sensor.ir_right_2 == 1)  //��תС
    {
      error = -2;flag=1;dflag=0;
    }

    else if (sensor.ir_right_1 == 1 && sensor.ir_right_2 == 1)
    {
      error = -2;flag=0;dflag=0;           //��תС
    }

    else if (((sensor.ir_mid == 0) && (sensor.ir_left_1 == 0)) || ((sensor.ir_mid == 0) && (sensor.ir_right_1 == 0))) //ֱ��
    {
      error = 0;flag=1;dflag=0;
    }
    else if (sensor.ir_right_3 == 1)   //��ת��
    {
      error = -4;flag=1;dflag=1;
    }
    else if (sensor.ir_right_2 == 1 && sensor.ir_right_3 == 1) //��ת��
    {
      error = -4;flag=1;dflag=0;
    }
    else if (sensor.ir_left_2 == 1 && sensor.ir_left_3 == 1) //��ת��
    {
      error = 4;flag=1;dflag=0;
    }
    return error;
}
void part4()
{  while(PART==4)
    {
    LeftMotor_Forward(254);
    RightMotor_Forward(247);
    delay(60);
    reload_shift_reg();

        while((sensor.ir_left_3==0)&&(sensor.ir_left_2==0)&&(sensor.ir_left_1==0)&&(sensor.ir_mid==0)&&(sensor.ir_right_1==0)&&(sensor.ir_right_2==0)&&(sensor.ir_right_3==0))
         {
            delay(13);//������
            reload_shift_reg();
            if((sensor.ir_left_3==0)&&(sensor.ir_left_2==0)&&(sensor.ir_left_1==0)&&(sensor.ir_mid==0)&&(sensor.ir_right_1==0)&&(sensor.ir_right_2==0)&&(sensor.ir_right_3==0))
             {
                LeftMotor_Forward(254);
                    RightMotor_Forward(200);
                    delay(20);
                reload_shift_reg(); //ˢ�¼Ĵ���
                ir_check();
             }
            else
            {
                RightMotor_Forward(0);
                LeftMotor_Forward(0);
                delay(100);
                break;
            }
         }
        RightMotor_Forward(0);
        LeftMotor_Forward(0);
        delay(100);
//         reload_shift_reg(); //ˢ�¼Ĵ���
         if(sensor.ir_left_2&&sensor.ir_right_2)
         {
             RightMotor_Forward(254);
             LeftMotor_Backward(254);
             delay(40);
         }
         else if(sensor.ir_left_3&&sensor.ir_right_3)
          {
              RightMotor_Forward(254);
              LeftMotor_Backward(254);
              delay(40);
          }
         else if(!sensor.ir_right_2&&sensor.ir_left_2&&!sensor.ir_left_3)//ֻ����2
         {
             RightMotor_Forward(254);
             LeftMotor_Backward(254);
             delay(50);
         }
         else if(sensor.ir_right_2&&!sensor.ir_left_2&&!sensor.ir_right_3)//ֻ����2
         {
             RightMotor_Forward(150);
             LeftMotor_Backward(100);//------------------------------
             delay(10);
         }
         else if(sensor.ir_left_3)
         {
             RightMotor_Forward(254);
             LeftMotor_Backward(254);
             delay(60);
         }
         else if(sensor.ir_right_3)
         {
             RightMotor_Forward(150);
             LeftMotor_Backward(100);//-------------------------------------------
             delay(10);
         }
         else if(sensor.ir_right_1)
          {
              RightMotor_Forward(100);
              LeftMotor_Forward(150);
              delay(10);
          }
         else if(sensor.ir_left_1)
          {
              RightMotor_Forward(254);
              LeftMotor_Backward(254);
              delay(60);
          }
         else//-----------------------------------------------------------------
         {
             RightMotor_Forward(254);
             LeftMotor_Backward(254);
             delay(30);
         }
        while(PART==4)
       {
         reload_shift_reg(); //ˢ�¼Ĵ���
         ir_check();
         part4_read_ir_values(); //ˢ��ת��Ƕ�
         calculate_pid(); //PID����
         motor_control(); //�������
         part4_motor_stop();
         assisted_control();
         switcher_check();
       }
    }
}


/***************************************************MAIN************************************************************/
void main(void)
 {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
    //initial
    part1_init();
    init();//OLED��ʼ��
    OLED_Init();
    choose();
//   PART=4;//-----------------------------------------------------------------------------------
    while(1)
    {
    part1();
    delay(10);

    part2();
    delay(10);

    part3();
    delay(10);
    you_1();//PART3->4��Ҫ��תһ��
    delay(15);//��תһ��--------------------------------------------------------------------------
    part4();
    }

}
