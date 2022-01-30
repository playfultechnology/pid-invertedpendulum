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
#include "show.h"
unsigned char i,temp;          //��������
unsigned char Send_Count; //������Ҫ���͵����ݸ���
float Vol;
/**************************************************************************
�������ܣ�OLED��ʾ
��ڲ�������
����  ֵ����
**************************************************************************/
void oled_show(void)
{
		//=============��1����ʾ�Ƕ�PD����P����Position_KP=======================//	
		OLED_ShowString(00,00,"B-KP");                   
		OLED_ShowNumber(40,00,Balance_KP,3,12);
		OLED_ShowString(57,00,"."),  
		OLED_ShowNumber(61,00,(int)(Balance_KP*10)%10,1,12);

		OLED_ShowString(95,00,"A:");	  
		OLED_ShowNumber(108,00,Amplitude1,2,12);

		//=============��2����ʾ�Ƕ�PD����D����=======================//	
		OLED_ShowString(00,10,"B-KD");
		OLED_ShowNumber(40,10,Balance_KD,3,12);
		OLED_ShowString(57,10,"."),  
		OLED_ShowNumber(61,10,(int)(Balance_KD*10)%10,1,12);

		OLED_ShowString(95,10,"A:");	  
		OLED_ShowNumber(108,10,Amplitude2,2,12);
		//=============��3����ʾ������Position_KP=======================//	
		OLED_ShowString(00,20,"P-KP");
		OLED_ShowNumber(40,20,Position_KP,3,12);
		OLED_ShowString(57,20,"."),  
		OLED_ShowNumber(61,20,(int)(Position_KP*10)%10,1,12);
		
		OLED_ShowString(95,20,"A:");	  
		OLED_ShowNumber(108,20,Amplitude3,2,12);
		//=============��4����ʾ������1=======================//	
		OLED_ShowString(00,30,"P-KD");
		OLED_ShowNumber(40,30,Position_KD,3,12);
		OLED_ShowString(57,30,"."),  
		OLED_ShowNumber(61,30,(int)(Position_KD*10)%10,1,12);
		
		OLED_ShowString(95,30,"A:");	  
		OLED_ShowNumber(108,30,Amplitude4,2,12);
		//======���ǹ����˵� ѡ����Ҫ���ڵ�PD����											
		  if(Menu==1)
	   	{
			 OLED_ShowChar(75,00,'Y',12,1);   
			 OLED_ShowChar(75,10,'N',12,1);
			 OLED_ShowChar(75,20,'N',12,1);
			 OLED_ShowChar(75,30,'N',12,1);
	  	}
		  else	if(Menu==2)
	   	{
			 OLED_ShowChar(75,00,'N',12,1);
			 OLED_ShowChar(75,10,'Y',12,1);
			 OLED_ShowChar(75,20,'N',12,1);
			 OLED_ShowChar(75,30,'N',12,1);
			}		
      else if(Menu==3)
	   	{			
			 OLED_ShowChar(75,00,'N',12,1);
			 OLED_ShowChar(75,10,'N',12,1);
			 OLED_ShowChar(75,20,'Y',12,1);
			 OLED_ShowChar(75,30,'N',12,1);
			}		
      else if(Menu==4)
	   	{				
			 OLED_ShowChar(75,00,'N',12,1);
			 OLED_ShowChar(75,10,'N',12,1);
			 OLED_ShowChar(75,20,'N',12,1);
			 OLED_ShowChar(75,30,'Y',12,1);
	 	  } 
			
	//=============��������ʾ��ѹ��Ŀ��λ��=======================//			
			OLED_ShowString(80,40,"T:");	  
			OLED_ShowNumber(95,40,Position_Zero,5,12) ; 
			OLED_ShowString(00,40,"VOL:");
			OLED_ShowString(41,40,".");
			OLED_ShowString(63,40,"V");
			OLED_ShowNumber(28,40,Voltage/100,2,12);
			OLED_ShowNumber(51,40,Voltage%100,2,12);
		 if(Voltage%100<10) 	OLED_ShowNumber(45,40,0,2,12);
		//=============��������ʾ��λ�ƴ������ͱ���������=======================//
		OLED_ShowString(80,50,"P:");    OLED_ShowNumber(95,50,Encoder,5,12); 
		OLED_ShowString(0,50,"ADC:");  OLED_ShowNumber(30,50,Angle_Balance,4,12);
		//=============ˢ��=======================//
		OLED_Refresh_Gram();	
	}

