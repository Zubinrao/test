#ifndef __select_H
#define __select_H
#include "stm32f10x.h"

#define 	select_num		4		//��Ϸ��ѡ����

extern u8 Select_State;
void Select_init(void);
void select(void);
void yes_no(void);
//void select_show(u8 state);
#endif


