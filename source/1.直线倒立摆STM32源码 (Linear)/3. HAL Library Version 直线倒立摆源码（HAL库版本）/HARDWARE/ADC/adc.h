#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
 /**************************************************************************
���ߣ�ƽ��С��֮�� 
�Ա����̣�http://shop114407458.taobao.com/
**************************************************************************/
#define Battery_Ch 6
u16 Get_Adc(u8 ch);
int Get_battery_volt(void);   
void Adc_Init(void);  
u16 Get_Adc_Average(u8 ch,u8 times);
#endif 















