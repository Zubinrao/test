#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32f10x.h"

#define IrDA1_IN	GPIOD->IDR>>9&0x0001
#define IrDA2_IN	GPIOD->IDR>>10&0x0001
#define IrDA3_IN	GPIOD->IDR>>11&0x0001

void EXTI_Config(void);

#endif /* __EXTI_H */
