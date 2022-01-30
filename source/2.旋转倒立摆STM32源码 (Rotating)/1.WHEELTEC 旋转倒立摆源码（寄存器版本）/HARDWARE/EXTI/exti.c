#include "exti.h"
#include "key.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
/**************************************************************************
�������ܣ��ⲿ�жϳ�ʼ��
��ڲ�������
����  ֵ���� 
**************************************************************************/
void EXTI_Init(void)
{
	KEY_Init();	
	Ex_NVIC_Config(GPIO_A,7,FTIR);		//�½��ش���
	Ex_NVIC_Config(GPIO_A,5,FTIR);		//�½��ش���
	Ex_NVIC_Config(GPIO_A,11,FTIR);		//�½��ش���
	Ex_NVIC_Config(GPIO_A,12,FTIR);		//�½��ش���

	MY_NVIC_Init(2,2,EXTI9_5_IRQn,2);  	//��ռ2�������ȼ�2����2
	MY_NVIC_Init(2,2,EXTI15_10_IRQn,2);	//��ռ2�������ȼ�2����2	  
}


//�ⲿ�ж�9~5�������
void EXTI9_5_IRQHandler(void)
{			
	delay_ms(5);   //����			 
   if(KEY5==0)	// 
	{
		Flag_Stop=!Flag_Stop;

	}		
	if(KEY7==0)	///Menu 
	{
		if(Menu++==4) Menu=1;
	}		
 		EXTI->PR=1<<5;     //���LINE5�ϵ��жϱ�־λ  
		EXTI->PR=1<<7;     //���LINE7�ϵ��жϱ�־λ
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
  EXTI->PR=1<<11; //���LINE11�ϵ��жϱ�־λ  
	EXTI->PR=1<<12; //���LINE12�ϵ��жϱ�־λ 
}





