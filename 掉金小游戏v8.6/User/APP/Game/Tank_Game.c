#include "Drawing.h"
#include "Control.h"
#include "WavPlay.h"
#include "math.h"
#include "Tank_Game.h"

static u8   L_X;							//左边坦克当前X位置
static u8	L_Y;							//左边坦克当前Y位置
static u8   R_X;							//右边坦克当前X位置
static u8	R_Y;							//右边坦克当前Y位置

static u8	Tank_Refresh_flag;				//坦克位置刷新标志
static u8	Tank_Position[2][6][2];			//坦克位置标记

static u8	Life_L;							//左坦克血量
static u8	Life_R;							//右坦克血量
static u8	motion_flag;					//跷跷板运动标志
static u8   motion_conut[2];				//运动模式计数
static u8	B_flag;							//子弹发射标志
static u8   Be_hit_flag;					//被击中标志
static u16  Be_hit_count[2];				//被击中闪烁时间计数
static u8  	Twinkle_flag;					//坦克闪烁标志 0-常亮 1-闪烁
static u8   Twinkle_state;					//坦克量灭状态 0-灭 1-亮
static u8   Anger_Mode;						//愤怒模式
static u8	Anger_Time[2];					//愤怒模式闪烁时间计数
static u8	Anger_Count[2];					//进入愤怒模式次数计数
static u8	Anger_Threshold_L;				//左愤怒模式阈值
static u8	Anger_Threshold_R;				//右愤怒模式阈值
static u8   Anger_flag;						//愤怒模式闪烁标志
static u8	Continuous_Hit[2];				//连续击中计数

static int16_t  count[5];					//公用计数

static float	B_P_X_L;					//左边坦克子弹位置
static u8		B_P_Y_L;
static float	B_P_X_R;					//右边坦克子弹位置
static u8		B_P_Y_R;

static u16	Game_End_count;					//游戏结束计时
static u8   LKey_count[4];					//按键长按计数
 u8 	Game_End=0;				//游戏结束标志
static u16	Audio_flag;						//音效标志
int tanktime=0;
 void Tank_Game_Init(void);
static void Tank_Model(u8 x,u8 y,u32 colour,u8 flag);
static void Wall_Mode(u8 x,u32 colour,u8 flag);
static u8 	Detection_Obstacle(u8 x,u8 y,u8 flag);
static void Game_Over(u8 flag);
void Flow_Lamp(void);

void Tank_Game(void);

/*描述: 游戏参数初始化
 *输入：void
 *输出：void
 */
 void Tank_Game_Init(void)
{
	L_X = 0;
	R_X = 47;
	L_Y = LED_Y/2-1;
	R_Y = LED_Y/2-1;
	Tank_Refresh_flag = 0;
	
	Life_L = 23;
	Life_R = 24;
	B_P_X_L = L_X + 3;
	B_P_Y_L = L_Y;
	B_P_X_R = R_X - 3;
	B_P_Y_R = R_Y;
	Anger_Threshold_L = L_X + 5;
	Anger_Threshold_R = R_X - 5;
	
	B_flag = 0;
	motion_flag = 0;
	motion_conut[0] = 0;
	motion_conut[1] = 0;
	Be_hit_flag = 0;
	Be_hit_count[0] = 0;
	Be_hit_count[1] = 0;
	Twinkle_flag = 0;
	Twinkle_state = 0;
	Game_End = 0;
	Audio_flag = 0;
	Anger_Mode = 0;
	Anger_Count[0] = 0;
	Anger_Count[1] = 0;
	Continuous_Hit[0] = 0;
	Continuous_Hit[1] = 0;
	
	Clear_screen();
	
//	/*建立坦克模型*/
//	Tank_Model(L_X,L_Y,TANK_C_L,LEFT);
//	Tank_Model(R_X,R_Y,TANK_C_R,RIGHT);
//	
//	/*建立围墙模型*/
//	Wall_Mode(Life_L,WALL_C_L,LEFT);
//	Wall_Mode(Life_R,WALL_C_R,RIGHT);
	
	//Update_Layer();
}

/*描述: 坦克模型
 *输入：y - 坦克位置
 *	    colour - 颜色
 *		flag = 0 - 左坦克
 *           = 1 - 右坦克
 *输出：void
 */
