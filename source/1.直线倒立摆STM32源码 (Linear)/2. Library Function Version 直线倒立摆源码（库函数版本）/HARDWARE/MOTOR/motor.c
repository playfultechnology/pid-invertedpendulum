#include "motor.h"

/**************************************************************************
Function: Initialize Motor Interface
Input   : none
Output  : none
�������ܣ���ʼ������ӿ�
��ڲ�������
����  ֵ����
**************************************************************************/
void MiniBalance_Motor_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	//Enable port clock //ʹ�ܶ˿�ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//Port configuration //�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
	//Push-pull output //�������	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M
	//Initialize GPIO with the specified parameters   //�����趨������ʼ��GPIO
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//IO���0����ֹ�����ת
  GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13);
}

/**************************************************************************
Function: Initialize PWM to drive motor
Input   : arr��Auto reload value��psc��Prescaler coefficient
Output  : none
�������ܣ���ʼ��PWM������������� 
��ڲ�����arr���Զ���װֵ��psc��Ԥ��Ƶϵ��
����  ֵ����
**************************************************************************/
void MiniBalance_PWM_Init(u16 arr,u16 psc)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	//��ʼ������ӿ�
	MiniBalance_Motor_Init();
	//Enable timer 3 //ʹ�ܶ�ʱ��3 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  //Enable GPIO peripheral clock //ʹ��GPIO����ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//Port configuration //�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
	//Reuse push-pull output //����������� 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //50M
	//Initialize GPIO with the specified parameters   //�����趨������ʼ��GPIO
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//Sets the value of the auto-reload register cycle for the next update event load activity
	//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 
	TIM_TimeBaseStructure.TIM_Period = arr; 
	//Sets the pre-divider value used as the TIMX clock frequency divisor
	//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	//Set the clock split :TDTS = Tck_tim
	//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_ClockDivision = 1; 
	//Up counting mode 
	//���ϼ���ģʽ  
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	//Initializes the timebase unit for TIMX based on the parameter specified in TIM_TIMEBASEINITSTRUCT
	//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 

  //Select Timer mode :TIM Pulse Width Modulation mode 1
  //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
 	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	//Compare output enablement
	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	//Set the pulse value of the capture comparison register to be loaded
	//���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_Pulse = 0; 
  //Output polarity :TIM output polarity is higher	
  //�������:TIM����Ƚϼ��Ը�	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	//Initialize the peripheral TIMX based on the parameter specified in TIM_OCINITSTRUCT
  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);  
	//Channel preload enable
	//ͨ��Ԥװ��ʹ��
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  

  // Enable the TIMX preloaded register on the ARR
  //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���	
	TIM_ARRPreloadConfig(TIM3, ENABLE); 
	
	//Enable TIM3
	//ʹ��TIM3
	TIM_Cmd(TIM3, ENABLE);  
} 

