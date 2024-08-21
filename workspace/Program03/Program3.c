#include "msp.h"

#include <driverlib.h>
#include "adc14.c"
#include "aes256.c"
#include "comp_e.c"
#include "cpu.c"
#include "crc32.c"
#include "dma.c"
#include "eusci.h"
#include "flash.c"
#include "fpu.c"
#include "gpio.c"
#include "i2c.c"
#include "interrupt.c"
#include "mpu.c"
#include "pcm.c"
#include "pmap.c"
#include "pss.c"
#include "ref_a.c"
#include "reset.c"
#include "rtc_c.c"
#include "spi.c"
#include "sysctl.c"
#include "systick.c"
#include "timer32.c"
#include "timer_a.c"
#include "uart.c"
#include "wdt_a.c"
#include <cs.c>

#include "oled.h"
/**
 * main.c
 * ���̿��ƶ��ת����openmv�ɼ�ͼ��ת��Ϊ����ͼ����OLED����ʾ
 */
int ANGLE=44;
char PICTURE[64][128];
char tempPICTURE[64][128];
char prePICTURE[64][128];
void servo_init()
{   //P2.4/PM_TA0.1
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_1 | // ACLK
                    TIMER_A_CTL_MC_1|TIMER_A_CTL_CLR; // up mode
    P2->DIR |= BIT4; // P2.6-7 option selection TA0.3-4
    P2->SEL0 |= BIT4;
    P2->SEL1 &=~(BIT4);
}
//angle from 12~13~44~76~87
void servo_angle(int ang)
{
    // Configure GPIO
    TIMER_A0->CCR[0]=655;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCR[1]=ang;
}
//1.1 1.4�޸�ת���Ƕ�
void PORT1_IRQHandler()
{
    int j=0;
    for(j=10000;j>0;j--);//����
    //˫��
    if((P1->IFG&(BIT1|BIT4))==(BIT1|BIT4))
    {
        P5->OUT |= BIT5;
    }
    //����
    else
    {
        if(P1->IFG&BIT1)//��ת
        {
            ANGLE+=3;
            if(ANGLE>=76)
            {
                ANGLE=76;
            }
        }
        else if(P1->IFG&BIT4)//��ת
        {
            ANGLE-=3;
            if(ANGLE<=13)
            {
                ANGLE=13;
            }
        }
        servo_angle(ANGLE);
    }
    P1->IFG =0;

}

const eUSCI_UART_Config uartConfig2 =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source->3MHz->9600 BRD
     19,                                      // BRDIV
     8,                                       // UCxBRF
     0,                                       // UCxBRS
     EUSCI_A_UART_NO_PARITY,                  // No Parity
     EUSCI_A_UART_LSB_FIRST,                  // MSB First
     EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
     EUSCI_A_UART_MODE,                       // UART mode
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
};
//OLED
void OLED_Print()//�Լ�д��һ���������������黭ͼ
{
    int x=0,y=0;
    for(x=0;x<128;x++)//���д�ӡ
    {
        for(y=0;y<8;y++)//ÿ�а˸�������
        {
            unsigned char temp=0,index=0;
            OLED_Set_Pos(x,y);
            for(index=0;index<8;index++)//ÿ�����������8�����ص�
            {
                if(PICTURE[y*8+index][x]=='1')
                {
                    temp |= (1<<index);//��λ��������8λ������temp����0~127
                }
            }
            OLED_WR_Byte(temp,1);//temp����ֵ����8�����ص������(��2��8�η���)
        }
    }
    //����󣬴���tempPICTURE��PICTURE����ָ���ʼ״̬
    for(x=0;x<128;x++)
    {
        for(y=0;y<64;y++)
        {
            tempPICTURE[y][x]=PICTURE[y][x];
            PICTURE[y][x]='0';
        }
    }
}
//openmv
void openmv_init()
{
    //uartʱ�ӳ�ʼ��ʹ��Ĭ��SMCLK->DCO��3M
    //uart initial
    P3->SEL0 |= (BIT2 | BIT3); // set 3-UART pin as secondary function
    P3->SEL1 &=~(BIT2 | BIT3);

    //![Simple UART Example]z
    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig2);
    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A2_BASE);
    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
    MAP_Interrupt_enableMaster();
    //LED init
    P2->DIR |= (BIT0+BIT1+BIT2);
    P2->OUT &=~(BIT0+BIT1+BIT2);
}
void EUSCIA2_IRQHandler(void)
{
    static int i=0,j=0;
    //ָʾ��
    P1->OUT |= BIT0;
    uint8_t r='0';
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)  // �����жϣ��Ƿ�Ϊ 1��������һλ
    {
        r =MAP_UART_receiveData(EUSCI_A2_BASE);    // ��ȡ���յ�������
        PICTURE[i][j]=r;
    }
    j++;
    if(j==128)
    {
        i++;
        j=0;
    }
    if(i==64)
    {
        //һ�βɼ����
        i=0;
        P1->OUT &=~BIT0;
        P5->OUT &=~BIT5;//�ر�openmv
        OLED_Clear();//����
        //�Լ�д��һ����������������64x128��ͼ
        OLED_Print();
    }
    P1->OUT &=~BIT0;
}
//KeyArray
void KeyArray_init()
{
    //KeyArray initial
    P6->DIR |= (BIT4+BIT5+BIT6+BIT7);
    P6->OUT &=~(BIT4+BIT5+BIT6+BIT7);
    P2->DIR &=~(BIT3+BIT5+BIT6+BIT7);
    P2->REN |= (BIT3+BIT5+BIT6+BIT7);
    P2->OUT &=~(BIT3+BIT5+BIT6+BIT7);//������Ϊ�����͵�ƽ
}


