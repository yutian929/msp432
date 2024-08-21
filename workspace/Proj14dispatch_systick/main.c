#include "msp.h"
#include "cs.h"
#include "systick.h"
#include "lpm.h"
/**
 * main.c
 * date: 20230809
 * content: extended task, dispatch of systick
 */

long int cnt_ms = 0;
long int cnt_s = 0;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	cs_init();  // core clock = 24MHz
	gpio_led_key_onboard_init();
	systick_init();
	systick_start();
	lpm_goto_lpm4();
	__no_operation();
}
void SysTick_Handler(void)
{
    SysTick->VAL = 0x01;
    cnt_ms += 1;
    if (cnt_ms == 1000){  // every 1s
        cnt_ms = 0;
        cnt_s += 1;
        // test
        P1->OUT ^= BIT0;
    }
}
