/*
 * ram.c
 *
 *  Created on: 2023年8月6日
 *      Author: Zaki
 */

void ram_write_string(char* ram_addr, char* string){
    while(*string != '\0'){
        *ram_addr = *string;
        ram_addr++;
        string++;
    }
    *ram_addr = '\0';
    return;
}

char ram_string_iseq(char* ptr1, char* ptr2){
    while(*ptr1 && *ptr2){
        if (*ptr1 == *ptr2){
            ptr1++;
            ptr2++;
        }
        else{
            return 0;
        }
    }
    if (*ptr1 == '\0' && *ptr2 == '\0'){
        return 1;
    }
    else{
        return 0;
    }
}




