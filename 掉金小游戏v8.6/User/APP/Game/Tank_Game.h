#ifndef __TANK_GAME__H
#define __TANK_GAME__H

#include "stm32f10x.h"

#define LEFT				0				//��̹��
#define RIGHT				1				//��̹��

#define ANGLE_MIN			6				//��ͽǶ�(��)
#define Tank_Height			3				//̹�˸߶�
#define B_V_L 				70.0			//��̹���ӵ������ٶ�
#define	B_V_R				70.0			//��̹���ӵ������ٶ�
#define TWINKLE_TIME		1000			//̹�˱�������˸ʱ��(ms)
#define TWINKLE_NUMBER		3				//̹�˱�������˸����
#define GAME_OVER_TWINKLE_INTERVAL		200	//��Ϸ������Ϣ��˸���
#define GAME_OVER_TWINKLE_NUMBER		10	//��Ϸ������Ϣ��˸����		

#define YES_C				0x106025		//ʤ����־��ɫ
#define NO_C				0xff0000		//ʧ�ܱ�־��ɫ

#define FLOW_LAMP_C			0xffffff		//�������ɫ

/*******************����ɫ����*******************/
#define TANK_C_L			0xff0000		//̹����ɫ
#define BULLET_C_L			0xff0000		//̹���ӵ���ɫ
#define WALL_C_L			0x000022		//Χǽ��ɫ

/*******************����ɫ����*******************/
#define TANK_C_R			0x0000ff		//̹����ɫ
#define BULLET_C_R			0x00ff00		//̹���ӵ���ɫ
#define WALL_C_R			0x220000		//Χǽ��ɫ

/************Audio_flag��Ч��־λ����************/
#define Audio_bit0			0x0001			//��Ϸ��ʼ��Ч
#define Audio_bit1			0x0002			//��ŭģʽ��Ч
#define Audio_bit2			0x0004			//����������Ч
#define Audio_bit3			0x0008			//
#define Audio_bit4			0x0010			//
#define Audio_bit5			0x0020			//
#define Audio_bit6			0x0040			//
#define Audio_bit7			0x0080			//
#define Audio_bit8			0x0100			//
#define Audio_bit9			0x0200			//
#define Audio_bit10			0x0400			//
#define Audio_bit11			0x0800			//
#define Audio_bit12			0x1000			//
#define Audio_bit13			0x2000			//
#define Audio_bit14			0x4000			//
#define Audio_bit15			0x8000			//��ֹ����������Ч��־
extern u8 	Game_End;
extern void Tank_Game_Init(void);
void Tank_Game(void);
void Flow_Lamp(void);

#endif
