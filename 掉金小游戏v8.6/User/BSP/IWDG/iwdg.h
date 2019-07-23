#ifndef __IWDG__H
#define __IWDG__H

#include "stm32f10x.h"

#define feed_dog	IWDG->KR = 0xAAAA

void iwdg_init(void);

#endif
