#ifndef __exti_H
#define __exti_H

#include "sys.h"

void MiniBalance_EXTI_Init(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

extern u8 Swing_up;
#endif





