static void Tank_Model(u8 x,u8 y,u32 colour,u8 flag)
{
	if(flag == 0)
	{
		if(y != 0)
		{
			Draw_Point(x,y-1,colour,0);
			Draw_Point(x+1,y-1,colour,0);
			
			Tank_Position[0][0][0] = x;
			Tank_Position[0][0][1] = y-1;
			
			Tank_Position[0][1][0] = x+1;
			Tank_Position[0][1][1] = y-1;
		}
		else
		{
			Tank_Position[0][0][0] = 0xff;
			Tank_Position[0][0][1] = 0xff;
			
			Tank_Position[0][1][0] = 0xff;
			Tank_Position[0][1][1] = 0xff;
		}
		if(y < LED_Y-1)
		{
			Draw_Point(x,y+1,colour,0);
			Draw_Point(x+1,y+1,colour,0);
			
			Tank_Position[0][4][0] = x;
			Tank_Position[0][4][1] = y+1;
			
			Tank_Position[0][5][0] = x+1;
			Tank_Position[0][5][1] = y+1;
		}
		else
		{
			Tank_Position[0][4][0] = 0xff;
			Tank_Position[0][4][1] = 0xff;
			
			Tank_Position[0][5][0] = 0xff;
			Tank_Position[0][5][1] = 0xff;
		}
		Draw_Point(x+1,y,colour,0);
		Draw_Point(x+2,y,colour,0);
		
		Tank_Position[0][2][0] = x+1;
		Tank_Position[0][2][1] = y;
		
		Tank_Position[0][3][0] = x+2;
		Tank_Position[0][3][1] = y;
	}
	else if(flag == 1)
	{
		if(y != 0)
		{
			Draw_Point(x,y-1,colour,0);
			Draw_Point(x-1,y-1,colour,0);
			
			Tank_Position[1][0][0] = x;
			Tank_Position[1][0][1] = y-1;
			
			Tank_Position[1][1][0] = x-1;
			Tank_Position[1][1][1] = y-1;
		}
		else
		{
			Tank_Position[1][0][0] = 0xff;
			Tank_Position[1][0][1] = 0xff;
			
			Tank_Position[1][1][0] = 0xff;
			Tank_Position[1][1][1] = 0xff;
		}
		
		if(y < LED_Y-1)
		{
			Draw_Point(x,y+1,colour,0);
			Draw_Point(x-1,y+1,colour,0);
			
			Tank_Position[1][4][0] = x;
			Tank_Position[1][4][1] = y+1;
			
			Tank_Position[1][5][0] = x-1;
			Tank_Position[1][5][1] = y+1;
		}
		else
		{
			Tank_Position[1][4][0] = 0xff;
			Tank_Position[1][4][1] = 0xff;
			
			Tank_Position[1][5][0] = 0xff;
			Tank_Position[1][5][1] = 0xff;
		}
		
		Draw_Point(x-1,y,colour,0);
		Draw_Point(x-2,y,colour,0);
		
		Tank_Position[1][2][0] = x-1;
		Tank_Position[1][2][1] = y;
		
		Tank_Position[1][3][0] = x-2;
		Tank_Position[1][3][1] = y;
	}
}

/*描述: 建立围墙模型
 *输入：x - 位置
 *	    colour - 颜色
 *      flag = 0 - 建立左边围墙模型
 *           = 1 - 建立右边围墙模型
 *输出：void
 */
static void Wall_Mode(u8 x,u32 colour,u8 flag)
{
	u8 i;
	static u8 X_buf1 = 0xff;
	static u8 X_buf2 = 0xff;
	
	if(flag == 0)
	{
		if(x >= LED_X/2)
		{
			return;
		}
		
		for(i=0;i<LED_Y;i++)
		{
			Draw_Point(X_buf1,i,0x000000,0);
			Draw_Point(x,i,colour,0);
		}
		X_buf1 = x;
	}
	else if(flag == 1)
	{
		if(x < LED_X/2)
		{
			return;
		}
		
		for(i=0;i<LED_Y;i++)
		{
			Draw_Point(X_buf2,i,0x000000,0);
			Draw_Point(x,i,colour,0);
		}
		X_buf2 = x;
	}
}

/*描述: 检测障碍物
 *输入：x,y - 子弹位置
 *	    flag = 0 - 左边障碍物
 *           = 1 - 右边障碍物
 *输出：0 - 此位置无障碍物
 *      1 - 此位置有坦克
 *      2 - 此位置有围墙
 */
