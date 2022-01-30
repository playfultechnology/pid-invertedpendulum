#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
�ҵ��Ա�С�꣺http://shop114407458.taobao.com/
**************************************************************************/
#define PI 3.14159265
#define ZHONGZHI 3100
#define FILTERING_TIMES  4

#define POSITION_MIDDLE 7925
#define ANGLE_ORIGIN 1020
#define ANGLE_MIDDLE 3145
extern	int Balance_Pwm,Velocity_Pwm;

extern float Bias;                       //���ƫ��
extern float Last_Bias,D_Bias;    //PID��ر���
extern int balance;                     //PWM����ֵ 

extern float Position_PWM,Last_Position,Position_Bias,Position_Differential;
extern float Position_Least;

extern float x, x_speed, accel, angle, angle_speed, u; //λ��,�ٶ� ,���ٶ�,�Ƕ�,���ٶ�,��ѹ	


int TIM1_UP_IRQHandler(void);
int Balance(float angle);
int Position(int Encoder);
void Set_Pwm(int moto);
void Key(void);
void Xianfu_Pwm(void);
u8 Turn_Off(int voltage);
int myabs(int a);
void Find_Zero(void);
void Auto_run(void);
int Incremental_PI (int Encoder,int Target);
void Get_D_Angle_Balance(void);
int Mean_Filter(int sensor);
int Position_PID (int Encoder,int Target);
int Pre_Position(int Encoder);
float angle_count(float angle_adc);
int Sign(float value);
#endif
