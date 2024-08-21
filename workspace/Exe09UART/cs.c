#include "msp.h"
/*
 * cs.c
 *
 *  Created on: 2023年8月7日
 *      Author: Zaki
 */


void cs_init(void)
{
    // unlock CS module for register access
    CS->KEY = 0x695A;
    //set MCLK source from hfxtclk
    CS->CTL1=CS->CTL1&(~CS_CTL1_SELM_MASK)|CS_CTL1_SELM__HFXTCLK;
    //set MCLK/4
    CS->CTL1=CS->CTL1&(~CS_CTL1_DIVM_MASK)|CS_CTL1_DIVM__1;  // now MCLK = 24MHz
    //set ACLK source from LFXTCLK
    CS->CTL1=CS->CTL1&(~CS_CTL1_SELA_MASK)|CS_CTL1_SELA__LFXTCLK;  // now ACLK = 32768Hz
    //set SMCLK source from LFXTCLK
    CS->CTL1=CS->CTL1&(~CS_CTL1_SELS_MASK)|CS_CTL1_SELS__REFOCLK;  // now SMCLK = REFOCLK_Hz
    //set HFXTFREQ frequency range
    CS->CTL2=CS->CTL2&(~CS_CTL2_HFXTFREQ_MASK)|CS_CTL2_HFXTFREQ_6|CS_CTL2_HFXTDRIVE;  // HFXTCLK = 48MHz
    // lock CS module from unintended access
    CS->KEY = 0;
}

void cs_smclk_12m_init(){
    CS->KEY = CS_KEY_VAL; // Unlock CS module for register access
    CS->CTL0 = 0; // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3; // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 | // Select ACLK = REFO
    CS_CTL1_SELS_3 | // SMCLK = DCO
    CS_CTL1_SELM_3; // MCLK = DCO
    CS->KEY = 0; // Lock CS module from unintended accesses
}


