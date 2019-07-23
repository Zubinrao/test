#include "Drawing.h"
#include "Control.h"
#include "bsp.h"
#include "WavPlay.h"
#include "math.h"
#include "Marbles.h"
#include "stdio.h"

/*外部定义的模拟量采集数据，根据这个数据生成随机数*/
extern __IO u16 AD_Value[CH_N*Sampling_N];
static int excount=0;  //外部计数延时
static int movecount=0;  //球移动声音计时
static int rf_flag =5;  //动画横线刷新次数（教学部分传递的标志位，教学部分已经取消）
u8 nor_rf_flag=10;  //正常游戏运动刷新标记
static	unsigned char LVN[2];
static	unsigned char cL[3];
static u8 aninmation_count=10;  //动画闪烁计数标记1
static u8 aninmation_exit=8;  //动画闪烁退出标记
static u8 LV=1;
/***************************************/
struct BALL balls[4]; //球结构体

/*******隐藏线全局变量**********/
static u32 LINE_COLOR;
/*************球拍全局变量***********************/
static u8 Bat_L_X;      // 左边球拍x坐标
static u8 Bat_R_X;      // 右边球拍x坐标
static u32 Bat_ColorL;   //球拍蓝色
static u32 Bat_ColorR;   //球拍红色
u8 Buff_Position[9][2]; //球拍位置
/*************************************************/

/*********分数******************/
static int L_COUNT;   //左面分数
static int  R_COUNT;	//右边分数
static int L_LINE=23;   //左面分数
static int R_LINE=24;	//右边分数
static int L_Point;   //左面分数
static int R_Point;   //右面分数
static int R_LINE;	//右边分数
/*****************************/
/*static u8 	Game_End_Flag=0;				游戏结束标志
												0   游戏开始前期
												1  游戏开始
												2  游戏结束
												3  等待游戏技术音效播放完成,
												4  加球
											*/								
											
static u16	MAudio_flag=0x8000;						//音效标志
static int16_t  count[5];					//公用计数
u8 Game_End_Flag=0;						
static	unsigned char cL[3];	//用于整形转字符串。左边
static unsigned char cR[3];	//用于整形转字符串。右边
static void LINE_init(void);
static void Balls_init(u8 n);
static void normalgame(u8 n);
static void normalinit(u8 n);
static void normal_begining_aninmation(u8 n);
static void Game_Begin(int8_t x);
static void Bat_Model(u8 x,  u8 flag,  u32 color);
static void Ball_Model(u8 x, u8 y, u32 color,u8 flag);
static void user_input(void);

static void Game_Begin(int8_t x);
static void pointdis(void);

void Marbles_Game(void);
void Mar_init(void);
void Mar_init(void)
{
	OS_ERR err;
	L_COUNT=0;   //左面分数
  R_COUNT=0;//右边分数
	R_Point=0;
	L_Point=0;
	L_LINE=15;   //左面彩线
  R_LINE=33;
	LV=1;
	Game_End_Flag = 0;
	Clear_screen();
	OSTimeDly(10,OS_OPT_TIME_DLY,&err);
	Update_Layer();
}
/*比分显示函数 
输入：void
输出：void
*/
static void pointdis(void)
{							
	OS_ERR err;
	Clear_screen();
	sprintf(cL,"%d", L_COUNT);    
	sprintf(cR,"%d", R_COUNT);   
		
	//调用显示函数，显示比分
	DispStrCH(5,cL,0x0000ff,0);
	DispStrCH(21,":",0xff00,0);
	DispStrCH(27,cR,0xff0000,0);
	Update_Layer();
	OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
	OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
	Clear_screen();
}
/*描述: 生成随机数
 *输入：x-y 随机数范围 (y > x)
 *输出：产生的随机数
 */
u8 Random1(u8 x,u8 y)
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
/*
	描述：隐藏线参数初始化
··输入：
·	输出：
*/
static void LINE_init(void)
{
	int temp;
	temp=Random1(0,10);
	switch(temp%2)
	{
		case 0:
			LINE_COLOR = BLUE;
		break;
		case 1:
			LINE_COLOR = RED;
		break;
		default:
			break;
	}
}

