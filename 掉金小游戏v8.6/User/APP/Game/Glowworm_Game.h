#ifndef __GLOWWORM_GAME__H
#define __GLOWWORM_GAME__H

#include "stm32f10x.h"

#define REFRESH_INTERVAL	200			//ө���ˢ�¼��
#define GWM_COLOUR			0xB0FF00	//ө�����ɫ
#define NEST_COLOUR			0xffffff	//��Ѩ��ɫ
#define LEVEL				31			//���ȵȼ�

#define GWM_NUMBER_MAX		100			//���ө�������
#define GWM_NUMBER_MIN		5			//����ө�������

void Glowworm_Game_Init(void);
void Glowworm_Game(void);

#endif
