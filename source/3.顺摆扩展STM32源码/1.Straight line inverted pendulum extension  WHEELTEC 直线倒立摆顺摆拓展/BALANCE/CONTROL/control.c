#include "control.h"		
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
int Balance_Pwm,Position_Pwm;
u8 Flag_Target,Position_Target;
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
				 if(++delay_50==10)	 delay_50=0,delay_flag=0;          //===给主函数提供50ms的精准延时
			 }		
    	Encoder=Read_Encoder(4);             	                   //===更新编码器位置信息
      Angle_Balance=Get_Adc_Average(3,15);                     //===更新姿态	
     	Balance_Pwm =Balance(Angle_Balance);                     //===角度PD控制	
	    if(++Position_Target>4)	Position_Pwm=Position(Encoder),Position_Target=0;     //===位置PD控制 25ms进行一次位置控制
      Moto=Balance_Pwm-Position_Pwm;        //===计算电机最终PWM
		  Xianfu_Pwm();                         //===PWM限幅 反正占空比100%带来的系统不稳定因素
		  Turn_Off(Voltage);                    //===低压和倾角过大保护
			 
			if(Flag_Stop==0)
			Set_Pwm(Moto);                        //===赋值给PWM寄存器
			else
			Set_Pwm(0);
			
	  	Led_Flash(100);                       //===LED闪烁指示系统正常运行 
	    Voltage=Get_battery_volt();           //===获取电池电压	      
			Key();                                //===扫描按键变化
	}       	
	 return 0;	  
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
	 static float Last_Bias,D_Bias;    //PID相关变量
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
   static float Position_PWM,Last_Position,Position_Bias,Position_Differential;
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
    	if(moto>0)			BIN2=1,			BIN1=0;
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
函数功能：按键修改小车运行状态  控制摆杆的位置
入口参数：无
返回  值：无
**************************************************************************/
void Key(void)
{	
	int position=1040; //目标位置 电机原始位置是10000  转一圈是1040 和编码器精度有关，默认是电机转一圈，输出1040个跳变沿
	static int tmp,flag,count;
	tmp=click_N_Double(100); 
	
	if(tmp==1)flag=1;//++
  if(tmp==2)flag=2;//--
	
	if(flag==1) //摆杆顺时针运动
	{
		Position_Zero+=4;
		count+=4;	
		if(count==position) 	flag=0,count=0;
	}	
		if(flag==2) //摆杆逆时针运动
	{
		Position_Zero-=4;
		count+=4;	
		if(count==position) 	flag=0,count=0;
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
			if(1==Flag_Stop) //电池电压过低，关闭电机
			{	      
      Flag_Stop=1;				
      temp=1;                                            
			AIN1=0;                                            
			AIN2=0;
      }
			else
      temp=0;
			
			
			if(Angle_Balance<(ZHONGZHI-800)||Angle_Balance>(ZHONGZHI+800)||voltage<700)
			{
				Flag_Stop=1;
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
