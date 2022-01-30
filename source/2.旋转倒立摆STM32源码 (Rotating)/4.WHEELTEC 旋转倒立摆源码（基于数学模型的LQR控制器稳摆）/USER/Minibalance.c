#include "sys.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
u8 Flag_Stop=1,delay_50,delay_flag;         //ֹͣ��־λ 50ms��׼��ʾ��־λ
u8 system_start=0;                          //�����ں�����־λ
u8 tips_flag = 0;                           //OLED��ʾ������־λ
int Encoder,Position_Zero=10000;            //���������������
int Moto;                                   //���PWM���� Ӧ��Motor�� ��Moto�¾�	
int Voltage;                                //��ص�ѹ������صı���
float Angle_Balance;                        //��λ�ƴ���������
float Balance_KP=50,Balance_KD=244,Position_KP=25,Position_KD=600;  //PIDϵ��
float Position_KP_1=90,Position_KD_1=180;  //PIDϵ��
float Menu=1,Amplitude1=1,Amplitude2=10,Amplitude3=1,Amplitude4=10; //PID������ز���
int main(void)
{ 
	Stm32_Clock_Init(9);            //=====ϵͳʱ������
	delay_init(72);                 //=====��ʱ��ʼ��
	JTAG_Set(JTAG_SWD_DISABLE);     //=====�ر�JTAG�ӿ�
	JTAG_Set(SWD_ENABLE);           //=====��SWD�ӿ� �������������SWD�ӿڵ���
	delay_ms(1000);                 //=====��ʱ�������ȴ�ϵͳ�ȶ�
	delay_ms(1000);                 //=====��ʱ�������ȴ�ϵͳ�ȶ� ��2s
	LED_Init();                     //=====��ʼ���� LED ���ӵ�Ӳ���ӿ�
	EXTI_Init();                    //=====������ʼ��(�ⲿ�жϵ���ʽ)
	OLED_Init();                    //=====OLED��ʼ��
	uart_init(72,128000);           //=====��ʼ������1
  MiniBalance_PWM_Init(7199,0);   //=====��ʼ��PWM 10KHZ������������� 
	Encoder_Init_TIM4();            //=====��ʼ����������TIM2�ı������ӿ�ģʽ�� 
	Adc_Init();                     //=====ADC�ɼ���ʼ��
	Timer1_Init(199,7199);           //=====��ʱ�жϳ�ʼ�� 
	while(1)
		{      
				Tips();                   //===OLED��ʾ����ʾ
				delay_flag=1;	            //===50ms�жϾ�׼��ʱ��־λ		  
				while(delay_flag);        //===50ms�жϾ�׼��ʱ  ��Ҫ�ǲ�����ʾ��λ����Ҫ�ϸ��50ms��������
		} 
}
