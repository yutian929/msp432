/*
 * pid.h
 *
 *  Created on: 2022年10月22日
 *      Author: Zaki
 */

#ifndef PID_H_
#define PID_H_

int lvelocity=0;
int rvelocity=0;//编码电机读出的速度
int SPEEDINDEX=300;//设定的速度值
int left=19;//左转angle
int mid=50;//直行angle
int right=81;//右转angle
int lspeed=50;//左轮速度
int rspeed=50;//右轮速度
int recline=0; //第几条横线
int stopflag=0;//停车标志
int mode=0;//用来选择停车后的启动方式，0为倒库，1为侧方，见P6中断
/*************参数列表**************/
//PID算法参数
//  比例调节Kp：从中间到两边
    /*成比例地反映控制系统的偏差信号，偏差一旦产生，立即产生控制作用以减小偏差。*/
//  积分调节KI：从0到大
    /*微分环节的作用能反映偏差信号的变化趋势（变化速率），
    并能在偏差信号的值变得太大之前，在系统中引入一个有效的早期修正信号，从而加快系统的动作速度，减小调节时间。*/
//  微分调节KD：从0到大
    /*积分控制作用的存在与偏差e(t)的存在时间有关，只要系统存在着偏差，积分环节就会不断起作用，
    对输入偏差进行积分，使控制器的输出及执行器的开度不断变化，产生控制作用以减小偏差。*/
float Kp = 20, Ki = 0.01, Kd = 10;   //kp、ki、kd是各自的参数
int error = 0;
float P = 0, I = 0, D = 0, PID_value = 0;     //error可以是openmv反馈的值
float previous_error = 0, previous_I = 0;

//取最小值，pid要用
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
  PID_value = constrain(PID_value, -50, 50);  //暂时限制在50内

  previous_error = error;

  turn(51+PID_value);   //利用PID对小车方向进行调整
}


#endif /* PID_H_ */
