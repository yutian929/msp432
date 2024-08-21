/*
 * gpio.h
 *
 *  Created on: 2023年8月6日
 *      Author: Zaki
 */

#ifndef GPIO_H_
#define GPIO_H_

void gpio_init();
void gpio_key_onboard_interrupt_init();
void gpio_led_key_onboard_init();
void gpio_red_led_on();
void gpio_green_led_on();
void gpio_blue_led_on();
void gpio_led_off();
char gpio_is_key_pressed();

#endif /* GPIO_H_ */
