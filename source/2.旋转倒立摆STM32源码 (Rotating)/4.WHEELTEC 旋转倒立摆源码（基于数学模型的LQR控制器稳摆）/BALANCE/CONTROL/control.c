#include "control.h"	
#include "math.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
int Balance_Pwm,Position_Pwm; //Ŀ��Ƕ�PWM��Ŀ��λ��PWM
u8 Position_Target;//���ڱ��λ�ÿ��Ƶ�ʱ��
u8 step1_flag=0;//�Զ���ڵ�һ����־λ�������ж��Զ����ǰ�Ƿ�����ڵĵ�һ��
u16 step1_target=10000;//�Զ����˦��λ�ã����ھ����Զ����ʱ�ڸ˻ζ��ķ���
u16 success_count=0; //��¼�ڸ���ƽ��λ�õĴ����������ж��Ƿ���Խ���ƽ��״̬
u8 hand_start=0,auto_start=0; //�ֶ���ڡ��Զ�����жϣ������жϵ�ǰ״̬�Ǵ����Զ����ģʽ�����ֶ����ģʽ
u8 fail_count=0;//�Զ����ʧ�ܴ����������жϺ�ʱ��ʼ����ȥ�ж�����Ƿ�ɹ�
u8 hand_flag=0; //�ֶ���ڱ�־λ�������ж��Ƿ�Ҫ¼���������ֵ��ΪĿ��ֵ����ƽ��
int Last_Encoder=10000;//������һ�α���������ֵ�����ڱ�����һ�α���������ֵ�����������������ε�ֵ֮��
int abnormal_motor;//ǰ��������Ĳ�ֵ�������ж��Ƿ�Ҫͣת���

//PID��ر���
float Last_Bias;    
float Last_Position;

//
float theta=0, theta_dot, alpha=0, alpha_dot, last_alpha=0;//��ת�۵�ת�ǣ���ת�۵Ľ��ٶȣ��ڸ˵���ǣ��ڸ˵Ľ��ٶ�
float offset=0;
float k1=-1.6349, k2=-1.7563, k3=26.8000, k4=2.3682;//����ϵ��
float t = 0.020;//����ʱ�䣨�����жϵ�ʱ�䣩
float u;//��ѹ
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
				 if(++delay_50==3)	 delay_50=0,delay_flag=0;          //===���������ṩ50ms�ľ�׼��ʱ  10��*5ms = 50ms
			 }	
			 
    	Encoder=Read_Encoder(4);             	                   //===���±�����λ����Ϣ	 
			theta_dot = ( ( (Last_Encoder - Encoder) / 1040.0f )*2*PI ) / t;
			theta += theta_dot*t;
			Last_Encoder = Encoder;
			
			Angle_Balance=Get_Adc_Average(3,15);                     //===������̬
			alpha = angle_count(Angle_Balance) /180.0f * PI;
			alpha = -alpha;
			alpha_dot = (alpha - last_alpha) / t;
			last_alpha = alpha;
			
			 //����
			 if (alpha>-0.7854 && alpha<0.7854)
				 {
					 u = -(k1*(theta+offset) + k2*theta_dot + k3*alpha + k4*alpha_dot); //�Ȱ�
					 }
				 else
					 {
						 u=0;
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
	Key();                                //===ɨ�谴���仯    	
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
float angle_count(int angle_adc)
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
�������ܣ����PD����
��ڲ������Ƕ�
����  ֵ����ǿ���PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int Balance(float Angle)
{  
   float Bias;                       //���ƫ��
	 static float D_Bias;    //PID��ر���
	 int balance;                      //PWM����ֵ 
	 Bias=Angle-ZHONGZHI;              //���ƽ��ĽǶ���ֵ �ͻ�е���
	 D_Bias=Bias-Last_Bias;            //���ƫ���΢�� ����΢�ֿ���
	 balance=-Balance_KP*Bias-D_Bias*Balance_KD;   //===������ǿ��Ƶĵ��PWM  PD����
   Last_Bias=Bias;                   //������һ�ε�ƫ��
	 return balance;
}

/**************************************************************************
�������ܣ�λ��PD���� 
��ڲ�����������
����  ֵ��λ�ÿ���PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int Position(int Encoder)
{  
   static float Position_PWM,Position_Bias,Position_Differential;
	 static float Position_Least;
  	Position_Least =Encoder-Position_Zero;             //===
    Position_Bias *=0.8;		   
    Position_Bias += Position_Least*0.2;	             //===һ�׵�ͨ�˲���  
	  Position_Differential=Position_Bias-Last_Position;
	  Last_Position=Position_Bias;
		Position_PWM=Position_Bias*Position_KP+Position_Differential*Position_KD; //===�ٶȿ���		
	  return Position_PWM;
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
	  int Amplitude=7000;    //===PWM������7200 ������7000
	  if(Moto<-Amplitude) Moto=-Amplitude;	
		if(Moto>Amplitude)  Moto=Amplitude;		
}
/**************************************************************************
�������ܣ������޸Ŀ��ưڸ˵�λ��
��ڲ�������
����  ֵ����
**************************************************************************/
void Key(void)
{	

	static int tmp,flag,count;

	tmp=click_N_Double(20); 
	if(tmp==1)flag=1;//++
  if(tmp==2)flag=2;//--
	
	if(flag==1) //�ڸ�˳ʱ���˶�
	{
		offset+=0.035;
		count++;	
		if(count==367) 	flag=0,count=0;   //����Ŀ��λ�� +2080
	}	
		if(flag==2) //�ڸ���ʱ���˶�
	{
		offset-=0.035;
		count++;	
		if(count==367) 	flag=0,count=0;   //����Ŀ��λ�� +2080
	}


}

/**************************************************************************
�������ܣ��쳣�رյ��
��ڲ�������ѹ
����  ֵ��1���쳣  0������
**************************************************************************/
u8 Turn_Off(int voltage)
{
	    u8 temp; 
			if(1==Flag_Stop||Angle_Balance<(ZHONGZHI-800)||Angle_Balance>(ZHONGZHI+800)||voltage<1110) //��ص�ѹ���ͣ��رյ��
			{	      
      Flag_Stop=1;				
      temp=1;                                            
			AIN1=0;                                            
			AIN2=0;
      }
			else
      temp=0;
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


