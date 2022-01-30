/***********************************************
��˾����Ȥ�Ƽ�����ݸ�����޹�˾
Ʒ�ƣ�WHEELTEC
������wheeltec.net
�Ա����̣�shop114407458.taobao.com 
����ͨ: https://minibalance.aliexpress.com/store/4455017
�汾��1.0
�޸�ʱ�䣺2021-12-09

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version:1.0
Update��2021-12-09

All rights reserved
***********************************************/
#include "encoder.h"
/**************************************************************************
�������ܣ���TIM4��ʼ��Ϊ�������ӿ�ģʽ
��ڲ�������
����  ֵ����
**************************************************************************/
void Encoder_Init_TIM4(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Enable timer 4 clock //ʹ�ܶ�ʱ��4��ʱ�� 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	//Enable pB port clock //ʹ��PB�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//Port configuration, PB6, PB7 //�˿����ã�PB6��PB7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  //Float input //��������	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	//Initialize GPIOB with the specified parameters //�����趨������ʼ��GPIOB
	GPIO_Init(GPIOB, &GPIO_InitStructure);					      
  
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	//Set up the pre-divider //����Ԥ��Ƶ�� 
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; 
	//Set the counter to automatically reload //�趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; 
	//Select the clock frequency division: no frequency //ѡ��ʱ�ӷ�Ƶ������Ƶ	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//Up counting mode //���ϼ���ģʽ 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//Initialize timer 4//��ʼ����ʱ��4
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	//Use encoder mode 3 //ʹ�ñ�����ģʽ3
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	
	//Fill in each parameter in the TIM_ICInitStruct with the default value
	//��TIM_ICInitStruct �е�ÿһ��������ȱʡֵ����
	TIM_ICStructInit(&TIM_ICInitStructure);
	//Set the filter length //�����˲�������
	TIM_ICInitStructure.TIM_ICFilter = 10;
	//Initialize the peripheral TIMX based on the parameter TIM_ICINITSTRUCT //���� TIM_ICInitStruct �Ĳ�����ʼ������	TIMx	
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	
	//Clear the update bit for Tim //���TIM�ĸ��±�־λ
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	//Enable the timer to interrupt //ʹ�ܶ�ʱ���ж�
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	//Reset the timer count //��������ʼֵ��ֵΪ10000
	TIM_SetCounter(TIM4,10000);
	//Enable timer 4 //ʹ�ܶ�ʱ��4
	TIM_Cmd(TIM4, ENABLE); 
}
/**************************************************************************
�������ܣ���λʱ���ȡ����������
��ڲ�������ʱ��
����  ֵ���ٶ�ֵ
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
	if(TIM4->SR&0X0001)//����ж�
	{
			    				   				     	    	
	}				   
	TIM4->SR&=~(1<<0);//����жϱ�־λ 	    
}
