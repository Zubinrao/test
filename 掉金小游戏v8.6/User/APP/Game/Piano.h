#ifndef __PIANO__H
#define __PIANO__H

#include "stm32f10x.h"

#define	LED_Number				30		//LED组数
#define	Trigger_Angle			15		//触发角度(°)

void Piano_Init(void);
void Piano_Game(void);

#endif
