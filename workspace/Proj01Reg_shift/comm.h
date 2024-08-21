/*
 * comm.h
 *
 *  Created on: 2022年7月8日
 *      Author: Zaki
 */

#ifndef COMM_H_
#define COMM_H_

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

void delay(int mul)
{
    uint32_t count=mul*5000;
    uint32_t counter;
    for(counter=0;counter<count;counter++)
    {}
}
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
//刷新传感器数据
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

/**
 * main.c
 * 编写程序，能正确读写出KEY1，KEY2按键值以及6个防撞开关状态和红外对管状态，只要有一个防撞开关按下，就在LED1(P1.0)常亮显示，否则不显示。
 * P1.5;        //五号接输出方波信号
   P1.7;        //七号接口输入数据
   P4.6;        //控制SL为输出模式
 */

void key_check()
{
    //检测按键
    if(sensor.key_1||sensor.key_2)    //按键1/2被按下
    {
        delay(3);
        if(sensor.key_1)//红灯
        {
            P2->OUT |= BIT0;
            P2->OUT &=~BIT1;
        }
        else if(sensor.key_2)//绿灯
        {
            P2->OUT |= BIT1;
            P2->OUT &=~BIT0;
        }
    }
}
void switcher_check()
{
    //检测碰撞开关
    if(sensor.switcher_front_left_2||sensor.switcher_front_left_1||sensor.switcher_front_right_1||sensor.switcher_front_right_2||sensor.switcher_back_left||sensor.switcher_back_right)
    {
        delay(3);
        if(sensor.switcher_front_left_2||sensor.switcher_front_right_2)//左2右2为红
        {
            P2->OUT |= BIT0;
            P2->OUT &=~BIT1;
            P2->OUT &=~BIT2;
        }
        else if(sensor.switcher_front_left_1||sensor.switcher_front_right_1)//左1右1为绿
        {
            P2->OUT |= BIT1;
            P2->OUT &=~BIT0;
            P2->OUT &=~BIT2;
        }
        else if(sensor.switcher_back_left||sensor.switcher_back_right)//后左右为蓝
        {
            P2->OUT |= BIT2;
            P2->OUT &=~BIT0;
            P2->OUT &=~BIT1;
        }
    }
}
void ir_check()
{
    //检测红外
    //检测到红外就返回1，被黑线吃掉就为0
    if((!sensor.ir_left_3)||(!sensor.ir_left_2)||(!sensor.ir_left_1)||(!sensor.ir_mid)||(!sensor.ir_right_1)||(!sensor.ir_right_2)||(!sensor.ir_right_3))
    {
        delay(2);
        {
            if((!sensor.ir_left_3)||(!sensor.ir_left_2)||(!sensor.ir_left_1)||(!sensor.ir_mid)||(!sensor.ir_right_1)||(!sensor.ir_right_2)||(!sensor.ir_right_3))
            {
                if(!sensor.ir_left_3)//红
                {
                    P2->OUT |= BIT0;//RED
                    //P2->OUT &=~BIT1;//GREEN
                    //P2->OUT &=~BIT2;//BLUE
                }
                if(!sensor.ir_left_2)//绿
                {
                    P2->OUT |= BIT1;
                    //P2->OUT &=~BIT0;
                    //P2->OUT &=~BIT2;
                }
                if(!sensor.ir_left_1)//红+绿
                {
                    P2->OUT |= BIT1;
                    P2->OUT |= BIT0;
                    //P2->OUT &=~BIT2;
                }
                if(!sensor.ir_mid)//红+绿+蓝
                {
                    P2->OUT |= BIT1;
                    P2->OUT |= BIT0;
                    P2->OUT |= BIT2;
                }
                if(!sensor.ir_right_1)//蓝+绿
                {
                    P2->OUT |= BIT2;
                    P2->OUT |= BIT1;
                    //P2->OUT &=~BIT0;
                }
                if(!sensor.ir_right_2)//红+蓝
                {
                    P2->OUT |= BIT2;
                    P2->OUT |= BIT0;
                    //P2->OUT &=~BIT1;
                }
                if(!sensor.ir_right_3)//蓝
                {
                    P2->OUT |= BIT2;
                    //P2->OUT &=~BIT1;
                    //P2->OUT &=~BIT0;
                }
            }
        }
    }
    else
    {
        P2->OUT &=~BIT2;
        P2->OUT &=~BIT1;
        P2->OUT &=~BIT0;
    }
}

#endif /* COMM_H_ */
