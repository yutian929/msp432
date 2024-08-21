#include "msp.h"

unsigned char Is_key_pressed(void);
void delay_ms(int microsend);

/**
 * main.c
 */
void main(void)
{

    int keycode=0;
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	// intial IO
	 P1->DIR |= BIT0; // P1.0 set output
	 P1->OUT &=  ~BIT0;//turn off led
	  //Setting key port P2.6 P2.7
	 P1->DIR  &= ~(BIT1+BIT4);//setting IO for input
	 P1->REN  |=BIT1+BIT4;// pull enable
	 P1->OUT  |=BIT1+BIT4;//pull up

	 while(1)
	  {
	     keycode=Is_key_pressed();
	     switch(keycode)
	     {

	       case 1:  {P1->OUT |=BIT0;break; }
	       case 2:  {P1->OUT &= ~BIT0;break;}
	       default: break;
	     }

	  }

}
 unsigned char Is_key_pressed(void)
	   {
	    if ((P1IN & (BIT1+BIT4))==(BIT1+BIT4))
	       return 0;            //no key pressed}
	    else
	       {delay_ms(1000); // wait 10mS for debounce.
	          if ((P1IN & BIT1) == 0)    // is key1 still has pressed status after 10mS delay?
	             {return 1;        }
	           if ((P1IN & BIT4) == 0)    // is key2 still has pressed status after 10mS delay?
	           { return 2;   }
	          return 0;
	       }
	   }

void delay_ms(int microsend)
{
  int i;
  for (i=microsend;i>0;i--) ;
}
