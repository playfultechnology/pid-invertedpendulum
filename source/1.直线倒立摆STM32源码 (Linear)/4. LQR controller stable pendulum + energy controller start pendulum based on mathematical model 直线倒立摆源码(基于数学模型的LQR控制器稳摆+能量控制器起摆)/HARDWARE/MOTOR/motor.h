#ifndef __MOTOR_H
#define __MOTOR_H
#include <sys.h>	 
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
#define PWMB   TIM3->CCR4    //�����ڽ�ʹ��һ·���� ʹ�õ�B·
#define BIN2   PBout(12)     //�����ڽ�ʹ��һ·���� ʹ�õ�B·
#define BIN1   PBout(13)     //�����ڽ�ʹ��һ·���� ʹ�õ�B·
#define AIN1   PBout(14)     //Ԥ�� ����
#define AIN2   PBout(15)     //Ԥ�� ����
#define PWMA   TIM3->CCR3    //Ԥ�� ����
void MiniBalance_PWM_Init(u16 arr,u16 psc);
void MiniBalance_Motor_Init(void);
#endif