static u8 Detection_Obstacle(u8 x,u8 y,u8 flag)
{
	u8 i;
	
	if(flag == 0)
	{
		/*检测坦克位置*/
		for(i=0;i<6;i++)
		{
			if(Tank_Position[0][i][0] == x)
			{
				if(Tank_Position[0][i][1] == y)
					return 1;
			}
		}
	}
	else
	{
		/*检测坦克位置*/
		for(i=0;i<6;i++)
		{
			if(Tank_Position[1][i][0] == x)
			{
				if(Tank_Position[1][i][1] == y)
					return 1;
			}
		}
	}
	
	return 0;
}

/*描述: 子弹模型
 *输入：x,y - 子弹位置
 *	    colour - 颜色
 *      flag = 0 - 左子弹模型
 *      flag = 1 - 右子弹模型
 *输出：void
 */
static void Bullet_Model(u8 x,u8 y,u32 colour,u8 flag)
{
	static u32 colour_L=0x000000;
	static u32 colour_R=0x000000;
	
	if(colour == 0x000000)
	{
		if(flag == 0)
		{
			if(Detection_Pixel(x,y) == colour_L)
				Draw_Point(x,y,0x000000,0);
		}
		else if(flag == 1)
		{
			if(Detection_Pixel(x,y) == colour_R)
				Draw_Point(x,y,0x000000,0);
		}
	}
	Draw_Point(x,y,colour,1);
	if(flag == 0)	colour_L = colour;
	else if(flag == 1) colour_R = colour;
}

/*描述: 游戏开始信息显示
 *输入：x - 位置
 *输出：void
 */
static void Game_Begin(int8_t x)
{
	Clear_screen();
	DispStrCH(x,"GAME1",0x00ff00,0);
}

/*描述: 游戏结束信息显示
 *输入：flag = 0 - 左坦克胜利
 *			 = 1 - 右坦克胜利
 *输出：void
 */
static void Game_Over(u8 flag)
{
	if(flag == 0)
	{
		Draw_Point(13,3,YES_C,0);
		Draw_Point(14,3,YES_C,0);
		Draw_Point(15,3,YES_C,0);
		Draw_Point(16,3,YES_C,0);
		Draw_Point(17,2,YES_C,0);
		Draw_Point(17,4,YES_C,0);
		Draw_Point(18,1,YES_C,0);
		Draw_Point(18,5,YES_C,0);
		Draw_Point(19,1,YES_C,0);
		Draw_Point(19,5,YES_C,0);
		
		Draw_LineX(40,47,1,NO_C);
		Draw_LineX(40,47,6,NO_C);
		Draw_Point(41,5,NO_C,0);
		Draw_Point(42,4,NO_C,0);
		Draw_Point(43,4,NO_C,0);
		Draw_Point(44,3,NO_C,0);
		Draw_Point(45,3,NO_C,0);
		Draw_Point(46,2,NO_C,0);
	}
	else if(flag == 1)
	{
		Draw_LineX(12,19,1,NO_C);
		Draw_LineX(12,19,6,NO_C);
		Draw_Point(13,5,NO_C,0);
		Draw_Point(14,4,NO_C,0);
		Draw_Point(15,4,NO_C,0);
		Draw_Point(16,3,NO_C,0);
		Draw_Point(17,3,NO_C,0);
		Draw_Point(18,2,NO_C,0);
		
		Draw_Point(43,3,YES_C,0);
		Draw_Point(44,3,YES_C,0);
		Draw_Point(45,3,YES_C,0);
		Draw_Point(46,3,YES_C,0);
		Draw_Point(42,2,YES_C,0);
		Draw_Point(42,4,YES_C,0);
		Draw_Point(41,1,YES_C,0);
		Draw_Point(41,5,YES_C,0);
		Draw_Point(40,1,YES_C,0);
		Draw_Point(40,5,YES_C,0);
	}
}

/*描述: 流水灯
 *输入：void
 *输出：void
 */
