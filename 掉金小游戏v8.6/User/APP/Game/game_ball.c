/**
 * @file game_ball.c
 * @brief 彩色弹球游戏程序文件
 * @details 彩色探求游戏，参与者切换各自球拍颜色使之与球色相同。球色随机变化，如果集中球拍时颜色不同，则对方得分
 * @mainpage 主页
 * @author 刘泉波 Billy Liu
 * @email weblqb@qq.com
 * @version v1.0
 * @date 2019-3-24
 * @license 软件作者保留对该软件的所有合法权利
 */
#include "game_ball.h"
#include "debug.h"
#include "Control.h"
#include "math.h"
#include "WavPlay.h"
#include "randADC.h"
#include "shapeBalls.h"
#include "Drawing.h"
#include "Tank_Game.h"
#include "stdlib.h"

#define angabs(x)     (((x)>0)?(x):-(x))
#define k_rl 1
#define k_rr 2
#define k_lr 3
#define k_ll 4
#define length_song 8
GAME_CLASS Piano;
float 	coefficient;		
static u8		P_Now;				
static u8		P_Last;				
static u32 Piano_color[6]={0Xff0000,0Xffff00,0Xff00ff,0X33ff00,0XBF3EFF,0XDC143C};

static u16	SAudio_flag=0x0000;				
static u8 location_old=0;
static u8 location;
static int piano=0;
static u32 breath_color=0;
static u8 breath_order=0;
u8 j = 0;
uint8_t music[length_song] = {176,177,175,173,171,170,174,172};
uint8_t Director[length_song] = {160,161,162,163,164,165,166,167};
u32 temp = 0,temp1 = 0,temp2 = 0,temp3 = 0,temp4 = 0,value = 0,temp5 = 0;
//uint8_t Music_time[6] = {};
//uint8_t Music_F[6] = {};
static int count1 = 0;
u32 time = 0,time_out = 0;
static void gameTankTerminate(void);

GAME_CLASS * PainoReg(){
    // 构建结构体
    Piano.funcGame = Piano_Game;
    Piano.funcInit = Piano_Init;
    Piano.pLogo = shapeBallLogo;
    Piano.pLogoHL = shapeBallLogoHL;
    Piano.nameWAV = 150;
    Piano.funcTerminate = gameTankTerminate;
    Debug_SendMsg("gameBall", "Game Struct set.", DEBUG_MSG_INFO);
    // 注册到状态机
    gameReg(&Piano);
    return &Piano;
}

static void gameTankTerminate(void)
{
	OS_ERR err;
	Debug_SendMsg("GameTankWar", "Terminate", DEBUG_MSG_INFO);
	Play_List(156);
	OSTimeDly(7000,OS_OPT_TIME_DLY,&err);
  generalTerminate();
}


void Piano_Init(void)
{
	OS_ERR err;
	piano=0;
	j = 0;
	count1 = 0;
	temp = 0,temp1 = 0,temp2 = 0,temp3 = 0,temp4 = 0,value = 0,temp5 = 0;
	piano_time_count = 0;
		if( j == 0)
	{
	int k;
	Clear_screen();
	OSTimeDly(10,OS_OPT_TIME_DLY,&err);
	for(k = 0; k< 47; k++)
	{
		if(k%3 == 0)
		{
		Draw_LineY(k,3,7,0x3300ff);
		Draw_LineY(k,0,2,0xffffff);
		}
		//Draw_LineY((temp3+1),0,3,0x000033);
		else
		Draw_LineY(k,0,5,0xffffff);
		//Draw_LineY((temp2+1),0,7,0xffffff);
		//temp2+=4;
		//temp3+=4;
	}
	Update_Layer();
		j = 1;
		OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
		Play_List(155);
		OSTimeDly(10000,OS_OPT_TIME_DLY,&err);
	}
}


void key_board(u8 location)
{	
			Draw_LineY(location*6+3,0,7,Piano_color[location]);
			Draw_LineY(location*6+4,0,7,Piano_color[location]);
			Draw_LineY(location*6+5,0,7,Piano_color[location]);
			Draw_LineY(location*6+6,0,7,Piano_color[location]);
			Draw_LineY(location*6+7,0,7,Piano_color[location]);
			Draw_LineY(location*6+8,0,7,Piano_color[location]);
}
u8 key()
{
	if(Ctl_Data.Key_Pflag)
	{
			switch(Ctl_Data.Key_Pflag)
			{
				case key_RL:
				return k_rl;
					break;
				case key_RR:
				return k_rr;
					break;
				case key_LL:
				return k_ll;
					break;
				case key_LR:
				return k_lr;
					break;
				default:
					return 0;
					break;
			}
		}
}
void breath_light(void)//o???
{
	if((0<=breath_color<=255)&&breath_order==0)
	{
		breath_color++;
		if(breath_color==254)
			breath_order=1;
	}
	if((0<=breath_color<=255)&&breath_order==1)
	{
	breath_color--;
	if(breath_color==1)
		breath_order=0;
	}
	Draw_LineX(0,47,9,breath_color);
	Draw_LineX(0,47,10,breath_color);
}