/**************************************************************************
�������ܣ�����ʾ��������λ���������� �ر���ʾ��
��ڲ�������
����  ֵ����
��    �ߣ�ƽ��С��֮��
**************************************************************************/
void DataScope(void)
{   
		DataScope_Get_Channel_Data( Angle_Balance, 1 );      
		DataScope_Get_Channel_Data( Encoder, 2 );         
		DataScope_Get_Channel_Data( 0, 3 );              
//		DataScope_Get_Channel_Data( 0 , 4 );   
//		DataScope_Get_Channel_Data(0, 5 ); //����Ҫ��ʾ�������滻0������
//		DataScope_Get_Channel_Data(0 , 6 );//����Ҫ��ʾ�������滻0������
//		DataScope_Get_Channel_Data(0, 7 );
//		DataScope_Get_Channel_Data( 0, 8 ); 
//		DataScope_Get_Channel_Data(0, 9 );  
//		DataScope_Get_Channel_Data( 0 , 10);
		Send_Count = DataScope_Data_Generate(3);
		for( i = 0 ; i < Send_Count; i++) 
		{
		while((USART1->SR&0X40)==0);  
		USART1->DR = DataScope_OutPut_Buffer[i]; 
		}
}	

//��ʾҳ ����⵽�ڸ���Ҫ���ڣ��������ʾ���ڰڸˣ�����һҳ������ʼ����
void show_Tips(void)
{
 //������λ�ư�װ���ģʽ���������ʾ�����������û�����ȷ��...
  OLED_ShowChinese_12(28,0,0,1);
	OLED_ShowChinese_12(42,0,1,1);
	OLED_ShowChinese_12(56,0,2,1);
	OLED_ShowChinese_12(70,0,3,1);
	OLED_ShowChinese_12(84,0,4,1);
	OLED_ShowChinese_12(98,0,5,1);
	OLED_ShowChinese_12(112,0,6,1);
	OLED_ShowChinese_12(0,16,7,1);
	OLED_ShowChinese_12(14,16,8,1);
	OLED_ShowChinese_12(28,16,9,1);
	OLED_ShowChinese_12(42,16,10,1);
	OLED_ShowChinese_12(56,16,11,1);
	OLED_ShowChinese_12(70,16,12,1);
	OLED_ShowChinese_12(84,16,13,1);
	OLED_ShowChinese_12(98,16,14,1);
	OLED_ShowChinese_12(112,16,15,1);
	OLED_ShowChinese_12(0,32,16,1);
	OLED_ShowChinese_12(14,32,17,1);
	OLED_ShowChinese_12(28,32,18,1);
	OLED_ShowChinese_12(42,32,19,1);
	OLED_ShowChinese_12(56,32,20,1);
	OLED_ShowChinese_12(70,32,21,1);
	OLED_ShowChinese_12(84,32,22,1);
	OLED_ShowChinese_12(98,32,23,1);
	OLED_ShowChinese_12(112,32,24,1);
	OLED_ShowChinese_12(0,48,25,1);
	OLED_ShowChinese_12(14,48,26,1);
	OLED_ShowChinese_12(28,48,27,1);
	OLED_ShowChinese_12(42,48,28,1);
	OLED_ShowChinese_12(46,48,28,1);
	OLED_ShowChinese_12(50,48,28,1);
	
	OLED_Refresh_Gram();
}

