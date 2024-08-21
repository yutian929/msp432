#include "msp.h"


/**
 * main.c
实验2：按S1键，读取红外对管的信号，并用LED2灯指示是哪个红外对管在黑线上。
按S2键，读取防撞开关状态，用LED1灯指示是否有碰撞。没有按键的时候，系统进入LPM4模式。

 */


//存储所有传感器状态数据
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

struct SENSORS sensor;

void comm_init(void)
{
    P4DIR |= BIT6;
    P4OUT |= BIT6;  // P4.6 OUT ->锁存SL

    P1DIR &=~ BIT7;
    P1REN |= BIT7;  // P1.7 IN ->读取电平SOUT

    P1DIR |= BIT5;  // P1.5 OUT ->时钟CLK(默认使能)
}
//刷新传感器数据
void reload_shift_reg()
{
    char i=0;  // index

    P1->OUT &=~ BIT5;  // 为了上升沿,CLK先低

    P4->OUT &=~BIT6;  // SL低电平有效，锁住寄存器
    __delay_cycles(50000);
    P4->OUT |= BIT6;  // 不要忘了变回高电平

    sensor.reg0 = 0;  // 读取红外对管的8位寄存器
    sensor.reg1 = 0;  // 读取按键开关的8位寄存器

    for(i=0;i<8;i++)  // 读取红外对管的状态
    {
        sensor.reg0 <<= 1;  // 左移一位，空出一个0(unsigned char是逻辑左移)
        if(((P1->IN)&BIT7)==BIT7)  // 如果此时的SOUT是高电平,就把这个0变成1,否则还是0
        {
            sensor.reg0 |= 0x01;
        }
        P1->OUT |= BIT5;  // CLK上升沿，读取下一个红外对管的状态
        P1->OUT &=~ BIT5;
    }

    /* 此时,7个红外对管的状态信息存在了sensor.reg0 (8bit,有一个bit是没用的) 中，下面继续读取6个按键开关的电平状态*/

    for(i=0;i<8;i++)  // 以下同理
    {
        sensor.reg1<<=1;
        if(((P1->IN)&BIT7)==BIT7)
        {
            sensor.reg1 |= 0x01;
        }
        P1->OUT |= BIT5;
        P1->OUT &=~ BIT5;
    }

    /* 此时, sensor.reg0 和 sensor.reg1 分别存储了7个红外对管和6个按键开关的电平状态，
     * 我们当然可以用两个unsigned char变量去做逻辑分析,但是不人性化,太麻烦了,下面我们转换到我们比较喜欢的显示状态的方式：
     * 用7个ir_XXX(dir)_XXX(num)来对应表示每个红外对管的电平状态，同样，用6个switcher_XXX(dir)_XXX(num)来表示……*/

    sensor.ir_left_3 = !!(sensor.reg0&(1<<0));  // 如果reg0的第0位为1,那么ir_left_3就为1,反之为0
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

/*好了，下面可以直接用ir_XXX_XXX, switcher_XXX_XXX编写所需要的函数了，比如检测红外进行亮灯……*/


void key_check()
{
    //检测按键
    if(sensor.key_1||sensor.key_2)    //按键1/2被按下
    {
        if(sensor.key_1)//红灯
        {
            P2->OUT ^= BIT0;
          //  P2->OUT &=~BIT1;
        }
        else if(sensor.key_2)//绿灯
        {
            P2->OUT ^= BIT1;
           // P2->OUT &=~BIT0;
        }
    }
   // P2OUT &=~ BIT0+BIT1+BIT2;
  //  delay(5);
}
void switcher_check()
{
    //检测碰撞开关
   // if(sensor.switcher_front_left_2||sensor.switcher_front_left_1||sensor.switcher_front_right_1||sensor.switcher_front_right_2||sensor.switcher_back_left||sensor.switcher_back_right)
    //{
      //  delay(3);
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
   // }
}

void ir_check()
{
    //检测红外
    //if((sensor.ir_left_3||sensor.ir_left_2||sensor.ir_left_1||sensor.ir_mid||sensor.ir_right_1||sensor.ir_right_2||sensor.ir_right_3))
    //{
    //    delay(2);

             if(!sensor.ir_left_3)//绿
                {
                 P2OUT &=~ BIT0;
                  P2OUT &=~ BIT1;
                    P2OUT &=~ BIT2;
                    P2->OUT |= BIT0;


                }
                else if(!sensor.ir_left_2)//绿
                {
                    P2OUT &=~ BIT0;
                                         P2OUT &=~ BIT1;
                                         P2OUT &=~ BIT2;
                    P2->OUT |= BIT1;
                }
                else if(!sensor.ir_left_1)//绿
                {
                    P2OUT &=~ BIT0;
                                         P2OUT &=~ BIT1;
                                         P2OUT &=~ BIT2;
                    P2->OUT |= BIT1|BIT0;
                }
                else if(!sensor.ir_mid)//红
                {
                    P2OUT &=~ BIT0;
                                         P2OUT &=~ BIT1;
                                         P2OUT &=~ BIT2;
                    P2->OUT |= BIT2;

                }
                else if(!sensor.ir_right_1)//蓝
                {
                    P2->OUT |= BIT2|BIT0;
                }
                else if(!sensor.ir_right_2)//蓝
                {
                    P2->OUT |= BIT2|BIT1;
                }
                else if(!sensor.ir_right_3)//蓝
                {
                    P2->OUT |= BIT2|BIT1|BIT0;
                }
                else
                {  P2OUT &=~ BIT0;
                                         P2OUT &=~ BIT1;
                                         P2OUT &=~ BIT2;

                }

}

void delay(int mul)
{
    uint32_t count=mul*5000;
    uint32_t counter;
    for(counter=0;counter<count;counter++)
    {}
}


