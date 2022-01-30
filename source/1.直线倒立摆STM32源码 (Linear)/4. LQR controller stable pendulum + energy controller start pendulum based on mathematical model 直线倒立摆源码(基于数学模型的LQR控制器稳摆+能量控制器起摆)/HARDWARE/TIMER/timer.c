#include "timer.h"
#include "led.h"
 /**************************************************************************
���ߣ�ƽ��С��֮�� 
�Ա����̣�http://shop114407458.taobao.com/
**************************************************************************/
/**************************************************************************
Timed interrupt initialization
Entry parameters: 
	arr: auto-reload value 
	psc: clock prescaler
Return value: none 
**************************************************************************/
void Timer1_Init(u16 arr, u16 psc)  
{  
	RCC->APB2ENR|=1<<11;	//TIM1ʱ��ʹ��    
 	TIM1->ARR = arr;      //�趨�������Զ���װֵ   
	TIM1->PSC = psc;      //Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��
	TIM1->DIER|=1<<0;   //��������ж�				
	TIM1->DIER|=1<<6;   //�������ж�	   
	TIM1->CR1|=0x01;    //ʹ�ܶ�ʱ��
	MY_NVIC_Init(1,3,TIM1_UP_IRQn,2);
}  