//static void Flow_Lamp(void)
void Flow_Lamp(void)
{
	u8 i,y;
	static u8 x=9,X,j=0,k=0;
	static u32 grade[3][10]={
	{0x0000ff,0x0000CA,0x0000B4,0x0000A0,0x0000B4,0x000082,0x000046,0x000028,0x00000A,0x000000},
	{0x00ff00,0x00CA00,0x00B400,0x00A000,0x00B400,0x008200,0x004600,0x002800,0x000A00,0x000000},
	{0xff0000,0xCA0000,0xB40000,0xA00000,0xB40000,0x820000,0x460000,0x280000,0x0A0000,0x000000}};
	
	Draw_LineX(0,59,9,0x000000);
	Draw_LineX(0,59,10,0x000000);
	
	if((x < 9) && (x > 110))
	{
		X = x;
		y = 14;
		for(i=0;i<9;i++)
		{
			Draw_Point(X,y,grade[j][i],0);
			if(X > 0)
			{
				X--;
			}
			else if(X == 0)
			{
				X = 119;
				y = 15;
			}
		}
	}
	else if((x > 59) && (x < 69))
	{
		y = 15;
		X = 119-x;
		for(i=0;i<9;i++)
		{
			Draw_Point(X,y,grade[j][i],0);
			if(y == 15)
				X++;
			else if(y == 14)
				X--;
			if(X == 60)
			{
				X = 59;
				y = 14;
			}
		}
	}
	else if((x >= 9) && (x < 69))
	{
		X = x;
		for(i=0;i<=9;i++)
		{
			Draw_Point(X-i,14,grade[j][i],0);
		}
	}
	else if(x >= 69)
	{
		X = x;
		for(i=0;i<=9;i++)
		{
			Draw_Point(119-X+i,15,grade[j][i],0);
		}
	}
	
	k++;
	if(k == 2)
	{
		k = 0;
		x++;
		if( x == 120)
		{
			j++;
			if(j == 3)	j = 0;
			x = 0;
		}
	}
}

