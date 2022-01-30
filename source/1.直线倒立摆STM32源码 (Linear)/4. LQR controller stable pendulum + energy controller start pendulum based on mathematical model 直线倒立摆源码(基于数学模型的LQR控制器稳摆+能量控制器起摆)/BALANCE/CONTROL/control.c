#include "control.h"		
#include "math.h"		
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/

float E;
float t=0.020;
float last_angle=0;
float x=0, x_speed, accel, angle, angle_speed, u; //λ��,�ٶ� ,���ٶ�,�Ƕ�,���ٶ�,��ѹ	
float k1 = -36.4232, k2 = -40.0996, k3 = -101.4914, k4 = -15.8376;			//LQR״̬����ϵ��
int last_Encoder=10000; //������ֵ

/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
          TIM1���Ƶ�5ms��ʱ�ж� 
**************************************************************************/
int TIM1_UP_IRQHandler(void)  
{    
	if(TIM1->SR&0X0001)//5ms��ʱ�ж�
	{
			 TIM1->SR&=~(1<<0);                                       //===�����ʱ��1�жϱ�־λ	                     
	     if(delay_flag==1)
			 {
				 if(++delay_50==4)	 delay_50=0,delay_flag=0;          //===���������ṩ50ms�ľ�׼��ʱ  10��*5ms = 50ms
			 }
			 //��ñ�������ֵ����ʼ����ֵ10000
			 Encoder=Read_Encoder(4); 
			 
			 //�ٶ� 
			 x_speed = ( ( (last_Encoder - Encoder) / 1040.0f )*0.036* PI ) / t; //�����ʱ��
			 
			 //�����ʵ���ƶ���λ��
			 x += x_speed*t;
			 
			 last_Encoder = Encoder; //������һ�α�������ֵ
			 
			 //��ýǶ�ԭʼֵ 
       Angle_Balance=Get_Adc_Average(3,10); 
			 
			 //�Ƕȼ��� 
		   angle = angle_count(Angle_Balance) /180.0f * PI; //����Ƕ�ֵ
			 
			 //���ٶȼ���
			 angle_speed = (angle - last_angle) / t;
			 last_angle = angle ;//������һ�εĽǶ�ֵ
			 
			 //����
			 E = 0.0032*angle_speed*angle_speed+0.1764*(cos(angle)-1);
			 
			 //����
			 if (angle>-0.5236 && angle<0.5236)
			 {
				 u = -(k1* x + k2 * x_speed + k3* angle +  k4 * angle_speed);  //�Ȱ�
			 }
			 else
			 {
				 if ( E > 0 )
				 {
					 u = 0;
				 }
				 else
				 {
						 accel = 98*Sign( E*angle_speed*cos(angle) ) + Sign(x)*log( 1 - myabs(x*100)/15 );
						 u = 0.0651*accel + 11.5556*x_speed + (0.0028*cos(angle)*(0.1764*sin(angle)-0.0180*accel))/0.0012 - 0.8440*sin(angle)*angle_speed*angle_speed;  //���
				 }	
			 }
			 Moto = (int)(7200*(u/12.0f));
			 Xianfu_Pwm();
			 if(Flag_Stop==0)
			 {
				 Set_Pwm(Moto);
			 }
			 else
			 {
				 Set_Pwm(0);
			 }
       if(Flag_Stop==1)	
			 Set_Pwm(0);

		}
		
    if(Flag_Stop==0)	Led_Flash(100);      //===LED��˸ָʾϵͳ�������� 
		Voltage=Get_battery_volt();           //===��ȡ��ص�ѹ	                 
	  return 0;	  
} 

/**************************************************************************
�������ܣ����ź���
**************************************************************************/
int Sign(float value)
{
	if (value>0)
	{
		return 1;
	}
	else if (value<0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
/**************************************************************************
�������ܣ�����ǶȺ���
��ڲ�����ADCֵ
����  ֵ���Ƕ�ֵ
��    �ߣ�ƽ��С��֮��
**************************************************************************/
float angle_count(float angle_adc)
{
	long int angle_int;
	float angle;

	angle_int = 3100 - angle_adc;
	angle = angle_int*0.0879f;
	
	if(angle_adc>=0&&angle_adc<=1052)
	{
		angle -=360;
	}

	return angle;
}


/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ�����PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int moto)
{
    	if(moto<0)			BIN2=1,			BIN1=0;
			else 	          BIN2=0,			BIN1=1;
			PWMB=myabs(moto);
}

/**************************************************************************
�������ܣ�����PWM��ֵ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=6900;    //===PWM������7200 ������6900
	  if(Moto<-Amplitude) Moto=-Amplitude;	
		if(Moto>Amplitude)  Moto=Amplitude;		
}

/**************************************************************************
�������ܣ��쳣�رյ��
��ڲ�������ѹ
����  ֵ��1���쳣  0������
**************************************************************************/
u8 Turn_Off(int voltage)
{
	    u8 temp; 
			if(1==Flag_Stop) //��ص�ѹ���ͣ��رյ��
			{	      
      Flag_Stop=1;				
      temp=1;                                            
			BIN1=0;                                            
			BIN2=0;
      }
			else
      temp=0;
				
			if(!(Angle_Balance>(ZHONGZHI-500)&&Angle_Balance<(ZHONGZHI+500))||(voltage<700))
			{
				Flag_Stop=1;
				temp=1;
			}

      return temp;			
}

/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}