/*
	描述：球随机初始化
	输入：
	输出：
*/
static void Balls_init(u8 n)
{		
	u8 i,k; int temp[4];
	u8 j,m=0;u8 repeat=1; 
	/***先生成四个不同随机数存在数组里**/
	do
	{
		temp[0]=Random1(0,5);
		temp[1]=Random1(0,5);
		temp[2]=Random1(0,5);
		temp[3]=Random1(0,5);
		for(i=0;i<3;i++)
		{
			for(j=i;j<3;j++)
			{
				if(temp[i]==temp[j+1])
					m++;
			}
		}
		if (m==0)
		{
			repeat = 0;
		}
		else
			m=0;
	}while(repeat==1);
 /************************/
		
/*********球位置、速度初始化*************/	
	for(i=0;i<n;i++)
	{	
		switch (temp[i])
		{
			case 0:               //以x,y点为坐标原点，（1,-1）
			{
				balls[i].x = L_LINE-1;
				balls[i].y = 1;
				break;
			}
			case 1:               //以x,y点为坐标原点，（1,0）
			{
				balls[i].x = R_LINE+1;
				balls[i].y = 1;
				break;
			}
				case 2:               //以x,y点为坐标原点，（1,1）
			{
				balls[i].x = R_LINE+3;
				balls[i].y = 6;
				break;
			}
			case 3:               //以x,y点为坐标原点，（0,-1）
			{
				balls[i].x = L_LINE-3;
				balls[i].y = 6;
				break;
			}
				case 4:               //以x,y点为坐标原点，（0,1）
			{
				balls[i].x = L_LINE-5;
				balls[i].y = 1;
				break;
			}
				case 5:               //以x,y点为坐标原点，（-1,-1）
			{
				balls[i].x =  L_LINE+5;
				balls[i].y = 6;
				break;
			}
			default:
				break;
		}
	balls[i].COLOR = LINE_COLOR;
			k = Random1(0,5);
				switch (k)
				{
					case 0:
					balls[i].vx = 1;
					balls[i].vy = 0;
					break;
					case 1:
					balls[i].vx = 1;
					balls[i].vy = -1;
					case 2:
					balls[i].vx = 1;
					balls[i].vy = 1;
					break;
					case 3:
					balls[i].vx = -1;
					balls[i].vy = 1;
					break;
					case 4:
					balls[i].vx = -1;
					balls[i].vy = -1;
					break;
					case 5:
					balls[i].vx = -1;
					balls[i].vy = 0;
					break;
					default:
					break;
					
				}
	}
	for(i=4;i<n;i--)   //未使能球初始化
	{
		balls[i].x=-5;
		balls[i].y=-5;
		balls[i].vx=0;
		balls[i].vy=0;
	}
	/************************************/
}

/*描述：球拍模型
 *输入：x      纵轴位置
		color  颜色
		flag = 0 左左拍   左为靠近零点的一方   基本没用到，先不删了
			 = 1 左右拍
			 = 2 右左拍
			 = 3 右右拍
 *输出：void
 */
static void Bat_Model(u8 x, u8 flag,  u32 color)

{	
   u8 i;
	switch (flag)
	{
		case 0:   //	左左拍模型
		{
			Draw_LineY(x, 0, 7, color);   //横着的
			break;
		}
		case 1: 
		{
			Draw_LineY(x, 0, 7, color); 			
			break;
		}
		case 2:   //	右右拍模型
		{
			Draw_LineY(x, 0, 7, color);
			break;
		}
		case 3:   //	右左拍模型
		{
			Draw_LineY(x, 0, 7, color);
			break;
		}
		default:
			break;
	}
}
	
/*描述：球模型
 *输入：x,y - 球位置
 * 		color - 颜色
		flag   = 0 - 覆盖原来的颜色
 *			    = 1 - 保留原来的颜色(0x000000除外)
 *               = 2 - 透明显示
 *输出：void
 */
static void Ball_Model(u8 x, u8 y, u32 color,u8 flag)
{	
	Draw_Point(x,y,color,flag);
}
/*描述; 游戏开始前动画
 *输入：关卡n  = 1,2,3,4
 */
