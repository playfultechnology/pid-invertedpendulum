#include "control.h"		
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
				 if(++delay_50==10)	 delay_50=0,delay_flag=0;          //===给主函数提供50ms的精准延时  10次*5ms = 50ms
			 }		
    	Encoder=Read_Encoder(4);             	                   //===更新编码器位置信息	 
      Angle_Balance=Get_Adc_Average(3,15);                     //===更新姿态	
			 
		//开机后只能进行一次自动起摆，而手动起摆可以在任意时刻进行	  
     if(Flag_Stop==0&&auto_start==1&&hand_start==0)
		 {
			 Auto_run(); 	//自动起摆，开机后只能自动起摆一次	 
		 }
		 
		 else if (Flag_Stop==0&&hand_start==1&&auto_start==0)//手动起摆模式
		 {
			 if(hand_flag==0) hand_flag=1,Position_Zero=Read_Encoder(4);//手动起摆前，获取一次编码器现在的位置，并作为起摆的平衡参考点，实现任意位置任意时刻都能手动起摆

			 Balance_Pwm =Balance(Angle_Balance);                                          //===角度PD控制	
			 if(++Position_Target>4)  Position_Pwm=Position(Encoder),Position_Target=0;    //===位置PD控制 25ms进行一次位置控制
			  
			 if(Turn_Off(Voltage)) Flag_Stop=1;                                             //===低压和倾角过大保护
			 else
			  Xianfu_Pwm(),                         //===PWM限幅 防止占空比100%带来的系统不稳定因素
	    	Moto=Balance_Pwm-Position_Pwm,       //===计算电机最终PWM
		    Set_Pwm(Moto);                      //===赋值给PWM寄存器
		 }
		 
		 else if (Flag_Stop==1)
		 Set_Pwm (0),hand_flag=0;
		
    //电机飞转保护，如果检测到电机飞转，则把标志位置1，系统停止		 
		 abnormal_motor=Encoder-Last_Encoder; //记录编码器的变差
		 Last_Encoder=Encoder; //保存上一次编码器值
		 if(myabs(abnormal_motor)>15)  //如果偏差的绝对值大于15，说明此时电机转动很快，是异常状态，停转电机
		 {
			  Flag_Stop=1;
		 }
			 
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
	  int Amplitude=6900;    //===PWM满幅是7200 限制在6900
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
	// 产品使用的是13线霍尔传感器电机，20减速比，程序上用的是4倍频，电机转一圈编码器上的数值是13*4*20 = 1040 
	int position=2080; //目标位置 电机原始位置是10000  转一圈是1040 和编码器精度有关，默认是摆杆Z轴转一圈，输出1040个跳变沿
	static int tmp,flag,count;
	
		
	if(Flag_Stop==1) //如果没有开始平衡，才进行判断
	{
		if(Angle_Balance>(ZHONGZHI-500)&&Angle_Balance<(ZHONGZHI+500))  //如果在平衡位置上
	{
		hand_start=1;//手动起摆
		auto_start=0;//关掉自动起摆标志
	}
	  else auto_start=1,hand_start=0; //  不在平衡位置上，自动起摆
	}

	tmp=click_N_Double(100); 
	if(tmp==1)flag=1;//++
  if(tmp==2)flag=2;//--
	
	if(flag==1) //摆杆顺时针运动
	{
		Position_Zero++;
		count++;	
		if(count==position) 	flag=0,count=0;   //即把目标位置 +2080
	}	
		if(flag==2) //摆杆逆时针运动
	{
		Position_Zero--;
		count++;	
		if(count==position) 	flag=0,count=0;//即把目标位置 -2080
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


/**************************************************************************
函数功能：位置式PID控制器
入口参数：编码器测量位置信息，目标位置
返回  值：电机PWM
根据位置式离散PID公式 
pwm=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,,k;
pwm代表输出
**************************************************************************/
int Position_PID (int Encoder,int Target)
{ 	
	 static float Bias,Pwm,Integral_bias,Last_Bias;
	 Bias=Encoder-Target;                                   //计算偏差
	 Integral_bias+=Bias;	                                  //求出偏差的积分
	 Pwm=Position_KP_1*Bias+0*Integral_bias+Position_KD_1*(Bias-Last_Bias);       //位置式PID控制器
	 Last_Bias=Bias;                                        //保存上一次偏差 
	 return Pwm;                                            //增量输出
}


/**************************************************************************
函数功能：自动起摆
入口参数：无
返回  值：无
**************************************************************************/
void Auto_run(void)
{
	static u16 try_count=0;  //用于辅助判断是否起摆成功
	static u16 stop_count=0; //记录电机停转的时间
	
	if(step1_flag==0)  //甩杆  只甩一次
	{
		fail_count++; //记录进入函数的次数 会进入两次 如果两次进入而且角度不在平衡位置，说明起摆失败
		Moto=Position_PID(Encoder,step1_target);  //第一次 step1_target=10000 获取开机时的位置并计为10000
	  step1_target=10341; //赋值，下一次进入函数后甩杆   记录可用值：10334 10337
		Xianfu_Pwm();    //pwm限幅
		
		/*  读取编码器位置进行判断，不在初始值会导致电机飞转  */
		if(fail_count==1&&(Encoder<9980||Encoder>10020))  //第一次甩杆需要对编码器的值进行判断，如果编码器的值偏离初始值过大，会导致电机飞转，所以禁止在偏差过大时自动起摆
		{
			fail_count=0,step1_target=10000,Flag_Stop=1;   //读取编码器的值，如果不在合适位置，所有改动过的数据都变回原始值，电机启动标志位置1，等待下一次自动起摆
		}
	  else Set_Pwm(Moto);	 //甩杆
		
	 }

if(++try_count&&(Angle_Balance>ZHONGZHI-350&&Angle_Balance<ZHONGZHI+350))   //  如果甩杆到达平衡位置
 {
	 try_count=0;//如果进入了这个函数，又失败了，是起摆失败情况2
//	 if(success_count==0)  Balance_KP=60,Balance_KD=350,Position_KP=35,Position_KD=750; //录入自动起摆时所需要的pid参数
	 if(success_count==0)  Balance_KP=30,Balance_KD=150,Position_KP=15,Position_KD=500; //录入自动起摆时所需要的pid参数
	 success_count++; //记录在平衡位置的次数
	 
	 if(success_count>10) //如果超过10次
	 {
		 step1_flag=1; //标记起摆成功，不要再进入甩杆函数
		 //起摆成功即可以对摆杆进行pid控制
		 Balance_Pwm =Balance(Angle_Balance); 
		 if(success_count>109) //起摆后过一小段时间，需要让摆杆减少位置控制的频率，把pid参数调整回原来稳定时的数值
		 {
			 if(success_count==110)  Balance_KP=50,Balance_KD=244,Position_KP=25,Position_KD=600;//起摆一段时间后，让pid参数回到手动起摆的值
			 success_count=115; //锁住success_count值，防止溢出循环
       if(++Position_Target>4)  Position_Pwm=Position(Encoder),Position_Target=0; //===位置PD控制 25ms进行一次位置控制
		 }
     else 	Position_Zero=step1_target+0, Position_Pwm=Position(Encoder);  //刚开始起摆时需要立刻有位置控制加入，保持平衡	 平衡的目标位置比甩杆的距离更远，有利于平衡的稳定性，过冲补偿

		stop_count=0; //电机停转计时，如果在平衡状态，复位计时，防止有时间累积误判 
		Xianfu_Pwm(); //pwm限幅
		Moto=Balance_Pwm-Position_Pwm;  //融合数据 
	  Set_Pwm(Moto);  //发送pwm
	 }
 }
	
 //电机停转分三种情况： 1、起摆时没有进入过平衡位置   2、起摆后进入平衡位置，又失败了  3、人为把摆杆拿到初始位置
 if((!(Angle_Balance>ZHONGZHI-500&&Angle_Balance<ZHONGZHI+500))&&fail_count>2&&try_count==0) //情况2：起摆后进入过平衡位置，但是失败了
 {
	step1_flag=1; //甩杆标志位，禁止进入甩杆
	Set_Pwm(0);	 //停转电机
	 
	 //电机停转后，延迟一点时间再把电机使能标志位置1，目的是防止误判，使得原本能保持平衡的摆杆失去电机的控制
	 stop_count++; //电机停转开始计时
	 if(stop_count>200) stop_count=0,Flag_Stop=1; //标志位置1
 }
 
 //倾角、电压保护
 if(((!(Angle_Balance>ZHONGZHI-500&&Angle_Balance<ZHONGZHI+500))&&fail_count>2&&try_count>120)||Voltage<1110)  //情况1和3 ：起摆后没有进入过平衡位置，起摆失败(从起摆到确认正确起摆或确认起摆失败，try_count会++超过100)
 {
	try_count=150; //锁住try_count的值，防止溢出循环
	step1_flag=1;  //甩杆标志位，禁止进入甩杆
	Set_Pwm(0);	 //停转电机
	 
	 //电机停转后，延迟一点时间再把电机使能标志位置1，目的是防止误判，使得原本能保持平衡的摆杆失去电机的控制
	 stop_count++; //电机停转开始计时
	 if(stop_count>200) stop_count=0,Flag_Stop=1; //标志位置1
 }
 
 
}

