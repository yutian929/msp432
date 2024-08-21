/* DriverLib Includes */
#include <driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#include "usart.h"
#include "sysinit.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "oled.h"
#include "bsp_mpu6050.h"
#include "inv_mpu.h"
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





char tmp_buf[33];           //×Ö·û´®Êý×é
float pitch,roll,yaw;       //Å·À­½Ç:¸©Ñö½Ç£¬Æ«º½½Ç£¬¹ö×ª½Ç
short aacx,aacy,aacz;       //¼ÓËÙ¶È´«¸ÐÆ÷Ô­Ê¼Êý¾Ý  angular acceleration
short gyrox,gyroy,gyroz;    //ÍÓÂÝÒÇÔ­Ê¼Êý¾Ý  gyroscope
short temp;                 //ÎÂ¶È

struct MPU6050              //MPU6050½á¹¹Ìå
{
    uint8_t flag;               //²É¼¯³É¹¦±êÖ¾Î»
    uint8_t speed;              //ÉÏ±¨ËÙ¶È
}mpu6050;                   //Î¨Ò»½á¹¹Ìå±äÁ¿

void MPU_Read(void);        //MPU6050Êý¾Ý¶ÁÈ¡º¯Êý
void DATA_Report(void);     //MPU6050Êý¾ÝÉÏ±¨

void MPU_Read(void)
{

    if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)//dmp´¦ÀíµÃµ½Êý¾Ý£¬¶Ô·µ»ØÖµ½øÐÐÅÐ¶Ï
    {
        temp=MPU_Get_Temperature();                 //µÃµ½ÎÂ¶ÈÖµ
        MPU_Get_Accelerometer(&aacx,&aacy,&aacz);   //µÃµ½¼ÓËÙ¶È´«¸ÐÆ÷Êý¾Ý
        MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);    //µÃµ½ÍÓÂÝÒÇÊý¾Ý
        mpu6050.speed++;                            //ÉÏ±¨ËÙ¶È×Ô¼Ó
        if(mpu6050.speed == 4)                      //ÉÏ±¨ËÙ¶ÈãÐÖµÉèÖÃ
        {
            mpu6050.flag = 1;                       //²É¼¯³É¹¦±êÖ¾Î»ÉèÖÃÎªÓÐÐ§
            mpu6050.speed = 0;                      //ÉÏ±¨ËÙ¶È¹éÁã
        }
    }
    else                                            //²É¼¯²»³É¹¦
    {
        mpu6050.flag = 0;                           //²É¼¯³É¹¦±êÖ¾Î»ÉèÖÃÎªÎÞÐ§
    }
}

