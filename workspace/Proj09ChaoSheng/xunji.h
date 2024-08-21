#include <math.h>

#ifndef XUNJI_H_
#define XUNJI_H_
//PID�㷨����
float Kp = 20, Ki = 0.01, Kd = 10;
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float previous_error = 0, previous_I = 0;
int   left_motor_speed = 0;
int   right_motor_speed = 0;
int   motor_flag = 0;
extern unsigned int left_pulse = 0;
extern unsigned int right_pulse = 0;
int   a=0,b=0,c=0;
int   left_velocity = 0;
int   right_velocity = 0;
int   left_update = 0;
int   right_update = 0;
int   flag=0;
int   dflag=0;
int   hitcase=0;
int   count=0,rcount=0;
int   cross=0,rcross=0;
int   DELAY=80;
int   flag_back=0;
//CS
void cs_init(void)
{
    CS->KEY=0x695A;
    CS->CTL1=CS->CTL1&(~CS_CTL1_SELA_MASK)|CS_CTL1_SELA__LFXTCLK;
    CS->KEY=0;
}
//ѡ�񲡷���,ȫ�ֱ���countָʾ������
void choose()
{
    P1DIR &=~ (BIT1+BIT4);
    P1REN |= BIT1+BIT4;
    P1OUT |= BIT1+BIT4;
    while(1)
    {
        if(!(P1->IN&BIT1))
        {
            do
            {
                if(!(P1IN & BIT1))  //���S1����
                {
                    delay(5);
                    do{}while(!(P1IN & BIT1));   //���S1�ɿ�
                    P1->OUT ^= BIT0;
                    count++;
                }
            }while((P1IN & BIT4));//���S2����
            P1->OUT &=~BIT0;
            break;
        }
        else if(!(P1->IN&BIT4))
       {
            do
            {
                if(!(P1IN & BIT4))  //���S2����
                {
                    delay(5);
                    do{}while(!(P1IN & BIT4));   //���S2�ɿ�
                    P1->OUT ^= BIT0;
                    rcount++;
                }
            }while((P1IN & BIT1));//���S1����
            P1->OUT &=~BIT0;
            break;
        }
     }
}
//���ƺ���
float constrain(float mid,float low,float up)
{
    if(mid<low)
    {
        mid=low;
    }
    else if(mid>up)
    {
        mid=up;
    }
    return mid;
}
//PID�㷨
void calculate_pid()
{
  P = error;  //ƫ��ı���
  I = I + error;
  D = error - previous_error;

  PID_value = (Kp * P) + (Ki * I) + (Kd * D);
  PID_value = constrain(PID_value, -50, 50);

  previous_error = error;
//  ��������Kp�����м䵽����
    /*�ɱ����ط�ӳ����ϵͳ��ƫ���źţ�ƫ��һ�������������������������Լ�Сƫ�*/
//  ���ֵ���KI����0����
    /*΢�ֻ��ڵ������ܷ�ӳƫ���źŵı仯���ƣ��仯���ʣ���
    ������ƫ���źŵ�ֵ���̫��֮ǰ����ϵͳ������һ����Ч�����������źţ��Ӷ��ӿ�ϵͳ�Ķ����ٶȣ���С����ʱ�䡣*/
//  ΢�ֵ���KD����0����
    /*���ֿ������õĴ�����ƫ��e(t)�Ĵ���ʱ���йأ�ֻҪϵͳ������ƫ����ֻ��ھͻ᲻�������ã�
    ������ƫ����л��֣�ʹ�������������ִ�����Ŀ��Ȳ��ϱ仯���������������Լ�Сƫ�*/
}
//������
struct SENSORS
{
    //��λ�Ĵ�����ȡֵ
    unsigned char reg0;
    unsigned char reg1;

    //�·����⴫����
    //                         ir_mid
    //                ir_left_1      ir_right_1
    //        ir_left_2                       ir_right_2
    //ir_left_3                                        ir_right_3
    char ir_left_1;
    char ir_left_2;
    char ir_left_3;
    char ir_mid;
    char ir_right_1;
    char ir_right_2;
    char ir_right_3;
    //��Χ��ײ����
    //        switcher_front_left1    switcher_front_right1
    //switcher_front_left2                    switcher_front_right2
    //
    //
    //    switcher_back_left              switcher_back_right
    char switcher_front_left_1;
    char switcher_front_left_2;
    char switcher_front_right_1;
    char switcher_front_right_2;
    char switcher_back_left;
    char switcher_back_right;
    //���ذ���
    char key_1;
    char key_2;
};
//�洢���д�����״̬����
struct SENSORS sensor;
//delay����
void delay(int mul)
{
    uint32_t count=mul*2000;
    uint32_t counter;
    for(counter=0;counter<count;counter++)
    {}
}
//ˢ�´���������
void LOAD()
{

    P4->OUT &=~BIT6;
    delay(5);
    P4->OUT |= BIT6;
}
void CLK0()
{

    P1->OUT &=~BIT5;
}
void CLK1()
{

    P1->OUT |= BIT5;
}
int MISO()
{
    //P1->OUT &=~BIT7;
    if(((P1->IN)&BIT7)==BIT7)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}
