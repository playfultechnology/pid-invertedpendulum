#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
// SPI�����ٶ����� 
#define SPI_SPEED_2   0
#define SPI_SPEED_8   1
#define SPI_SPEED_16  2
#define SPI_SPEED_256 3
						  	    													  
void SPIx_Init(void);			 //��ʼ��SPI��
void SPIx_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u8 SPIx_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
		 
#endif