//��0ҳ����̶��õ����ڣ�ʹ�ڸ˴�ֱ���²����־�ֹ����װ��Ϻ���һҳ��������
void step_0(void)
{
  OLED_ShowChinese_12(0,0,12,1);
	OLED_ShowChinese_12(14,0,29,1);
	OLED_ShowChinese_12(28,0,30,1);
	OLED_ShowChinese_12(42,0,31,1);
	OLED_ShowChinese_12(56,0,32,1);
	OLED_ShowChinese_12(70,0,33,1);
	OLED_ShowChinese_12(84,0,34,1);
	OLED_ShowChinese_12(98,0,11,1);
	OLED_ShowChinese_12(112,0,35,1);
	OLED_ShowChinese_12(0,16,36,1);
	OLED_ShowChinese_12(14,16,37,1);
	OLED_ShowChinese_12(28,16,38,1);
	OLED_ShowChinese_12(42,16,39,1);
	OLED_ShowChinese_12(56,16,40,1);
	OLED_ShowChinese_12(70,16,41,1);
	OLED_ShowChinese_12(84,16,42,1);
	OLED_ShowChinese_12(98,16,43,1);
	OLED_ShowChinese_12(112,16,44,1);
	OLED_ShowChinese_12(0,32,45,1);
	OLED_ShowChinese_12(14,32,46,1);
	OLED_ShowChinese_12(28,32,19,1);
	OLED_ShowChinese_12(42,32,47,1);
	OLED_ShowChinese_12(56,32,48,1);
	OLED_ShowChinese_12(70,32,49,1);
	OLED_ShowChinese_12(84,32,50,1);
	OLED_ShowChinese_12(98,32,51,1);
	OLED_ShowChinese_12(112,32,24,1);
	OLED_ShowChinese_12(0,48,41,1);
	OLED_ShowChinese_12(14,48,52,1);
	OLED_ShowChinese_12(28,48,53,1);
	OLED_ShowChinese_12(42,48,24,1);
	OLED_ShowChinese_12(56,48,25,1);
	OLED_ShowChinese_12(70,48,54,1);
	OLED_ShowChinese_12(84,48,55,1);
	OLED_ShowChinese_12(98,48,28,1);
	OLED_ShowChinese_12(102,48,28,1);
	OLED_ShowChinese_12(106,48,28,1);
	OLED_Refresh_Gram();
}

//��1ҳ�������Ӱڸ˺ͽ�λ�ƴ������ϵ�������˿š�ɣ�Ȼ����ڰڸ˵���ť����
void step_1(void)
{
	OLED_ShowChinese(0,0,60,1);
	OLED_ShowChinese(16,0,61,1);
	OLED_ShowChinese(32,0,62,1);
	OLED_ShowChinese(48,0,7,1);
	OLED_ShowChinese(64,0,8,1);
	OLED_ShowChinese(80,0,63,1);
	OLED_ShowChinese(96,0,64,1);
	OLED_ShowChinese(112,0,65,1);
	OLED_ShowChinese(0,16,66,1);
	OLED_ShowChinese(16,16,67,1);
	OLED_ShowChinese(32,16,68,1);
	OLED_ShowChinese(48,16,69,1);
	OLED_ShowChinese(64,16,70,1);
	OLED_ShowChinese(80,16,71,1);
	OLED_ShowChinese(96,16,72,1);
	OLED_ShowChinese(112,16,73,1);
	OLED_ShowChinese(0,32,74,1);
	OLED_ShowChinese(16,32,75,1);
	OLED_ShowChinese(32,32,19,1);
	OLED_ShowChinese(48,32,76,1);
	OLED_ShowChinese(64,32,17,1);
	OLED_ShowChinese(80,32,77,1);
	OLED_ShowChinese(96,32,48,1);
	OLED_ShowChinese(112,32,5,1);
	OLED_ShowChinese(0,48,6,1);
	OLED_ShowChinese(16,48,7,1);
	OLED_ShowChinese(32,48,8,1);
	OLED_ShowChinese(48,48,27,1);
	OLED_ShowChinese(64,48,28,1);
	OLED_ShowChinese(80,48,29,1);
	OLED_ShowChinese(96,48,17,1);
	OLED_ShowChinese(112,48,41,1);	
	OLED_Refresh_Gram();
}

