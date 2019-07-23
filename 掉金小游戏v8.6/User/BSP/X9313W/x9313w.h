#ifndef __X9313W__H
#define __X9313W__H

#include "stm32f10x.h"

#define	x9313w_Grade		32							//X9313W ³éÍ·ÊýÁ¿

#define INC_L		GPIOE->BRR = GPIO_Pin_2
#define INC_H		GPIOE->BSRR = GPIO_Pin_2
#define UD_L		GPIOE->BRR = GPIO_Pin_3
#define UD_H		GPIOE->BSRR = GPIO_Pin_3
#define CS_L		GPIOE->BRR = GPIO_Pin_4
#define CS_H		GPIOE->BSRR = GPIO_Pin_4

void X9313W_Init(void);
void X9313W_Reset(void);
void X9313W_Set(u8 p);

#endif
