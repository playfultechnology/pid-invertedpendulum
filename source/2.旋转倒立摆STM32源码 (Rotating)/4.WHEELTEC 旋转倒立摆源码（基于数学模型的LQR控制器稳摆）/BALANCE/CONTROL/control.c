#include "control.h"	
#include "math.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
int Balance_Pwm,Position_Pwm; //目标角度PWM、目标位置PWM
u8 Position_Target;//用于标记位置控制的时间
u8 step1_flag=0;//自动起摆第一步标志位，用于判断自动起摆前是否在起摆的第一步
u16 step1_target=10000;//自动起摆甩摆位置，用于决定自动起摆时摆杆晃动的幅度
u16 success_count=0; //记录摆杆在平衡位置的次数，用于判断是否可以进入平衡状态
u8 hand_start=0,auto_start=0; //手动起摆、自动起摆判断，用于判断当前状态是处于自动起摆模式还是手动起摆模式
u8 fail_count=0;//自动起摆失败次数，用于判断何时开始可以去判断起摆是否成功
u8 hand_flag=0; //手动起摆标志位，用于判断是否要录入编码器的值作为目标值保持平衡
int Last_Encoder=10000;//保存上一次编码器的数值，用于保存上一次编码器的数值，方便计算编码器两次的值之差
int abnormal_motor;//前后编码器的差值，用于判断是否要停转电机

//PID相关变量
float Last_Bias;    
float Last_Position;

//
float theta=0, theta_dot, alpha=0, alpha_dot, last_alpha=0;//旋转臂的转角，旋转臂的角速度，摆杆的倾角，摆杆的角速度
float offset=0;
float k1=-1.6349, k2=-1.7563, k3=26.8000, k4=2.3682;//反馈系数
float t = 0.020;//采样时间（进入中断的时间）
float u;//电压
/**************************************************************************
函数功能：所有的控制代码都在这里面
          TIM1控制的5ms定时中断 
**************************************************************************/
int TIM1_UP_IRQHandler(void)  
{    
	if(TIM1->SR&0X0001)//5ms定时中断
	{   
		  TIM1->SR&=~(1<<0);                                       //===清除定时器1中断标志位	                     
	     if(delay_flag==1)
			 {
				 if(++delay_50==3)	 delay_50=0,delay_flag=0;          //===给主函数提供50ms的精准延时  10次*5ms = 50ms
			 }	
			 
    	Encoder=Read_Encoder(4);             	                   //===更新编码器位置信息	 
			theta_dot = ( ( (Last_Encoder - Encoder) / 1040.0f )*2*PI ) / t;
			theta += theta_dot*t;
			Last_Encoder = Encoder;
			
			Angle_Balance=Get_Adc_Average(3,15);                     //===更新姿态
			alpha = angle_count(Angle_Balance) /180.0f * PI;
			alpha = -alpha;
			alpha_dot = (alpha - last_alpha) / t;
			last_alpha = alpha;
			
			 //控制
			 if (alpha>-0.7854 && alpha<0.7854)
				 {
					 u = -(k1*(theta+offset) + k2*theta_dot + k3*alpha + k4*alpha_dot); //稳摆
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
		
  if(Flag_Stop==0)	Led_Flash(100);      //===LED闪烁指示系统正常运行 
	Voltage=Get_battery_volt();           //===获取电池电压	      
	Key();                                //===扫描按键变化    	
	return 0;
}
/**************************************************************************
函数功能：符号函数
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
函数功能：计算角度函数
入口参数：ADC值
返回  值：角度值
作    者：平衡小车之家
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
函数功能：倾角PD控制
入口参数：角度
返回  值：倾角控制PWM
作    者：平衡小车之家
**************************************************************************/
int Balance(float Angle)
{  
   float Bias;                       //倾角偏差
	 static float D_Bias;    //PID相关变量
	 int balance;                      //PWM返回值 
	 Bias=Angle-ZHONGZHI;              //求出平衡的角度中值 和机械相关
	 D_Bias=Bias-Last_Bias;            //求出偏差的微分 进行微分控制
	 balance=-Balance_KP*Bias-D_Bias*Balance_KD;   //===计算倾角控制的电机PWM  PD控制
   Last_Bias=Bias;                   //保持上一次的偏差
	 return balance;
}

/**************************************************************************
函数功能：位置PD控制 
入口参数：编码器
返回  值：位置控制PWM
作    者：平衡小车之家
**************************************************************************/
int Position(int Encoder)
{  
   static float Position_PWM,Position_Bias,Position_Differential;
	 static float Position_Least;
  	Position_Least =Encoder-Position_Zero;             //===
    Position_Bias *=0.8;		   
    Position_Bias += Position_Least*0.2;	             //===一阶低通滤波器  
	  Position_Differential=Position_Bias-Last_Position;
	  Last_Position=Position_Bias;
		Position_PWM=Position_Bias*Position_KP+Position_Differential*Position_KD; //===速度控制		
	  return Position_PWM;
}

/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int moto)
{
    	if(moto<0)			BIN2=1,			BIN1=0;
			else 	          BIN2=0,			BIN1=1;
			PWMB=myabs(moto);
}

/**************************************************************************
函数功能：限制PWM赋值 
入口参数：无
返回  值：无
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=7000;    //===PWM满幅是7200 限制在7000
	  if(Moto<-Amplitude) Moto=-Amplitude;	
		if(Moto>Amplitude)  Moto=Amplitude;		
}
/**************************************************************************
函数功能：按键修改控制摆杆的位置
入口参数：无
返回  值：无
**************************************************************************/
void Key(void)
{	

	static int tmp,flag,count;

	tmp=click_N_Double(20); 
	if(tmp==1)flag=1;//++
  if(tmp==2)flag=2;//--
	
	if(flag==1) //摆杆顺时针运动
	{
		offset+=0.035;
		count++;	
		if(count==367) 	flag=0,count=0;   //即把目标位置 +2080
	}	
		if(flag==2) //摆杆逆时针运动
	{
		offset-=0.035;
		count++;	
		if(count==367) 	flag=0,count=0;   //即把目标位置 +2080
	}


}

/**************************************************************************
函数功能：异常关闭电机
入口参数：电压
返回  值：1：异常  0：正常
**************************************************************************/
u8 Turn_Off(int voltage)
{
	    u8 temp; 
			if(1==Flag_Stop||Angle_Balance<(ZHONGZHI-800)||Angle_Balance>(ZHONGZHI+800)||voltage<1110) //电池电压过低，关闭电机
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
函数功能：绝对值函数
入口参数：int
返回  值：unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}


