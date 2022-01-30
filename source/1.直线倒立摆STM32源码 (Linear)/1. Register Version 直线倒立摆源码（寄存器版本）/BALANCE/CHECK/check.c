#include "check.h"
#include "delay.h"
#include "led.h"
/**************************************************************************
�������ܣ�OLED��ʾ������λ���Ƿ���Ҫ���ڼ�⡢��λ�Ƶ��ں���
��ڲ�������
����  ֵ����
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
			tips_flag = 0;                 //�������⺯����˵���жϱ����Σ���Ҫ��������Ҫ�õ����жϲ�ˢ��OLED
			
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
		 OLED_Clear();     //����
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
		page_tips: //��ʾҳ	
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
						delay_ms(30);   //�ӳ���������ֹ��һҳ���Ϸ�ʱ̫������������ҳ
						key = click(); // Detect input
						step_0();     // Display message: Please fix the pendulum so that the pendulum is straight down and still. After installation, press the Next button to continue... 
					 if(key==11)//��һҳ
					 {
						 OLED_Clear();
						 while(1)  
						 {
			         page_1:	        //�����1ҳ 
           		 step_1();     //��ʾ��Ϣ�������Ӱڸ˺ͽ�λ�ƴ������ϵ�������˿š�ɣ�Ȼ����ڰڸ˵���ť����
							 delay_ms(30);   //�ӳ���������ֹ��һҳ���Ϸ�ʱ̫������������ҳ
							 key = click();  //��ⰴ��
							 if(key==11)   //��һҳ
							 {
								 OLED_Clear();
								 while(1) 
								 {
									 page_2:            //�����2ҳ
									 step_2();        //��ʾ��Ϣ��֤�ڸ˴�ֱ����ʱADC��ֵ��1015��1025֮�䣬Ȼ��š�����Ӱڸ˺ͽ�λ
									 delay_ms(30);    //�ӳ���������ֹ��һҳ���Ϸ�ʱ̫������������ҳ
									 key = click();  //��ⰴ��
									 if(key ==11)    //��һҳ
									 {
						    		 OLED_Clear();
										 while(1)
										 {
											 page_3:        //�����3ҳ
											 Angle_Balance=Get_Adc_Average(3,15);  //��3ҳ��ȡADCֵ�����ڵ���
											 step_3();    //��ʾ��Ϣ���ƴ������ϵ�������˿�����ڳɹ������û��������
											 delay_ms(50);   //�ӳ�����+�ӳ���ʾ����ֹADC��ֵˢ�¹������Էֱ�
											 key = click();   //��ⰴ��
											 if(key==12)   //��һҳ�������£����ص�2ҳ
											 {
												 OLED_Clear();
												 goto page_2;
											 }																		 
											 else if (key==5)  //�û����������£�����ϵͳ����жϽ׶�
											 {
												 OLED_Clear();
												 i=0;
												 key = 10;//������һ�����key����������������ʱ
												 while(1)
												 {
													 Angle_Balance=Get_Adc_Average(3,15); //��ȡADC��ֵ
													 OLED_ShowNumber(88,16,key,2,16); //��ʾ����ʱ
													 check_display(); //��ʾ��Ϣ�����ڼ������Ƿ���ȷ........10     �뱣�ְڸ˾�ֹ											 
													 count++; //count��ʱ
													 delay_ms(30); //�����ӳ٣����ʱ������
													 if(count<=200) //��count <= 200 �ڼ䣬��¼�½Ƕ���ȷʱ�Ĵ���
													 {
															 if(Angle_Balance>=1010&&Angle_Balance<=1030) //��countС��200�ڼ䣬���Ƕ��Ƿ���ȷ
															 {
																 i++;
																 if(i>150)  //��countС��200�ڼ�,�Ƕ���ȷ�����ﵽ��150����Ϊ�Ƕ��ǶԵ�
																 {
																	 OLED_Clear();
																	 while(1)
																	 {
																		 count=0;
																		 success_display(); //��ʾ�ɹ���Ϣ���ȴ��û���������
																		 if(click()==5) goto begin;		//�û��������£������������ں���	
																	 }
																 }
															 }
															 if(count%20==1)  //���ڴ�Լ8~10s�ĵ���ʱ
															 {
																 key--;
															 }
													 }
													 
													 else //�����count<=200�ڼ䣬�Ƕ���ȷ����������Ҫ�����жϵ���ʧ�ܣ��������ص��ڽ����������
													 {
														 count=0; //��ʱ���㣬���Ǹ���̬����
														 OLED_Clear();   //����
														 fail_display(); //��ʾ��Ϣ����鲻ͨ��  �����µ���
														 delay_ms(3000);
														 delay_ms(2000);  //�ӳ���ʾ��Ϣ
														 OLED_Clear();
														 goto page_3;    //���ص��ڽ������µ���
													 }														
												 }
											 }
										 }
									 }
									 else if (key ==12) {OLED_Clear();goto page_1; } //��һҳ����(X1)�����£���Ӧ��2ҳ���ص�1ҳ֮�����ת
								 }				 
							 }
						  else if(key==12) {OLED_Clear();goto page_0;}  //��һҳ����������(X1)����Ӧ��1ҳ���ص�0ҳ֮�����ת
						 }
					 }
            else if (key==12) {OLED_Clear();goto page_tips;}	//��һҳ����������(X1),��Ӧ��0ҳ������ʾҳ				 
				 }		 			 				 
			 }
			 else if (key==12) {goto begin;}  //��Ӧ�û��ڿ�ʼҳ���ѡ��
		 }			 	 
	 }
}
