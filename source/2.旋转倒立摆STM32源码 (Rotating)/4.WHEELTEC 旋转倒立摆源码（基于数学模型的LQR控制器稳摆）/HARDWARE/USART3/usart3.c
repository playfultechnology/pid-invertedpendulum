
#include "usart3.h"
u8 Usart3_Receive;
 u8 mode_data[8];
 u8 six_data_stop[3]={0X59,0X59,0X59};  //ֹͣ��������
 u8 six_data_start[3]={0X58,0X58,0X58};  //������������

/**************************************************************************
�������ܣ�����3��ʼ��
��ڲ�����pclk2:PCLK2 ʱ��Ƶ��(Mhz)    bound:������
����  ֵ����
**************************************************************************/
void uart3_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
  mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<3;   //ʹ��PORTB��ʱ��  
	RCC->APB1ENR|=1<<18;  //ʹ�ܴ���ʱ�� 
	GPIOB->CRH&=0XFFFF00FF; 
	GPIOB->CRH|=0X00008B00;//IO״̬����
	GPIOB->ODR|=1<<10;	  
	RCC->APB1RSTR|=1<<18;   //��λ����1
	RCC->APB1RSTR&=~(1<<18);//ֹͣ��λ	   	   
	//����������
 	USART3->BRR=mantissa; // ����������	 
	USART3->CR1|=0X200C;  //1λֹͣ,��У��λ.
	//ʹ�ܽ����ж�
	USART3->CR1|=1<<8;    //PE�ж�ʹ��
	USART3->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(2,1,USART3_IRQn,2);//��2��������ȼ� 
}

/**************************************************************************
�������ܣ�����3�����ж�
��ڲ�������
����  ֵ����
**************************************************************************/
void USART3_IRQHandler(void)
{	
	if(USART3->SR&(1<<5))//���յ�����
	{	  
	 static	int uart_receive=0;//����������ر���
	 uart_receive=USART3->DR; 
	 mode_data[0]=uart_receive;
//			if(mode_data[0]==six_data_start[0]			&&mode_data[1]==six_data_start[1]			&&mode_data[2]==six_data_start[2]			)
//		{	
//			Flag_Stop=0;   //3�����ٵ� С���رյ��
//			mode_data[0]=0;	mode_data[1]=0;	mode_data[2]=0;	
//		}
//			if(mode_data[0]==six_data_stop[0]			&&mode_data[1]==six_data_stop[1]			&&mode_data[2]==six_data_stop[2]			)
//		{	
//			Flag_Stop=1;   //3�����ٵ� С���������
//			mode_data[0]=0;	mode_data[1]=0;	mode_data[2]=0;	
//		}
	
		
		
//	  if(uart_receive>10)  //Ĭ��ʹ��appΪ��MiniBalanceV3.5 ��ΪMiniBalanceV3.5��ң��ָ��ΪA~H ��HEX������10
//    {			
//			if(uart_receive==0x5A)	Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ɲ��
//			else if(uart_receive==0x41)	Flag_Qian=1,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ǰ
//			else if(uart_receive==0x45)	Flag_Qian=0,Flag_Hou=1,Flag_Left=0,Flag_Right=0;//////////////��
//			else if(uart_receive==0x42||uart_receive==0x43||uart_receive==0x44)	
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=1;  //��
//			else if(uart_receive==0x46||uart_receive==0x47||uart_receive==0x48)	    //��
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=1,Flag_Right=0;
//			else Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ɲ��
//  	}
//		if(uart_receive<10)     //����appΪ��MiniBalanceV1.0  ��ΪMiniBalanceV1.0��ң��ָ��ΪA~H ��HEX��С��10
//		{			
//			Flag_sudu=1;//�л������ٵ�
//			if(uart_receive==0x00)	Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ɲ��
//			else if(uart_receive==0x01)	Flag_Qian=1,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ǰ
//			else if(uart_receive==0x05)	Flag_Qian=0,Flag_Hou=1,Flag_Left=0,Flag_Right=0;//////////////��
//			else if(uart_receive==0x02||uart_receive==0x03||uart_receive==0x04)	
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=1;  //��
//			else if(uart_receive==0x06||uart_receive==0x07||uart_receive==0x08)	    //��
//														Flag_Qian=0,Flag_Hou=0,Flag_Left=1,Flag_Right=0;
//			else Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;//////////////ɲ��
//  	}	
		mode_data[2]=mode_data[1];
		mode_data[1]=mode_data[0];
	}  											 
} 


