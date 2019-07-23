#include "Glowworm_Game.h"
#include "Drawing.h"
#include "Control.h"
#include "WavPlay.h"
#include "string.h"
#include "math.h"
#include "ADC.h"

/*外部定义的模拟量采集数据，根据这个数据生成随机数*/
extern __IO u16 AD_Value[CH_N*Sampling_N];

static u8  Motion;								//跷跷板运动控制量
static u8 	Gwm_Number;							//萤火虫数量
static u8	Gwm_state_flag[GWM_NUMBER_MAX];		//萤火虫状态标记 0-不存在 1-存在
static u8	Gwm_direction[GWM_NUMBER_MAX];		//萤火虫下次运动方向 0-不动 1-左 2-上 3-右 4-左上 5-右上
static u8	Gwm_Position[GWM_NUMBER_MAX][2];	//萤火虫x,y位置记录表
static u8	Gwm_Birth_P[GWM_NUMBER_MAX];		//萤火虫出生位置记录表 0-从右边出生 1-从左边出生
static u8	Gwm_Level[GWM_NUMBER_MAX];			//萤火虫亮度记录表
static u8   GWM_Change[GWM_NUMBER_MAX];			//亮度变化方向记录表 0-亮度减小 1-亮度增加

static const u32 Gwm_Brightness[LEVEL]={
0x00000000,0x00060900,0x000C1100,0x00121A00,0x00172200,0x001D2B00,
0x00233300,0x00293C00,0x002F4400,0x00354D00,0x003B5500,0x00415E00,
0x00466600,0x004C6F00,0x00527700,0x00588000,0x005E8800,0x00649100,
0x006A9900,0x006FA200,0x0075AA00,0x007BB300,0x0081BB00,0x0087C300,
0x008DCC00,0x0093D500,0x0099DD00,0x009EE600,0x00A4EE00,0x00AAF700,0x00B0FF00};

u8	Count[10];							//公用计数器

/*描述: 游戏参数初始化
 *输入：void
 *输出：void
 */
void Glowworm_Game_Init(void)
{
	Gwm_Number = 0;
	
	memset(Gwm_state_flag,0,GWM_NUMBER_MAX);
//	memset(Gwm_direction,0,GWM_NUMBER_MAX);
//	memset(Gwm_Position,0,GWM_NUMBER_MAX*2);
//	memset(Gwm_Level,LEVEL-1,GWM_NUMBER_MAX);
//	memset(GWM_Change,0,GWM_NUMBER_MAX);
	
	Draw_LineX(0,59,15,NEST_COLOUR);
	Draw_LineX(0,59,14,NEST_COLOUR);
}

/*描述: 生成随机数
 *输入：x-y 随机数范围 (y > x)
 *输出：产生的随机数
 */
u8 Random(u8 x,u8 y)
{
	u8 buf;
	static u16	Indexes=0;
	static u16	Indexes_max = CH_N*Sampling_N;
	
	buf = AD_Value[Indexes] % (y-x+1) + x;
	Indexes++;
	if(Indexes == Indexes_max)
		Indexes = 0;
	
	return buf;
}

