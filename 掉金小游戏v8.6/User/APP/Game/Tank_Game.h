#ifndef __TANK_GAME__H
#define __TANK_GAME__H

#include "stm32f10x.h"

#define LEFT				0				//左坦克
#define RIGHT				1				//右坦克

#define ANGLE_MIN			6				//最低角度(度)
#define Tank_Height			3				//坦克高度
#define B_V_L 				70.0			//左坦克子弹发射速度
#define	B_V_R				70.0			//右坦克子弹发射速度
#define TWINKLE_TIME		1000			//坦克被击中闪烁时间(ms)
#define TWINKLE_NUMBER		3				//坦克被击中闪烁次数
#define GAME_OVER_TWINKLE_INTERVAL		200	//游戏结束信息闪烁间隔
#define GAME_OVER_TWINKLE_NUMBER		10	//游戏结束信息闪烁次数		

#define YES_C				0x106025		//胜利标志颜色
#define NO_C				0xff0000		//失败标志颜色

#define FLOW_LAMP_C			0xffffff		//跑马灯颜色

/*******************左颜色控制*******************/
#define TANK_C_L			0xff0000		//坦克颜色
#define BULLET_C_L			0xff0000		//坦克子弹颜色
#define WALL_C_L			0x000022		//围墙颜色

/*******************右颜色控制*******************/
#define TANK_C_R			0x0000ff		//坦克颜色
#define BULLET_C_R			0x00ff00		//坦克子弹颜色
#define WALL_C_R			0x220000		//围墙颜色

/************Audio_flag音效标志位定义************/
#define Audio_bit0			0x0001			//游戏开始音效
#define Audio_bit1			0x0002			//愤怒模式音效
#define Audio_bit2			0x0004			//连续击中音效
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
#define Audio_bit15			0x8000			//禁止播放其它音效标志
extern u8 	Game_End;
extern void Tank_Game_Init(void);
void Tank_Game(void);
void Flow_Lamp(void);

#endif
