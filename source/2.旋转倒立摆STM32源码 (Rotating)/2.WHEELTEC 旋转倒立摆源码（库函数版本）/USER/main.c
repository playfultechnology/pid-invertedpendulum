/***********************************************
公司：轮趣科技（东莞）有限公司
品牌：WHEELTEC
官网：wheeltec.net
淘宝店铺：shop114407458.taobao.com 
速卖通: https://minibalance.aliexpress.com/store/4455017
版本：1.0
修改时间：2021-12-09

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version:1.0
Update：2021-12-09

All rights reserved
***********************************************/
#include "stm32f10x.h"
#include "sys.h"
u8 Flag_Stop=1,delay_50,delay_flag;         //停止标志位 50ms精准演示标志位
u8 system_start=0;                          //检测调节函数标志位
u8 tips_flag = 0;                           //OLED提示函数标志位
int Encoder,Position_Zero=10000;            //编码器的脉冲计数
int Moto;                                   //电机PWM变量 应是Motor的 向Moto致敬	
int Voltage;                                //电池电压采样相关的变量
float Angle_Balance;                        //角位移传感器数据
float Balance_KP=50,Balance_KD=244,Position_KP=25,Position_KD=600;  //PID系数
float Position_KP_1=90,Position_KD_1=180;  //PID系数
float Menu=1,Amplitude1=1,Amplitude2=10,Amplitude3=1,Amplitude4=10; //PID调试相关参数
int main(void)
{
  JTAG_Set(JTAG_SWD_DISABLE);                     //关闭JTAG接口，才能开启OLED显示
	JTAG_Set(SWD_ENABLE);                           //开启SWD调试接口
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断优先级分组2
	delay_init();                   //=====延时初始化
	delay_ms(1000);                 //=====延时启动，等待系统稳定
	delay_ms(1000);                 //=====延时启动，等待系统稳定 共2s
  LED_Init();                     //=====初始化与 LED 连接的硬件接口
	MiniBalance_EXTI_Init();        //=====按键初始化(外部中断的形式)
	OLED_Init();                    //=====OLED初始化
	uart_init(128000);              //=====初始化串口1
  MiniBalance_PWM_Init(7199,0);   //=====初始化PWM 10KHZ，用于驱动电机 
	Encoder_Init_TIM4();            //=====初始化编码器（TIM2的编码器接口模式） 
	Adc_Init();                     //=====角位移传感器模拟量采集初始化
	Timer1_Init(49,7199);           //=====定时中断初始化 
  while(1)
	{
		DataScope();	            //===上位机
		Tips();                   //===OLED显示与提示
		delay_flag=1;	            //===50ms中断精准延时标志位		  
		while(delay_flag);        //===50ms中断精准延时  主要是波形显示上位机需要严格的50ms传输周期
	}
}