void DATA_Report(void)
{
    if(mpu6050.flag == 1)                       //²É¼¯³É¹¦Ê±
    {
        if(temp<0)                              //¶ÔÊý¾ÝÕý¸ºÅÐ¶Ï£¬ÅÐ¶ÏÎª¸ºÊ±
        {
            temp=-temp;                         //¶Ô¸ºÊý¾ÝÈ¡·´
        }
        else                                    //ÅÐ¶ÏÎªÕýÊ±
        {
        }
        printf("temp:%d.%d,",temp/100,temp%10); //Í¨¹ý´®¿Ú1Êä³öÎÂ¶È

        temp=pitch*10;                           //¸³tempÎªpitch
        if(temp<0)                              //¶ÔÊý¾ÝÕý¸ºÅÐ¶Ï£¬ÅÐ¶ÏÎª¸ºÊ±
        {
            temp=-temp;                         //¶Ô¸ºÊý¾ÝÈ¡·´
        }
        else                                    //ÅÐ¶ÏÎªÕýÊ±
        {
        }
        sprintf((char *)tmp_buf,"Pitch:%d.%d",temp/10,temp%10);
        OLED_ShowString(0,16,(uint8_t *)tmp_buf,16);
        printf("pitch:%d.%d,",temp/10,temp%10); //Í¨¹ý´®¿Ú1Êä³öpitch

        temp=roll*10;                            //¸³tempÎªroll
        if(temp<0)                              //¶ÔÊý¾ÝÕý¸ºÅÐ¶Ï£¬ÅÐ¶ÏÎª¸ºÊ±
        {
            temp=-temp;                         //¶Ô¸ºÊý¾ÝÈ¡·´
        }
        else                                    //ÅÐ¶ÏÎªÕýÊ±
        {
        }
        sprintf((char *)tmp_buf,"roll:%d.%d",temp/10,temp%10);
        OLED_ShowString(0,32,(uint8_t *)tmp_buf,16);
        printf("roll:%d.%d,",temp/10,temp%10);//Í¨¹ý´®¿Ú1Êä³öroll

        temp=yaw*10;                           //¸³tempÎªyaw
        if(temp<0)                              //¶ÔÊý¾ÝÕý¸ºÅÐ¶Ï£¬ÅÐ¶ÏÎª¸ºÊ±
        {
            temp=-temp;                         //¶Ô¸ºÊý¾ÝÈ¡·´
        }
        else                                    //ÅÐ¶ÏÎªÕýÊ±
        {
        }
        sprintf((char *)tmp_buf,"yaw:%d.%d",temp/10,temp%10);
        OLED_ShowString(0,48,(uint8_t *)tmp_buf,16);
        printf("yaw:%d.%d,",temp/10,temp%10);//Í¨¹ý´®¿Ú1Êä³öyaw
        printf("gyrox:%d,gyroy:%d,gyroz:%d,aacx:%d,aacy:%d,aacz:%d\r\n",gyrox,gyroy,gyroz,aacx,aacy,aacz);//ÉÏ±¨½ÇËÙ¶ÈÊý¾Ý£¬½Ç¼ÓËÙ¶ÈÊý¾Ý¸
        mpu6050.flag = 0;                                   //²É¼¯³É¹¦±êÖ¾Î»ÉèÖÃÎªÎÞÐ§
    }
    else ;                                                      //·À¿¨ËÀ
}


void SYS_Init(void)
{
    MAP_WDT_A_holdTimer();
    delay_init();                                    //ÑÓÊ±º¯Êý³õÊ¼»¯
    uart_init(115200);                               //´®¿Ú³õÊ¼»¯Îª115200
    LED_Init();                                      //³õÊ¼»¯ÓëLEDÁ¬½ÓµÄÓ²¼þ½Ó¿Ú
    OLED_Init();                                   //OLED³õÊ¼»¯
    MPU_Init();                                      //³õÊ¼»¯MPU6050
    while(mpu_dmp_init())                            //³õÊ¼»¯mpu_dmp¿â
    {
        OLED_ShowString(0,0,"Failed",16);            //ÏÔÊ¾×Ö·û´®
        //OLED_Refresh();                                //Ë¢ÐÂÏÔ´æ
        LED_R_Tog();delay_ms(50);LED_R_Tog();     //LEDÉÁË¸Ö¸Ê¾
        printf("Initialization failed£¡\r\n");      //´®¿Ú³õÊ¼»¯Ê§°ÜÉÏ±¨
//        OLED_ShowString(0,0,"      ",16);          //ÏÔÊ¾×Ö·û´®
//        OLED_Refresh();                               //Ë¢ÐÂÏÔ´æ
    }
    printf("Initialization successed£¡\r\n");       //´®¿Ú³õÊ¼»¯³É¹¦ÉÏ±¨
//    OLED_ShowString(0,0,"  OK!",16);                //ÏÔÊ¾×Ö·û´®
//    OLED_Refresh();                                   //Ë¢ÐÂÏÔ´æ
    delay_ms(999);                                  //ÑÓÊ±³õ½çÃæÏÔÊ¾
    mpu6050.flag = 0;                               //²É¼¯³É¹¦±êÖ¾Î»³õÊ¼»¯
    mpu6050.speed = 0;                              //ÉÏ±¨ËÙ¶È³õÊ¼»¯
}

int main(void)
{
    SYS_Init();//ÏµÍ³³õÊ¼»¯×Üº¯Êý
    while(1)   //Ö÷Ñ­»·
    {
        MPU_Read();    //MPU6050Êý¾Ý¶ÁÈ¡
        DATA_Report(); //MPU6050Êý¾ÝÉÏ±¨
    }
//    OLED_ShowChar(0,0,'A',8);
//    OLED_ShowChar(8,0,'B',8);
//    OLED_ShowChar(0,8,'C',8);
//    OLED_ShowChar(8,8,'D',8);
//    OLED_ShowNum(16,16,123,3,8);
    __sleep();
}
