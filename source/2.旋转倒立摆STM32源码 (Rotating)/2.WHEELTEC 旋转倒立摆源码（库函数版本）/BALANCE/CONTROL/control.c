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
#include "control.h"		
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
/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
          TIM1���Ƶ�5ms��ʱ�ж� 
**************************************************************************/
int TIM1_UP_IRQHandler(void)  
{    
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ //5ms��ʱ�ж�
	{   
		 TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );             //===�����ʱ��1�жϱ�־λ	                                      //===�����ʱ��1�жϱ�־λ	                     
	     if(delay_flag==1)
			 {
				 if(++delay_50==10)	 delay_50=0,delay_flag=0;          //===���������ṩ50ms�ľ�׼��ʱ  10��*5ms = 50ms
			 }		
    	Encoder=Read_Encoder(4);             	                   //===���±�����λ����Ϣ	 
      Angle_Balance=Get_Adc_Average(3,15);                     //===������̬	
			 
		//������ֻ�ܽ���һ���Զ���ڣ����ֶ���ڿ���������ʱ�̽���	  
     if(Flag_Stop==0&&auto_start==1&&hand_start==0)
		 {
			 Auto_run(); 	//�Զ���ڣ�������ֻ���Զ����һ��	 
		 }
		 
		 else if (Flag_Stop==0&&hand_start==1&&auto_start==0)//�ֶ����ģʽ
		 {
			 if(hand_flag==0) hand_flag=1,Position_Zero=Read_Encoder(4);//�ֶ����ǰ����ȡһ�α��������ڵ�λ�ã�����Ϊ��ڵ�ƽ��ο��㣬ʵ������λ������ʱ�̶����ֶ����

			 Balance_Pwm =Balance(Angle_Balance);                                          //===�Ƕ�PD����	
			 if(++Position_Target>4)  Position_Pwm=Position(Encoder),Position_Target=0;    //===λ��PD���� 25ms����һ��λ�ÿ���
			  
			 if(Turn_Off(Voltage)) Flag_Stop=1;                                             //===��ѹ����ǹ��󱣻�
			 else
			  Xianfu_Pwm(),                         //===PWM�޷� ��ֹռ�ձ�100%������ϵͳ���ȶ�����
	    	Moto=Balance_Pwm-Position_Pwm,       //===����������PWM
		    Set_Pwm(Moto);                      //===��ֵ��PWM�Ĵ���
		 }
		 
		 else if (Flag_Stop==1)
		 Set_Pwm (0),hand_flag=0;
		
    //�����ת�����������⵽�����ת����ѱ�־λ��1��ϵͳֹͣ		 
		 abnormal_motor=Encoder-Last_Encoder; //��¼�������ı��
		 Last_Encoder=Encoder; //������һ�α�����ֵ
		 if(myabs(abnormal_motor)>15)  //���ƫ��ľ���ֵ����15��˵����ʱ���ת���ܿ죬���쳣״̬��ͣת���
		 {
			  Flag_Stop=1;
		 }
			 
	  	Led_Flash(100);                       //===LED��˸ָʾϵͳ�������� 
	    Voltage=Get_battery_volt();           //===��ȡ��ص�ѹ	      
			Key();                                //===ɨ�谴���仯
	}       	
	 return 0;	  
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
	  int Amplitude=6900;    //===PWM������7200 ������6900
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
	// ��Ʒʹ�õ���13�߻��������������20���ٱȣ��������õ���4��Ƶ�����תһȦ�������ϵ���ֵ��13*4*20 = 1040 
	int position=2080; //Ŀ��λ�� ���ԭʼλ����10000  תһȦ��1040 �ͱ����������йأ�Ĭ���ǰڸ�Z��תһȦ�����1040��������
	static int tmp,flag,count;
	
		
	if(Flag_Stop==1) //���û�п�ʼƽ�⣬�Ž����ж�
	{
		if(Angle_Balance>(ZHONGZHI-500)&&Angle_Balance<(ZHONGZHI+500))  //�����ƽ��λ����
	{
		hand_start=1;//�ֶ����
		auto_start=0;//�ص��Զ���ڱ�־
	}
	  else auto_start=1,hand_start=0; //  ����ƽ��λ���ϣ��Զ����
	}

	tmp=click_N_Double(100); 
	if(tmp==1)flag=1;//++
  if(tmp==2)flag=2;//--
	
	if(flag==1) //�ڸ�˳ʱ���˶�
	{
		Position_Zero++;
		count++;	
		if(count==position) 	flag=0,count=0;   //����Ŀ��λ�� +2080
	}	
		if(flag==2) //�ڸ���ʱ���˶�
	{
		Position_Zero--;
		count++;	
		if(count==position) 	flag=0,count=0;//����Ŀ��λ�� -2080
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


/**************************************************************************
�������ܣ�λ��ʽPID������
��ڲ���������������λ����Ϣ��Ŀ��λ��
����  ֵ�����PWM
����λ��ʽ��ɢPID��ʽ 
pwm=Kp*e(k)+Ki*��e(k)+Kd[e��k��-e(k-1)]
e(k)������ƫ�� 
e(k-1)������һ�ε�ƫ��  
��e(k)����e(k)�Լ�֮ǰ��ƫ����ۻ���;����kΪ1,2,,k;
pwm�������
**************************************************************************/
int Position_PID (int Encoder,int Target)
{ 	
	 static float Bias,Pwm,Integral_bias,Last_Bias;
	 Bias=Encoder-Target;                                   //����ƫ��
	 Integral_bias+=Bias;	                                  //���ƫ��Ļ���
	 Pwm=Position_KP_1*Bias+0*Integral_bias+Position_KD_1*(Bias-Last_Bias);       //λ��ʽPID������
	 Last_Bias=Bias;                                        //������һ��ƫ�� 
	 return Pwm;                                            //�������
}


/**************************************************************************
�������ܣ��Զ����
��ڲ�������
����  ֵ����
**************************************************************************/
void Auto_run(void)
{
	static u16 try_count=0;  //���ڸ����ж��Ƿ���ڳɹ�
	static u16 stop_count=0; //��¼���ͣת��ʱ��
	
	if(step1_flag==0)  //˦��  ֻ˦һ��
	{
		fail_count++; //��¼���뺯���Ĵ��� ��������� ������ν�����ҽǶȲ���ƽ��λ�ã�˵�����ʧ��
		Moto=Position_PID(Encoder,step1_target);  //��һ�� step1_target=10000 ��ȡ����ʱ��λ�ò���Ϊ10000
	  step1_target=10341; //��ֵ����һ�ν��뺯����˦��   ��¼����ֵ��10334 10337
		Xianfu_Pwm();    //pwm�޷�
		
		/*  ��ȡ������λ�ý����жϣ����ڳ�ʼֵ�ᵼ�µ����ת  */
		if(fail_count==1&&(Encoder<9980||Encoder>10020))  //��һ��˦����Ҫ�Ա�������ֵ�����жϣ������������ֵƫ���ʼֵ���󣬻ᵼ�µ����ת�����Խ�ֹ��ƫ�����ʱ�Զ����
		{
			fail_count=0,step1_target=10000,Flag_Stop=1;   //��ȡ��������ֵ��������ں���λ�ã����иĶ��������ݶ����ԭʼֵ�����������־λ��1���ȴ���һ���Զ����
		}
	  else Set_Pwm(Moto);	 //˦��
		
	 }

if(++try_count&&(Angle_Balance>ZHONGZHI-350&&Angle_Balance<ZHONGZHI+350))   //  ���˦�˵���ƽ��λ��
 {
	 try_count=0;//��������������������ʧ���ˣ������ʧ�����2
//	 if(success_count==0)  Balance_KP=60,Balance_KD=350,Position_KP=35,Position_KD=750; //¼���Զ����ʱ����Ҫ��pid����
	 if(success_count==0)  Balance_KP=30,Balance_KD=150,Position_KP=15,Position_KD=500; //¼���Զ����ʱ����Ҫ��pid����
	 success_count++; //��¼��ƽ��λ�õĴ���
	 
	 if(success_count>10) //�������10��
	 {
		 step1_flag=1; //�����ڳɹ�����Ҫ�ٽ���˦�˺���
		 //��ڳɹ������Զ԰ڸ˽���pid����
		 Balance_Pwm =Balance(Angle_Balance); 
		 if(success_count>109) //��ں��һС��ʱ�䣬��Ҫ�ðڸ˼���λ�ÿ��Ƶ�Ƶ�ʣ���pid����������ԭ���ȶ�ʱ����ֵ
		 {
			 if(success_count==110)  Balance_KP=50,Balance_KD=244,Position_KP=25,Position_KD=600;//���һ��ʱ�����pid�����ص��ֶ���ڵ�ֵ
			 success_count=115; //��סsuccess_countֵ����ֹ���ѭ��
       if(++Position_Target>4)  Position_Pwm=Position(Encoder),Position_Target=0; //===λ��PD���� 25ms����һ��λ�ÿ���
		 }
     else 	Position_Zero=step1_target+0, Position_Pwm=Position(Encoder);  //�տ�ʼ���ʱ��Ҫ������λ�ÿ��Ƽ��룬����ƽ��	 ƽ���Ŀ��λ�ñ�˦�˵ľ����Զ��������ƽ����ȶ��ԣ����岹��

		stop_count=0; //���ͣת��ʱ�������ƽ��״̬����λ��ʱ����ֹ��ʱ���ۻ����� 
		Xianfu_Pwm(); //pwm�޷�
		Moto=Balance_Pwm-Position_Pwm;  //�ں����� 
	  Set_Pwm(Moto);  //����pwm
	 }
 }
	
 //���ͣת����������� 1�����ʱû�н����ƽ��λ��   2����ں����ƽ��λ�ã���ʧ����  3����Ϊ�Ѱڸ��õ���ʼλ��
 if((!(Angle_Balance>ZHONGZHI-500&&Angle_Balance<ZHONGZHI+500))&&fail_count>2&&try_count==0) //���2����ں�����ƽ��λ�ã�����ʧ����
 {
	step1_flag=1; //˦�˱�־λ����ֹ����˦��
	Set_Pwm(0);	 //ͣת���
	 
	 //���ͣת���ӳ�һ��ʱ���ٰѵ��ʹ�ܱ�־λ��1��Ŀ���Ƿ�ֹ���У�ʹ��ԭ���ܱ���ƽ��İڸ�ʧȥ����Ŀ���
	 stop_count++; //���ͣת��ʼ��ʱ
	 if(stop_count>200) stop_count=0,Flag_Stop=1; //��־λ��1
 }
 
 //��ǡ���ѹ����
 if(((!(Angle_Balance>ZHONGZHI-500&&Angle_Balance<ZHONGZHI+500))&&fail_count>2&&try_count>120)||Voltage<1110)  //���1��3 ����ں�û�н����ƽ��λ�ã����ʧ��(����ڵ�ȷ����ȷ��ڻ�ȷ�����ʧ�ܣ�try_count��++����100)
 {
	try_count=150; //��סtry_count��ֵ����ֹ���ѭ��
	step1_flag=1;  //˦�˱�־λ����ֹ����˦��
	Set_Pwm(0);	 //ͣת���
	 
	 //���ͣת���ӳ�һ��ʱ���ٰѵ��ʹ�ܱ�־λ��1��Ŀ���Ƿ�ֹ���У�ʹ��ԭ���ܱ���ƽ��İڸ�ʧȥ����Ŀ���
	 stop_count++; //���ͣת��ʼ��ʱ
	 if(stop_count>200) stop_count=0,Flag_Stop=1; //��־λ��1
 }
 
 
}