void Glowworm_Game(void)
{
	u8 i,x,y;
	u32 buf;
	
	Count[0] += 20;
	if(Count[0] >= REFRESH_INTERVAL)
	{
		Count[0] = 0;
		
		/*通过跷跷板角速度控制萤火虫数量*/
		Motion = fabs(DMP_Data.Angular_Velocity)/15;
		if(Motion < GWM_NUMBER_MIN)	Motion = GWM_NUMBER_MIN;
		else if(Motion > GWM_NUMBER_MAX) Motion = GWM_NUMBER_MAX;
		
		/*控制萤火虫运动方向*/
		for(i=0;i<GWM_NUMBER_MAX;i++)
		{
			if(Gwm_state_flag[i] != 0)
			{
				x = Gwm_Position[i][0];
				y = Gwm_Position[i][1];
				
				if(Gwm_Birth_P[i] == 0)									//从右边出生的萤火虫
				{
					if(y == 15)
						Draw_Point(x,y,NEST_COLOUR,0);					//萤火虫从巢穴出来后恢复巢穴颜色
					else
						Draw_Point(x,y,0x000000,0);						//清除上一次的运动轨迹
				
					/*控制萤火虫运动方向*/
					switch(Gwm_direction[i])
					{
						case 0:											//不动
						{
							break;
						}
						case 1:											//左运动
						{
							if(x > 0)
							{
								x--;
								Gwm_Position[i][0] = x;
							}
							else										//萤火虫消失
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							
							break;
						}
						case 2:											//上运动
						{
							if(y > 0)
							{
								if(y == 15)	y = 7;
								else y--;
								Gwm_Position[i][1] = y;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							
							break;
						}
						case 3:											//右运动
						{
							if(x < (LED_X-1))
							{
								x++;
								Gwm_Position[i][0] = x;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							
							break;
						}
						case 4:											//左上
						{
							if((y > 0) && (x > 0))
							{
								x--;
								if(y == 15)	y = 7;
								else y--;
								
								Gwm_Position[i][0] = x;
								Gwm_Position[i][1] = y;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							break;
						}
						case 5:											//右上
						{
							if((y > 0) && (x < (LED_X-1)))
							{
								x++;
								if(y == 15)	y = 7;
								else y--;
								
								Gwm_Position[i][0] = x;
								Gwm_Position[i][1] = y;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							break;
						}
						default:
							break;
					}
				}
				else if(Gwm_Birth_P[i] == 1)							//从左边出生的萤火虫
				{
					if(y == 14)
						Draw_Point(x,y,NEST_COLOUR,0);
					else
						Draw_Point(x,y,0x000000,0);
					
					switch(Gwm_direction[i])
					{
						case 0:											//不动
						{
							break;
						}
						case 1:											//左运动
						{
							if(x < (LED_X-1))
							{
								x++;
								Gwm_Position[i][0] = x;
							}
							else										//萤火虫消失
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							
							break;
						}
						case 2:											//上运动
						{
							if(y != (LED_Y-1))
							{
								if(y == 14)	y = 0;
								else y++;
								Gwm_Position[i][1] = y;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							
							break;
						}
						case 3:											//右运动
						{
							if(x > 0)
							{
								x--;
								Gwm_Position[i][0] = x;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							
							break;
						}
						case 4:											//左上
						{
							if((y != (LED_Y-1)) && (x < (LED_X-1)))
							{
								x++;
								if(y == 14)	y = 0;
								else y++;
								
								Gwm_Position[i][0] = x;
								Gwm_Position[i][1] = y;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							break;
						}
						case 5:											//右上
						{
							if((y != (LED_Y-1)) && (x > 0))
							{
								x--;
								if(y == 14)	y = 0;
								else y++;
								
								Gwm_Position[i][0] = x;
								Gwm_Position[i][1] = y;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							break;
						}
						default:
							break;
					}
				}
			}
		}
		
		/*设置萤火虫下一次的运动方向*/
		for(i=0;i<GWM_NUMBER_MAX;i++)
		{
			Gwm_direction[i] = Random(0,5);
		}
		
		/*控制萤火虫出生数量*/
		if(Gwm_Number < Motion)
		{
			for(i=0;i<Motion;i++)
			{
				/*查询已经消失了的萤火虫*/
				if(Gwm_state_flag[i] == 0)
				{
					Gwm_Birth_P[i] = Random(0,1);				//设置萤火虫的出生方向
					Gwm_Position[i][0] = Random(0,LED_X-1);		//设置萤火虫的出生位置 X
					if(Gwm_Birth_P[i] == 0)
						Gwm_Position[i][1] = 15;				//设置萤火虫的出生位置 Y
					else
						Gwm_Position[i][1] = 14;
					Gwm_Level[i] = Random(0,LEVEL-1);			//出生亮度随机
					GWM_Change[i] = Random(0,1);				//出生亮度变化方向随机
					Gwm_direction[i] = 2;						//设置萤火虫下次运动方向
					Gwm_Number++;								//记录萤火虫数量
					Gwm_state_flag[i] = 1;						//标记萤火虫已经出生
				}
			}
		}
	}
	
	/*亮度控制*/
	for(i=0;i<GWM_NUMBER_MAX;i++)
	{
		if(Gwm_state_flag[i] !=0 )
		{
			x = Gwm_Position[i][0];
			y = Gwm_Position[i][1];
			
			buf = Gwm_Brightness[Gwm_Level[i]];		//读取此时刻的亮度
			if(GWM_Change[i] !=0 )					//下一时刻的亮度增加
			{
				if(Gwm_Level[i] >= (LEVEL-1))
					GWM_Change[i] = 0;				//亮度翻转
				else
					Gwm_Level[i]++;
			}
			else									//下一时刻的亮度减小
			{
				if(Gwm_Level[i] <= 0)
					GWM_Change[i] = 1;
				else
					Gwm_Level[i]--;
			}
			
			Draw_Point(x,y,buf,0);
		}
	}
	Update_Layer();
}