void reload_shift_reg(void)
{
    char i=0;
    CLK0();
    LOAD();
    sensor.reg0 = 0;
    sensor.reg1 = 0;

    for(i=0;i<8;i++)
    {
        sensor.reg0<<=1;
        if(MISO())
        {
            sensor.reg0 |= 0x01;
        }
        CLK1();
        CLK0();
    }
    for(i=0;i<8;i++)
    {
        sensor.reg1<<=1;
        if(MISO())
        {
            sensor.reg1 |= 0x01;
        }
        CLK1();
        CLK0();
    }
    sensor.ir_left_3 = !!(sensor.reg0&(1<<0));
    sensor.ir_left_2 = !!(sensor.reg0&(1<<1));
    sensor.ir_left_1 = !!(sensor.reg0&(1<<2));
    sensor.ir_mid = !!(sensor.reg0&(1<<3));
    sensor.ir_right_1 = !!(sensor.reg0&(1<<4));
    sensor.ir_right_2 = !!(sensor.reg0&(1<<5));
    sensor.ir_right_3 = !!(sensor.reg0&(1<<6));

    sensor.switcher_front_left_2 = (!(sensor.reg1&(1<<0)));
    sensor.switcher_front_left_1 = (!(sensor.reg1&(1<<1)));
    sensor.switcher_front_right_1 = (!(sensor.reg1&(1<<2)));
    sensor.switcher_front_right_2 = (!(sensor.reg1&(1<<3)));
    sensor.switcher_back_left = (!(sensor.reg1&(1<<4)));
    sensor.switcher_back_right = (!(sensor.reg1&(1<<5)));

    sensor.key_1 = (!(sensor.reg1&(1<<6)));
    sensor.key_2 = (!(sensor.reg1&(1<<7)));

}
//�����⼰����
void ir_check()
{
    //������
    //��⵽����ͷ���0�������߳Ե���Ϊ1
    P2->OUT &=~BIT2;
    P2->OUT &=~BIT1;
    P2->OUT &=~BIT0;
    if((sensor.ir_left_3)&&(sensor.ir_left_2)&&(sensor.ir_left_1)&&(sensor.ir_mid)&&(sensor.ir_right_1)&&(sensor.ir_right_2)&&(sensor.ir_right_3))
    {
        P2->OUT &=~BIT2;
        P2->OUT &=~BIT1;
        P2->OUT &=~BIT0;
    }
    else
    {
        if(sensor.ir_left_3)//��
        {
            P2->OUT |= BIT0;//RED
        }
        if(sensor.ir_left_2)//��
        {
            P2->OUT |= BIT0;
        }
        if(sensor.ir_left_1)//��+��
        {
            P2->OUT |= BIT1;
        }
        if(sensor.ir_mid)//��
        {
            P2->OUT |= BIT1;
            P2->OUT |= BIT0;
            P2->OUT |= BIT2;
        }
        if(sensor.ir_right_1)//��+��
        {
            P2->OUT |= BIT1;
        }
        if(sensor.ir_right_2)//��+��
        {
            P2->OUT |= BIT2;

        }
        if(sensor.ir_right_3)//��
        {
            P2->OUT |= BIT2;
        }

    }

}
//���ǰ��
void LeftMotor_Forward(int rate)
{
    //����   P2.7 and P2.4
      P2->SEL0 |= BIT4;//��ǰ
      P2->SEL1 &= ~BIT4;
      P2->DIR |=  BIT4;
      TIMER_A0->CCR[0] = 255;
      TIMER_A0->CCTL[1] = OUTMOD_7;
      TIMER_A0->CCR[1] = rate*1/3;
      TIMER_A0->CTL = TASSEL_1+MC_1+TACLR;
      P2->SEL0 &= ~BIT7;
      P2->SEL1 &= ~BIT7;
      P2->DIR |= BIT7;
      P2->OUT &= ~BIT7;
}
void RightMotor_Forward(int rate)
{
    //����   P5.6 and P2.6
      P2->SEL0 |= BIT6;//��ǰ
      P2->SEL1 &= ~BIT6;
      P2->DIR |=  BIT6;
      TIMER_A0->CCR[0] = 255;
      TIMER_A0->CCTL[3] = OUTMOD_7;
      TIMER_A0->CCR[3] = rate*1/3;
      TIMER_A0->CTL = TASSEL_1+MC_1+TACLR;
      P5->SEL0 &= ~BIT6;
      P5->SEL1 &= ~BIT6;
      P5->DIR |= BIT6;
      P5->OUT &= ~BIT6;
}
void LeftMotor_Backward(int rate)
{
    //����   P2.7 and P2.4
      P2->SEL0 |= BIT7;//��ǰ
      P2->SEL1 &= ~BIT7;
      P2->DIR |=  BIT7;
      TIMER_A0->CCR[0] = 255;
      TIMER_A0->CCTL[4] = OUTMOD_7;
      TIMER_A0->CCR[4] = rate*1/3;
      TIMER_A0->CTL = TASSEL_1+MC_1+TACLR;
      P2->SEL0 &= ~BIT4;
      P2->SEL1 &= ~BIT4;
      P2->DIR |= BIT4;
      P2->OUT &= ~BIT4;
}
void RightMotor_Backward(int rate)
{
    //����   P5.6 and P2.6
      P5->SEL0 |= BIT6;//��ǰ
      P5->SEL1 &= ~BIT6;
      P5->DIR |=  BIT6;
      TIMER_A2->CCR[0] = 255;
      TIMER_A2->CCTL[1] = OUTMOD_7;
      TIMER_A2->CCR[1] = rate*1/3;
      TIMER_A2->CTL = TASSEL_1+MC_1+TACLR;
      P2->SEL0 &= ~BIT6;
      P2->SEL1 &= ~BIT6;
      P2->DIR |= BIT6;
      P2->OUT &= ~BIT6;
}
//���������ٶ�+LED2����ʾ�ٶ�
void motor_set_PWM(int left, int right)
{
    if(fabs(left)>50||fabs(right)>50)
    {
        P2->OUT |= BIT0;
        P2->OUT &=~(BIT1+BIT2);
//        OLED_Part_Clear(2,4,90,6);
//        OLED_ShowString(2,4," HIGH! ");
    }
    else if(fabs(left)>20||fabs(right)>20)
    {
        P2->OUT |= BIT1;
        P2->OUT &=~(BIT0+BIT2);
//        OLED_Part_Clear(2,4,90,6);
//        OLED_ShowString(2,4," MID!  ");
    }
    else
    {
        P2->OUT |= BIT2;
        P2->OUT &=~(BIT0+BIT1);
//        OLED_Part_Clear(2,4,90,6);
//        OLED_ShowString(2,4," LOW!  ");
    }
  if (left >= 0)
  {
      LeftMotor_Forward(left);
  }
  else
  {
      LeftMotor_Forward(255+left);
  }

  if (right >= 0)
  {
      RightMotor_Forward(right);

  }
  else
  {
      RightMotor_Forward(255+right);
  }
}
//motor��ÿһ��
void motor_step(int left, int right, int step_left, int step_right)
{
    static int left_tar = 0, right_tar = 0;
    static int pwm_l = 0;
    static int pwm_r = 0;
    static float balance_l = 1;
    static float balance_r = 1;

    left=-left;
    right=-right;

    if (left_tar != left)  //���ó�ʼpwm
    {
        pwm_l = left;
        left_tar = left;
    }
    if (right_tar != right)
    {
        pwm_r = right;
        right_tar = right;
    }

    if((left_tar || right_tar) && step_left == 0 && step_right == 0) //�趨�ٶȵ���ָ������
    {
        if (left_velocity > abs(left_tar))    pwm_l>0 ? pwm_l-- : pwm_l++; else left_tar>0 ? pwm_l++ : pwm_l--;
        if (right_velocity > abs(right_tar))  pwm_r>0 ? pwm_r-- : pwm_r++; else right_tar>0 ? pwm_r++ : pwm_r--;

        if (pwm_l > 255) pwm_l = 255;
        if (pwm_r > 255) pwm_r = 255;
        if (pwm_l < -255) pwm_l = -255;
        if (pwm_r < -255) pwm_r = -255;

        if (left_tar != 0)
        {
            balance_l = abs((float)(pwm_l) / (float)(left_tar));
            if (balance_l > 1.5) balance_l = 1.5;
            if (balance_l < 0.66) balance_l = 0.66;
        }
        if (right_tar != 0)
        {
            balance_r = abs((float)(pwm_r) / (float)(right_tar));
            if (balance_r > 1.5) balance_r = 1.5;
            if (balance_r < 0.66) balance_r = 0.66;
        }

        motor_set_PWM(pwm_l, pwm_r);

        left_update++;
        right_update++;
        if (left_update >= 10)
        {
            left_velocity = 0;
            left_update = 1;
        }
        if (right_update >= 10)
        {
            right_velocity = 0;
            right_update = 1;
        }
        delay(1);
    }
   else if
    ((left_tar || right_tar) && (step_left || step_right))//�趨�ٶ���ָ������
    {
       volatile unsigned int t_pulse_r = right_pulse;
       volatile unsigned int t_pulse_l = left_pulse;

        motor_set_PWM(left_tar * balance_l, right_tar * balance_r);
    if (!left_tar) //��಻ת
      while (right_pulse - t_pulse_r <= (unsigned int)step_right)
        delay(1);

    else if (!right_tar) //�Ҳ಻ת
      while (left_pulse - t_pulse_l <= (unsigned int)step_left)
        delay(1);

    else  //��ת
      while (right_pulse - t_pulse_r <= (unsigned int)step_right || left_pulse - t_pulse_l <= (unsigned int)step_left)
        delay(1);
    }
    else//ֹͣ
        {
          motor_set_PWM(0, 0);
        }

}
//���õ���ٶ�
void motor_control()
{
  //����ÿ��������ٶ�
  left_motor_speed = 10 - PID_value;
  right_motor_speed =10 + PID_value;
  motor_step(left_motor_speed, right_motor_speed,0,0);
}
void assisted_control()
{
    if(flag==0&&dflag==0)//����ССת
    {
        delay(0);
    }
    else if(flag==1&&dflag==0)//��ͨ����ת
    {
        delay(1);
    }
    else if(flag==0&&dflag==1)//���ת
    {
        LeftMotor_Forward(0);
        delay(DELAY);
    }
    else if(flag==1&&dflag==1)//���Ҵ�ת
    {
        RightMotor_Forward(0);
        delay(DELAY);
    }

}
//��ײ���ؼ��+����ײ
void switcher_check()
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
        else if(sensor.switcher_front_left_1||sensor.switcher_front_right_1)//��1��1Ϊ��
        {
            P2->OUT |= BIT1;
            P2->OUT &=~BIT0;
            P2->OUT &=~BIT2;
            if(sensor.switcher_front_left_1) hitcase=2;
            else hitcase=3;
        }
        else if(sensor.switcher_back_left||sensor.switcher_back_right)//������Ϊ��
        {
            P2->OUT |= BIT2;
            P2->OUT &=~BIT0;
            P2->OUT &=~BIT1;
            if(sensor.switcher_front_left_1) hitcase=5;
            else hitcase=6;
        }
    }
    hit_def();
}
void hit_def()
{
   switch(hitcase)
   {
   case 1:{LeftMotor_Backward(180);RightMotor_Backward(180);delay(70);LeftMotor_Forward(left_motor_speed);RightMotor_Forward(right_motor_speed);break;}
   case 2:{LeftMotor_Backward(180);RightMotor_Backward(180);delay(70);LeftMotor_Forward(left_motor_speed);RightMotor_Forward(right_motor_speed);break;}
   case 3:{LeftMotor_Backward(180);RightMotor_Backward(180);delay(70);LeftMotor_Forward(left_motor_speed);RightMotor_Forward(right_motor_speed);break;}
   case 4:{LeftMotor_Backward(180);RightMotor_Backward(180);delay(70);LeftMotor_Forward(left_motor_speed);RightMotor_Forward(right_motor_speed);break;}
   case 5:{LeftMotor_Forward(180);RightMotor_Forward(180);delay(70);LeftMotor_Forward(left_motor_speed);RightMotor_Forward(right_motor_speed);break;}
   case 6:{LeftMotor_Forward(180);RightMotor_Forward(180);delay(70);LeftMotor_Forward(left_motor_speed);RightMotor_Forward(right_motor_speed);break;}
   default:hitcase=0;break;
   }
   hitcase=0;

}
//��ת����
void zuo()
{
    RightMotor_Forward(254);
    LeftMotor_Forward(0);
    delay(DELAY);
}
//��ת����
void you()
{
    RightMotor_Forward(0);
    LeftMotor_Forward(254);
    delay(DELAY);
}
//����ʱ
void start()
{
    LeftMotor_Forward(200);
    RightMotor_Forward(200);
    delay(DELAY);
}
float read_ir_values() //����С��ƫ����  --->��������Ϊ1
{
    //ͣ��
    if (cross>0&&(sensor.ir_mid == 1) && (sensor.ir_left_1 == 1) && (sensor.ir_left_2 == 1) && (sensor.ir_left_3 == 1) && (sensor.ir_right_1 == 1) && (sensor.ir_right_2 == 1) && (sensor.ir_right_3 == 1))
    {
        error = 5;flag=1;dflag=0;
        motor_stop();
    }
    //ʮ��·��
    else if (((sensor.ir_left_2 == 1)&&(sensor.ir_right_2))||(sensor.ir_left_3&&sensor.ir_right_3))
    {
      cross++;
      //delay(12);//��ֹ���
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
      if(cross==2&&count!=3&&count!=4)
      {
          RightMotor_Forward(200);
          LeftMotor_Forward(200);
          delay(25);
      }
      if(cross==3&&(count==5||count==7))
      {
          zuo();
      }
      if(cross==3&&(count==6||count==8))
      {
          you();
      }
      if(cross==4&&(count==5||count==6))
      {
          you();
      }
      if(cross==4&&(count==7||count==8))
      {
          zuo();
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
//����ʱ
int turn=0;
float rread_ir_values() //����С��ƫ����  --->��������Ϊ1
{
    error=0;
    //ͣ��
    if ((sensor.ir_mid == 1) && (sensor.ir_left_1 == 1) && (sensor.ir_left_2 == 1) && (sensor.ir_left_3 == 1) && (sensor.ir_right_1 == 1) && (sensor.ir_right_2 == 1) && (sensor.ir_right_3 == 1))
    {
        error = 5;flag=1;dflag=0;
        motor_stop();
    }
    //����·��
    else if (sensor.ir_right_3 == 1)//��ת��,ֻ��5678����
    {
        delay(2);
        if((rcount==7||rcount==8)&&turn==0)
        {
            turn++;
            you();
        }
        else if((rcount==5||rcount==7)&&turn==1)
        {
            turn++;
            you();
        }
    }
    else if (sensor.ir_left_3 == 1)//��ת��,ֻ��5678����
    {
        delay(2);
        if((rcount==5||rcount==6)&&turn==0)
        {
            turn++;
            zuo();
            delay(15);
        }
        else if((rcount==6||rcount==8)&&turn==1)
        {
            turn++;
            zuo();
            delay(25);
        }
    }
    //ʮ��·��
    else if ((sensor.ir_left_2 == 1)&&(sensor.ir_right_2))
    {
        rcross++;
        delay(5);
        if(rcross==1&&rcount==1)
        {
            you();
        }
        if(rcross==1&&rcount==2)
        {
            zuo();
        }
        if(rcross==1&&rcount==3)
        {
            you();
        }
        if(rcross==1&&rcount==4)
        {
            zuo();
        }
    }
    else if (sensor.ir_right_2 == 1 && sensor.ir_right_3 == 1) //��ת��
    {
        error = -4;flag=1;dflag=0;
    }
    else if (sensor.ir_left_2 == 1 && sensor.ir_left_3 == 1) //��ת��
    {
        error = 4;flag=1;dflag=0;
    }
    else if (sensor.ir_left_2 == 1)  //��תС
    {
        error = 2;flag=1;dflag=0;
    }
    else if (sensor.ir_left_1 == 1 && sensor.ir_left_2 == 1) //��תС
    {
        error = 2;flag=0;dflag=0;
    }
    else if (sensor.ir_right_2 == 1)  //��תС
    {
        error = -2;flag=1;dflag=0;
    }
    else if (sensor.ir_right_1 == 1 && sensor.ir_right_2 == 1) //��תС
    {
        error = -2;flag=0;dflag=0;
    }
    else if (((sensor.ir_mid == 0) && (sensor.ir_left_1 == 0)) || ((sensor.ir_mid == 0) && (sensor.ir_right_1 == 0))) //ֱ��
    {
        error = 0;flag=1;dflag=0;
    }
    return error;
}
//�ж�ֹͣ
void motor_stop()//�жϺ�ʱֹͣ
{
  if(error==5)//��һ�γ���error=5
  {
//     delay(1);
//     reload_shift_reg();//ˢ�¼Ĵ���
//     read_ir_values(); //ˢ��ת��Ƕ�
//     calculate_pid();  //PID����
//     motor_control();  //�������
//     assisted_control();
//     while(error==5)//�ٴγ���error=5
//     {
       while(1)
           {
             RightMotor_Forward(0);
             LeftMotor_Forward(0);
             rcount=count;
             if(!(P1->IN&BIT4))
             {
               flag_back=1;   break;
             }

           }
//     }
  }
}




#endif /* XUNJI_H_ */
