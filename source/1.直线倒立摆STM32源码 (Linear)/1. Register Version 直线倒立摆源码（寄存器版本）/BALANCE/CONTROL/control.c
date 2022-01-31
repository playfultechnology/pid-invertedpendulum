#include "control.h"		
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
int Balance_Pwm,Position_Pwm; //Ŀ��Ƕ�PWM��Ŀ��λ��PWM
u8 Position_Target;//���ڱ��λ�ÿ��Ƶ�ʱ��
u8 Swing_up=1; //���ڱ���ֶ����ʱ���Ƿ��ǵ�һ�ν����ֶ���ں���

//���PD�������õ��Ĳ���
float Bias;                       //���ƫ��
float Last_Bias,D_Bias;    //PID��ر���
int balance;                      //PWM����ֵ 

//λ��PD�������õ��Ĳ���
float Position_PWM,Last_Position,Position_Bias,Position_Differential;
float Position_Least;

u8 auto_run=0; //�ֶ���ڻ��Զ���ڱ�־λ��Ĭ�����ֶ����
u8 autorun_step0=0; //�Զ���ڵ�0�����ҵ��е㣬�ȴ����
u8 autorun_step1=1; //�Զ���ڵ�1��
u8 autorun_step2=0; //�Զ���ڵ�2��
long Target_Position;//Ŀ��λ��
float D_Angle_Balance; //�ڸ˽Ƕȱ仯��
long success_count=0;//�ڸ���ƽ��λ�õĳɹ�������¼
u8 success_flag=0; //�Զ����ʱ����ƽ�����������ڵı�־λ
long wait_count=0; //�ȴ���������ʱʱ�䵽�󣬻�ȡ��ڳɹ���λ��
long D_Count;//���ڸ�����ȡ�ڸ˽Ƕȱ仯�ʵ��м����
float Last_Angle_Balance; //���ڻ�ȡ�ڸ˽Ƕȱ仯�ʺ����У�������һ�νǶ�

u8 left,right;
/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
          TIM1���Ƶ�5ms��ʱ�ж� 
**************************************************************************/
int TIM1_UP_IRQHandler(void) {
	// 5ms timer interrupt 
	if(TIM1->SR&0X0001)//5ms��ʱ�ж�
	{
		// Clear Timer 1 Interrupt Flag 
		TIM1->SR&=~(1<<0);                                       //===�����ʱ��1�жϱ�־λ	                     
		if(delay_flag==1) {
			if(++delay_50==10)	 delay_50=0, delay_flag=0;          //===���������ṩ50ms�ľ�׼��ʱ  10��*5ms = 50ms
		}		
		Encoder=Read_Encoder(4);             	                   //===���±�����λ����Ϣ	 
		Angle_Balance=Get_Adc_Average(3,10);                     //===������̬	
		Get_D_Angle_Balance();                                   //===��ðڸ˽��ٶ�
			
		if(auto_run==1) //�Զ����ģʽ
		{
			if(Flag_Stop==0)
			{	
				// Start swing step 0 and return to midpoint on the edge.
				// The internal implementation of the function executes the function only once 
				Find_Zero();//��ڵ�0�����ڱ�Ե�ص��е㡣�����ڲ�ʵ��ִֻ��һ�ι���
				if(autorun_step0==1) Auto_run();//ִ���Զ����			
				Xianfu_Pwm();
				
				// Slider edge protection in auto-lift step 1 
				//�Զ���ڲ���1�еĻ����Ե����
				if(autorun_step1==0&&(Encoder>=9900||Encoder<=5900))
				Set_Pwm(0),Flag_Stop=1;		
				
				// Angle protection after successful swing, edge protection 
				//��ڳɹ���ĽǶȱ�������Ե����
				if((success_flag==1)&&((!((Angle_Balance>ANGLE_MIDDLE-600)&&(Angle_Balance<ANGLE_MIDDLE+600))) ||(Encoder>=9900||Encoder<=5900)))
				Set_Pwm(0),Flag_Stop=1;
					
				else
					Set_Pwm(Moto);
			}
		}						 
		if(auto_run==0)
    {
			// Low voltage protection
			Turn_Off(Voltage);//��ǡ���ѹ����
			if(Swing_up==0) Position_Zero=Encoder, Last_Position=0, Last_Bias=0, Position_Target=0, Swing_up=1;	
			if(Flag_Stop==0)
			{
				Balance_Pwm =Balance(Angle_Balance);                                          //===�Ƕ�PD����	
				if(++Position_Target>4) Position_Pwm=Position(Encoder),Position_Target=0;    //===λ��PD���� 25ms����һ��λ�ÿ���
				Moto=Balance_Pwm-Position_Pwm;      //===����������PWM
				Xianfu_Pwm(),                         //===PWM�޷� ��ֹռ�ձ�100%������ϵͳ���ȶ�����
		    Set_Pwm(Moto);                      //===��ֵ��PWM�Ĵ���				
			}
		}
    if(Flag_Stop==1)	
			Set_Pwm(0);
	}
	
	// Blinking LED indicates normal operation
  if(Flag_Stop==0)	Led_Flash(100);      //===LED��˸ָʾϵͳ��������
	
	// Get battery voltage 
	Voltage=Get_battery_volt();           //===��ȡ��ص�ѹ

	// Read input
	Key();                                //===ɨ�谴���仯
	
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
  	Position_Least =Encoder-Position_Zero;             //===
    Position_Bias *=0.8;		   
    Position_Bias += Position_Least*0.2;	             //===һ�׵�ͨ�˲���  
	  Position_Differential=Position_Bias-Last_Position;
	  Last_Position=Position_Bias;
		Position_PWM=Position_Bias*Position_KP+Position_Differential*Position_KD; //===�ٶȿ���		
//    Position_PWM=Position_Bias*(Position_KP+Basics_Position_KP)/2+Position_Differential*(Position_KD+Basics_Position_KD)/2; //===λ�ÿ���	
	  return Position_PWM;
}

