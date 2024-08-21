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
 * 2022/8/20 Zaki 张雨田
 * 使用UART通信协议,用EUSCI_A2通信
 * 查数据手册得
 * P3.2/UCA2RXD
 * P3.3/UCA2TXD
 * 但是不知道为什么用A3就是不行，用A2就是很好，所以可能和蓝牙撞
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
    //uart时钟初始化使用默认SMCLK->DCO：3M
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
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)  // 接收中断，是否为 1，接收完一位
    {
        r =MAP_UART_receiveData(EUSCI_A2_BASE);    // 读取接收到的数据
        if(r=='d')  //全黑
        {
            P2->OUT &=~(BIT0+BIT1+BIT2);
        }
        else if(r=='a') //1红
        {
            P2->OUT |= BIT0;
            P2->OUT &=~(BIT1+BIT2);
        }
        else if(r=='f')//5蓝
        {
            P2->OUT |= BIT2;
            P2->OUT &=~(BIT0+BIT1);
        }
        else if(r=='b')//2红+绿
        {
            P2->OUT |= (BIT0+BIT1);
            P2->OUT &=~BIT2;
        }
        else if(r=='e')//4蓝+绿
        {
            P2->OUT |= (BIT1+BIT2);
            P2->OUT &=~BIT0;
        }
        else if(r=='c')//3绿
        {
            P2->OUT |= BIT1;
            P2->OUT &=~(BIT0+BIT2);
        }
        else//红绿蓝
        {
            P2->OUT |= (BIT0+BIT1+BIT2);
        }
    }
}
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    openmv_init();
    //设置睡眠
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    __DSB();
    __sleep();

}
