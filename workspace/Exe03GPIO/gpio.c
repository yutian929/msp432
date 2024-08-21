#include "msp.h"
/*
 * gpio.c
 *
 *  Created on: 2023年8月6日
 *      Author: Zaki
 */



void gpio_init(){
    // set P2.0/1/2 as output
    P2->DIR |= (BIT0|BIT1|BIT2);
    P2->OUT &=~ (BIT0|BIT1|BIT2);  // led off initially

    // set key P1.1/4 as input with pull-up
    P1->DIR &=~ (BIT1|BIT4);
    P1->REN |= (BIT1|BIT4);  // pull-up
    P1->OUT |= (BIT1|BIT4);

//    P1->REN |= (BIT0|BIT4);  // pull-down
//    P1->OUT &=~ (BIT0|BIT4);

}

void gpio_red_led_on(){  // only red
    P2->OUT |= BIT0;  // red on
    P2->OUT &=~ BIT1;  // green off
    P2->OUT &=~ BIT2;  // blue off
}
void gpio_green_led_on(){
    P2->OUT |= BIT1;
    P2->OUT &=~ BIT0;
    P2->OUT &=~ BIT2;
}
void gpio_blue_led_on(){
    P2->OUT |= BIT2;
    P2->OUT &=~ BIT1;
    P2->OUT &=~ BIT0;
}

void gpio_led_off(){
    P2->OUT &=~ (BIT0|BIT1|BIT2);
}

char gpio_is_key_pressed(){  // key_id = 1/2, if pressed, return 1/2 respectively, else return 0
    if (P1->IN & (BIT1|BIT4) == (BIT1|BIT4)){
        return 0;// no key is pressed
    }
    else{  // any key is pressed
        int i = 3000;
        for(;i>0;i--){
            ;  // delay, smooth shack
        }
        if (!(P1->IN & BIT1) && (P1->IN & BIT4)){  // only key 1.10 is pressed
            return 1;
        }
        else if (!(P1->IN & BIT4) && (P1->IN & BIT1)){  // only key 1.4 is pressed
            return 2;
        }
        else if (!(P1->IN & BIT1) && !(P1->IN & BIT4)){  // both pressed
            return 3;
        }
        else{
            return 0;
        }
    }
}


