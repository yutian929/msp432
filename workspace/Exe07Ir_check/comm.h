#include "msp.h"


/**
 * main.c
ʵ��2����S1������ȡ����Թܵ��źţ�����LED2��ָʾ���ĸ�����Թ��ں����ϡ�
��S2������ȡ��ײ����״̬����LED1��ָʾ�Ƿ�����ײ��û�а�����ʱ��ϵͳ����LPM4ģʽ��

 */


//�洢���д�����״̬����
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

struct SENSORS sensor;

void comm_init(void)
{
    P4DIR |= BIT6;
    P4OUT |= BIT6;  // P4.6 OUT ->����SL

    P1DIR &=~ BIT7;
    P1REN |= BIT7;  // P1.7 IN ->��ȡ��ƽSOUT

    P1DIR |= BIT5;  // P1.5 OUT ->ʱ��CLK(Ĭ��ʹ��)
}
//ˢ�´���������
void reload_shift_reg()
{
    char i=0;  // index

    P1->OUT &=~ BIT5;  // Ϊ��������,CLK�ȵ�

    P4->OUT &=~BIT6;  // SL�͵�ƽ��Ч����ס�Ĵ���
    __delay_cycles(50000);
    P4->OUT |= BIT6;  // ��Ҫ���˱�ظߵ�ƽ

    sensor.reg0 = 0;  // ��ȡ����Թܵ�8λ�Ĵ���
    sensor.reg1 = 0;  // ��ȡ�������ص�8λ�Ĵ���

    for(i=0;i<8;i++)  // ��ȡ����Թܵ�״̬
    {
        sensor.reg0 <<= 1;  // ����һλ���ճ�һ��0(unsigned char���߼�����)
        if(((P1->IN)&BIT7)==BIT7)  // �����ʱ��SOUT�Ǹߵ�ƽ,�Ͱ����0���1,������0
        {
            sensor.reg0 |= 0x01;
        }
        P1->OUT |= BIT5;  // CLK�����أ���ȡ��һ������Թܵ�״̬
        P1->OUT &=~ BIT5;
    }

    /* ��ʱ,7������Թܵ�״̬��Ϣ������sensor.reg0 (8bit,��һ��bit��û�õ�) �У����������ȡ6���������صĵ�ƽ״̬*/

    for(i=0;i<8;i++)  // ����ͬ��
    {
        sensor.reg1<<=1;
        if(((P1->IN)&BIT7)==BIT7)
        {
            sensor.reg1 |= 0x01;
        }
        P1->OUT |= BIT5;
        P1->OUT &=~ BIT5;
    }

    /* ��ʱ, sensor.reg0 �� sensor.reg1 �ֱ�洢��7������Թܺ�6���������صĵ�ƽ״̬��
     * ���ǵ�Ȼ����������unsigned char����ȥ���߼�����,���ǲ����Ի�,̫�鷳��,��������ת�������ǱȽ�ϲ������ʾ״̬�ķ�ʽ��
     * ��7��ir_XXX(dir)_XXX(num)����Ӧ��ʾÿ������Թܵĵ�ƽ״̬��ͬ������6��switcher_XXX(dir)_XXX(num)����ʾ����*/

    sensor.ir_left_3 = !!(sensor.reg0&(1<<0));  // ���reg0�ĵ�0λΪ1,��ôir_left_3��Ϊ1,��֮Ϊ0
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

/*���ˣ��������ֱ����ir_XXX_XXX, switcher_XXX_XXX��д����Ҫ�ĺ����ˣ����������������ơ���*/


void key_check()
{
    //��ⰴ��
    if(sensor.key_1||sensor.key_2)    //����1/2������
    {
        if(sensor.key_1)//���
        {
            P2->OUT ^= BIT0;
          //  P2->OUT &=~BIT1;
        }
        else if(sensor.key_2)//�̵�
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
    //�����ײ����
   // if(sensor.switcher_front_left_2||sensor.switcher_front_left_1||sensor.switcher_front_right_1||sensor.switcher_front_right_2||sensor.switcher_back_left||sensor.switcher_back_right)
    //{
      //  delay(3);
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
   // }
}

void ir_check()
{
    //������
    //if((sensor.ir_left_3||sensor.ir_left_2||sensor.ir_left_1||sensor.ir_mid||sensor.ir_right_1||sensor.ir_right_2||sensor.ir_right_3))
    //{
    //    delay(2);

             if(!sensor.ir_left_3)//��
                {
                 P2OUT &=~ BIT0;
                  P2OUT &=~ BIT1;
                    P2OUT &=~ BIT2;
                    P2->OUT |= BIT0;


                }
                else if(!sensor.ir_left_2)//��
                {
                    P2OUT &=~ BIT0;
                                         P2OUT &=~ BIT1;
                                         P2OUT &=~ BIT2;
                    P2->OUT |= BIT1;
                }
                else if(!sensor.ir_left_1)//��
                {
                    P2OUT &=~ BIT0;
                                         P2OUT &=~ BIT1;
                                         P2OUT &=~ BIT2;
                    P2->OUT |= BIT1|BIT0;
                }
                else if(!sensor.ir_mid)//��
                {
                    P2OUT &=~ BIT0;
                                         P2OUT &=~ BIT1;
                                         P2OUT &=~ BIT2;
                    P2->OUT |= BIT2;

                }
                else if(!sensor.ir_right_1)//��
                {
                    P2->OUT |= BIT2|BIT0;
                }
                else if(!sensor.ir_right_2)//��
                {
                    P2->OUT |= BIT2|BIT1;
                }
                else if(!sensor.ir_right_3)//��
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


