/*
 * pid.h
 *
 *  Created on: 2022��10��22��
 *      Author: Zaki
 */

#ifndef PID_H_
#define PID_H_

int lvelocity=0;
int rvelocity=0;//�������������ٶ�
int SPEEDINDEX=300;//�趨���ٶ�ֵ
int left=19;//��תangle
int mid=50;//ֱ��angle
int right=81;//��תangle
int lspeed=50;//�����ٶ�
int rspeed=50;//�����ٶ�
int recline=0; //�ڼ�������
int stopflag=0;//ͣ����־
int mode=0;//����ѡ��ͣ�����������ʽ��0Ϊ���⣬1Ϊ�෽����P6�ж�
/*************�����б�**************/
//PID�㷨����
//  ��������Kp�����м䵽����
    /*�ɱ����ط�ӳ����ϵͳ��ƫ���źţ�ƫ��һ�������������������������Լ�Сƫ�*/
//  ���ֵ���KI����0����
    /*΢�ֻ��ڵ������ܷ�ӳƫ���źŵı仯���ƣ��仯���ʣ���
    ������ƫ���źŵ�ֵ���̫��֮ǰ����ϵͳ������һ����Ч�����������źţ��Ӷ��ӿ�ϵͳ�Ķ����ٶȣ���С����ʱ�䡣*/
//  ΢�ֵ���KD����0����
    /*���ֿ������õĴ�����ƫ��e(t)�Ĵ���ʱ���йأ�ֻҪϵͳ������ƫ����ֻ��ھͻ᲻�������ã�
    ������ƫ����л��֣�ʹ�������������ִ�����Ŀ��Ȳ��ϱ仯���������������Լ�Сƫ�*/
float Kp = 20, Ki = 0.01, Kd = 10;   //kp��ki��kd�Ǹ��ԵĲ���
int error = 0;
float P = 0, I = 0, D = 0, PID_value = 0;     //error������openmv������ֵ
float previous_error = 0, previous_I = 0;

//ȡ��Сֵ��pidҪ��
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
  PID_value = constrain(PID_value, -50, 50);  //��ʱ������50��

  previous_error = error;

  turn(51+PID_value);   //����PID��С��������е���
}


#endif /* PID_H_ */