static void normal_begining_aninmation(u8 n)
{	
	OS_ERR err;
	if(excount>=rf_flag)  //刷新率
	{		excount=0;
			Bat_Model(10-aninmation_count, Bat_flagLL,  BLUE);   //流水线
			Bat_Model(screen_length-10+aninmation_count, Bat_flagLL,  BLUE);
			Update_Layer();
			//OSTimeDly(10,OS_OPT_TIME_DLY,&err);
			aninmation_count--;
	}
	if(aninmation_count==1)
	{	
		rf_flag=10;  //这里要大于下面的aninmation_exit，不然会返回上个if执行
		Clear_screen();
		OSTimeDly(10,OS_OPT_TIME_DLY,&err);
		Update_Layer();
		OSTimeDly(10,OS_OPT_TIME_DLY,&err);
		sprintf(LVN,"%d", LV);  //关卡n转换为字符串
	}
	if(excount>=aninmation_exit)
	{		aninmation_count++;
			excount=0;
		if(aninmation_count%2==0)
		{
			
			DispStrCH(15,"LV",BLUE,0);     //关卡信息显示
			DispStrCH(30,LVN,BLUE,0);
		}
		else if(aninmation_count%2!=0)
		{
			DispStrCH(15,"LV",RED,0);
			DispStrCH(30,LVN,RED,0);
		}
//		Bat_Model(10-aninmation_count, Bat_flagLL,  0x000000);
//		Bat_Model(screen_length-10+aninmation_count, Bat_flagLL,  0x000000);
		Update_Layer();
		if(aninmation_count==8)     //置初始化标志位
		{	rf_flag=5;
			aninmation_count=10;
			Game_End_Flag =((n+1)*3);
		}
	}			
}			

/*描述：正式游戏参数初始化
 *输入：球个数n，4
 *输出：void
 */
static void normalinit(u8 n)
{	//  u8 i;  
		OS_ERR err;
		Clear_screen();
		Update_Layer();
		LINE_init();
		Balls_init(n);
		Bat_R_X=screen_length;
		Bat_L_X=0;
		Bat_ColorL=0xff3030;
		Bat_ColorR=0xff3030;
		Bat_Model(Bat_R_X,  Bat_flagRR, Bat_ColorR);
		Bat_Model(Bat_L_X,  Bat_flagLL, Bat_ColorL);
		excount=0;
}

