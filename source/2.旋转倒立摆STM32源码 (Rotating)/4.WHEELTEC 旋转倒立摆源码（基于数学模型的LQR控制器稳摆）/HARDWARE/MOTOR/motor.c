#include "motor.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
void MiniBalance_Motor_Init(void)
{
	RCC->APB2ENR|=1<<3;       //PORTB时钟使能   
	GPIOB->CRH&=0X0000FFFF;   //PORTB12 13 14 15推挽输出
	GPIOB->CRH|=0X22220000;   //PORTB12 13 14 15推挽输出
}
void MiniBalance_PWM_Init(u16 arr,u16 psc)
{		 					 
MiniBalance_Motor_Init(); //初始化电机控制所需IO
	RCC->APB1ENR|=1<<1;       //TIM3时钟使能    
	RCC->APB2ENR|=1<<3;       //PORTB时钟使能     
	GPIOB->CRL&=0XFFFFFF00;   //PORTB0 1复用输出
	GPIOB->CRL|=0X000000BB;   //PORTB0 1复用输出
	TIM3->ARR=arr;//设定计数器自动重装值 
	TIM3->PSC=psc;//预分频器不分频
	TIM3->CCMR2|=6<<12;//CH4 PWM1模式	
	TIM3->CCMR2|=6<<4; //CH3 PWM1模式	
	TIM3->CCMR2|=1<<11;//CH4预装载使能	 
	TIM3->CCMR2|=1<<3; //CH3预装载使能	  
	TIM3->CCER|=1<<12; //CH4输出使能	   
	TIM3->CCER|=1<<8;  //CH3输出使能	
	TIM3->CR1=0x80;  //ARPE使能 
	TIM3->CR1|=0x01;   //使能定时器3 	
} 

