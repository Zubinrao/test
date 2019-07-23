#ifndef __IN_OUTPUT_H
#define __IN_OUTPUT_H

#include "stm32f10x.h"

#define	OFF				0
#define ON				1

#define Key1			(GPIOB->IDR&GPIO_Pin_6)
#define Key2			(GPIOB->IDR&GPIO_Pin_7)
#define Key3			(GPIOB->IDR&GPIO_Pin_8)
#define Key4			(GPIOB->IDR&GPIO_Pin_9)

#define TF_Power(A)		A?(GPIOE->BSRR=GPIO_Pin_1):(GPIOE->BRR=GPIO_Pin_1)

#define SW_1			(GPIOD->IDR & GPIO_Pin_10) >> 10
#define SW_2			(GPIOD->IDR & GPIO_Pin_11) >> 11
#define SW_3			(GPIOD->IDR & GPIO_Pin_8)  >> 8
#define SW_4			(GPIOD->IDR & GPIO_Pin_9)  >> 9

#define OE_H			GPIOD->BSRR=GPIO_Pin_5,GPIOG->BSRR=GPIO_Pin_12
#define OE_L			GPIOD->BRR=GPIO_Pin_5,GPIOG->BRR=GPIO_Pin_12
#define DI(A)			A?(GPIOD->BSRR=GPIO_Pin_6,GPIOG->BSRR=GPIO_Pin_13):\
						  (GPIOD->BRR=GPIO_Pin_6,GPIOG->BRR=GPIO_Pin_13)

#define SIM900A_RST_L	GPIOA->BRR=GPIO_Pin_1
#define SIM900A_RST_H	GPIOA->BSRR=GPIO_Pin_1

void 	IO_Init(void);
uint8_t Read_SW(void);

#endif

