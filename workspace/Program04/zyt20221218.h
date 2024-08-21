/*
 * zyt.h
 *
 *  Created on: 2022��9��1��
 *      Author: Zaki
 */
#include <driverlib.h>
#ifndef ZYT_H_
#define ZYT_H_

//LED-P20-P21-P22-P10
void led_init();/*2.0,2.1,2.2,1.0�ĳ�ʼ��*/

//LPM
void go_LPM();/*����˯��״̬,sleepdeep=1*/

//delay��ʱ
#define clock_source 3    //ʱ��Ƶ����д�������λMHz��
void delay_us(unsigned long us);

void time_count_start(int i);/*���i*0.25s����һ��systick���ж�,�жϺ�����zyt.c*/

//Buzzer����-P25
void buzzer(int n,int j);/*����������,n��Ƶ�ʣ�j�ǳ���ʱ��(�����趨��zyt.c)*/

//Photoresistance����
void Photoresistance_init_suspend();/*��ѯģʽ*/

int light_value_suspend();/*���ع�������ֵ,ԽС,��ǿԽǿ*/

int LIGHTVAL;/*�ж�ģʽ*/

void Photoresistance_init_interrupt();

void light_value_start();/*startһ��,�ı�һ��ȫ�ֱ���*/

//SoundIntensity��ǿ
void soundIntensit_init_interrupt();/*3.7�ڵ��жϣ������ﵽһ���̶�(�ɵ�)�ͻ�����жϣ���zyt.c*/

//Bluetooth����
void bluetooth_init();/*3.2RX,3.3TX,���յ���Ϣ�����EUSCIA2���ж�,��zyt.c,����ʵ�����ӻ���Ϣ�����շ������ȴ�������*/

//IrDistance������
void Ir_distance_init();/*������,���ص�ѹģ��ֵ����ڻƽ�vcc,gnd,5.5*/

int Ir_distance();

//openmv����ͷ
void openmv_init();/*��ʼ��openmv,��uart2,3.2RX,3.3TX �ǵ���openmv��main��ҲҪ��ʼ����������ͬ������9600,֮����յ���λһ�ֽ���Ϣ��ͻ�����ж�,��zyt.c*/

//KeyArray�������
void KeyArray_init();/*���ƾ������,����6.4 6.5 6.6 6.7,����2.3 2.5 2.6 2.7*/

char Key();/*û������,Ĭ�Ϸ���'\0'*/

//SG90���
void servo_init();//TIMERA1���PWM�����ƣ��źſ�P2.4

void servo_angle(int ang);//angle from 12~13~44~76~87

//HCSR04����
void HCSR04_Init_suspend();/*��ѯģʽ,6.4Trig 6.5Echo*/

float distance_suspend();/*���ؾ��룬��һ�����볤֮��ʧЧ*/

void HCSR04_Init_interrupt(void);/*�ж�ģʽ,����Ч���������������,6.4Trig 6.5Echo,�ҿɲⳤ����,��Ҳ����С����*/

void trig();/*����һ�θߵ�ƽ,֮��������ж�,��zyt.c*/


//WIFI_ESP8266
/*��Lab_WIFI_ESP8266,ֱ�ӵ��ü���*/

//ICM20948
/*��Lab_ICM20948,ֱ�ӵ��ü���*/

//OLED
/*����ļ�oled.h,ֱ�ӵ��ü���,�Լ�д��һ������,��zyt.c*/

//TB6612�������
void TB6612_init();//P82,P84->left direction,P85,P86->right direction,P24->PWMA,P26->PWMB
void motor_control(int a,int b,int apwm,int bpwm);//a,b��Ӧ����������˶�����0ֹͣ��1��ת��2��ת��3�ƶ�
//P50P52��A��ת��,P54P55��B��ת��

#endif /* ZYT_H_ */
