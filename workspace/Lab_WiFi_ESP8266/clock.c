#include "clock.h"


/***************************************************************************
�������ƣ�Mcu_ClockInit
��    �ܣ� ʱ�ӵĳ�ʼ����8MHz
��    ���� ��
����ֵ ����
****************************************************************************/
void Mcu_ClockInit(void)
{
    unsigned int temp;

    _DINT();
    BCSCTL1 &= ~XT2OFF;                       // XT2on
    do
    {
       IFG1 &= ~OFIFG;                        // Clear OSCFault flag
       for (temp = 0xFF; temp > 0; temp--);   // Time for flag to set
    }
    while ((IFG1 & OFIFG));                   // OSCFault flag still set?
    BCSCTL2 |= SELM1 + SELS;                  // XT2��MCLK,SCLK=8MHz

}
