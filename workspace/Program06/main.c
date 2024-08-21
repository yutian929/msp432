#include "msp.h"
#include <math.h>
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
 * 20230803鐢佃禌锛屾晳鍛斤紒
 * v0:涓�涓崟鐗囨満鍋氫竴涓猘dc锛屼竴涓富涓�涓粠锛屼袱鑰匞PIO涓柇閫氫俊锛岃涓�涓猼imer锛屼腑鏂疄鐜�
 * v1:A0 5.5        A1 5.4
 * v2:鏈夋垙锛�4閫氶亾鎵弿
 */
int SOUNDVAL[2] = {0, 0};
int adcch = 0;

//int ltime = 0;
//int rtime = 0;
int timeroverflow = 0;
long int deltatime = 0;

int whoisfirst = 0;

long int TIMERCLKFREQ = 30000000;
int intensethreshold_l = 4000;
int intensethreshold_r = 4000;

#define N 200
#define M 4

static uint16_t resultsBuffer[M];

void uart_init()
{
    //    //led initial
        CS->KEY =CS_KEY_VAL;
        CS->CTL0 = 0;
        CS->CTL0 = CS_CTL0_DCORSEL_3; // Set DCO to 12MHz (nominal, center of 8-16MHz range)
        CS->CTL1 = CS_CTL1_SELS_3;    // sMCLK = LFXT
        CS->KEY = 0; // Lock CS module from unintended accesses

        //uart initial
        P3->SEL0 |= BIT2 | BIT3; // set 2-UART pin as secondary function
        EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
        EUSCI_A2->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
                EUSCI_B_CTLW0_SSEL__SMCLK; // Configure eUSCI clock source for SMCLK
    // Baud Rate calculation
    // 12000000/(16*9600) = 78.125
    // Fractional portion = 0.125
    // User's Guide Table 21-4: UCBRSx = 0x00
    // UCBRFx = int ( (78.125-78)*16) = 2
        EUSCI_A2->BRW = 78; // 12000000/16/9600
        EUSCI_A2->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
                EUSCI_A_MCTLW_OS16;
        EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
        EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG; // Clear eUSCI RX interrupt flag



}

void ADC_Config(void)
{
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    /* Initializing ADC (MCLK/1/1) */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_4, ADC_DIVIDER_5, ADC_NOROUTE);

#if M == 1
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);
    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A0, false);
    MAP_ADC14_enableInterrupt(ADC_INT0);

#elif M == 2
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5 | GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);
    MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true); /
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A0, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A1, false);
    MAP_ADC14_enableInterrupt(ADC_INT1);

#elif M == 3
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5 | GPIO_PIN4 | GPIO_PIN3, GPIO_TERTIARY_MODULE_FUNCTION); //
    MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A0, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A1, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM2, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A2, false);
    MAP_ADC14_enableInterrupt(ADC_INT2);

#elif M == 4
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5 | GPIO_PIN4 | GPIO_PIN3 | GPIO_PIN2, GPIO_TERTIARY_MODULE_FUNCTION); //
    MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM3, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A0, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A1, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM2, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A2, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM3, ADC_VREFPOS_INTBUF_VREFNEG_VSS, ADC_INPUT_A3, false);
    MAP_ADC14_enableInterrupt(ADC_INT3);

#endif
    /* Enabling Interrupts */
    MAP_Interrupt_enableInterrupt(INT_ADC14);
    MAP_Interrupt_enableMaster();

    /* Setting up the sample timer to automatically step through the sequence
     * convert.
     */
    MAP_ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

    /* Triggering the start of the sample */
    MAP_ADC14_enableConversion();
    MAP_ADC14_toggleConversionTrigger();
}

void ADC14_IRQHandler(void)
{
    uint8_t i = 0;
    uint_fast64_t status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

#if M == 1
    if (ADC_INT0 & status)
#elif M == 2
    if (ADC_INT1 & status)
#elif M == 3
    if (ADC_INT2 & status)
#elif M == 4
    if (ADC_INT3 & status)
#endif
    {
        MAP_ADC14_getMultiSequenceResult(resultsBuffer);
        for (i = 0; i < M; i++)
            printf("[%d]:%d\r\t", i, resultsBuffer[i]);

        printf("\r\n");
    }
}

void gpio_init(){
    //GPIO Setup
    P1->OUT &= ~BIT0; // Clear LED to start
    P1->DIR |= BIT0; // Set P1.0/LED to output
    P2OUT &=~ (BIT1+BIT0+BIT2);
    P2->DIR |= (BIT0+BIT1+BIT2);
    //P3.6寮曡剼鍒濆鍖�
    P3DIR &=~ BIT6;
    P3REN &=~ BIT6;
    P3OUT &=~ BIT6;

    P3IES &=~ BIT6;
    P3IFG = 0;
    P3IE |= BIT6;

    //閰嶇疆涓柇涓庝綆鍔熻��
    NVIC->ISER[1]=1<<((PORT3_IRQn)&31);  //P1鍏佽琚腑鏂�
}

void PORT3_IRQHandler(void)
{
    if(P3->IFG&BIT6)
    {
    }
    P3->IFG = 0;
}

void cs_initial()
{
    //ACLK璁句负LFXTCLK-32768Hz
    CS->KEY=0x695A;
    CS->CTL1=CS->CTL1&(~CS_CTL1_SELS_MASK)| CS_CTL1_SELS__HFXTCLK ;
    CS->KEY=0;
}

void main(void)
{
        WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
        cs_initial();
        ADC_Config();
        uart_init();
        while (1)
        {
        }
}
