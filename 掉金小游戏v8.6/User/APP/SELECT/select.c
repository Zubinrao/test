#include "select.h"
#include "Control.h"
#include "Drawing.h"
#include "math.h"
#include "WavPlay.h"
#include "Marbles.h"
#include "snake.h"
#define YES_C				0x00ff00		//胜利标志颜色
#define NO_C				0xffff00		//失败标志颜色

#define YES_LOWC				0x003300		//胜利标志颜色
#define NO_LOWC				0x333300		//失败标志颜色

#define YES_NO_Y		2		//胜利标志文位置
#define YES_X	16		//胜利/失败标志文位置
#define NO_X	27		//失败标志位置

u32 color_table[8]={0x00330000,0x00003300,0x00000033,0x00003333,0x00ff0000,0x0000ff00,0x000000ff,0x0000ffff};
u8 	color_x[4]={12,19,26,33};
u8 	color_y=2;
u8 	color_num=4;
u8 Select_State;
u8 Select_State_old;
u8 Select_time=0;
u8 yes_no_flag_old=0x01;
void Color_Block(u8 X,u8 Y,u32 Color)
{
	Draw_LineX(X,X+4,Y,Color);
	Draw_LineX(X,X+4,Y+1,Color);
	Draw_LineX(X,X+4,Y+2,Color);
	Draw_LineX(X,X+4,Y+3,Color);
}
void Select_init(void)
{	
	u8 i;
	Select_State=0;
	Clear_screen();
	Update_Layer();

	for(i=0;i<color_num;i++)
		{
			Color_Block(color_x[i],color_y,color_table[i]);
		}	
	Update_Layer();
}


void select_show(u8 state)
{
	u8 i;
	for(i=0; i<color_num;i++)
		{
			if(state==i)
				Color_Block(color_x[i],color_y,color_table[i+4]);
			else
				Color_Block(color_x[i],color_y,color_table[i]);
		}	
}


void yes_no_show(u8 state)
{
	OS_ERR err;
	Select_time++;
	if(Select_time==3)
		{
			Select_time=0;
			Clear_screen();
			OSTimeDly(10,OS_OPT_TIME_DLY,&err);	
			Update_Layer();
			
			if(state==1)
			{	
				Color_Block(YES_X,YES_NO_Y,YES_C);
				Color_Block(NO_X,YES_NO_Y,NO_LOWC);
						//继续游戏
			}
			else
			{	
				Color_Block(YES_X,YES_NO_Y,YES_LOWC);
				Color_Block(NO_X,YES_NO_Y,NO_C);
							//退出游戏
			}
			
				OSTimeDly(10,OS_OPT_TIME_DLY,&err);	
			Update_Layer();	
				
		}
	
		if(state==1)
		{	
			Color_Block(YES_X,YES_NO_Y,YES_C);
			Color_Block(NO_X,YES_NO_Y,NO_LOWC);
			Play_List(9);				//继续游戏
		}
		else
		{	
			Color_Block(YES_X,YES_NO_Y,YES_LOWC);
			Color_Block(NO_X,YES_NO_Y,NO_C);
			Play_List(10);				//退出游戏
		}
	OSTimeDly(10,OS_OPT_TIME_DLY,&err);	
	Update_Layer();
}
void select(void)
{
	OS_ERR err;	
	Select_State=abs((int)DMP_Data.Pitch)/2;
		
		if((Select_State_old!=Select_State)&&(Select_State<color_num))
		{
				select_show(Select_State);
				Play_List(11+Select_State);					//播放选择提示音
				Select_State_old=Select_State;
		}
		else
		{
				select_show(Select_State);
		}
		
		if(Ctl_Data.Key_Pflag&&(Select_State<color_num))
		{	
				Ctl_Data.Key_Pflag &= ~	Ctl_Data.Key_Pflag ;
				if(Ctl_Data.payment==1)
				{
					Clear_screen();
					Ctl_Data.Mode=Select_State+1;
					Select_State_old = 0x20;
					Ctl_Data.Game_State = 0x01;
				}
				else
				{
					Play_List(253);					//播放扫码提示音
				}
		}
		
	Update_Layer();
}


void yes_no(void)
{

	u8 yes_no_flag;
		if((int)DMP_Data.Pitch>2)
			yes_no_flag=0x01;//继续游戏
		else
			yes_no_flag=0x00;//退出游戏重新选择
		
		if(yes_no_flag_old!=yes_no_flag)
		{
			yes_no_show(yes_no_flag);
			yes_no_flag_old=yes_no_flag;
		}

		if(Ctl_Data.Key_Pflag)
		{
			Ctl_Data.Key_Pflag &= ~	Ctl_Data.Key_Pflag ;
			Ctl_Data.Game_exit=0x00;				
			yes_no_flag_old=0x20;
			
			if(yes_no_flag==0x00)
			{
				Mode_Swicth(Ctl_Data.Mode);			
					
				Ctl_Data.Mode=0x00;//回到主界面选择游戏
			}
			else
			{
				Ctl_Data.Game_State = 0x01;
				if(Ctl_Data.Mode==snakes)
				{
					freelink();
					cycleflag=0;
				}
			}
			Clear_screen();
		}
		
	Update_Layer();

}













