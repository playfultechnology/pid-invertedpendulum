/***********************************************
��˾����Ȥ�Ƽ�����ݸ�����޹�˾
Ʒ�ƣ�WHEELTEC
������wheeltec.net
�Ա����̣�shop114407458.taobao.com 
����ͨ: https://minibalance.aliexpress.com/store/4455017
�汾��1.0
�޸�ʱ�䣺2021-12-09

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version:1.0
Update��2021-12-09

All rights reserved
***********************************************/
#include "adc.h"
/**************************************************************************
�������ܣ�ADC��ʼ��
��ڲ�������
����  ֵ����
��    �ߣ�ƽ��С��֮��
**************************************************************************/
void Adc_Init(void)
{    
 	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
	//����ģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
}		

/**************************************************************************
�������ܣ�AD����
��ڲ�����ADC1 ��ͨ��
����  ֵ��ADת�����
��    �ߣ�ƽ��С��֮��
**************************************************************************/
u16 Get_Adc(u8 ch)   
{
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	if(ch==6) ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	     //��Դ��ѹͨ������֤׼ȷ�ԣ�ѡ���������
	else if (ch==3)   ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_1Cycles5 ); //��λ�ƴ�����ͨ������֤�����ٶȣ�ѡ����С����			     
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		       //ʹ��ָ����ADC1�����ת����������		 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));  //�ȴ�ת������
	return ADC_GetConversionValue(ADC1);	           //�������һ��ADC1�������ת�����
}

/**************************************************************************
�������ܣ���ȡ��ص�ѹ 
��ڲ�������
����  ֵ����ص�ѹ ��λMV
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int Get_battery_volt(void)   
{  
	int Volt;                                          //��ص�ѹ
	Volt=Get_Adc(Battery_Ch)*3.3*11*100/1.0/4096;		 //�����ѹ���������ԭ��ͼ�򵥷������Եõ�
	return Volt;
}


//��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//ch:ͨ�����
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_us(200);
	}
	return temp_val/times;
} 
	 








