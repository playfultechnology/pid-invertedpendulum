#include "motor.h"

/**************************************************************************
Function: Initialize Motor Interface
Input   : none
Output  : none
函数功能：初始化电机接口
入口参数：无
返回  值：无
**************************************************************************/
void MiniBalance_Motor_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	//Enable port clock //使能端口时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//Port configuration //端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
	//Push-pull output //推挽输出	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M
	//Initialize GPIO with the specified parameters   //根据设定参数初始化GPIO
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//IO输出0，防止电机乱转
  GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13);
}

/**************************************************************************
Function: Initialize PWM to drive motor
Input   : arr：Auto reload value；psc：Prescaler coefficient
Output  : none
函数功能：初始化PWM，用于驱动电机 
入口参数：arr：自动重装值；psc：预分频系数
返回  值：无
**************************************************************************/
void MiniBalance_PWM_Init(u16 arr,u16 psc)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	//初始化电机接口
	MiniBalance_Motor_Init();
	//Enable timer 3 //使能定时器3 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  //Enable GPIO peripheral clock //使能GPIO外设时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//Port configuration //端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
	//Reuse push-pull output //复用推挽输出 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M
	//Initialize GPIO with the specified parameters   //根据设定参数初始化GPIO
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//Sets the value of the auto-reload register cycle for the next update event load activity
	//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 
	TIM_TimeBaseStructure.TIM_Period = arr; 
	//Sets the pre-divider value used as the TIMX clock frequency divisor
	//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	//Set the clock split :TDTS = Tck_tim
	//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_ClockDivision = 1; 
	//Up counting mode 
	//向上计数模式  
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	//Initializes the timebase unit for TIMX based on the parameter specified in TIM_TIMEBASEINITSTRUCT
	//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 

  //Select Timer mode :TIM Pulse Width Modulation mode 1
  //选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	//Compare output enablement
	//比较输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	//Set the pulse value of the capture comparison register to be loaded
	//设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_Pulse = 0; 
  //Output polarity :TIM output polarity is higher	
  //输出极性:TIM输出比较极性高	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	//Initialize the peripheral TIMX based on the parameter specified in TIM_OCINITSTRUCT
  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);  
	//Channel preload enable
	//通道预装载使能
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  

  // Enable the TIMX preloaded register on the ARR
  //使能TIMx在ARR上的预装载寄存器	
	TIM_ARRPreloadConfig(TIM3, ENABLE); 
	
	//Enable TIM3
	//使能TIM3
	TIM_Cmd(TIM3, ENABLE);  
} 

