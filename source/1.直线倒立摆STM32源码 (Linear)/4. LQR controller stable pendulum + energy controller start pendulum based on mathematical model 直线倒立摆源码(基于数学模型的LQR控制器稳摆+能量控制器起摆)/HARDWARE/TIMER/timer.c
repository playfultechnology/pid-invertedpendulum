#include "timer.h"
#include "led.h"
 /**************************************************************************
作者：平衡小车之家 
淘宝店铺：http://shop114407458.taobao.com/
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
	RCC->APB2ENR|=1<<11;	//TIM1时钟使能    
 	TIM1->ARR = arr;      //设定计数器自动重装值   
	TIM1->PSC = psc;      //预分频器7200,得到10Khz的计数时钟
	TIM1->DIER|=1<<0;   //允许更新中断				
	TIM1->DIER|=1<<6;   //允许触发中断	   
	TIM1->CR1|=0x01;    //使能定时器
	MY_NVIC_Init(1,3,TIM1_UP_IRQn,2);
}  
