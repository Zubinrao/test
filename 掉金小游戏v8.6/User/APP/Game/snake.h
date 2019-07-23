#ifndef __SNAKE__H
#define __SNAKE__H

#include "stm32f10x.h"

//游戏常量
#define wall_l     30
#define wall_w     8
#define MAX_length 20



//颜色
#define clrpink    0xff00ff
#define clrgreen   0x00ff00
#define clryellow  0xffff00
#define clrblue    0x0000ff
#define clrazure   0x00ffff
#define clrred     0xff0000

#define l_flag 0
#define r_flag 1

#define low_vel 50
#define mid_vel 40
#define high_vel 30

#define up_f 1
#define right_f 2
#define down_f 3
#define left_f 4


/************Audio_flag音效标志位定义************/
#define SAudio_bit0			0x0001			//游戏开始音效
#define SAudio_bit1			0x0002			//愤怒模式音效
#define SAudio_bit2			0x0004			//连续击中音效
#define SAudio_bit3			0x0008			//
#define SAudio_bit4			0x0010			//
#define SAudio_bit5			0x0020			//
#define SAudio_bit6			0x0040			//
#define SAudio_bit7			0x0080			//
#define SAudio_bit8			0x0100			//
#define SAudio_bit9			0x0200			//
#define SAudio_bit10			0x0400			//
#define SAudio_bit11			0x0800			//
#define SAudio_bit12			0x1000			//
#define SAudio_bit13			0x2000			//
#define SAudio_bit14			0x4000			//
#define SAudio_bit15			0x8000			//禁止播放其它音效标志

///********************按键标志位定义**********************/
//#define key_RR    0x01
//#define key_RL		0x02
//#define key_LR		0x04
//#define key_LL		0x08

//贪吃蛇定义
typedef struct node{
	int x;
	int y;
	int color;
	struct node *next;
}snake,*Linkist;

//果实定义
typedef struct
{
	int x;
	int y;
	int color;
}food;

typedef enum {FALSE = 0,TRUE = 1} bool;


extern u8 snake_flag;
extern int cycleflag;
extern int linkclr;
//extern int gol_flag;
//extern int gor_flag;
//extern u8 fb_flag;
//extern int vel_flag;
//extern u16	over_count;	

void snake_game(void);
void snake_init(void);
void Random_t(food *a);
void mself(void);
void randfood(void);
void refresh(int lf,int rf);
void twinkle(snake *head);
extern void snakeover(void);
void freelink(void);
//void lvsnake(void);
void rand_db(void);
void rand_bomb(void);

void tpfood(int snum);
void createfood(food *temfood);

#endif