void Tank_Game(void)
{
	u8 buf;
	OS_ERR err;
	if(Audio_flag & Audio_bit15)			//检测禁止播放其它音效标志位
	{
		if(Detection_idle() == 1)
		{
			Audio_flag = 0x0000;			//允许播放其它音效
		}
	}
	
	if(Game_End == 0)					//游戏开始前期
	{
		if((Audio_flag &Audio_bit0) == 0x00)
		{
			count[0] = 0;
			count[1] = -37;
			Audio_flag |= Audio_bit0;
			Play_List(3);					//播放游戏开始音效
		}
		else if(Audio_flag & Audio_bit0)	//等待游戏开始音效播放完成
		{
			count[0]++;
			if(count[0] >= 3)
			{
				count[0] = 0;
				count[1]++;
				if(count[1] < 6)
					Game_Begin(count[1]);
			}
			
			if(Detection_idle() == 1)
			{
				Audio_flag &= ~Audio_bit0;
				Tank_Game_Init();
					
				/*建立坦克模型*/
				Tank_Model(L_X,L_Y,TANK_C_L,LEFT);
				Tank_Model(R_X,R_Y,TANK_C_R,RIGHT);
				
				/*建立围墙模型*/
				Wall_Mode(Life_L,WALL_C_L,LEFT);
				Wall_Mode(Life_R,WALL_C_R,RIGHT);
							Game_End = 1;
			}
		}
	}
	else if(Game_End == 1)				//游戏开始
	{
		/*检测是否有发射子弹动作*/
//		if(fabs(DMP_Data.Pitch) >= ANGLE_MIN)
//		{
//			if(DMP_Data.Pitch < -ANGLE_MIN)
//			{
//				if((B_flag&0x01) == 0x00)
//				{
//					motion_flag |= 0x01;				//左边达到最低角度
//				}
//			}
//			else if(DMP_Data.Pitch > ANGLE_MIN)
//			{
//				if((B_flag&0x10) == 0x00)
//				{
//					motion_flag |= 0x10;				//右边达到最低角度
//				}
//			}
//		}
//		else
//		{
			
			
			tanktime++;
		/*检测是否有发射子弹动作*/
		if(tanktime==100)//50x20 ms自动发射子弹
		{
				tanktime=0;
				if((B_flag&0x01) == 0x00)
					{
						motion_flag |= 0x01;				//左边达到最低角度
					}
				if((B_flag&0x10) == 0x00)
					{
						motion_flag |= 0x10;				//右边达到最低角度
					}
		}
//		if(fabs(DMP_Data.Pitch) >= ANGLE_MIN)
//		{

		
				if(DMP_Data.Pitch >ANGLE_MIN)
				{
					
					if((B_flag&0x01) == 0x00)
					{
						motion_flag |= 0x01;				//左边达到最低角度
					}
					
				}
				else if(DMP_Data.Pitch < -ANGLE_MIN)
				{
					
					if((B_flag&0x10) == 0x00)
					{
						motion_flag |= 0x10;				//右边达到最低角度
					}
				}
			
			if(motion_flag & 0x01)
			{
				motion_conut[0]++;
				if(motion_conut[0] >= 10)
				{
					motion_conut[0] = 0;
					motion_flag &= 0xf0;
					motion_flag |= 0x02; 			//检测到左边发射子弹动作
				}
			}
			if(motion_flag & 0x10)
			{
				motion_conut[1]++;
				if(motion_conut[1] >= 10)
				{
					motion_conut[1] = 0;
					motion_flag &= 0x0f;
					motion_flag |= 0x20; 			//检测到右边发射子弹动作
				}
			}
//		}
		
		/*检测是否需要发射子弹*/
		if((motion_flag & 0x02) || (motion_flag & 0x20))
		{
			/*正在播放愤怒模式、连续击中音效时不发射子弹*/
			if((Audio_flag & (Audio_bit1 | Audio_bit2)) == 0x0000)
			{
				if(motion_flag & 0x02)
				{
					motion_flag &= ~0x02;
					
					if((Be_hit_flag&0x0f) == 0x00)				//已经度过被击中状态
					{
						B_P_X_L = L_X+3;						//记录子弹X位置
						B_P_Y_L = L_Y;							//记录子弹Y位置
						B_flag |= 0x01;							//标记左边坦克发射子弹
						
						if((Audio_flag & Audio_bit15) == 0x0000)
							Play_List(1);							//发射子弹音效
					}
				}
				
				if(motion_flag & 0x20)
				{
					motion_flag &= ~0x20;
					
					if((Be_hit_flag&0xf0) == 0x00)				//已经度过被击中状态
					{
						B_P_X_R = R_X-3;
						B_P_Y_R = R_Y;							//记录子弹Y位置
						B_flag |= 0x10;							//标记右边坦克发射子弹
						if((Audio_flag & Audio_bit15) == 0x0000)
							Play_List(1);							//发射子弹音效
					}
				}
			}
			else
			{
				motion_flag &= ~0x02;
				motion_flag &= ~0x20;
			}
		}
		
		/*发射子弹过程*/
		if(B_flag)
		{
			if(B_flag & 0x01)						//左坦克发射子弹
			{
				Bullet_Model(B_P_X_L,B_P_Y_L,0x000000,LEFT);
				
				B_P_X_L +=  B_V_L * 0.02;
				
				buf = Detection_Obstacle(B_P_X_L,B_P_Y_L,RIGHT);
				if(buf == 1)								//判断是否击中右边坦克
				{
					Be_hit_flag |= 0x10;					//右坦克被击中
					B_flag &= ~0x01;						//清除左坦克子弹标志
					B_P_X_L = L_X + 3;						//恢复左坦克子弹初始位置
					
					Continuous_Hit[1]++;					//连续击中计数
					switch(Continuous_Hit[1])
					{
						case 5:
							Audio_flag |= Audio_bit2 | Audio_bit15;
							Play_List(8);					//被连续击中音效
							break;
						case 8:
							break;
						default:
							break;
					}
				}
				else
				{
					if(B_P_X_L < LED_X)
					{
						Bullet_Model(B_P_X_L,B_P_Y_L,BULLET_C_L,LEFT);
					}
					else
					{
						B_flag &= ~0x01;					//清除左坦克子弹标志
						B_P_X_L = L_X + 3;					//恢复左坦克子弹初始位置
						Continuous_Hit[1] = 0;				//连续击中计数清零
					}
				}
			}
			if(B_flag & 0x10)						//右坦克发射子弹
			{
				Bullet_Model(B_P_X_R,B_P_Y_R,0x000000,RIGHT);
				B_P_X_R -=  B_V_R * 0.02;
				
				/*判断是否击中左边坦克*/
				buf = Detection_Obstacle(B_P_X_R,B_P_Y_R,LEFT);
				if(buf == 1)
				{
					Be_hit_flag |= 0x01;					//左坦克被击中
					B_flag &= ~0x10;						//清除右坦克子弹标志
					B_P_X_R = R_X - 3;						//恢复右坦克子弹初始位置
					
					Continuous_Hit[0]++;					//连续击中计数
					switch(Continuous_Hit[0])
					{
						case 5:
							Audio_flag |= Audio_bit2 | Audio_bit15;
							Play_List(7);					//被连续击中音效
							break;
						case 8:
							break;
						default:
							break;
					}
				}
				else
				{
					if(B_P_X_R >= 0)
					{
						Bullet_Model(B_P_X_R,B_P_Y_R,BULLET_C_R,RIGHT);
					}
					else
					{
						B_flag &= ~0x10;					//清除右坦克子弹标志
						B_P_X_R = R_X - 3;					//恢复右坦克子弹初始位置
						Continuous_Hit[0] = 0;				//连续击中计数清零
					}
				}
			}
		}
		
		/*被击中信息处理*/
		if(Be_hit_flag != 0x00)
		{
			if(Be_hit_flag & 0x01)
			{
				Be_hit_flag &= ~0x01;
				Be_hit_flag |= 0x02;				//标记闪烁
				Life_L--;							//左坦克生命值减一
				
				if(Anger_Mode & 0x10)
				{
					Anger_Mode &= ~0x10;			//取消右坦克愤怒模式
					Twinkle_flag &= ~0x10;			//取消右坦克闪烁
					Tank_Refresh_flag = 1;			//更新坦克状态
					Life_R -= 3;
					Wall_Mode(Life_R,WALL_C_R,RIGHT);
				}
				
				if(Life_L > L_X+2)					//判断左坦克是否剩余生命值
				{
					if((Life_L == Anger_Threshold_L) && (Anger_Count[0] < 1))
					{
						Anger_Mode |= 0x01;			//进入愤怒模式
						Anger_Count[0]++;			//进入愤怒模式次数计数
						Twinkle_flag |= 0x01;		//坦克闪烁
						Anger_Time[0] = 0;
						
						if((Audio_flag & Audio_bit15) == 0x0000)
						{
							Play_List(5);				//进入愤怒模式，语音提示
							Audio_flag |= Audio_bit15;	//禁止播放其它音效
							Audio_flag |= Audio_bit1;
						}
					}
					Wall_Mode(Life_L,WALL_C_L,LEFT);
					
					if((Audio_flag & Audio_bit15) == 0x0000)
						Play_List(0);					//被击中音效
				}
				else
				{
					Game_End = 2;				//游戏结束
				}
			}
			if(Be_hit_flag & 0x10)
			{
				Be_hit_flag &= ~0x10;
				Be_hit_flag |= 0x20;
				Life_R++;							//右坦克生命值减一
				
				if(Anger_Mode & 0x01)
				{
					Anger_Mode &= ~0x01;
					Twinkle_flag &= ~0x01;
					Tank_Refresh_flag = 1;
					Life_L += 3;
					Wall_Mode(Life_L,WALL_C_L,LEFT);
				}
				
				if(Life_R < R_X-2)					//判断右坦克是否剩余生命值
				{
					if((Life_R == Anger_Threshold_R) && (Anger_Count[1] < 1))
					{
						Anger_Mode |= 0x10;			//进入愤怒模式
						Anger_Count[1]++;
						Twinkle_flag |= 0x10;
						Anger_Time[1] = 0;
						
						if((Audio_flag & Audio_bit15) == 0x0000)
						{
							Play_List(6);				//进入愤怒模式，语音提示
							Audio_flag |= Audio_bit15;	//禁止播放其它音效
							Audio_flag |= Audio_bit1;
						}
					}
					Wall_Mode(Life_R,WALL_C_R,RIGHT);
					
					if((Audio_flag & Audio_bit15) == 0x0000)
						Play_List(0);				//被击中音效
				}
				else
				{
					Game_End = 2;
				}
			}
			
			/*被击中闪烁控制*/
			if(Be_hit_flag & 0x02)
			{
				Twinkle_flag |= 0x01;
				Be_hit_count[0] += 20;
				if(Be_hit_count[0] >= TWINKLE_TIME)
				{
					Be_hit_count[0] = 0;
					Be_hit_flag &= ~0x02;
					Tank_Refresh_flag = 1;
					if((Anger_Mode &0x01) == 0x00)
						Twinkle_flag &= ~0x01;
				}
			}
			if(Be_hit_flag & 0x20)
			{
				Twinkle_flag |= 0x10;
				Be_hit_count[1] += 20;
				if(Be_hit_count[1] >= TWINKLE_TIME)
				{
					Be_hit_count[1] = 0;
					Be_hit_flag &= ~0x20;
					Tank_Refresh_flag = 1;
					if((Anger_Mode &0x10) == 0x00)
						Twinkle_flag &= ~0x10;
				}
			}
		}
		
		/*检测是否需要坦克闪烁*/
		if(Twinkle_flag != 0)
		{
			if(Twinkle_flag & 0x01)								//左边坦克闪烁
			{
				count[2] += 20;									//闪烁间隔计数
				if(count[2] >= 100)								//判断是否大于间隔时间
				{
					count[2] = 0;
					if(Twinkle_state & 0x01)					//判断闪烁标志位上一次的状态
					{
						Twinkle_state &= ~0x01;
						Tank_Model(L_X,L_Y,TANK_C_L,LEFT);
					}
					else
					{
						Twinkle_state |= 0x01;					//左坦克闪烁标志
						Tank_Model(L_X,L_Y,0x000000,LEFT);
					}
				}
			}
			if(Twinkle_flag & 0x10)								//右边坦克闪烁
			{
				count[3] += 20;
				if(count[3] >= 100)
				{
					count[3] = 0;
					if(Twinkle_state & 0x10)
					{
						Twinkle_state &= ~0x10;
						Tank_Model(R_X,R_Y,TANK_C_R,RIGHT);
					}
					else
					{
						Twinkle_state |= 0x10;
						Tank_Model(R_X,R_Y,0x000000,RIGHT);
					}
				}
			}
		}
		
		/*检测是否需要进入愤怒模式*/
		if(Anger_Mode)
		{
			if(Anger_Mode & 0x01)
			{
				Anger_Time[0] += 20;
				if(Anger_Time[0] >= 100)
				{
					Anger_Time[0] = 0;
					if(Anger_flag & 0x01)
					{
						Anger_flag &= ~0x01;
						Wall_Mode(Life_L,WALL_C_L,LEFT);
					}
					else
					{
						Anger_flag |= 0x01;
						Wall_Mode(Life_L,0x000000,LEFT);
					}
				}
			}
			
			if(Anger_Mode & 0x10)
			{
				Anger_Time[1] += 20;
				if(Anger_Time[1] >= 100)
				{
					Anger_Time[1] = 0;
					if(Anger_flag & 0x10)
					{
						Anger_flag &= ~0x10;
						Wall_Mode(Life_R,WALL_C_R,RIGHT);
					}
					else
					{
						Anger_flag |= 0x10;
						Wall_Mode(Life_R,0x000000,RIGHT);
					}
				}
			}
		}
		/*检测是否需要左右移动坦克位置*/
		if(Ctl_Data.Key_Pflag || Ctl_Data.Key_Lflag)
		{
			Tank_Model(L_X,L_Y,0x000000,LEFT);
			Tank_Model(R_X,R_Y,0x000000,RIGHT);
			Tank_Refresh_flag = 1;
			
			/*点动控制*/
			if(Ctl_Data.Key_Pflag)
			{
				if(Ctl_Data.Key_Pflag & key_LL)
				{
					Ctl_Data.Key_Pflag &= ~key_LL;			//清除按键
					if(L_Y > 0) L_Y--;
				}
				else if(Ctl_Data.Key_Pflag & key_LR)
				{
					Ctl_Data.Key_Pflag &= ~key_LR;
					if(L_Y < LED_Y-1) L_Y++;
				}
				
				if(Ctl_Data.Key_Pflag & key_RL)
				{
					Ctl_Data.Key_Pflag &= ~key_RL;			//清除按键
					if(R_Y < LED_Y-1) R_Y++;
				}
				else if(Ctl_Data.Key_Pflag & key_RR)
				{
					Ctl_Data.Key_Pflag &= ~key_RR;
					if(R_Y > 0) R_Y--;
				}
			}
			/*长按控制*/
			else if(Ctl_Data.Key_Lflag)
			{
				if(Ctl_Data.Key_Lflag & key_LL)
				{
					if(LKey_count[0] == 0)
					{
						if(L_Y > 0) L_Y--;
						LKey_count[0] += 20;
					}
					else if(LKey_count[0] >= 100)
					{
						LKey_count[0] = 0;
					}
					else
					{
						LKey_count[0] += 20;
					}
				}
				else if(Ctl_Data.Key_Lflag & key_LR)
				{
					if(LKey_count[1] == 0)
					{
						if(L_Y < LED_Y-1) L_Y++;
						LKey_count[1] += 20;
					}
					else if(LKey_count[1] >= 100)
					{
						LKey_count[1] = 0;
					}
					else
					{
						LKey_count[1] += 20;
					}
				}
				
				if(Ctl_Data.Key_Lflag & key_RL)
				{
					if(LKey_count[2] == 0)
					{
						if(R_Y < LED_Y-1) R_Y++;
						LKey_count[2] += 20;
					}
					else if(LKey_count[2] >= 100)
					{
						LKey_count[2] = 0;
					}
					else
					{
						LKey_count[2] += 20;
					}
				}
				else if(Ctl_Data.Key_Lflag & key_RR)
				{
					if(LKey_count[3] == 0)
					{
						if(R_Y > 0) R_Y--;
						LKey_count[3] += 20;
					}
					else if(LKey_count[3] >= 100)
					{
						LKey_count[3] = 0;
					}
					else
					{
						LKey_count[3] += 20;
					}
				}
			}
				
			if((Audio_flag & Audio_bit15) == 0x0000)
			{
				//坦克移动音效
			}
		}
		else
		{
			LKey_count[0] = 0;
			LKey_count[1] = 0;
			LKey_count[2] = 0;
			LKey_count[3] = 0;
		}
		
		/*更新坦克状态*/
		if(Tank_Refresh_flag == 1)
		{
			Tank_Refresh_flag = 0;
			Tank_Model(L_X,L_Y,TANK_C_L,LEFT);
			Tank_Model(R_X,R_Y,TANK_C_R,RIGHT);
		}
	}
	else if(Game_End == 2)				//游戏结束，判断胜利方
	{
		Clear_screen();						//清屏
		count[4] = 0;
		Game_End_count = 0;
		Play_List(4);						//播放游戏结束音效
		
		if(Life_L > (LED_X-1-Life_R))
		{
			Game_End = 3;
		}
		else if(Life_L < (LED_X-1-Life_R))
		{
			Game_End = 4;
		}
		else
		{
			Game_End = 5;
		}
	}
	else if(Game_End == 3)				//左边坦克胜利
	{
		if(Game_End_count == 0)
		{
			Game_End_count += 20;
			Game_Over(LEFT);
		}
		else if(Game_End_count == GAME_OVER_TWINKLE_INTERVAL)
		{
			Game_End_count += 20;
			Clear_screen();
		}
		else if(Game_End_count == GAME_OVER_TWINKLE_INTERVAL*2)
		{
			Game_End_count = 0;
			count[4]++;
			if(count[4] >= GAME_OVER_TWINKLE_NUMBER)
			{
				count[4] = 0;
				Game_End = 6;		//等待游戏结束音效播放完成
				Clear_screen();
				DispStrCH(10,"END",0xff0000,0);
			}
		}
		else
		{
			Game_End_count += 20;
		}
	}
	else if(Game_End == 4)				//右边坦克胜利
	{
		if(Game_End_count == 0)
		{
			Game_End_count += 20;
			Game_Over(RIGHT);
		}
		else if(Game_End_count == 200)
		{
			Game_End_count += 20;
			Clear_screen();
		}
		else if(Game_End_count == 400)
		{
			Game_End_count = 0;
			count[4]++;
			if(count[4] >= GAME_OVER_TWINKLE_NUMBER)
			{
				count[4] = 0;
				Game_End = 6;		//等待游戏结束音效播放完成
				Clear_screen();
				DispStrCH(10,"END",0xff0000,0);
				
			}
		}
		else
		{
			Game_End_count += 20;
		}
	}
	else if(Game_End == 5)				//平局
	{
		/***********平（左）****************/
		Draw_LineY(16,1,5,0xff0000);
		Draw_LineY(13,0,6,0xff0000);
		Draw_LineX(9,15,3,0xff0000);
			Draw_Line(15,1,14,2,0xff0000);
		Draw_Line(15,6,14,5,0xff0000);
		/***********************************/

		/***********平（右）****************/
		Draw_LineY(31,2,6,0xff0000);
		Draw_LineY(34,7,1,0xff0000);
		Draw_LineX(32,38,3,0xff0000);
			Draw_Line(32,6,33,5,0xff0000);
		Draw_Line(32,2,31,3,0xff0000);
		/***********************************/
		Update_Layer();
		OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
		Game_End = 6;
	}
	else if(Game_End == 6)
	{
		if(Detection_idle() == 1)
		{	
			Ctl_Data.Game_exit=0x01;//重新选择游戏
				Game_End =0;
			Clear_screen();
//			Game_End = 0;			//重新开始
//			Tank_Game_Init();
		}
	}
	
	Flow_Lamp();
	
	Update_Layer();
	if(Ctl_Data.Trigger_schedule==1)
	{
		Tank_Game_Init();
		Ctl_Data.Trigger_schedule=0;
	}
}

	

