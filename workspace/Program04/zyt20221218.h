/*
 * zyt.h
 *
 *  Created on: 2022年9月1日
 *      Author: Zaki
 */
#include <driverlib.h>
#ifndef ZYT_H_
#define ZYT_H_

//LED-P20-P21-P22-P10
void led_init();/*2.0,2.1,2.2,1.0的初始化*/

//LPM
void go_LPM();/*进入睡眠状态,sleepdeep=1*/

//delay延时
#define clock_source 3    //时钟频率填写到这里（单位MHz）
void delay_us(unsigned long us);

void time_count_start(int i);/*间隔i*0.25s进入一次systick的中断,中断函数见zyt.c*/

//Buzzer蜂鸣-P25
void buzzer(int n,int j);/*蜂鸣器函数,n是频率，j是持续时间(具体设定见zyt.c)*/

//Photoresistance光敏
void Photoresistance_init_suspend();/*查询模式*/

int light_value_suspend();/*返回光敏电阻值,越小,光强越强*/

int LIGHTVAL;/*中断模式*/

void Photoresistance_init_interrupt();

void light_value_start();/*start一次,改变一次全局变量*/

//SoundIntensity声强
void soundIntensit_init_interrupt();/*3.7口的中断，声音达到一定程度(可调)就会进入中断，见zyt.c*/

//Bluetooth蓝牙
void bluetooth_init();/*3.2RX,3.3TX,接收到消息后进入EUSCIA2的中断,见zyt.c,如若实现主从机消息互相收发，需先串口配置*/

//IrDistance红外测距
void Ir_distance_init();/*红外测距,返回电压模拟值，红黑黄接vcc,gnd,5.5*/

int Ir_distance();

//openmv摄像头
void openmv_init();/*初始化openmv,用uart2,3.2RX,3.3TX 记得在openmv的main中也要初始化，配置相同比特率9600,之后接收到八位一字节信息后就会进入中断,见zyt.c*/

//KeyArray矩阵键盘
void KeyArray_init();/*自制矩阵键盘,引发6.4 6.5 6.6 6.7,接收2.3 2.5 2.6 2.7*/

char Key();/*没键按下,默认返回'\0'*/

//SG90舵机
void servo_init();//TIMERA1输出PWM波控制，信号口P2.4

void servo_angle(int ang);//angle from 12~13~44~76~87

//HCSR04超声
void HCSR04_Init_suspend();/*查询模式,6.4Trig 6.5Echo*/

float distance_suspend();/*返回距离，但一定距离长之后失效*/

void HCSR04_Init_interrupt(void);/*中断模式,可有效避免主程序的阻塞,6.4Trig 6.5Echo,且可测长距离,但也存在小问题*/

void trig();/*产生一段高电平,之后便会进入中断,见zyt.c*/


//WIFI_ESP8266
/*见Lab_WIFI_ESP8266,直接调用即可*/

//ICM20948
/*见Lab_ICM20948,直接调用即可*/

//OLED
/*更多的见oled.h,直接调用即可,自己写了一个函数,见zyt.c*/

//TB6612驱动电机
void TB6612_init();//P82,P84->left direction,P85,P86->right direction,P24->PWMA,P26->PWMB
void motor_control(int a,int b,int apwm,int bpwm);//a,b对应两个电机的运动方向，0停止，1正转，2反转，3制动
//P50P52测A的转速,P54P55测B的转速

#endif /* ZYT_H_ */
