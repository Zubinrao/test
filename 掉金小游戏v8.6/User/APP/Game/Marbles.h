#ifndef __MARBLES__H
#define __MARBLES__H

#include "stm32f10x.h"

#define	 Bat_flagLL  0  //球拍选择0，1，2，3对应左左，左右，右左，右右
#define	 Bat_flagLR  1 
#define	 Bat_flagRL  2
#define	 Bat_flagRR  3
#define  screen_length 47   //屏幕长度


#define  ball_color   0x222222  //球0颜色
#define  ball1_color   0x777777  //球1颜色
#define  BLUE  0xffff00   //球拍黄色
#define  RED 0x00ff00  //球拍绿色

#define BALLNUM4 4    //球个数

#define ANGLE_MIN			6				//最低角度(度)

/************Audio_flag音效标志位定义************/
#define MAudio_bit0			0x0001			//游戏开始音效
#define MAudio_bit1			0x0002			//愤怒模式音效
#define MAudio_bit2			0x0004			//连续击中音效
#define MAudio_bit3			0x0008			//
#define MAudio_bit4			0x0010			//
#define MAudio_bit5			0x0020			//
#define MAudio_bit6			0x0040			//
#define MAudio_bit7			0x0080			//
#define MAudio_bit8			0x0100			//
#define MAudio_bit9			0x0200			//
#define MAudio_bit10			0x0400			//
#define MAudio_bit11			0x0800			//
#define MAudio_bit12			0x1000			//
#define MAudio_bit13			0x2000			//
#define MAudio_bit14			0x4000			//
#define MAudio_bit15			0x8000			//禁止播放其它音效标志

#define angabs(x)     (((x)>0)?(x):-(x))
/**********球结构体定义*************/
struct BALL
{
	u32 COLOR;
	float x,y;
	float vx,vy;
};

extern struct BALL balls[4];
extern u8 nor_rf_flag;
extern u8 Game_End_Flag;
//extern int excount;
void Mar_init(void);
void Marbles_Game(void);
void mtest(void);
#endif 
