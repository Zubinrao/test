#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"

#define		SET_SPI_CS1		GPIOA->BSRR = GPIO_Pin_4
#define 	CLR_SPI_CS1		GPIOA->BRR = GPIO_Pin_4

void SPI_Mode_Init(void);
void SPI_Send_Data(void);

#endif
