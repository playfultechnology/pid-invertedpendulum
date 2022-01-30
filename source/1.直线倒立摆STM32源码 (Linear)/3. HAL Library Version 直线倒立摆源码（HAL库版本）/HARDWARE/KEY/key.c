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
#include "gpio.h"

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

/**************************************************************************
函数功能：长按检测
入口参数：无
返回  值：按键状态 0：无动作 1：长按2s
**************************************************************************/
u8 Long_Press(void)
{
			static u16 Long_Press_count,Long_Press=0;
	    if(Long_Press==0&&KEY7==0)  Long_Press_count++;   //长按标志位未置1
      else                       Long_Press_count=0; 
		  if(Long_Press_count>20)		
			{
				Long_Press=1;	
				Long_Press_count=0;
				return 1;
			}				
			 if(Long_Press==1)     //长按标志位置1
			{
				  Long_Press=0;
			}
			return 0;
}

u8 Long_Press_KEY2(void)
{
			static u16 Long_Press_count,Long_Press=0;
	    if(Long_Press==0&&KEY2==0)  Long_Press_count++;   //长按标志位未置1
      else                       Long_Press_count=0; 
		  if(Long_Press_count>200)		
			{
				Long_Press=1;	
				Long_Press_count=0;
				return 1;
			}				
			 if(Long_Press==1)     //长按标志位置1
			{
				  Long_Press=0;
			}
			return 0;
}

///**************************************************************************
//函数功能：按键扫描
//入口参数：无
//返回  值：按键状态 0：无动作 1：单击 
//**************************************************************************/
u8 click(void)
{
			static u8 flag_key=1;//按键按松开标志
			if(flag_key&&(KEY5==0||KEY11==0||KEY12==0))
			{
			flag_key=0;
			HAL_Delay(50);
			if(KEY5==0)return 5;	
      else if (KEY11==0) return 11;
      else if (KEY12==0)	return 12;		
			}
			else	flag_key=1;
			return 0;//无按键按下
}