/* 描述 ：正式游戏主程序
* 输入： 球个数（<4）
* 输出：无
*/
static void normalgame(u8 n)
{		
	OS_ERR err;

	u8 i,j,m;
	u8 flash1;
	u8 twikle1;
	u8 tem[4]; u8 repeat_flag=1;
	/************角度决定球速***********************/
		if((angabs(DMP_Data.Angular_Velocity))>100)
			{
				switch(LV)
				{
						case 1:
							nor_rf_flag=2;
						break;
						case 2:
							nor_rf_flag=2;
						break;
							case 3:
							nor_rf_flag=2;
						break;
							case 4:
							nor_rf_flag=2;
						break;
				}
	
			}
			else
			{
					switch(LV)
				{
						case 1:
							nor_rf_flag=4;
						break;
						case 2:
							nor_rf_flag=4;
						break;
							case 3:
							nor_rf_flag=4;
						break;
							case 4:
							nor_rf_flag=4;
						break;
				}
			}
		/*****************************/	
	if (excount>=nor_rf_flag)
	{	
		excount=0;
		movecount++;
		if(movecount>=5)
		{
			movecount=0;
			Play_List(35);
		}
		
		for(flash1=0;flash1<n;flash1++)  //n个球熄灭
		{
			Ball_Model(balls[flash1].x, balls[flash1].y,  0x000000, 0);
		}

		Update_Layer();
		OSTimeDly(10,OS_OPT_TIME_DLY,&err);  
		
		user_input();	      //检测用户输入

		 /****球碰撞处理******/
		for(i=0;i<n;i++)      
		{		
		 /* *********出界处理******************/
		 if(balls[i].x==(Bat_R_X-1))   
		 {
		   if(balls[i].COLOR==Bat_ColorR)
		  {
				if((MAudio_flag & MAudio_bit15) == 0x0000)
				Play_List(24);		
				balls[i].vx = -balls[i].vx;
				
					switch(n)
				{
					case 1:
						R_COUNT+=3;		
						break;
					case 2:
						R_COUNT++;
						break;
					case 3:
						R_COUNT++;
				    break;
					case 4:
						R_COUNT++;
						break;	
				}
				if(R_COUNT>8)   //限制边界
				{
						R_COUNT=8;
				}
	
			}
			else             // 右边出界
			{
				
				Update_Layer();
				OSTimeDly(10,OS_OPT_TIME_DLY,&err);
				
				switch(n)
				{
					case 1:
						R_COUNT-=3;		
						break;
					case 2:
						R_COUNT--;
						break;
					case 3:
						R_COUNT--;
				    break;
					case 4:
						R_COUNT--;
						break;	
					
				}
				
//		if(Detection_idle() == 1)
//			{
						Play_List(18);
//			}
		
				
				/************更新右面豆豆*****************/
				Draw_LineY(R_LINE,7,0,0x000000);
				
					if(R_COUNT<=0)
				{
					R_COUNT=0;
					Draw_LineY(R_LINE,7,0,0x000011);  //暗色
				
				}
				else
				{
					Draw_LineY(R_LINE,7,8-R_COUNT,0x0000ff);//接到球数目的显示
					if(R_COUNT<8)
					{
					Draw_LineY(R_LINE,0,7-R_COUNT,0x000011);//待接球（暗色）显示
					}
				}
				/****************************************/
				
				for(twikle1=0;twikle1<10;twikle1++)    //闪烁右边球拍   //这部分先不做去延时处理
				{
					Ball_Model(balls[i].x, balls[i].y,  balls[i].COLOR, 0);					
					Bat_Model(Bat_R_X,  Bat_flagRR,0x000000);
					Update_Layer();
					OSTimeDly(100,OS_OPT_TIME_DLY,&err);
					Bat_Model(Bat_R_X,  Bat_flagRR,Bat_ColorR);
					Update_Layer();
					OSTimeDly(100,OS_OPT_TIME_DLY,&err);
				}
				Game_End_Flag=(3+3*n);
				}
			}
			if(balls[i].x==Bat_L_X+1)
			{
				if(balls[i].COLOR==Bat_ColorL)
				{
					if((MAudio_flag & MAudio_bit15) == 0x0000)
  				Play_List(24);	
					balls[i].vx = -balls[i].vx;
					
					switch(n)   //长豆豆
					{
						case 1:
							L_COUNT+=3;
							break;
						case 2:
							L_COUNT++;
							break;
						case 3:
							L_COUNT++;
							break;
						case 4:
							L_COUNT++;
							break;
					}
							if(L_COUNT>8)
					{
							L_COUNT=8;
					}
			
				}
				else              // 左边出界
				{	
					
									
					switch(n)   //长豆豆
					{
						case 1:
							L_COUNT-=3;
							break;
						case 2:
							L_COUNT--;
							break;
						case 3:
							L_COUNT--;
							break;
						case 4:
							L_COUNT--;
							break;
					}

							
//		if(Detection_idle() == 1)
//			{
						Play_List(18);
//			}
		
				/***********更新当前左边豆豆*************/
					Draw_LineY(L_LINE,0,7,0x000000);
						if(L_COUNT<=0)
				{
					L_COUNT=0;
					Draw_LineY(L_LINE,0,7,0x110000);//暗色
				}
				else
				{
					Draw_LineY(L_LINE,0,L_COUNT-1,0xff0000); //接到球的显示
					Draw_LineY(L_LINE,L_COUNT,7,0x110000); //	待接球（暗色）显示
				}
				/******************************************/
				
					for(twikle1=0;twikle1<10;twikle1++)       // 闪烁左边球拍
					{
						Ball_Model(balls[i].x, balls[i].y,  balls[i].COLOR, 0);
						Bat_Model(Bat_L_X,  Bat_flagLL, 0x000000);
						Update_Layer();
						OSTimeDly(100,OS_OPT_TIME_DLY,&err);
						Bat_Model(Bat_L_X,  Bat_flagLL, Bat_ColorL);
						Update_Layer();
						OSTimeDly(100,OS_OPT_TIME_DLY,&err);
					}
					
					Game_End_Flag=(3+3*n);
			}
	   }			
		//撞中间线变色
		if((balls[i].x==24))
		{
			LINE_init();
			balls[i].COLOR = LINE_COLOR;
		}
		//围墙反弹
		if((balls[i].y==0)||(balls[i].y==7))
		{
			balls[i].vy = -balls[i].vy;		
		}
	}

	for(i=0;i<n;i++)     //球亮起来
	 {
		 balls[i].x = balls[i].x + balls[i].vx;
		 balls[i].y = balls[i].y + balls[i].vy;
		 Ball_Model(balls[i].x, balls[i].y,  balls[i].COLOR, 0);
		 
	 }		 
			if(R_COUNT<=0)
				{
					R_COUNT=0;
					Draw_LineY(R_LINE,7,0,0x000011);  //暗色
				}
				else
				{
					Draw_LineY(R_LINE,7,8-R_COUNT,0x0000ff);//接到球数目的显示
					if(R_COUNT<8)
					{
					Draw_LineY(R_LINE,0,7-R_COUNT,0x000011);//待接球（暗色）显示
					}
				}
					if(L_COUNT<=0)
				{
					L_COUNT=0;
					Draw_LineY(L_LINE,0,7,0x110000);//暗色
				}
				else
				{
					Draw_LineY(L_LINE,0,L_COUNT-1,0xff0000); //接到球的显示
					Draw_LineY(L_LINE,L_COUNT,7,0x110000); //	待接球（暗色）显示
				}

	 Update_Layer();
	 OSTimeDly(10,OS_OPT_TIME_DLY,&err);   //总延时
	}
}	
/*描述：用户输入相关
		包括角度和按键 无长按功能
 *输入：void
 *输出：void
 */
