#include "sys.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
u8 Flag_Stop=1,delay_50,delay_flag;         //停止标志位 50ms精准演示标志位
u8 system_start=0;                          //检测调节函数标志位
u8 tips_flag = 0;                           //OLED提示函数标志位
int Encoder;                                 //编码器的脉冲计数
int Moto;                                   //电机PWM变量 应是Motor的 向Moto致敬	
int Voltage;                                //电池电压采样相关的变量
float Angle_Balance;                        //角位移传感器数据

int main(void)
{ 
	Stm32_Clock_Init(9);            //=====系统时钟设置
	delay_init(72);                 //=====延时初始化
	JTAG_Set(JTAG_SWD_DISABLE);     //=====关闭JTAG接口
	JTAG_Set(SWD_ENABLE);           //=====打开SWD接口 可以利用主板的SWD接口调试
	delay_ms(1000);                 //=====延时启动，等待系统稳定
	delay_ms(1000);                 //=====延时启动，等待系统稳定 共2s
	LED_Init();                     //=====初始化与 LED 连接的硬件接口
	EXTI_Init();                    //=====按键初始化(外部中断的形式)
	OLED_Init();                    //=====OLED初始化
	uart_init(72,128000);           //=====初始化串口1
	
	// HARDWARE/MOTOR/motor.c
  MiniBalance_PWM_Init(7199,0);   // Initialize PWM 10KHZ for driving the motor 
	
	// Defined in HARDWARE/ENCODER/encoder.c
	Encoder_Init_TIM4();            // Initialize the encoder (encoder interface mode of TIM2) 
	
	//Define in HARDWARE/ADC/adc.c
	Adc_Init();                     //=====角位移传感器模拟量采集初始化
	
	// Defined in HARDWARE/Timer/timer.c
	Timer1_Init(199,7199);						//=====定时中断初始化 
	while(1) {      
		  Tips();                   //===OLED显示与提示
			delay_flag=1;	            //===80ms中断精准延时标志位		  
			while(delay_flag);        //===80ms中断精准延时 减缓oled刷新频率
		} 
}
