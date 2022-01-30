#include "key.h"
 /**************************************************************************
作者：平衡小车之家 
淘宝店铺：http://shop114407458.taobao.com/
**************************************************************************/
/**************************************************************************
函数功能：按键初始化
入口参数：无
返回  值：无 
**************************************************************************/
void KEY_Init(void)
{
	RCC->APB2ENR|=1<<2;    //使能PORTA时钟	   	 
	GPIOA->CRH&=0XFFF00FFF; 
	GPIOA->CRH|=0X00088000;
	
	GPIOA->CRL&=0X0F0FF0FF; 
	GPIOA->CRL|=0X80800800;
	
  GPIOA->ODR|=1<<2; //PA 2 上拉	
  GPIOA->ODR|=1<<7; //PA 7 上拉	
  GPIOA->ODR|=1<<5; //PA5 上拉
	GPIOA->ODR|=3<<11; //PA11 12  上拉	
} 
/**************************************************************************
函数功能：按键扫描
入口参数：双击等待时间
返回  值：按键状态 0：无动作 1：单击 2：双击 
**************************************************************************/
u8 click_N_Double (u8 time)
{
		static	u8 flag_key,count_key,double_key;	
		static	u16 count_single,Forever_count;
	  if(KEY2==0)  Forever_count++;   //长按标志位未置1
     else        Forever_count=0;
		if(0==KEY2&&0==flag_key)		flag_key=1;	
	  if(0==count_key)
		{
				if(flag_key==1) 
				{
					double_key++;
					count_key=1;	
				}
				if(double_key==2) 
				{
					double_key=0;
					count_single=0;
					return 2;//双击执行的指令
				}
		}
		if(1==KEY2)			flag_key=0,count_key=0;
		
		if(1==double_key)
		{
			count_single++;
			if(count_single>time&&Forever_count<time)
			{
			double_key=0;
			count_single=0;	
			return 1;//单击执行的指令
			}
			if(Forever_count>time)
			{
			double_key=0;
			count_single=0;	
			}
		}	
		return 0;
}
///**************************************************************************
//函数功能：按键扫描
//入口参数：无
//返回  值：按键状态 0：无动作 1：单击 
//**************************************************************************/
//u8 click(void)
//{
//			static u8 flag_key=1;//按键按松开标志
//			if(flag_key&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
//			{
//			flag_key=0;
//			if(KEY1==0)return 1;
//			if(KEY2==0)return 2;
//			if(KEY3==0)return 3;
//			if(KEY4==0)return 4;				
//			}
//			else if(1==KEY1&&1==KEY2&&1==KEY3&&1==KEY4)			flag_key=1;
//			return 0;//无按键按下
//}
///**************************************************************************
//函数功能：长按检测
//入口参数：无
//返回  值：按键状态 0：无动作 1：长按2s
//**************************************************************************/
//u8 Long_Press(void)
//{
//			static u16 Long_Press_count,Long_Press;
//	    if(Long_Press==0&&KEY==0)  Long_Press_count++;   //长按标志位未置1
//      else                       Long_Press_count=0; 
//		  if(Long_Press_count>200)		
//			{
//				Long_Press=1;	
//				Long_Press_count=0;
//				return 1;
//			}				
//			 if(Long_Press==1)     //长按标志位置1
//			{
//				  Long_Press=0;
//			}
//			return 0;
//}
