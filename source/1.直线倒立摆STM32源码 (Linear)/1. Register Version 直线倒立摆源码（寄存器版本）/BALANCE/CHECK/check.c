#include "check.h"
#include "delay.h"
#include "led.h"
/**************************************************************************
函数功能：OLED显示、检测角位移是否需要调节检测、角位移调节函数
入口参数：无
返回  值：无
**************************************************************************/
void Tips(void) {
	static u8 i=0;     // for timing
	u8 key;            // for input
	static u8 count=0; // Used to count the number of times the correct ADC value was detected 
	
// Mark the position where the function starts. 
// If you need to adjust the angular displacement sensor, 
// jump to here to start the function after the adjustment. 	
begin: 
	
		if(tips_flag == 1 ) {
			tips_flag = 0;                 //进入过检测函数，说明中断被屏蔽，需要打开所有需要用到的中断并刷新OLED
			
			// Restoring interrupts masked when entering setup mode 
			EXTI->IMR |= 1<<5;	
			EXTI->IMR |= 1<<7;
			EXTI->IMR |= 1<<11;
			EXTI->IMR |= 1<<12;	
			TIM1->DIER |= 0x01;			
			OLED_Clear(); // Clear the screen, ready to enter the system 
		}
		
		// When the program is running normally, only this step will be performed.
		// If you need to enter the adjustment mode, other functions will be used. 
		else
			oled_show();	

	  key = Long_Press(); // If button held for 1 sec, enter the adjustment mode 
		if(key == 1) {
			system_start = 1;
		}
		
	// If the user presses the button for a long time, system_start=1, and enters the adjustment function 
	if(system_start == 1) {
		 system_start = 0;    //The flag does not enter the adjustment angular displacement function, this function can only be executed once after booting 
		 OLED_Clear();     //清屏
		 tips_flag = 1;	  // Set tips_flag = 1 to ensure that all interruptions can be recovered after adjusting the angle reasonably and that the OLED can be refreshed and displayed once 
		 PWMB=0;         // Turn off the motor to prevent the motor from keeping the value and keep moving 
		 
		// Shield the interrupts used by the system in regulation mode 
		 EXTI->IMR &= ~(1<<5);	
		 EXTI->IMR &= ~(1<<7);
		 EXTI->IMR &= ~(1<<11);
		 EXTI->IMR &= ~(1<<12);
		 TIM1->DIER &= 0xFE;   // After entering debug mode, temporarily disable timer 1 interrupt 
		 
	// Tips page 
	while(1) {
		page_tips: //提示页	
			key = click();
			// Display information: Start the angular displacement installation check mode, 
			// please follow the prompts. Press the user key to confirm... 
			show_Tips();     
			delay_ms(30);   // Delay and debounce to prevent it from being too easy to turn two pages in a row when the next page is turned up 
			// If the user button is pressed, clear the screen and go to the next step 
			// or if the user presses the back button, exit the adjustment 			
			if(key==5) {
				 OLED_Clear();  
				 while(1) {
					 page_0:
						delay_ms(30);   //延迟消抖，防止下一页往上翻时太容易连续翻两页
						key = click(); // Detect input
						step_0();     // Display message: Please fix the pendulum so that the pendulum is straight down and still. After installation, press the Next button to continue... 
					 if(key==11)//下一页
					 {
						 OLED_Clear();
						 while(1)  
						 {
			         page_1:	        //进入第1页 
           		 step_1();     //显示信息：把连接摆杆和角位移传感器上的两个螺丝拧松，然后调节摆杆的旋钮，保
							 delay_ms(30);   //延迟消抖，防止下一页往上翻时太容易连续翻两页
							 key = click();  //检测按键
							 if(key==11)   //下一页
							 {
								 OLED_Clear();
								 while(1) 
								 {
									 page_2:            //进入第2页
									 step_2();        //显示信息：证摆杆垂直向下时ADC的值是1015到1025之间，然后拧紧连接摆杆和角位
									 delay_ms(30);    //延迟消抖，防止下一页往上翻时太容易连续翻两页
									 key = click();  //检测按键
									 if(key ==11)    //下一页
									 {
						    		 OLED_Clear();
										 while(1)
										 {
											 page_3:        //进入第3页
											 Angle_Balance=Get_Adc_Average(3,15);  //第3页获取ADC值，用于调节
											 step_3();    //显示信息：移传感器上的两个螺丝。调节成功后按下用户按键检查
											 delay_ms(50);   //延迟消抖+延迟显示，防止ADC的值刷新过快难以分辨
											 key = click();   //检测按键
											 if(key==12)   //上一页按键按下，跳回第2页
											 {
												 OLED_Clear();
												 goto page_2;
											 }																		 
											 else if (key==5)  //用户按键被按下，进入系统检查判断阶段
											 {
												 OLED_Clear();
												 i=0;
												 key = 10;//进入这一步后把key变量用来当作倒计时
												 while(1)
												 {
													 Angle_Balance=Get_Adc_Average(3,15); //获取ADC的值
													 OLED_ShowNumber(88,16,key,2,16); //显示倒计时
													 check_display(); //显示信息：正在检查调节是否正确........10     请保持摆杆静止											 
													 count++; //count计时
													 delay_ms(30); //稍作延迟，检测时间拉长
													 if(count<=200) //在count <= 200 期间，记录下角度正确时的次数
													 {
															 if(Angle_Balance>=1010&&Angle_Balance<=1030) //在count小于200期间，检查角度是否正确
															 {
																 i++;
																 if(i>150)  //在count小于200期间,角度正确次数达到了150，认为角度是对的
																 {
																	 OLED_Clear();
																	 while(1)
																	 {
																		 count=0;
																		 success_display(); //提示成功信息，等待用户按键按下
																		 if(click()==5) goto begin;		//用户按键按下，跳出整个调节函数	
																	 }
																 }
															 }
															 if(count%20==1)  //用于大约8~10s的倒计时
															 {
																 key--;
															 }
													 }
													 
													 else //如果在count<=200期间，角度正确次数不满足要求，则判断调节失败，重新跳回调节界面继续调节
													 {
														 count=0; //计时清零，这是个静态变量
														 OLED_Clear();   //清屏
														 fail_display(); //显示信息：检查不通过  请重新调节
														 delay_ms(3000);
														 delay_ms(2000);  //延迟提示信息
														 OLED_Clear();
														 goto page_3;    //跳回调节界面重新调节
													 }														
												 }
											 }
										 }
									 }
									 else if (key ==12) {OLED_Clear();goto page_1; } //上一页按键(X1)被按下，对应第2页跳回第1页之间的跳转
								 }				 
							 }
						  else if(key==12) {OLED_Clear();goto page_0;}  //上一页按键被按下(X1)，对应第1页跳回第0页之间的跳转
						 }
					 }
            else if (key==12) {OLED_Clear();goto page_tips;}	//上一页按键被按下(X1),对应第0页跳回提示页				 
				 }		 			 				 
			 }
			 else if (key==12) {goto begin;}  //对应用户在开始页面的选择
		 }			 	 
	 }
}
