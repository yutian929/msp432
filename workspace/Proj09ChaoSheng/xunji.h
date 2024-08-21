#include <math.h>

#ifndef XUNJI_H_
#define XUNJI_H_
//PID算法参数
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
//选择病房号,全局变量count指示病房号
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
                if(!(P1IN & BIT1))  //检测S1按下
                {
                    delay(5);
                    do{}while(!(P1IN & BIT1));   //检测S1松开
                    P1->OUT ^= BIT0;
                    count++;
                }
            }while((P1IN & BIT4));//检测S2按下
            P1->OUT &=~BIT0;
            break;
        }
        else if(!(P1->IN&BIT4))
       {
            do
            {
                if(!(P1IN & BIT4))  //检测S2按下
                {
                    delay(5);
                    do{}while(!(P1IN & BIT4));   //检测S2松开
                    P1->OUT ^= BIT0;
                    rcount++;
                }
            }while((P1IN & BIT1));//检测S1按下
            P1->OUT &=~BIT0;
            break;
        }
     }
}
//限制函数
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
//PID算法
void calculate_pid()
{
  P = error;  //偏差的比例
  I = I + error;
  D = error - previous_error;

  PID_value = (Kp * P) + (Ki * I) + (Kd * D);
  PID_value = constrain(PID_value, -50, 50);

  previous_error = error;
//  比例调节Kp：从中间到两边
    /*成比例地反映控制系统的偏差信号，偏差一旦产生，立即产生控制作用以减小偏差。*/
//  积分调节KI：从0到大
    /*微分环节的作用能反映偏差信号的变化趋势（变化速率），
    并能在偏差信号的值变得太大之前，在系统中引入一个有效的早期修正信号，从而加快系统的动作速度，减小调节时间。*/
//  微分调节KD：从0到大
    /*积分控制作用的存在与偏差e(t)的存在时间有关，只要系统存在着偏差，积分环节就会不断起作用，
    对输入偏差进行积分，使控制器的输出及执行器的开度不断变化，产生控制作用以减小偏差。*/
}
//传感器
struct SENSORS
{
    //移位寄存器读取值
    unsigned char reg0;
    unsigned char reg1;