//��2ҳ��֤�ڸ˴�ֱ����ʱADC��ֵ��1010��1030֮�䣬Ȼ��š�����Ӱڸ˺ͽ�λ
void step_2(void)
{
	OLED_ShowChinese(0,0,78,1);
	OLED_ShowChinese(16,0,7,1);
	OLED_ShowChinese(32,0,8,1);
	OLED_ShowChinese(48,0,42,1);
	OLED_ShowChinese(64,0,43,1);
	OLED_ShowChinese(80,0,44,1);
	OLED_ShowChinese(96,0,45,1);
	OLED_ShowChinese(112,0,79,1);
	OLED_ShowChar(0,16,'A',16,1);
	OLED_ShowChar(8,16,'D',16,1);
	OLED_ShowChar(16,16,'C',16,1);
	OLED_ShowChinese(24,16,71,1);
	OLED_ShowChinese(40,16,31,1);
	OLED_ShowChinese(56,16,80,1);
	OLED_ShowNumber(72,16,1010,4,16);
	OLED_ShowChinese(104,16,33,1);//��
	OLED_ShowNumber(0,32,1030,4,16);
	OLED_ShowChinese(32,32,34,1);
	OLED_ShowChinese(48,32,35,1); //��
	OLED_ShowChinese(64,32,17,1);
	OLED_ShowChinese(80,32,77,1);
	OLED_ShowChinese(96,32,48,1);
	OLED_ShowChinese(112,32,19,1);
	OLED_ShowChinese(0,48,81,1);
	OLED_ShowChinese(16,48,61,1);
	OLED_ShowChinese(32,48,62,1);
	OLED_ShowChinese(48,48,7,1);
	OLED_ShowChinese(64,48,8,1);
	OLED_ShowChinese(80,48,63,1);
	OLED_ShowChinese(96,48,64,1);
	OLED_ShowChinese(112,48,65,1);
	OLED_Refresh_Gram();
}


//��3ҳ���ƴ������ϵ�������˿�����ڳɹ������û��������
void step_3(void)
{
	OLED_ShowChinese(0,0,66,1);
	OLED_ShowChinese(16,0,67,1);
	OLED_ShowChinese(32,0,68,1);
	OLED_ShowChinese(48,0,69,1);
	OLED_ShowChinese(64,0,70,1);
	OLED_ShowChinese(80,0,71,1);
	OLED_ShowChinese(96,0,72,1);
	OLED_ShowChinese(112,0,73,1);
	OLED_ShowChinese(0,16,74,1);
	OLED_ShowChinese(16,16,75,1);
	OLED_ShowChinese(32,16,18,1); // ��
	OLED_ShowChinese(48,16,5,1);//������ɺ����û��������
	OLED_ShowChinese(64,16,6,1);
	OLED_ShowChinese(80,16,46,1);
	OLED_ShowChinese(96,16,47,1);
	OLED_ShowChinese(112,16,48,1);
	OLED_ShowChinese(0,32,9,1);
	OLED_ShowChinese(16,32,10,1);
	OLED_ShowChinese(32,32,11,1);
	OLED_ShowChinese(48,32,12,1);
	OLED_ShowChinese(64,32,13,1);
	OLED_ShowChinese(80,32,14,1);
	OLED_ShowChinese(96,32,56,1);
	OLED_ShowChinese(112,32,94,1);
	OLED_ShowChar(24,48,'A',16,1);
	OLED_ShowChar(32,48,'D',16,1);
	OLED_ShowChar(40,48,'C',16,1);
  OLED_ShowNumber(78,48,Angle_Balance,4,16);
	OLED_Refresh_Gram();
}

