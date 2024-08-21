/*
 * timer.h
 *
 *  Created on: 2023年8月7日
 *      Author: Zaki
 */

#ifndef TIMER_H_
#define TIMER_H_

void timer_A0_com_init();

void timer_A2_cap_init();

void timer_A0_1_4_pwm_init(int ccr0);

void timer_A0_1_pwm_on(int ccr1);

void timer_A0_4_pwm_on(int ccr4);

void timer_A0_1_pwm_off();

void timer_A0_4_pwm_off();

#endif /* TIMER_H_ */