void light_led()
{
	OS_ERR err;
	Clear_screen();
	OSTimeDly(10,OS_OPT_TIME_DLY,&err);
	Update_Layer();
}
void play_music(int i,u32 time_length)
{
		OS_ERR err;
		if(temp == 1 || temp >= time_length)
		{	
			Play_List(Director[i]);
			OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
			Play_List(music[i]);
			light_led();
			time_out = 0;
			time = 0;
			
		}
}
u32 Scanning(int i)
{
	u32 LED_Scanning = 0;
	switch(i)
	{
		case 0:
		return LED_Scanning = 300;
		break;
		case 1:
		return LED_Scanning = 350;
		break;
		case 2:
		return LED_Scanning = 300;
		break;
		case 3:
		return LED_Scanning = 350;
		break;
		case 4:
		return LED_Scanning = 450;
		break;
		case 5:
		return LED_Scanning = 500;
		break;
		case 6:
		return LED_Scanning = 350;
		break;
		case 7:
		return LED_Scanning = 350;
		break;
		default:
		break;
	}
}

//*******************************************
//
//每首歌的播放时间
//
//*******************************************
u32 change_time_length()
{
	u32 time_length = 0,LED_Scanning = 0;
	LED_Scanning = Scanning(count1);
	switch(count1)
	{
		case 0:
		return time_length = (216000 / (LED_Scanning + 20)) + 3;
		break;
		case 1:
		return time_length = (175000 / (LED_Scanning + 20)) + 3;
		break;
		case 2:
		return time_length = (162000 / (LED_Scanning + 20)) + 3;
		break;
		case 3:
		return time_length = (194000 / (LED_Scanning + 20)) + 3;
		break;
		case 4:
		return time_length = (63000 / (LED_Scanning + 20)) + 3;
		break;
		case 5:
		return time_length = (53000 / (LED_Scanning + 20)) + 3;
		break;
		case 6:
		return time_length = (180000 / (LED_Scanning + 20)) + 3;
		break;
		case 7:
		return time_length = (155000 / (LED_Scanning + 20)) + 3;
		break;
		default:
		break;
		
	}
}

//随机数
unsigned int random()
{
	unsigned int value;
	value = rand() % (47+1-1)+1;
//	value = 10;
	return value;
}






void Piano_Game(void)
{	
	OS_ERR err;
	int i;
	Clear_screen();
	OSTimeDly(10,OS_OPT_TIME_DLY,&err);
	for(i = 0; i< 47; i++)
	{
		if(i%3 == 0)
		{
		Draw_LineY(i,3,7,0x3300ff);
		Draw_LineY(i,0,2,0xffffff);
		}
		//Draw_LineY((temp3+1),0,3,0x000033);
		else
		Draw_LineY(i,0,5,0xffffff);
		//Draw_LineY((temp2+1),0,7,0xffffff);
		//temp2+=4;
		//temp3+=4;
	}
	
	temp4 = random();
	value = rand() % (47+1-1)+1;
	if(temp4 % 3 == 0)
	{
		temp1=temp4+1;
		temp3=temp4+2;
		temp5 = temp4;
	}
	else if(temp4% 3 == 1)
	{
		temp1 = temp4;
		temp3 = temp4 + 1;
		temp5 = temp4 -1;
	}
	else 
	{
		temp1 = temp4;
		temp3 = temp4 - 1;
		temp5 = temp4 + 1;
	}
		temp2++;
	if(temp2 == 6)
		temp2 = 0;
	if(value % 3 == 0)
	{
		Draw_LineY(temp5,3,7,Piano_color[temp2]);
	}
	else 
	{
		Draw_LineY(temp1,0,5,Piano_color[temp2]);
		Draw_LineY(temp3,0,5,Piano_color[temp2]);	
	}
//	Update_Layer();
	OSTimeDly(Scanning(count1),OS_OPT_TIME_DLY,&err);
	temp++; 
	if(temp == 1 || temp >= change_time_length())
	{
			temp = 1;
		count1++;
		if(count1 == length_song)
			count1 = 0;
		play_music(count1,change_time_length());
		OSTimeDly(100,OS_OPT_TIME_DLY,&err);
	}
	if(piano_time_count >= 30)
	{
		if(key() == 1 || key() == 2 || key() == 3 || key() == 4)
		{
			piano_time_count = 0;
			temp = 1;
				count1++;
			if(count1 == length_song)
				count1 = 0;
			//light_led1();
			play_music(count1,change_time_length());
			OSTimeDly(100,OS_OPT_TIME_DLY,&err);
		}
	}
	Update_Layer();
		
	/*

	if((angabs(DMP_Data.Angular_Velocity))>20)
	{
		Play_List(36);					//2?????a??D?
	OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
	Play_List(35);					//2?????a??D?
		OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
	}
	Clear_screen();
	OSTimeDly(10,OS_OPT_TIME_DLY,&err);
	//if(Ctl_Data.Key_Pflag)
	//{
		Draw_LineY(0,0,7,0x00ff00);
	Draw_LineX(1,7,4,0x00ff00);
	 Draw_Point(20,4,0x00ffff,0);
	Update_Layer();
		OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
	Clear_screen();
		    OSTimeDly(10,OS_OPT_TIME_DLY,&err);
//	}*/
}


