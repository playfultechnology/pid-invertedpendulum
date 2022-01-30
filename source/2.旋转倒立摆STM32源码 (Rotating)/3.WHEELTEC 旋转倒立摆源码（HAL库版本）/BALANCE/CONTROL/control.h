#ifndef __CONTROL_H
#define __CONTROL_H
#include "main.h"
  /**************************************************************************
作者：平衡小车之家
我的淘宝小店：http://shop114407458.taobao.com/
**************************************************************************/
#define PI 3.14159265
#define ZHONGZHI 3100

#define FILTERING_TIMES  4

extern	int Balance_Pwm,Velocity_Pwm;
int Balance(float angle);
int Position(int Encoder);
void Set_Pwm(int moto);
void Key(void);
void Xianfu_Pwm(void);
u8 Turn_Off(int voltage);
int myabs(int a);

int Position_PID (int Encoder,int Target);
void Auto_run(void);
	
#endif
