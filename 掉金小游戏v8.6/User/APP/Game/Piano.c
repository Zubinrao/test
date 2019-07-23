#include "Drawing.h"
#include "Control.h"
#include "WavPlay.h"
#include "math.h"
#include "Piano.h"
#include "Tank_Game.h"

float 	coefficient;		//系数
u8		P_Now;				//现在位置
u8		P_Last;				//上一时刻位置
u32 Piano_color[7]={0XFF0000,0XFF7F00,0XFFFF00,0X00FF00,0X00FFFF,0X0000FF,0X8B00FF};

u8 location_old=0;
u8 location;
int piano=0;
u32 breath_color=0;
u8 breath_order=0;
void Piano_Init(void)
{
	piano=0;
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

void breath_light(void)
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
void Piano_Game(void)
{
	OS_ERR err;
	piano++;
	
	location=(u8)fabs(DMP_Data.Pitch);
	if((0<=location)&&(location<7))
	{	
		if(location_old!=location)
		{
			location_old=location;
			Clear_screen();
			key_board(location_old);
		}
		
		breath_light();
		OSTimeDly(10,OS_OPT_TIME_DLY,&err);
		Update_Layer();	
		if(Ctl_Data.Key_Pflag)
		{
			Ctl_Data.Key_Pflag &= ~	Ctl_Data.Key_Pflag ;
			Play_List(41+location);				//被击中音效
		}
	}	
	else
	{
		if(Ctl_Data.Key_Pflag)
		{
			Ctl_Data.Key_Pflag &= ~	Ctl_Data.Key_Pflag ;
			Play_List(47);	//
		}
	}
		
	if(piano>2000)
	{
		piano=0;
		Clear_screen();
		OSTimeDly(10,OS_OPT_TIME_DLY,&err);
		Update_Layer();	
		Ctl_Data.Game_exit=0x01;
	}
	if(Ctl_Data.Trigger_schedule==1)
{
	Piano_Init();
	Ctl_Data.Trigger_schedule=0;
}
}