static void user_input()   //挡板前后移动,转动
{
	OS_ERR err;
	if((Ctl_Data.Key_Pflag & key_LL)||(Ctl_Data.Key_Pflag & key_LR))
	{
		Bat_Model(Bat_L_X,  Bat_flagLL,  0x000000);
	  Bat_Model(Bat_L_X,  Bat_flagLR,  0x000000);
		
		if(Ctl_Data.Key_Pflag & key_LL)
		{
			Ctl_Data.Key_Pflag &= ~key_LL;			//清除按键
			Bat_Model(Bat_L_X,  Bat_flagLL,  BLUE);
			Bat_ColorL=BLUE;
		 }
		else if(Ctl_Data.Key_Pflag & key_LR)
		{
			Ctl_Data.Key_Pflag &= ~key_LR;
			Bat_Model(Bat_L_X, Bat_flagLR, RED);
			Bat_ColorL=RED;
		}		
	}
	
	if((Ctl_Data.Key_Pflag & key_RL)||(Ctl_Data.Key_Pflag & key_RR))
	{ 
		Bat_Model(Bat_R_X,  Bat_flagRL,  0x000000);
	  Bat_Model(Bat_R_X,  Bat_flagRR,  0x000000);
		if(Ctl_Data.Key_Pflag & key_RL)
		{
			Ctl_Data.Key_Pflag &= ~key_RL;			//清除按键
			Bat_Model(Bat_R_X,  Bat_flagRL, BLUE);
			Bat_ColorR=BLUE;
		}
		else if(Ctl_Data.Key_Pflag & key_RR)
		{
			Ctl_Data.Key_Pflag &= ~key_RR;
			Bat_Model(Bat_R_X,  Bat_flagRR, RED);
			Bat_ColorR=RED;
		}
	}	
}

/*描述：游戏开始信息显示
 *输入：void
 *输出：void
*/
static void Game_Begin(int8_t x)
{
	Clear_screen();
	DispStrCH(x,"GAME2",0x00ff00,0);
}

