/*
 * adc.h
 *
 *  Created on: 2023年8月10日
 *      Author: Zaki
 */

#ifndef ADC_H_
#define ADC_H_

void adc_b12_m0_ch23_query_init();

void adc_b12_m0_ch23_interrupt_init();

void adc_b12_m0_batv_init();

void adc_start();

int adc_m0_query();

#endif /* ADC_H_ */
