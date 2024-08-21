#include "msp.h"
/*
 * lpm.c
 *
 *  Created on: 2023年8月7日
 *      Author: Zaki
 */

void lpm_goto_lpm4(){
    PCM->CTL1 = PCM_CTL1_KEY_VAL |PCM_CTL1_FORCE_LPM_ENTRY;
    // Setting the sleep deep bit
    SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
    // Do not wake up on exit from ISR
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
    // Ensures SLEEPONEXIT takes effect immediately
    __DSB();
    // Go to LPM4
    __sleep();
}