    //下方红外传感器
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
    //周围碰撞开关
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
    //板载按键
    char key_1;
    char key_2;
};
//存储所有传感器状态数据
struct SENSORS sensor;
//delay函数
void delay(int mul)
{
    uint32_t count=mul*2000;
    uint32_t counter;
    for(counter=0;counter<count;counter++)
    {}
}
//刷新传感器数据
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
//红外检测及亮灯
void ir_check()
{
    //检测红外
    //检测到红外就返回0，被黑线吃掉就为1
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
        if(sensor.ir_left_3)//红
        {
            P2->OUT |= BIT0;//RED
        }
        if(sensor.ir_left_2)//绿
        {
            P2->OUT |= BIT0;
        }
        if(sensor.ir_left_1)//红+绿
        {
            P2->OUT |= BIT1;
        }
        if(sensor.ir_mid)//绿
        {
            P2->OUT |= BIT1;
            P2->OUT |= BIT0;
            P2->OUT |= BIT2;
        }
        if(sensor.ir_right_1)//蓝+绿
        {
            P2->OUT |= BIT1;
        }
        if(sensor.ir_right_2)//红+蓝
        {
            P2->OUT |= BIT2;

        }
        if(sensor.ir_right_3)//蓝
        {
            P2->OUT |= BIT2;
        }

    }

}
//马达前进
void LeftMotor_Forward(int rate)
{
    //左轮   P2.7 and P2.4
      P2->SEL0 |= BIT4;//左前
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
    //右轮   P5.6 and P2.6
      P2->SEL0 |= BIT6;//左前
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
    //左轮   P2.7 and P2.4
      P2->SEL0 |= BIT7;//左前
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
    //右轮   P5.6 and P2.6
      P5->SEL0 |= BIT6;//左前
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
//给左右轮速度+LED2灯显示速度
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
//motor的每一步
void motor_step(int left, int right, int step_left, int step_right)
{
    static int left_tar = 0, right_tar = 0;
    static int pwm_l = 0;
    static int pwm_r = 0;
    static float balance_l = 1;
    static float balance_r = 1;

    left=-left;
    right=-right;

    if (left_tar != left)  //设置初始pwm
    {
        pwm_l = left;
        left_tar = left;
    }
    if (right_tar != right)
    {
        pwm_r = right;
        right_tar = right;
    }

    if((left_tar || right_tar) && step_left == 0 && step_right == 0) //设定速度但不指定距离
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
    ((left_tar || right_tar) && (step_left || step_right))//设定速度且指定距离
    {
       volatile unsigned int t_pulse_r = right_pulse;
       volatile unsigned int t_pulse_l = left_pulse;

        motor_set_PWM(left_tar * balance_l, right_tar * balance_r);
    if (!left_tar) //左侧不转
      while (right_pulse - t_pulse_r <= (unsigned int)step_right)
        delay(1);

    else if (!right_tar) //右侧不转
      while (left_pulse - t_pulse_l <= (unsigned int)step_left)
        delay(1);

    else  //都转
      while (right_pulse - t_pulse_r <= (unsigned int)step_right || left_pulse - t_pulse_l <= (unsigned int)step_left)
        delay(1);
    }
    else//停止
        {
          motor_set_PWM(0, 0);
        }

}
//设置电机速度
void motor_control()
{
  //计算每个电机的速度
  left_motor_speed = 10 - PID_value;
  right_motor_speed =10 + PID_value;
  motor_step(left_motor_speed, right_motor_speed,0,0);
}
void assisted_control()
{
    if(flag==0&&dflag==0)//左右小小转
    {
        delay(0);
    }
    else if(flag==1&&dflag==0)//普通左右转
    {
        delay(1);
    }
    else if(flag==0&&dflag==1)//左大转
    {
        LeftMotor_Forward(0);
        delay(DELAY);
    }
    else if(flag==1&&dflag==1)//向右大转
    {
        RightMotor_Forward(0);
        delay(DELAY);
    }

}
//碰撞开关检测+防碰撞
void switcher_check()
{

    if(sensor.switcher_front_left_2||sensor.switcher_front_left_1||sensor.switcher_front_right_1||sensor.switcher_front_right_2||sensor.switcher_back_left||sensor.switcher_back_right)
    {
        delay(3);
        if(sensor.switcher_front_left_2||sensor.switcher_front_right_2)//左2右2为红
        {
            P2->OUT |= BIT0;
            P2->OUT &=~BIT1;
            P2->OUT &=~BIT2;
            if(sensor.switcher_front_left_2) hitcase=1;
                       else hitcase=4;

        }
        else if(sensor.switcher_front_left_1||sensor.switcher_front_right_1)//左1右1为绿
        {
            P2->OUT |= BIT1;
            P2->OUT &=~BIT0;
            P2->OUT &=~BIT2;
            if(sensor.switcher_front_left_1) hitcase=2;
            else hitcase=3;
        }
        else if(sensor.switcher_back_left||sensor.switcher_back_right)//后左右为蓝
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
//左转函数
void zuo()
{
    RightMotor_Forward(254);
    LeftMotor_Forward(0);
    delay(DELAY);
}
//右转函数
void you()
{
    RightMotor_Forward(0);
    LeftMotor_Forward(254);
    delay(DELAY);
}
//出发时
void start()
{
    LeftMotor_Forward(200);
    RightMotor_Forward(200);
    delay(DELAY);
}
float read_ir_values() //计算小车偏移量  --->遇到黑线为1
{
    //停车
    if (cross>0&&(sensor.ir_mid == 1) && (sensor.ir_left_1 == 1) && (sensor.ir_left_2 == 1) && (sensor.ir_left_3 == 1) && (sensor.ir_right_1 == 1) && (sensor.ir_right_2 == 1) && (sensor.ir_right_3 == 1))
    {
        error = 5;flag=1;dflag=0;
        motor_stop();
    }
    //十字路口
    else if (((sensor.ir_left_2 == 1)&&(sensor.ir_right_2))||(sensor.ir_left_3&&sensor.ir_right_3))
    {
      cross++;
      //delay(12);//防止多读
      if(cross==1&&count==1)
      {
          zuo();
      }
      if(cross==1&&count==2)
      {
          you();
      }
      if(cross==1&&count!=1&&count!=2)//防止多读cross->防止提前转
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
    //正常摇摆
    else if (sensor.ir_left_2 == 1)  //左转小
    {
      error = 2;flag=1;dflag=0;
    }

    else if (sensor.ir_left_1 == 1 && sensor.ir_left_2 == 1)
    {
      error = 2;flag=0;dflag=0;           //左转小
    }

    else if (sensor.ir_right_2 == 1)  //右转小
    {
      error = -2;flag=1;dflag=0;
    }

    else if (sensor.ir_right_1 == 1 && sensor.ir_right_2 == 1)
    {
      error = -2;flag=0;dflag=0;           //右转小
    }

    else if (((sensor.ir_mid == 0) && (sensor.ir_left_1 == 0)) || ((sensor.ir_mid == 0) && (sensor.ir_right_1 == 0))) //直行
    {
      error = 0;flag=1;dflag=0;
    }
    else if (sensor.ir_right_3 == 1)   //右转大
    {
      error = -4;flag=1;dflag=1;
    }
    else if (sensor.ir_right_2 == 1 && sensor.ir_right_3 == 1) //右转大
    {
      error = -4;flag=1;dflag=0;
    }
    else if (sensor.ir_left_2 == 1 && sensor.ir_left_3 == 1) //左转大
    {
      error = 4;flag=1;dflag=0;
    }
    return error;
}
//返回时
int turn=0;
float rread_ir_values() //计算小车偏移量  --->遇到黑线为1
{
    error=0;
    //停车
    if ((sensor.ir_mid == 1) && (sensor.ir_left_1 == 1) && (sensor.ir_left_2 == 1) && (sensor.ir_left_3 == 1) && (sensor.ir_right_1 == 1) && (sensor.ir_right_2 == 1) && (sensor.ir_right_3 == 1))
    {
        error = 5;flag=1;dflag=0;
        motor_stop();
    }
    //卜字路口
    else if (sensor.ir_right_3 == 1)//右转大,只有5678有用
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
    else if (sensor.ir_left_3 == 1)//左转大,只有5678有用
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
    //十字路口
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
    else if (sensor.ir_right_2 == 1 && sensor.ir_right_3 == 1) //右转大
    {
        error = -4;flag=1;dflag=0;
    }
    else if (sensor.ir_left_2 == 1 && sensor.ir_left_3 == 1) //左转大
    {
        error = 4;flag=1;dflag=0;
    }
    else if (sensor.ir_left_2 == 1)  //左转小
    {
        error = 2;flag=1;dflag=0;
    }
    else if (sensor.ir_left_1 == 1 && sensor.ir_left_2 == 1) //左转小
    {
        error = 2;flag=0;dflag=0;
    }
    else if (sensor.ir_right_2 == 1)  //右转小
    {
        error = -2;flag=1;dflag=0;
    }
    else if (sensor.ir_right_1 == 1 && sensor.ir_right_2 == 1) //右转小
    {
        error = -2;flag=0;dflag=0;
    }
    else if (((sensor.ir_mid == 0) && (sensor.ir_left_1 == 0)) || ((sensor.ir_mid == 0) && (sensor.ir_right_1 == 0))) //直行
    {
        error = 0;flag=1;dflag=0;
    }
    return error;
}
//判断停止
void motor_stop()//判断何时停止
{
  if(error==5)//第一次出现error=5
  {
//     delay(1);
//     reload_shift_reg();//刷新寄存器
//     read_ir_values(); //刷新转弯角度
//     calculate_pid();  //PID计算
//     motor_control();  //电机控制
//     assisted_control();
//     while(error==5)//再次出现error=5
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
