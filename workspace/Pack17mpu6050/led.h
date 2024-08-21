/****************************************************/
//MSP432P401R
//LED椹卞姩
//Bilibili锛歮-RNA
//E-mail:m-RNA@qq.com
//鍒涘缓鏃ユ湡:2021/8/10
/****************************************************/

/**************************
  *
  * V1.2 2021/12/27
  * 鏀寔浣嶅甫鎿嶄綔浠ユ彁楂樻晥鐜�
  * 
  * V1.1 2021/9/13 
  * 娣诲姞鍏朵粬棰滆壊 
  * 骞朵负浜嗘敮鎸丮ulti_timer鎵╁睍鍖�
  * 灏嗗畯瀹氫箟淇敼涓哄嚱鏁�
  *
  * V1.0 2021/8/10
  * 瀹屾垚鍩烘湰椹卞姩 
  *
  ************************/

#ifndef __LED_H
#define __LED_H
#include <driverlib.h>

// 浣嶅甫鎿嶄綔
#define LED_RED BITBAND_PERI(P1OUT,0)
#define LED_R BITBAND_PERI(P2OUT,0)
#define LED_G BITBAND_PERI(P2OUT,1)
#define LED_B BITBAND_PERI(P2OUT,2)

void LED_Init(void);

void LED_RED_On(void);
void LED_RED_Off(void);
void LED_RED_Tog(void);

void LED_Y_On(void);
void LED_C_On(void);
void LED_P_On(void);

void LED_R_On(void);
void LED_G_On(void);
void LED_B_On(void);

void LED_R_Off(void);
void LED_G_Off(void);
void LED_B_Off(void);

void LED_R_Tog(void);
void LED_G_Tog(void);
void LED_B_Tog(void);

void LED_W_On(void);
void LED_W_Off(void);
void LED_W_Tog(void);

#endif