void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	//P1.0
	P1->DIR |= BIT0;
	P1->OUT &=~BIT0;
    //P1.1,1.4
    P1->DIR &=~(BIT1|BIT4);
    P1->REN |= (BIT1|BIT4);
    P1->OUT |= (BIT1|BIT4);
    P1->IE |= (BIT1|BIT4);
    P1->IES |= (BIT1|BIT4);
    P1->IFG =0;
    //NVIC
    NVIC->ISER[1] = 1<<3;
    __enable_interrupts();
    //Servo
	servo_init();
	servo_angle(ANGLE);
    //P5.5��openmv������
    P5->DIR |= BIT5;
    P5->OUT &=~BIT5;
    //openmv
    openmv_init();
    //OLED
    init();
    OLED_Init();

//    //�ն����exit�����˯
//    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
//    //����˯
//    __DSB();
//    //����LPM
//    __sleep();

    //KeyArray
    KeyArray_init();
    char KEYVALUE='\0';
    int i=0;
    while(1)
    {
        KEYVALUE='\0';
        //�����������Ϊ�ߵ�ƽ
        //����ɨ��:��1��
        P6->OUT |= BIT4;
        if(P2->IN&(BIT3+BIT5+BIT6+BIT7))
        {
            for(i=20000;i>0;i--);//����
            if( (P2->IN&BIT3))
            {
                KEYVALUE='1';
            }
            else if( (P2->IN&BIT5))
            {
                KEYVALUE='2';
            }
            else if( (P2->IN&BIT6))
            {
                KEYVALUE='3';
            }
            else if( (P2->IN&BIT7))
            {
                KEYVALUE='A';
            }
        }
        P6->OUT &=~BIT4;
        //����ɨ��:��2��
        P6->OUT |= BIT5;
        if(P2->IN&(BIT3+BIT5+BIT6+BIT7))
        {
            for(i=20000;i>0;i--);//����
            if( (P2->IN&BIT3))
            {
                KEYVALUE='4';
            }
            else if( (P2->IN&BIT5))
            {
                KEYVALUE='5';
            }
            else if( (P2->IN&BIT6))
            {
                KEYVALUE='6';
            }
            else if( (P2->IN&BIT7))
            {
                KEYVALUE='B';
            }
        }
        P6->OUT &=~BIT5;
        //����ɨ��:��3��
        P6->OUT |= BIT6;
        if(P2->IN&(BIT3+BIT5+BIT6+BIT7))
        {
            for(i=20000;i>0;i--);//����
            if( (P2->IN&BIT3))
            {
                KEYVALUE='7';
            }
            else if( (P2->IN&BIT5))
            {
                KEYVALUE='8';
            }
            else if( (P2->IN&BIT6))
            {
                KEYVALUE='9';
            }
            else if( (P2->IN&BIT7))
            {
                KEYVALUE='C';
            }
        }
        P6->OUT &=~BIT6;
        //����ɨ��:��4��
        P6->OUT |= BIT7;
        if(P2->IN&(BIT3+BIT5+BIT6+BIT7))
        {
            for(i=20000;i>0;i--);//����
            if( (P2->IN&BIT3))
            {
                KEYVALUE='#';
            }
            else if( (P2->IN&BIT5))
            {
                KEYVALUE='0';
            }
            else if( (P2->IN&BIT6))
            {
                KEYVALUE='*';
            }
            else if( (P2->IN&BIT7))
            {
                KEYVALUE='D';
            }
        }
        P6->OUT &=~BIT7;
        __no_operation();


        //����KEYVALUEѡ��״̬
        int px=0,py=0;
        int tx=0,ty=0;
        switch(KEYVALUE)
        {
        case 'C':P5->OUT |= BIT5;break;//capture picture
        case 'A':ANGLE+=2;if(ANGLE>=76){ANGLE=76;}servo_angle(ANGLE);break;//left
        case 'B':ANGLE-=2;if(ANGLE<=13){ANGLE=13;}servo_angle(ANGLE);break;//right
        case '0':
            //store tempPICTURE to prePICTURE
            P1->OUT |= BIT0;
            for(tx=0;tx<128;tx++)
            {
                for(ty=0;ty<64;ty++)
                {
                    prePICTURE[ty][tx]=tempPICTURE[ty][tx];
                }
            }
            P1->OUT &=~BIT0;
            break;
        case 'D':
            //show prePICTURE
            for(px=0;px<128;px++)//���д�ӡ
            {
                for(py=0;py<8;py++)//ÿ�а˸�������
                {
                    unsigned char temp=0,index=0;
                    OLED_Set_Pos(px,py);
                    for(index=0;index<8;index++)//ÿ�����������8�����ص�
                    {
                        if(prePICTURE[py*8+index][px]=='1')
                        {
                            temp |= (1<<index);//��λ��������8λ������temp����0~127
                        }
                    }
                    OLED_WR_Byte(temp,1);//temp����ֵ����8�����ص������(��2��8�η���)
                }
            }
            break;
        default:break;
        }


    }


}


//char Key()
//{

//    return KEYVALUE;
//}
//    //KeyArray
//    KeyArray_init();
//  while(1)
//  {
//      KEYVALUE=Key();

//  }
