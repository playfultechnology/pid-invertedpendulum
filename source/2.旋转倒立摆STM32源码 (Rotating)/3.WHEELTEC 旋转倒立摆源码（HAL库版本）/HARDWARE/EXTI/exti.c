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
#include "exti.h"
#include "key.h"
/**************************************************************************
�������ܣ��ⲿ�жϳ�ʼ��
��ڲ�������
����  ֵ���� 
**************************************************************************/
void MiniBalance_EXTI_Init(void)
{	
	 EXTI_InitTypeDef EXTI_InitStructure;
	 NVIC_InitTypeDef NVIC_InitStructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	 //������ʼ��
	 KEY_Init();
	 GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5);
	 EXTI_InitStructure.EXTI_Line=EXTI_Line5;
	 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
	 EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	 EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	 
	 GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7);
	 EXTI_InitStructure.EXTI_Line=EXTI_Line7;
	 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
	 EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	 EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	 
	 GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource11);
	 EXTI_InitStructure.EXTI_Line=EXTI_Line11;
	 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
	 EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	 EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	 
	 GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource12);
	 EXTI_InitStructure.EXTI_Line=EXTI_Line12;
	 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
	 EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	 EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	 
	 NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�1
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
   NVIC_Init(&NVIC_InitStructure);
	 
	 NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�1
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
   NVIC_Init(&NVIC_InitStructure);
}


//�ⲿ�ж�9~5�������
void EXTI9_5_IRQHandler(void)
{			
	delay_ms(5);   //����			 
   if(KEY5==0)	// 
	{
		Flag_Stop=!Flag_Stop;
		if(Flag_Stop==0) Swing_up=0;
	}		
	if(KEY7==0)	///Menu 
	{
		if(Menu++==4) Menu=1;
	}		
 	EXTI_ClearITPendingBit(EXTI_Line5); //���LINE5�ϵ��жϱ�־λ
	EXTI_ClearITPendingBit(EXTI_Line7); //���LINE7�ϵ��жϱ�־λ
}
//�ⲿ�ж�15~10�������
void EXTI15_10_IRQHandler(void)
{			
	delay_ms(5);   //����

  if(KEY12==0)	//PID-
	{
		if(Menu==1)        Balance_KP-=Amplitude1;
	  else	if(Menu==2)  Balance_KD-=Amplitude2;
		else  if(Menu==3)  Position_KP-=Amplitude3;
		else  if(Menu==4)  Position_KD-=Amplitude4;
	}		
	 if(KEY11==0)	//PID+ 
	{
			    if(Menu==1)  Balance_KP+=Amplitude1;
	  else	if(Menu==2)  Balance_KD+=Amplitude2;
		else  if(Menu==3)  Position_KP+=Amplitude3;
		else  if(Menu==4)  Position_KD+=Amplitude4;
	}		
	if(Balance_KP<=0) Balance_KP=0;
	if(Balance_KD<=0) Balance_KD=0;
	if(Position_KP<=0) Position_KP=0;
	if(Position_KD<=0) Position_KD=0;
  EXTI_ClearITPendingBit(EXTI_Line11); //���LINE11�ϵ��жϱ�־λ  
	EXTI_ClearITPendingBit(EXTI_Line12); //���LINE12�ϵ��жϱ�־λ 
}





