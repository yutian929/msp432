#include "msp.h"
#include "ram.h"

/**
 * main.c
 * 20230806
 * content: lab2,RAM
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	char* string = "Hi，micro computer！Enjoy MCU!";  // end with '\0'
	char* ram_ptr1 = (char*)0x20000000;
	char* ram_ptr2 = (char*)0x20001000;
//
//	// write ram
//	do{
//	    *string_ptr1 = *string;
//	    *string_ptr2 = *string;
//	    string++;ram_ptr1++;ram_ptr1++;
//	}while(*string != '\0');
//	*ram_ptr1 = '\0';  // add '\0' to form a complete string
//	*ram_ptr1 = '\0';
//
	ram_write_string(ram_ptr1, string);
	ram_write_string(ram_ptr2, string);

	__no_operation();
//
//	// test if equal
	char isequal;
//
//	string_ptr1 = (char*)0x20000000;
//	string_ptr2 = (char*)0x20001000;
//
	isequal = ram_string_iseq(ram_ptr1, ram_ptr2);
//
//
	// led
	P1->DIR |= BIT0;
	P1->OUT &=~ BIT0;
	int j = 0;
	while(1){
	    if (isequal){
	        P1->OUT ^= BIT0;
	        for(j=0;j<50000;j++){  // 7.��Ҫ�����Ŷ࣬ÿ����Ҫ��
	            ;//Delay
	        }
	    }
	    else{
	        P1OUT = BIT0;
	        for(j=0;j<50000;j++){
	            ;//Delay
	        }
	    }
	}
//
//
//


}
