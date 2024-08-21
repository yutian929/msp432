/*****************************************************
�����ܣ�MSP430F149��ESP8266ʵ��ͨ��
------------------------------------------------------
ͨ�Ÿ�ʽ��N.8.1, 115200
------------------------------------------------------
����˵�����򿪴��ڵ��Ծ��飬��ȷ����ͨ�Ÿ�ʽ���۲���Ļ
******************************************************/
#include <msp430x14x.h>
#include "uart.h"
#include "clock.h"
#include "esp8266.h"
//
unsigned char  DelayFlag=0; //��ʱ��־λ


//main
void main(void)
{

    WDTCTL = WDTPW + WDTHOLD;  // �ع�
    Mcu_ClockInit();           //ʱ�ӳ���ʼ����8MHz
    Mcu_UartInit();            //UART��ʼ��������UART0��UART1��
    Mcu_TimerA0Start();        //TimerA0��ʼ��-���ڽ���300ms��ʱ
    ESP8266_AT_Init();

    while(1)
    {
         Mcu_Uart0SendString("AT+CIPSEND=0,20\r\n");      //��PC��ӡlog
         Mcu_Uart1SendString("AT+CIPSEND=0,20\r\n");      //��ESP8266����ָ��
         delay_ms(1000);   //1000ms��ʱ

         Mcu_Uart0SendString("Hello World\r\n");              //��PC��ӡlog
         Mcu_Uart1SendString("Hello World\r\n");
         delay_ms(1000);
    }
}

/******************��ʱ300ms�жϷ����ӳ���**********/
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void){

   DelayFlag=1; //do nothing

}
//
#pragma vector = USART0RX_VECTOR
__interrupt void USART0_RXISR(void)
{
    TXBUF0=RXBUF0;               //�����յ������ݷ���ȥ
    while((UTCTL0 & 0X01)==0);   //�ȴ����ݷ������
}
#pragma vector = USART1RX_VECTOR
__interrupt void USART1_RXISR(void)
{

    //TXBUF1=RXBUF1;

    //while((UTCTL1 & 0X01)==0);   //�ȴ����ݷ������
}












