/*
 * Exe01_test.c
 *
 *  Created on: 2023��8��1��
 *      Author: Zaki
 */

void delay_1ms_block(int t){
    int i = 0;
    long int cut = t*10000;

    while(i <= cut){
        i++;
    }
}