//ϵͳ���ҳ�����ڼ������Ƿ���ȷ........10     �뱣�ְڸ˾�ֹ
void check_display(void)
{
	OLED_ShowChinese_12(21,0,56,1);
	OLED_ShowChinese_12(35,0,57,1);
	OLED_ShowChinese_12(49,0,58,1);
	OLED_ShowChinese_12(63,0,59,1);
	OLED_ShowChinese_12(77,0,60,1);
	OLED_ShowChinese_12(91,0,61,1);
	OLED_ShowChinese_12(105,0,62,1);
	OLED_ShowChinese_12(0,16,63,1);
	OLED_ShowChinese_12(14,16,64,1);
	OLED_ShowChinese_12(28,16,65,1);
	OLED_ShowChinese_12(46,16,28,1);
	OLED_ShowChinese_12(53,16,28,1);
	OLED_ShowChinese_12(60,16,28,1);
	OLED_ShowChinese_12(67,16,28,1);
	OLED_ShowChinese_12(74,16,28,1);
	OLED_ShowChinese_12(81,16,28,1);
	//����ʱ
	//�뱣�ְڸ˾�ֹ
	OLED_ShowChinese_12(14,32,12,1);
	OLED_ShowChinese_12(28,32,43,1);
	OLED_ShowChinese_12(42,32,44,1);
	OLED_ShowChinese_12(56,32,36,1);
	OLED_ShowChinese_12(70,32,37,1);
	OLED_ShowChinese_12(84,32,45,1);
	OLED_ShowChinese_12(98,32,46,1);
	OLED_ShowChar(24,48,'A',16,1);
	OLED_ShowChar(32,48,'D',16,1);
	OLED_ShowChar(40,48,'C',16,1);
  OLED_ShowNumber(78,48,Angle_Balance,4,16);
	OLED_Refresh_Gram();
}

//���ڳɹ�ҳ�����ڳɹ�  �����û���������ϵͳ
void success_display(void)
{
	OLED_ShowChinese(32,4,5,1);      /*��*/
	OLED_ShowChinese(48,4,6,1);      /*��*/
	OLED_ShowChinese(64,4,47,1);     /*��*/
	OLED_ShowChinese(80,4,87,1);     /*��*/
	OLED_ShowChinese(24,24,9,1);     /*��*/
	OLED_ShowChinese(40,24,10,1);    /*��*/
	OLED_ShowChinese(56,24,11,1);    /*��*/
	OLED_ShowChinese(72,24,12,1);    /*��*/
	OLED_ShowChinese(88,24,13,1);    /*��*/
	OLED_ShowChinese(104,24,14,1);   /*��*/
	OLED_ShowChinese(0,44,88,1);     /*��*/
	OLED_ShowChinese(16,44,89,1);    /*��*/
	OLED_ShowChinese(32,44,90,1);    /*ϵ*/
	OLED_ShowChinese(48,44,91,1);	   /*ͳ*/
	OLED_Refresh_Gram();
}

//����ʧ��ҳ��	��鲻ͨ��  �����µ���
void fail_display(void)
{
	OLED_ShowChinese(24,16,56,1);
	OLED_ShowChinese(40,16,94,1);
	OLED_ShowChinese(56,16,100,1);
	OLED_ShowChinese(72,16,101,1);
	OLED_ShowChinese(88,16,102,1);
	OLED_ShowChinese(24,32,0,1);
	OLED_ShowChinese(40,32,103,1);
	OLED_ShowChinese(56,32,104,1);
	OLED_ShowChinese(72,32,5,1);
	OLED_ShowChinese(88,32,6,1);
	OLED_Refresh_Gram();
}
