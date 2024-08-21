#include "msp.h"
#include "stdint.h"
#include "Exe01_test.h"

/**
 * main.c
 * Created on: 20230806
 * Author: Zaki
 * Content: lab2, RAM
 */

//char *string = "Hi，micro computer！Enjoy MCU!";

void main(void) {
    int i=0;
    char *RAM_ptr_1 = (char *) 0x20000000;
    char *RAM_ptr_2 = (char *) 0x20001000;

    WDTCTL = WDTPW + WDTHOLD;  // Stop WDT

    P1->DIR |= BIT0;
    P1->OUT &=~ BIT0;

//    while(*string != '\0'){
//        *RAM_ptr++ = *string++;
//    }


    for(i=0; i<NumToWrite; i++){
        *RAM_ptr++ = i;
    }

    RAM_ptr = (uint8_t *) 0x01000080;

    for(i=0; i<NumToWrite; i++){
        if(*RAM_ptr!=i){
            break;
        }
        else{
            RAM_ptr++;
        }
    }




    while(i==NumToWrite){
        P1->OUT ^= BIT0;
        for(j=0;j<50000;j++){  // 7.��Ҫ�����Ŷ࣬ÿ����Ҫ��
            ;//Delay
        }
    }
    while(i<NumToWrite){
        P1OUT = BIT0;
        for(j=0;j<50000;j++){
            ;//Delay
        }
        delay_1ms_block(100);  // 8.���׫д�Լ��ĺ���
    }
}
