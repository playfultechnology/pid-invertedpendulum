#include "control.h"		
#include "math.h"		
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/

float E;
float t=0.020;
float last_angle=0;
float x=0, x_speed, accel, angle, angle_speed, u; //位移,速度 ,加速度,角度,角速度,电压	
float k1 = -36.4232, k2 = -40.0996, k3 = -101.4914, k4 = -15.8376;			//LQR状态反馈系数
int last_Encoder=10000; //编码器值

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
				 if(++delay_50==4)	 delay_50=0,delay_flag=0;          //===给主函数提供50ms的精准延时  10次*5ms = 50ms
			 }
			 //获得编码器的值，初始化赋值10000
			 Encoder=Read_Encoder(4); 
			 
			 //速度 
			 x_speed = ( ( (last_Encoder - Encoder) / 1040.0f )*0.036* PI ) / t; //距离÷时间
			 
			 //计算出实际移动的位置
			 x += x_speed*t;
			 
			 last_Encoder = Encoder; //保存上一次编码器的值
			 
			 //获得角度原始值 
       Angle_Balance=Get_Adc_Average(3,10); 
			 
			 //角度计算 
		   angle = angle_count(Angle_Balance) /180.0f * PI; //计算角度值
			 
			 //角速度计算
			 angle_speed = (angle - last_angle) / t;
			 last_angle = angle ;//保存上一次的角度值
			 
			 //能量
			 E = 0.0032*angle_speed*angle_speed+0.1764*(cos(angle)-1);
			 
			 //控制
			 if (angle>-0.5236 && angle<0.5236)
			 {
				 u = -(k1* x + k2 * x_speed + k3* angle +  k4 * angle_speed);  //稳摆
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
						 u = 0.0651*accel + 11.5556*x_speed + (0.0028*cos(angle)*(0.1764*sin(angle)-0.0180*accel))/0.0012 - 0.8440*sin(angle)*angle_speed*angle_speed;  //起摆
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
		
    if(Flag_Stop==0)	Led_Flash(100);      //===LED闪烁指示系统正常运行 
		Voltage=Get_battery_volt();           //===获取电池电压	                 
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
	  int Amplitude=6900;    //===PWM满幅是7200 限制在6900
	  if(Moto<-Amplitude) Moto=-Amplitude;	
		if(Moto>Amplitude)  Moto=Amplitude;		
}

/**************************************************************************
函数功能：异常关闭电机
入口参数：电压
返回  值：1：异常  0：正常
**************************************************************************/
u8 Turn_Off(int voltage)
{
	    u8 temp; 
			if(1==Flag_Stop) //电池电压过低，关闭电机
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