void Marbles_Game()
{

	u8 i,flash1;
	OS_ERR err;
	u8 twikle1;
	excount++;
	breath_light();

	if(Game_End_Flag == 0)					//游戏开始前期
	{
//		if(MAudio_flag & MAudio_bit15)			//检测禁止播放其它音效标志位
//		{
//			if(Detection_idle() == 1)          //如果空闲
//			{
//				MAudio_flag = 0x0000;			//允许播放其它音效
//			}
//		}
//		if(MAudio_flag==0x0000)
		if((MAudio_flag&MAudio_bit0) == 0x0000)
		{
			MAudio_flag |= MAudio_bit0;
			count[0] = 0;
			count[1] = -37;
			Play_List(23);					//播放游戏开始音效
		}
		else if(MAudio_flag & MAudio_bit0)	//等待游戏开始音效播放完成
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
				MAudio_flag =0x0000;
				Clear_screen();
				Mar_init();
				Play_List(19);
				Game_End_Flag = 5;
			}
		}
	}
	else if(Game_End_Flag == 5)    //正式开始前动画，出现buff
	{
		
		normal_begining_aninmation(1);	
	}
	 else if(Game_End_Flag == 6)     //一个球初始化
	{	
		normalinit(1);
		Game_End_Flag = 7;
	}
	 else if(Game_End_Flag == 7)    //一个球运动
	 {
		 normalgame(1);
		 //if((L_Point>=1)||(R_Point>=1) )
		if((L_COUNT>=8)||(R_COUNT>=8)) 			 //接成功5次进入下一关 point_max=1
		 {
			Ctl_Data.Game_exit=0x01;
				LV++;  
				//rf_flag=5;
				pointdis();//分数显示
			 OSTimeDly(100,OS_OPT_TIME_DLY,&err); 
				Game_End_Flag = 18;
			 	 OSTimeDly(100,OS_OPT_TIME_DLY,&err); 
				L_COUNT=0;
				R_COUNT=0;
		 }
	 }
	 else if(Game_End_Flag == 18)
	 {	 Game_End_Flag = 8;
			 Play_List(19);
		 	
	 }
	 else if(Game_End_Flag == 8)   //进入下一关动画
	 {
		 normal_begining_aninmation(2);
	 }
	 else if(Game_End_Flag == 9)   //两个球初始化
	 {
		 normalinit(2);
		 Game_End_Flag = 10;
	 }
	 else if(Game_End_Flag == 10)    //两个球运动
	 {
		 normalgame(2);
		//if((L_Point>=(2))||(R_Point>=(2)))                  //又接成功15次进入下一关 point_max=4
		 	if((L_COUNT>=8)||(R_COUNT>=8)) 
		 { 
			 	Ctl_Data.Game_exit=0x01;
				LV++;  
			 //rf_flag=5;
			 	pointdis();//比分显示
			 	 OSTimeDly(100,OS_OPT_TIME_DLY,&err); 
				Game_End_Flag = 19;
			 	 OSTimeDly(100,OS_OPT_TIME_DLY,&err); 
			 	L_COUNT=0;
				R_COUNT=0;
		 }
	 }
	 	 else if(Game_End_Flag == 19)
	 { 	Game_End_Flag = 11;
			 Play_List(19);
	 }
	 else if(Game_End_Flag == 11)   //进入下一关动画
	 {
		normal_begining_aninmation(3);
	 }
	 else if(Game_End_Flag == 12)   //3个球初始化
	 {
	   normalinit(3);
		 Game_End_Flag = 13;
	 }
	 else if(Game_End_Flag == 13)    //3个球运动
	 {
		 normalgame(3);
		 //if((L_Point>=(4))||(R_Point>=(4)))                  //又接成功40次进入下一关 point_max=12
		 if((L_COUNT>=8)||(R_COUNT>=8)) 
		 { 	Ctl_Data.Game_exit=0x01;
				//rf_flag=5;
				LV++;  
			 	pointdis();//关卡显示
			 	 OSTimeDly(100,OS_OPT_TIME_DLY,&err); 
				Game_End_Flag = 20;
			 	 OSTimeDly(100,OS_OPT_TIME_DLY,&err); 
			 		L_COUNT=0;
				R_COUNT=0;
		 }
	 }
	 	else if(Game_End_Flag == 20)
	 { 	Game_End_Flag = 14;
			 Play_List(19);
	 }
	 else if(Game_End_Flag == 14)   //进入下一关动画
	 {
		normal_begining_aninmation(4);
	 }
	 else if(Game_End_Flag == 15)   //4个球初始化
	 {	//rf_flag=5;
		 normalinit(4);
		 Game_End_Flag = 16;
	 }
	 else if(Game_End_Flag == 16)    //4个球运动
	 {
		 //rf_flag=5;
		 normalgame(4); //关卡显示在该程序内
		 if((L_COUNT>=8)||(R_COUNT>=8)) 
		 {	
				//rf_flag=5;
				//LV++;  
			 Play_List(252);
			 
			 	pointdis();//关卡显示
			 OSTimeDly(1000,OS_OPT_TIME_DLY,&err);   //总延时
			 OSTimeDly(1000,OS_OPT_TIME_DLY,&err);   //总延时
				Mar_init();
			 	L_COUNT=0;
				R_COUNT=0;
			 	Ctl_Data.Game_exit=0x01;
			}
	 }

	Update_Layer();	
if(Ctl_Data.Trigger_schedule==1)
{
	Mar_init();
//Ctl_Data.Trigger_schedule=0;
}
}

void mtest(void)
{

	Draw_LineY(0, 0, 7, 0x00ff00);
	Update_Layer();	
}