/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ�����PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int moto)
{
	if(moto<0) BIN2=1, BIN1=0;
	else BIN2=0, BIN1=1;
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
	// The product uses a 13-wire Hall sensor motor with a reduction ratio of 20.
	// The program uses a 4-fold frequency.
	// The value on the encoder when the motor rotates once is therefore 13*4*20 = 1040 
	// ��Ʒʹ�õ���13�߻��������������20���ٱȣ��������õ���4��Ƶ�����תһȦ�������ϵ���ֵ��13*4*20 = 1040
	
	// Target position The original position of the motor is 10000, and one revolution is 1040.
	// It is related to the accuracy of the encoder.
	// The default is one revolution of the Z axis of the pendulum rod, and 1040 transition edges are output. 
	int position = 1040; //Ŀ��λ�� ���ԭʼλ����10000  תһȦ��1040 �ͱ����������йأ�Ĭ���ǰڸ�Z��תһȦ�����1040��������
	static int tmp,flag,count;
	tmp=click_N_Double(100); 
	
	if(tmp==1)flag=1;//++
  if(tmp==2)flag=2;//--

	// Positive
	if(flag==1) //�ڸ����������˶�
	{
		Position_Zero+=4;
		count+=4;	
		if(count==position)	flag=0,count=0;
	}
	// Negative
	if(flag==2) //�ڸ��򷴷����˶�
	{
		Position_Zero-=4;
		count+=4;	
		if(count==position)	flag=0,count=0;
	}
	
	if(Long_Press_KEY2()==1)
	{
		Position_Zero=10000;
		auto_run=!auto_run;
	}
	
	if(Flag_Stop==1) {
		if(auto_run==1) {
			// Automatic swing mode
			//��ʾ��Ϣ
			//�Զ����ģʽ
			LED=0;
		}
		
	else if(auto_run==0) LED=1;
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
	if(1==Flag_Stop) //��ص�ѹ���ͣ��رյ��
	{	      
		Flag_Stop=1;				
		temp=1;                                            
		BIN1=0;                                            
		BIN2=0;
	}
	else
		temp=0;
	
	// If the angle differs from the balance point by > 500, or voltage drops below 700 
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

/**************************************************************************
�������ܣ���λ�궨
��ڲ�������
����  ֵ����
**************************************************************************/
void Find_Zero(void)
{
	 static float count;
	
	//��ʼ��ʱ autorun_step0=0����Ȼ��ִ��
if(autorun_step0==0)  //�ص���λ
 {
		Position_Zero=POSITION_MIDDLE;//����Ŀ���м�λ��

	 
	 //˳��pid���ƣ��ðڸ˾����ȶ�������Ŀ��Ƕ�����ʼ�ǣ�Ŀ��λ������ʼλ�ã�
		Balance_Pwm=Balance(Angle_Balance+2070)/8;   //���PD����						
		Position_Pwm=Pre_Position(Encoder);
 
	 //���ƫ�����е�̫�࣬��ʼPID���ƻ�������
		if(Encoder<7950)Moto=Incremental_PI(Encoder,POSITION_MIDDLE); //λ�ñջ�����;//�뿪��Ե�ط��ٿ�ʼPID����
		else	Moto=-Balance_Pwm+Position_Pwm;     //��ȡPD���������PWM

 //�жϽǶȺ�λ���Ƿ���ԭʼλ�ã���� ��⵽200�� ��ԭʼλ�ã������Եȴ���� 
 //�ȴ���ڣ��Ѹո��ù��ı�־λ�����pwmֵȫ�����㣬����Target_Position=POSITION_MIDDL-668�����������ʱ�ĵ�һ��Ŀ��㣬ʹautorun_step0=1��Ҫ�ٽ����������
		if(Angle_Balance<(ANGLE_ORIGIN+300)&&Angle_Balance>(ANGLE_ORIGIN-300)&&(Encoder>(POSITION_MIDDLE-50)&&Encoder<(POSITION_MIDDLE+50)))  count++;
		if(Angle_Balance<(ANGLE_ORIGIN+300)&&Angle_Balance>(ANGLE_ORIGIN-300))count+=0.1;
		if(count>200)	autorun_step0=1,autorun_step1=0,Moto=0,Target_Position=POSITION_MIDDLE-668;//�ڸ��˶����м�λ�ã�ֹͣ //����Ŀ��λ�ã�׼��˦��
	 }
	 
	 //�Ե���ٶ��޷�����ֹ����ջ�λ�ÿ��ƹ���
		if(Moto>2500)	Moto=2500;   //����λ�ñջ����ƹ��̵��ٶ�
		if(Moto<-2500)Moto=-2500;
		Set_Pwm(Moto);//���Ƶ��
 }		
/**************************************************************************
�������ܣ��Զ����
��ڲ�������
����  ֵ����
**************************************************************************/
void Auto_run(void)
{
	static u8 speed=0;
	static u8 help_count=0;
	static u16 pid_adjust=0;

	if(autorun_step1==0)  //�Զ���ڵ�һ��   ����һ��ִ�У�һ���ǽ������
	{
		// Decide which way to turn
		//�ж�Ӧ������һ�߻���						
		if((Angle_Balance>(ANGLE_ORIGIN-120)&&Angle_Balance<(ANGLE_ORIGIN+120))) {
			if(D_Angle_Balance<=0) right=1;
			else if(D_Angle_Balance>0) left=1;
		}	

		//�жϵ��ڸ˻ص���ʼλ��ʱӦ�ø����ٶȺ�λ��
		if(left==1) {
			if((Angle_Balance>(ANGLE_ORIGIN-50)&&Angle_Balance<(ANGLE_ORIGIN+50))) {
				left=0;
				Target_Position=POSITION_MIDDLE+800;
				// Let the pendulum adjust slowly until it is stable
				if(speed>1) Target_Position=POSITION_MIDDLE+160; //�ðڸ˻�������ֱ�������Ȱ�
			}
		}	
		else if(right==1) {
			if((Angle_Balance>(ANGLE_ORIGIN-50)&&Angle_Balance<(ANGLE_ORIGIN+50))) {
				right=0;
				Target_Position=POSITION_MIDDLE-482;
				// Let the pendulum adjust slowly until it is stable
				if(speed>1) Target_Position=POSITION_MIDDLE-160; //�ðڸ˻�������ֱ�������Ȱ�
			}
		}							

		// Position closed loop control
		//λ�ñջ�����
	  Moto = Position_PID(Encoder, Target_Position); 
		
		// The pendulum has reached near the equilibrium point and begins the slow adjustment phase
		//�ڸ��Ѿ������ƽ��㸽������ʼ�������ڽ׶Ρ�
		if(Angle_Balance<(ANGLE_MIDDLE+385)&&Angle_Balance>(ANGLE_MIDDLE-385)) {
			speed++;
		}

		// Judging whether the current situation is in line with the swing: swing elements: 
		// the position is not on the edge, the angle is near the balance point, and the angular velocity is close to 0 
		//�жϵ�ǰ����Ƿ������ڣ����Ҫ�أ�λ�ò��ڱ�Ե���Ƕ���ƽ��㸽�������ٶȽӽ���0
		if(Angle_Balance<(ANGLE_MIDDLE+120)&&Angle_Balance>(ANGLE_MIDDLE-120)&&(Encoder>6300&&Encoder<9300)&&(D_Angle_Balance>-30&&D_Angle_Balance<30))
		{
			speed++;
			success_count++;		
		}
    
		// If it is not within the condition range, it needs to be reset to wait for the next judgment 
    else success_count = 0;//���û��������Χ�ڣ�����Ҫ��������ȴ��´��ж�

		// Satisfy 3 times of starting and swinging, the mark can be successfully started
		// and the pid at the moment of starting and swinging is given. 
		//����3������������ǿ��Գɹ���ڣ����������˲���pid
		if(success_count>3) { 
			autorun_step1=1,success_flag=1;
			Balance_KP=210,Balance_KD=150,Position_KP=8,Position_KD=130;//�Զ����˲���pid����
		}
	
		// Limits
		//�޷�
		if(Moto>4100)	Moto=4100;   //����λ�ñջ����ƹ��̵��ٶ�
		if(Moto<-5100)Moto=-5100;	
	}
	// Satisfy the start-up conditions, normal start-up 
	//��������������������
	else if(success_flag==1)//����ߵ㣬���
	{	
		// Before starting the swing, first obtain the current position as the target point of balance 
		if(wait_count==0) Position_Zero=Encoder;//���ǰ���Ȼ�ȡ��ǰλ����Ϊƽ���Ŀ���
		Balance_Pwm =Balance(Angle_Balance);                                          //===�Ƕ�PD����	
		if(++Position_Target>4)   Position_Pwm=Position(Encoder),Position_Target=0;    //===λ��PD���� 25ms����һ��λ�ÿ���
		
		// After starting for a while, let the pendulum slowly return to the middle point 
		//���һ��ʱ����ðڸ˻����ָ����м��
		wait_count++;
		if(wait_count>100&&wait_count<2000) {
			if(Position_Zero>8100) Position_Zero--;
			else if(Position_Zero<8100)Position_Zero++;
		}
		// Lock the counter value to prevent overflow loop 
		if(wait_count>2000) wait_count = 2001;//��ס������ֵ��ֹ���ѭ��
		
		// Adjust the pid stage after starting the swing.
		// The pid parameter required for the start-up moment is different from the stable swing.
		// Therefore, when the swing is successfully entered into the stable swing,
		// the pid parameter can be adjusted back to the value when the swing was stable. 
		//��ں����pid�׶�	 ���˲�����Ȱ���Ҫ��pid������һ�� ���Ե���ڳɹ������Ȱں󣬼����԰�pid���������Ȱ�ʱ����ֵ				 
		if(help_count==0)
		{
			pid_adjust ++ ;
			// Adjust slowly to avoid shock and instability 
			if( pid_adjust%100==0) //�������� ���������ʧ��
			{
				Balance_KP += 10;
				Balance_KD += 10;
				Position_KP += 0.5;
				Position_KD +=10;
			}
			if(Balance_KP>400) Balance_KP=400;
			if(Balance_KD>400) Balance_KD=400;
			if(Position_KP>20) Position_KP=20;
			if(Position_KD>300) Position_KD=300;
			if(Balance_KP==400&&Balance_KD==400&&Position_KP==20&&Position_KD==300) help_count=1; //����������Ϻ��ͷ�pid��ֵ�޷�����ʱ�û�����ͨ�������ı�pid����
		}		 
		//�ں����������Ч��
		Moto=Balance_Pwm-Position_Pwm;     //��ȡPD���������PWM
	}
}
/**************************************************************************
�������ܣ�����PI������
��ڲ���������������ֵ��Ŀ���ٶ�
����  ֵ�����PWM
��������ʽ��ɢPID��ʽ 
pwm+=Kp[e��k��-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)������ƫ�� 
e(k-1)������һ�ε�ƫ��  �Դ����� 
pwm�����������
�����ǵ��ٶȿ��Ʊջ�ϵͳ���棬ֻʹ��PI����
pwm+=Kp[e��k��-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PI (int Encoder,int Target)
{ 	
	 static float Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                                  //����ƫ��
	 Pwm+=10*(Bias-Last_bias)/20+10*Bias/20;   //����ʽPI������
	 Last_bias=Bias;	                                     //������һ��ƫ�� 
	 return Pwm;                                           //�������
}

/**************************************************************************
�������ܣ���ȡ�ڸ˽Ƕȱ仯��
��ڲ�������
����  ֵ����
**************************************************************************/
void Get_D_Angle_Balance(void)
{
		if(++D_Count>5) //��ȡ�Ƕȱ仯�ʣ���� ʱ�䳣��25ms
		{
			D_Angle_Balance=Mean_Filter(Angle_Balance-Last_Angle_Balance);	//ƽ���˲��õ�������С�İڸ˽��ٶ���Ϣ		
//			D_Angle_Balance=Angle_Balance-Last_Angle_Balance;	//�õ��ڸ˽��ٶ���Ϣ		
			Last_Angle_Balance=Angle_Balance; //������ʷ����
			D_Count=0;	//����������
		}
}

/**************************************************************************
�������ܣ�ƽ�� �˲�
��ڲ������ٶ�
����  ֵ���˲��������
**************************************************************************/
int Mean_Filter(int sensor)
{
  u8 i;
  s32 Sum_Speed = 0;     
	s16 Filter_Speed;   
  static  s16 Speed_Buf[FILTERING_TIMES]={0};
  for(i = 1 ; i<FILTERING_TIMES; i++)
  {
    Speed_Buf[i - 1] = Speed_Buf[i];
  }
  Speed_Buf[FILTERING_TIMES - 1] =sensor;

  for(i = 0 ; i < FILTERING_TIMES; i++)
  {
    Sum_Speed += Speed_Buf[i];
  }
  Filter_Speed = (s16)(Sum_Speed / FILTERING_TIMES);//
	return Filter_Speed;
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
	Position_Least = Encoder-Target;             //===
	Position_Bias *=0.8;		   
	Position_Bias += Position_Least*0.2;	             //===һ�׵�ͨ�˲���  
	Position_Differential = Position_Bias-Last_Position;
	Last_Position = Position_Bias;
	Position_PWM = Position_Bias*Position_KP+Position_Differential*Position_KD; //===�ٶȿ���		
	return Position_PWM;
}

/**************************************************************************
�������ܣ�˳��λ��PD���� 
��ڲ�����������
����  ֵ��λ�ÿ���PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int Pre_Position(int Encoder)
{  
	static float Position_PWM,Last_Position,Position_Bias,Position_Differential;
	Position_Bias = Encoder-Position_Zero; //===�õ�ƫ��
	Position_Differential = Position_Bias-Last_Position;//ƫ�����
	Last_Position = Position_Bias;//������һ��ƫ��
	Position_PWM = Position_Bias*25+Position_Differential*600; //===λ�ÿ���	
	return Position_PWM;//����ֵ
}

