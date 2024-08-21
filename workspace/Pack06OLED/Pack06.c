#include "msp.h"
#include <math.h>
#include <driverlib.h>
#include "adc14.c"
#include "aes256.c"
#include "comp_e.c"
#include "cpu.c"
#include "crc32.c"
#include "dma.c"
#include "eusci.h"
#include "flash.c"
#include "fpu.c"
#include "gpio.c"
#include "i2c.c"
#include "interrupt.c"
#include "mpu.c"
#include "pcm.c"
#include "pmap.c"
#include "pss.c"
#include "ref_a.c"
#include "reset.c"
#include "rtc_c.c"
#include "spi.c"
#include "sysctl.c"
#include "systick.c"
#include "timer32.c"
#include "timer_a.c"
#include "uart.c"
#include "wdt_a.c"
#include <cs.c>
#include "oled.h"
/**
 * main.c
 * 2022/8/29 Zaki
 * 直接调用库函数，初始化有两个
 * 引脚在oled.c里面查看
 */

int a[10][10]={0};


char PICTURE[64][128];
//自己写的一个函数，根据数组画图
void OLED_Print()
{
    int x=0,y=0;
    for(x=0;x<128;x++)//逐列打印
    {
        for(y=0;y<8;y++)//每列八个操作点
        {
            unsigned char temp=0,index=0;
            OLED_Set_Pos(x,y);
            for(index=0;index<8;index++)//每个操作点包含8个像素点
            {
                if(PICTURE[y*8+index][x]=='1')
                {
                    temp |= (1<<index);//移位操作，共8位，最终temp属于0~127
                }
            }
            OLED_WR_Byte(temp,1);//temp的数值决定8个像素点的亮法(共2的8次方种)
        }
    }
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	//OLED初始化
	init();
    OLED_Init();
    while(1){
        OLED_ShowChar(3,3,'L');
        OLED_ShowNum(3,3,12345,12,16);
        OLED_ShowChar(5,5,'R');
        OLED_ShowNum(5,5,54321,12,16);
        // OLED_Clear();
    }
    //OLED_ShowChar(8,8,'A');

    //OLED_ShowChar(1,3,'B');
    //OLED_ShowChar(2,5,'C');
    //OLED_ShowChar(3,6,'D');
    //OLED_ShowChar(4,8,'E');
    //OLED_ShowChar(5,10,'F');
    //OLED_ShowChar(7,12,'G');






    __no_operation();
}
