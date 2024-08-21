/*
 * comm.h
 *
 *  Created on: 2022��7��8��
 *      Author: Zaki
 */

#ifndef COMM_H_
#define COMM_H_

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
//ˢ�´���������
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
 * ��д��������ȷ��д��KEY1��KEY2����ֵ�Լ�6����ײ����״̬�ͺ���Թ�״̬��ֻҪ��һ����ײ���ذ��£�����LED1(P1.0)������ʾ��������ʾ��
 * P1.5;        //��Ž���������ź�
   P1.7;        //�ߺŽӿ���������
   P4.6;        //����SLΪ���ģʽ
 */

void key_check()
{
    //��ⰴ��
    if(sensor.key_1||sensor.key_2)    //����1/2������
    {
        delay(3);
        if(sensor.key_1)//���
        {
            P2->OUT |= BIT0;
            P2->OUT &=~BIT1;
        }
        else if(sensor.key_2)//�̵�
        {
            P2->OUT |= BIT1;
            P2->OUT &=~BIT0;
        }
    }
}
void switcher_check()
{
    //�����ײ����
    if(sensor.switcher_front_left_2||sensor.switcher_front_left_1||sensor.switcher_front_right_1||sensor.switcher_front_right_2||sensor.switcher_back_left||sensor.switcher_back_right)
    {
        delay(3);
        if(sensor.switcher_front_left_2||sensor.switcher_front_right_2)//��2��2Ϊ��
        {
            P2->OUT |= BIT0;
            P2->OUT &=~BIT1;
            P2->OUT &=~BIT2;
        }
        else if(sensor.switcher_front_left_1||sensor.switcher_front_right_1)//��1��1Ϊ��
        {
            P2->OUT |= BIT1;
            P2->OUT &=~BIT0;
            P2->OUT &=~BIT2;
        }
        else if(sensor.switcher_back_left||sensor.switcher_back_right)//������Ϊ��
        {
            P2->OUT |= BIT2;
            P2->OUT &=~BIT0;
            P2->OUT &=~BIT1;
        }
    }
}
void ir_check()
{
    //������
    //��⵽����ͷ���1�������߳Ե���Ϊ0
    if((!sensor.ir_left_3)||(!sensor.ir_left_2)||(!sensor.ir_left_1)||(!sensor.ir_mid)||(!sensor.ir_right_1)||(!sensor.ir_right_2)||(!sensor.ir_right_3))
    {
        delay(2);
        {
            if((!sensor.ir_left_3)||(!sensor.ir_left_2)||(!sensor.ir_left_1)||(!sensor.ir_mid)||(!sensor.ir_right_1)||(!sensor.ir_right_2)||(!sensor.ir_right_3))
            {
                if(!sensor.ir_left_3)//��
                {
                    P2->OUT |= BIT0;//RED
                    //P2->OUT &=~BIT1;//GREEN
                    //P2->OUT &=~BIT2;//BLUE
                }
                if(!sensor.ir_left_2)//��
                {
                    P2->OUT |= BIT1;
                    //P2->OUT &=~BIT0;
                    //P2->OUT &=~BIT2;
                }
                if(!sensor.ir_left_1)//��+��
                {
                    P2->OUT |= BIT1;
                    P2->OUT |= BIT0;
                    //P2->OUT &=~BIT2;
                }
                if(!sensor.ir_mid)//��+��+��
                {
                    P2->OUT |= BIT1;
                    P2->OUT |= BIT0;
                    P2->OUT |= BIT2;
                }
                if(!sensor.ir_right_1)//��+��
                {
                    P2->OUT |= BIT2;
                    P2->OUT |= BIT1;
                    //P2->OUT &=~BIT0;
                }
                if(!sensor.ir_right_2)//��+��
                {
                    P2->OUT |= BIT2;
                    P2->OUT |= BIT0;
                    //P2->OUT &=~BIT1;
                }
                if(!sensor.ir_right_3)//��
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
