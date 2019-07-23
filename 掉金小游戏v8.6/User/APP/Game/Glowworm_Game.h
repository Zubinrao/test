#ifndef __GLOWWORM_GAME__H
#define __GLOWWORM_GAME__H

#include "stm32f10x.h"

#define REFRESH_INTERVAL	200			//萤火虫刷新间隔
#define GWM_COLOUR			0xB0FF00	//萤火虫颜色
#define NEST_COLOUR			0xffffff	//巢穴颜色
#define LEVEL				31			//亮度等级

#define GWM_NUMBER_MAX		100			//最多萤火虫数量
#define GWM_NUMBER_MIN		5			//最少萤火虫数量

void Glowworm_Game_Init(void);
void Glowworm_Game(void);

#endif
