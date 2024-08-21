#include "msp.h"
#include "gpio.h"

/**
 * main.c
 * date: 20230806
 * content: lab3.1,3.2 gpio->waterflow led, key
 */

void delay_exe03(){
    int i=0;
    for(i=0;i<100000;i++){
        ;
    }
}

void main(void)
{
    int i = 0;

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	gpio_init();

	char key_value = 0;

	while(1){
	    key_value = gpio_is_key_pressed();
	    if (key_value == 1){  // key 1 is pressed
	        gpio_red_led_on();
	    }
	    else if (key_value == 2){  // key 2 is pressed
            gpio_green_led_on();
        }
        else if (key_value == 3){  // both pressed
            gpio_blue_led_on();
        }
        else{
            gpio_led_off();
        }

	}

//	while(1){
//	    gpio_red_led_on();
//	    delay_exe03();
//	    gpio_green_led_on();
//	    delay_exe03();
//	    gpio_blue_led_on();
//	    delay_exe03();
//	}


}
