#ifndef __OLED_H
#define __OLED_H
#include <driverlib.h>

#define HW_IIC 0
#define SW_IIC 1
#define HW_SPI 2
#define SW_SPI 3

#define IIC_1 10
#define IIC_2 11
#define IIC_3 12
#define IIC_4 13

/*****************************************************************/
//閰嶇疆椹卞姩
//1.HW_IIC  纭欢IIC
//2.SW_IIC  杞欢IIC
//3.HW_SPI	纭欢SPI
//4.SW_SPI	杞欢IIC

//鐜板湪鍙敮鎸� 纭欢IIC 鍜� 杞欢IIC
#define TRANSFER_METHOD SW_IIC

/*****************************************************************/

#if (TRANSFER_METHOD == HW_IIC)

//IIC_0: P1.7  -- SCL;  P1.6  -- SDA
//IIC_1: P6.5  -- SCL;  P6.4  -- SDA
//IIC_2: P3.7  -- SCL;  P3.6  -- SDA
//IIC_3: P10.3 -- SCL;  P10.2 -- SDA
#define USE_HW_IIC IIC_3

#elif (TRANSFER_METHOD == HW_SPI)
//#define USE_HW_SPI SPI_2
#endif

#if (TRANSFER_METHOD == HW_IIC)
#define OLED_ADDRESS 0x3C //閫氳繃璋冩暣0R鐢甸樆,灞忓彲浠�0x78鍜�0x7A涓や釜鍦板潃 -- 榛樿0x78  0x3C = 0x78 >> 1

#if (USE_HW_IIC == IIC_0) //宸查獙璇�
#define IIC_GPIOX GPIO_PORT_P1
#define IIC_SCL_Pin GPIO_PIN7
#define IIC_SDA_Pin GPIO_PIN6
#define EUSCI_BX EUSCI_B0_BASE

#elif (USE_HW_IIC == IIC_1) //宸查獙璇�
#define IIC_GPIOX GPIO_PORT_P6
#define IIC_SCL_Pin GPIO_PIN5
#define IIC_SDA_Pin GPIO_PIN4
#define EUSCI_BX EUSCI_B1_BASE

#elif (USE_HW_IIC == IIC_2) //宸查獙璇�
#define IIC_GPIOX GPIO_PORT_P3
#define IIC_SCL_Pin GPIO_PIN7
#define IIC_SDA_Pin GPIO_PIN6
#define EUSCI_BX EUSCI_B2_BASE

#elif (USE_HW_IIC == IIC_3) //宸查獙璇�
#define IIC_GPIOX GPIO_PORT_P10
#define IIC_SCL_Pin GPIO_PIN3
#define IIC_SDA_Pin GPIO_PIN2
#define EUSCI_BX EUSCI_B3_BASE
#endif

#elif TRANSFER_METHOD == SW_IIC

//OLED SSD1306 I2C SCL P1.7
#define OLED_SSD1306_SCL_PIN_NUM (7)
#define OLED_SSD1306_SCL_IO_INIT (BITBAND_PERI(P1DIR, OLED_SSD1306_SCL_PIN_NUM) = 1) // (Set_Bit(P1DIR, OLED_SSD1306_SCL_PIN_NUM))
#define OLED_SCL_Set() (BITBAND_PERI(P1OUT, OLED_SSD1306_SCL_PIN_NUM) = 1)
#define OLED_SCL_Clr() (BITBAND_PERI(P1OUT, OLED_SSD1306_SCL_PIN_NUM) = 0)

//OLED SSD1306 I2C SDA   P1.6
#define OLED_SSD1306_SDA_PIN_NUM (6)
#define OLED_SSD1306_SDA_IO_INIT (BITBAND_PERI(P1DIR, OLED_SSD1306_SDA_PIN_NUM) = 1)
#define OLED_SDA_Set() (BITBAND_PERI(P1OUT, OLED_SSD1306_SDA_PIN_NUM) = 1)
#define OLED_SDA_Clr() (BITBAND_PERI(P1OUT, OLED_SSD1306_SDA_PIN_NUM) = 0)

#elif (TRANSFER_METHOD == HW_SPI)
//鏆傛湭鏀寔
#endif

//OLED鎺у埗鐢ㄥ嚱鏁�
void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);
void OLED_WR_Byte(uint8_t dat, uint8_t cmd);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t sizey);
uint32_t oled_pow(uint8_t m, uint8_t n);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t sizey);
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t sizey);
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t no, uint8_t sizey);
void OLED_DrawBMP(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, uint8_t BMP[]);
void OLED_Init(void);
#endif
