/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, BIN2_Pin|BIN1_Pin|OLED_RSE_Pin|OLED_SDA_Pin
                          |OLED_SCL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = reserved_key_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(reserved_key_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = User_key_Pin|menu_key_Pin|pid_plus_Pin|pid_reduce_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = BIN2_Pin|BIN1_Pin|OLED_RSE_Pin|OLED_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = OLED_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OLED_DC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = OLED_SCL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OLED_SCL_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 2);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 2);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	HAL_Delay(5); //消抖
	
	//用户按键
  if(KEY5==0)	
	{
		Flag_Stop=!Flag_Stop;
		if(Flag_Stop==0) Swing_up=0;
	}

  //菜单按键	
	else if(KEY7==0)	
	{
		if(Menu++==4) Menu=1;
	}		
	
	//pid-按键
  else if(KEY12==0)	
	{
		if(Menu==1)        Balance_KP-=Amplitude1;
	  else	if(Menu==2)  Balance_KD-=Amplitude2;
		else  if(Menu==3)  Position_KP-=Amplitude3;
		else  if(Menu==4)  Position_KD-=Amplitude4;
	 }

  //pid+按键	 
	else if(KEY11==0)	
	{
			    if(Menu==1)  Balance_KP+=Amplitude1;
	  else	if(Menu==2)  Balance_KD+=Amplitude2;
		else  if(Menu==3)  Position_KP+=Amplitude3;
		else  if(Menu==4)  Position_KD+=Amplitude4;
	 }		
	if(Balance_KP<=0) Balance_KP=0;
	if(Balance_KD<=0) Balance_KD=0;
	if(Position_KP<=0) Position_KP=0;
	if(Position_KD<=0) Position_KD=0;
	 
	 /* 另一种写法不便于使用按键 */
//	//用户按键
//	if(GPIO_Pin==User_key_Pin) 
//	{
//		Flag_Stop=!Flag_Stop;
//		if(Flag_Stop==0) Swing_up=0;
//	}
//	
//	//菜单选择按键
//	else if(GPIO_Pin==menu_key_Pin)
//	{
//		if(Menu++==4) Menu=1;
//	}
//	
//	//pid+按键
//	else if(GPIO_Pin==pid_plus_Pin)
//	{
//			    if(Menu==1)  Balance_KP+=Amplitude1;
//	  else	if(Menu==2)  Balance_KD+=Amplitude2;
//		else  if(Menu==3)  Position_KP+=Amplitude3;
//		else  if(Menu==4)  Position_KD+=Amplitude4;
//	}
//	
//	//pid-按键
//	else if(GPIO_Pin==pid_reduce_Pin)
//	{
//		if(Menu==1)        Balance_KP-=Amplitude1;
//	  else	if(Menu==2)  Balance_KD-=Amplitude2;
//		else  if(Menu==3)  Position_KP-=Amplitude3;
//		else  if(Menu==4)  Position_KD-=Amplitude4;
//	}
}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
