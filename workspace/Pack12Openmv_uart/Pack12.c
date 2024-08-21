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
/**
 * main.c
 * 2022/8/20 Zaki ������
 * ʹ��UARTͨ��Э��,��EUSCI_A2ͨ��
 * �������ֲ��
 * P3.2/UCA2RXD
 * P3.3/UCA2TXD
 * ���ǲ�֪��Ϊʲô��A3���ǲ��У���A2���Ǻܺã����Կ��ܺ�����ײ
 */

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
    uint8_t r;
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)  // �����жϣ��Ƿ�Ϊ 1��������һλ
    {
        r =MAP_UART_receiveData(EUSCI_A2_BASE);    // ��ȡ���յ�������
        if(r=='d')  //ȫ��
        {
            P2->OUT &=~(BIT0+BIT1+BIT2);
        }
        else if(r=='a') //1��
        {
            P2->OUT |= BIT0;
            P2->OUT &=~(BIT1+BIT2);
        }
        else if(r=='f')//5��
        {
            P2->OUT |= BIT2;
            P2->OUT &=~(BIT0+BIT1);
        }
        else if(r=='b')//2��+��
        {
            P2->OUT |= (BIT0+BIT1);
            P2->OUT &=~BIT2;
        }
        else if(r=='e')//4��+��
        {
            P2->OUT |= (BIT1+BIT2);
            P2->OUT &=~BIT0;
        }
        else if(r=='c')//3��
        {
            P2->OUT |= BIT1;
            P2->OUT &=~(BIT0+BIT2);
        }
        else//������
        {
            P2->OUT |= (BIT0+BIT1+BIT2);
        }
    }
}
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    openmv_init();
    //����˯��
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    __DSB();
    __sleep();

}
