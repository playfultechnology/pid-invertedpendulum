/***********************************************
公司：轮趣科技（东莞）有限公司
品牌：WHEELTEC
官网：wheeltec.net
淘宝店铺：shop114407458.taobao.com 
速卖通: https://minibalance.aliexpress.com/store/4455017
版本：1.0
修改时间：2021-12-09

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version:1.0
Update：2021-12-09

All rights reserved
***********************************************/
#include "encoder.h"
/**************************************************************************
函数功能：把TIM4初始化为编码器接口模式
入口参数：无
返回  值：无
**************************************************************************/
void Encoder_Init_TIM4(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Enable timer 4 clock //使能定时器4的时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	//Enable pB port clock //使能PB端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//Port configuration, PB6, PB7 //端口配置，PB6、PB7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  //Float input //浮空输入	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	//Initialize GPIOB with the specified parameters //根据设定参数初始化GPIOB
	GPIO_Init(GPIOB, &GPIO_InitStructure);					      
  
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	//Set up the pre-divider //设置预分频器 
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; 
	//Set the counter to automatically reload //设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; 
	//Select the clock frequency division: no frequency //选择时钟分频：不分频	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//Up counting mode //向上计数模式 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//Initialize timer 4//初始化定时器4
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	//Use encoder mode 3 //使用编码器模式3
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	
	//Fill in each parameter in the TIM_ICInitStruct with the default value
	//把TIM_ICInitStruct 中的每一个参数按缺省值填入
	TIM_ICStructInit(&TIM_ICInitStructure);
	//Set the filter length //设置滤波器长度
	TIM_ICInitStructure.TIM_ICFilter = 10;
	//Initialize the peripheral TIMX based on the parameter TIM_ICINITSTRUCT //根据 TIM_ICInitStruct 的参数初始化外设	TIMx	
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	
	//Clear the update bit for Tim //清除TIM的更新标志位
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	//Enable the timer to interrupt //使能定时器中断
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	//Reset the timer count //计数器初始值赋值为10000
	TIM_SetCounter(TIM4,10000);
	//Enable timer 4 //使能定时器4
	TIM_Cmd(TIM4, ENABLE); 
}
/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器
返回  值：速度值
**************************************************************************/
int Read_Encoder(u8 TIMX)
{
    int Encoder_TIM;    
   switch(TIMX)
	 {
	   case 2:  Encoder_TIM= (short)TIM2 -> CNT; break;
		 case 3:  Encoder_TIM= (short)TIM3 -> CNT; break;	
		 case 4:  Encoder_TIM= (short)TIM4 -> CNT; break;	
		 default:  Encoder_TIM=0;
	 }
		return Encoder_TIM;
}

void TIM4_IRQHandler(void)
{ 		    		  			    
	if(TIM4->SR&0X0001)//溢出中断
	{
			    				   				     	    	
	}				   
	TIM4->SR&=~(1<<0);//清除中断标志位 	    
}
