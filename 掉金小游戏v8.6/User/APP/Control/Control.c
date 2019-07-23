#include "Control.h"
#include "in_output.h"
#include "WavPlay.h"
#include <string.h>
#include "math.h"
#include <stdio.h>
#include "GPRS.h"
#include "flash.h"

//#include "Tank_Game.h"
//#include "Marbles.h"
//#include "snake.h"
//#include "Piano.h"
//#include "select.h"

// Games
#include "tank_war.h"
#include "Drawing.h"
#include "debug.h"
#include "gameapi.h"

extern OS_TMR	 TMR_1s;
extern char RX_Buf[100];
extern void Tank_Game_Init(void);
Control		Ctl_Data;
System		System_Data;
MPU			DMP_Data;
// 游戏指针
GAME_CLASS * pgTankWar = (GAME_CLASS *)0;

/**
 *@brief	系统运行参数初始化
 *@param	
 *@retval	
 */
static void Control_Init(void)
{
	u16 Buf[2];
	OS_ERR err;
	u8 ID[5];
	FIL 		IDfile;
	UINT 		bw;
	u16 IDtemp;
	Debug_SendMsg("Control","Init Start", DEBUG_MSG_INFO);
	rendStartUp();
	FLASH_ReadHalfWord(Data_Add0,(u16 *)&System_Data,sizeof(System)/2);
	
	
	res_sd = f_open(&IDfile,"1:/ID.txt",FA_READ);
	if(res_sd == FR_OK)
	{
		res_sd = f_read(&IDfile,ID,4,&bw);
		ID[4]='\0';
		if(res_sd == FR_OK)
		{
			IDtemp=atoi(ID);
//			Ctl_Data.ID=(atoi(ID[0])<<3)|(atoi(ID[1])<<2)|(atoi(ID[2])<<1)|atoi(ID[3]);
			Ctl_Data.ID =(IDtemp/1000*1000)+((IDtemp%1000)/100*100)+((IDtemp%100)/10*10)+((IDtemp%10)*1);
		}
		else
		{
				Play_List(127);					//ID读取失败提示音
				Debug_SendMsg("Control_Init","SoftReset", DEBUG_MSG_INFO);
				SoftReset();
		}
		f_close(&IDfile);
	}
	else
	{
			Play_List(127);					//ID文件打开失败
			Debug_SendMsg("Control_Init_SDopenfail","SoftReset", DEBUG_MSG_INFO);
			SoftReset();
	}
	
//	if(System_Data.P_state != 0)						//程序更新失败
//	{
//		SoftReset();									//复位重启
//	}
	
	/**************
		游戏API初始化
	***************/
	gameAPIInit();
	//gameTankWarReg();
	
//	Ctl_Data.ID = 0x0017;
	Ctl_Data.Class = 0x0000;
	Ctl_Data.Game_State = 0x00;
//	Ctl_Data.Game_Time = 0x02;
	Ctl_Data.Com_Mode = 0;							//普通命令模式
	Ctl_Data.Game_init_flag = 1;				
	Ctl_Data.Game_exit =0x00;//默认不询问
	Ctl_Data.payment=0;
	Ctl_Data.updata_state=0;
	Debug_SendMsg("Control","Calibrating", DEBUG_MSG_INFO);
	Calibration();
	Debug_SendMsg("Control","Calibrated", DEBUG_MSG_INFO);
	Ctl_Data.updata_state=1;
	Ctl_Data.NetErrTime=0;
	Ctl_Data.HeartSendTime=0;
//	if(Read_SW() == 0x0f)								//是否需要进行传感器校准
//	{
//		Calibration();
//	}
//	else
//	{
//		FLASH_ReadHalfWord(Data_Add1,Buf,2);			//读取传感器校准值
//		if((Buf[0] == 0xffff) && (Buf[1] == 0xffff))
//		{
//			Calibration();
//		}
//		memcpy(&Ctl_Data.Origin_Offset,Buf,4);
//	}
	Ctl_Data.volume = 2;
	Ctl_Data.Standby_mode = Read_SW();
	Ctl_Data.Heartbeat_count = 0;
	Ctl_Data.Heartbeat_flag = 0;
	Ctl_Data.Trigger_flag = ENABLE;						//默认允许触发
	Ctl_Data.Game_State = 0x00;							//默认结束游戏
	Ctl_Data.Mode=0;
	Ctl_Data.Trigger_schedule=0;
	Debug_SendMsg("Control","Control init successfully", DEBUG_MSG_INFO);
	OSTmrStart(&TMR_1s,&err);							//启动定时器
}
/**
 *@brief	：系统运行控制
 *@param	：none
 *@retval	：none
 */
void System_Control(OS_FLAGS flag)
{
	OS_ERR err;
	u8 i=0;
	
	switch(flag)
	{
		case System_Startup_flag:
			Control_Init();
			OSFlagPost(&Ctl_Data.Flag,SIM900A_RST_flag|System_Startup_finished_flag,OS_OPT_POST_FLAG_SET,&err);
			break;
		case SIM900A_RST_flag:
			SIM900A_Init();
		  flag=Game_Begins;
			break;
		case Game_Begins:
//			Ctl_Data.Trigger_flag = DISABLE;					//禁止游戏
			Ctl_Data.Game_Time = RX_Buf[7];						//更新剩余时长
//				Ctl_Data.Game_Time =0x01;
//			Mode_Swicth(RX_Buf[8]);								//设置游戏类型
//			Mode_Swicth(Ctl_Data.Mode);//设置游戏类型
			
		if(RX_Buf[2] > 0x0B)
			{
				RX_Buf[RX_Buf[2]-2] = 0;
			}
//			Ctl_Data.Game_Time = 0x02;	//游戏时间
			Ctl_Data.payment=1;
			Ctl_Data.Mode=0x00;
			Ctl_Data.Game_State = 0x01;
//			Ctl_Data.Trigger_flag = ENABLE;
			
			break;
		case Game_Over_flag:
//			Ctl_Data.Trigger_flag = DISABLE;
			Ctl_Data.Game_exit=0x00;
			Ctl_Data.Game_State = 0x00;							//清除游戏状态
			Ctl_Data.Game_Time = 0;								//清除游戏时间
			Ctl_Data.Count = 0;
			Ctl_Data.payment=0;
			Ctl_Data.Trigger_schedule=1;
			Debug_SendMsg("Control", "Game_Over_flag gotten.", DEBUG_MSG_INFO);
			for(i=0;i<10;i++)
			{
				Reply_message(game_over);						//向服务器发送结束指令
				OSFlagPend( (OS_FLAG_GRP	*)&Ctl_Data.Flag,
							(OS_FLAGS	 	 )GameOver_Answer_flag,
							(OS_TICK		 )2000,
							(OS_OPT		 	 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
							(CPU_TS		 	 )0,
							(OS_ERR			 *)&err);
				
				/* 如果未收到结束回复则再次发送结束指令 */
				if(err == OS_ERR_NONE)
					break;
			}

			//Mode_Swicth(Ctl_Data.Mode);	
			Ctl_Data.Mode=0;	//清除游戏模式
			Clear_screen();
			OSTimeDly(10,OS_OPT_TIME_DLY,&err);
			Update_Layer();
			
			break;
		default:
			break;
	}
}

/*描述：角度传感器校准程序,校准完成之后需要断电重启
 *输入：void
 *输出：void
 */
static void Calibration(void)
{
	OS_ERR err;
	uint16_t Buf[2];
	char flag=0;
	while(1)
	{
		//if((fabs(DMP_Data.Angular_Velocity) <= 10.0) && (fabs(DMP_Data.Pitch) <= 20.0))
			//{	
		   
				Play_List(129);					//角度校准中
				WaitPlay_End(5000);								//等待播放完成
		    OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
				OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
				Ctl_Data.Origin_Offset = DMP_Data.Pitch;
				
				Play_List(128);					//角度校准完成
				break;
			//}
	}
//	while(1)
//	{
//		if((fabs(DMP_Data.Angular_Velocity) <= 2.0) && (fabs(DMP_Data.Pitch) <= 5.0))
//		{
//			OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
//			Ctl_Data.Origin_Offset = DMP_Data.Pitch;
//			memcpy(Buf,&Ctl_Data.Origin_Offset,4);
//			FLASH_WriteHalfWord(Data_Add1,Buf,2);
//			while(1)
//			{
//			}
//		}
//	}
}
/*描述：触发检测,20ms调用一次
 *输入：void
 *输出：void
 */
//void Trigger_judgment(void)
//{
//	OS_ERR err;
////	static u8 	flag=0;
////	static u16 	count=0;
//	static u16 	time_count=0;
//	CPU_TS  Buf;
//	
//	if(Ctl_Data.Trigger_flag == ENABLE)
//	{	
//		if(Ctl_Data.Game_exit==0x01)
//		{
//			Ctl_Data.Game_State = 0x00;
//			yes_no();
//		}
//		if(Ctl_Data.Mode==0x00)
//		{	
//			Clear_screen();
//			Ctl_Data.Game_exit=0x00;
//			Ctl_Data.Game_State = 0x00;
//			Ctl_Data.Trigger_schedule=0;
//			select();
//		}
//		
//		if((Ctl_Data.Game_State == 0x01))			//正常运行
//		{
//			Buf=OS_TS_GET();
//			switch(Ctl_Data.Mode)
//			{
//				case TANK_GAME:
//					Tank_Game();
//					break;
//				case Marbles:
//					Marbles_Game();
//					break;
//				case snakes:
//				  snake_game();
//				   break;
//				case PIANO:
//				  Piano_Game();
//				   break;
//				default:
//					break;
//			}
//			Ctl_Data.P_run_time = (OS_TS_GET() - Buf)/72;
//		}
//		
//	
//		
////		else											//待机状态
////		{
////			if(fabs(DMP_Data.Angular_Velocity) >= 50)	//如果待机状态下检测到跷跷板在运动
////			{
//////				if(flag ==0)	
//////					count = 0;
//////				flag = 1;
////			}
////			if(flag == 1)								//试运行
////			{
////				if(count >= 2000)						//试运行时间
////				{
////					if(count == 2000)
////					{
////						Play_List(253);					//播放缴费提示音
////					}
////					
////					count++;
////					if(count >= (2000 + 2000))			//待机空白时间
////					{
////						count = 0;
////						flag = 0;
////					}
////				}
////				else
////				{
//////					Elastic_Ball();						//试运行游戏模式
////					count++;
////				}
////			}
////			else
////			{
////				Standby_effect();						//待机灯光
////				Standby_music();						//待机音乐
////			}
////		}
////	}
////	else
////	{
////		flag = 0;
////		count = 0;
////	}
//	}
//}

/*描述：模式切换
 *输入：mode-模式类型
 *输出：void
 */
//void Mode_Swicth(u8 mode)
//{
//	Ctl_Data.Mode = mode;
//	
//	switch(mode)
//	{
//		case TANK_GAME:
//			Tank_Game_Init();
//			break;
//		case Marbles:
//			Mar_init();
//			break;
////		case snakes:
////			freelink();
////			break;
//		case PIANO:
//			Piano_Init();
//		break;
//		
//		default:
//			break;
//	}
//}

/*描述：待机效果,5ms调用一次
 *输入：
 *输出：
 */
static void Standby_effect(void)
{
//	static u8 count=0,i=0;
//	static u8 flag=0;
//	/* 呼吸灯亮度等级表 */
//	static uint8_t indexWave[100] = 
//	{0,0,0,1,1,2,2,2,3,3,3,4,4,4,5,5,6,6,7,8,9,10,11,13,15,17,19,21,23,26,29,32,40,44,49,54,61,68,75,
//	 84,89,93,95,97,98,99,99,100,100,100,100,99,99,98,97,95,93,89,84,75,68,61,54,49,44,40,32,29,26,23,
//	 21,19,17,15,13,11,10,9,8,7,6,6,5,5,4,4,4,3,3,3,2,2,2,1,1,0,0,0};
//	
//	switch(Ctl_Data.Standby_mode)
//	{
//		case 0:
//			{
//				memset(&Ctl_Data.LedCtl[0],indexWave[count],LED_Number);
//	
//				i++;
//				if(i >= 6)								//间隔时间 6*5ms
//				{
//					i=0;
//					count++;
//					if(count == 100)
//						count = 0;
//				}
//			}
//			break;
//		case 1:
//			{
//				i++;
//				if(i >= 40)								//间隔时间 40*5ms
//				{
//					i=0;
//					memset(&Ctl_Data.LedCtl[0],0,LED_Number);
//					Ctl_Data.LedCtl[count] = level;
//					if(flag == 0)
//					{
//						if(count == (LED_Number-1))
//						{
//							count = LED_Number-2;
//							flag = 1;
//						}
//						else
//							count += 2;
//					}
//					else
//					{
//						if(count == 1)
//						{
//							count = 0;
//							flag = 0;
//						}
//						else
//							count -= 2;
//					}
//				}
//			}
//			break;
//		case 2:
//			{
//				i++;
//				if(i >= 10)
//				{
//					i = 0;
//					if(flag == 0)
//					{
//						Ctl_Data.Block[0] = 0;
//						Ctl_Data.Block[1] = level * 0.1;
//						Ctl_Data.Block[2] = level * 0.3;
//						Ctl_Data.Block[3] = level * 0.6;
//						Ctl_Data.Block[4] = level * 0.9;
//						
//						memcpy(&Ctl_Data.LedCtl[0]-5+count,&Ctl_Data.Block[0],5);
//						
//						if(count >= (LED_Number -1 + 5))
//						{
//							flag = 1;
//						}
//						else
//							count++;
//					}
//					else
//					{
//						Ctl_Data.Block[0] = level * 0.9;
//						Ctl_Data.Block[1] = level * 0.6;
//						Ctl_Data.Block[2] = level * 0.3;
//						Ctl_Data.Block[3] = level * 0.1;
//						Ctl_Data.Block[4] = 0;
//						
//						memcpy(&Ctl_Data.LedCtl[0]-5+count,&Ctl_Data.Block[0],5);
//						
//						if(count == 0)
//						{
//							flag = 0;
//						}
//						else
//							count--;
//					}
//				}
//			}
//			break;
//		default:
//			break;
//	}
}

/*描述：待机音乐,5ms调用一次
 *输入：
 *输出：
 */
static void Standby_music(void)
{
	char buf[30];
	static u8	list=0;
	
	if(Wav_Play.State_flag != Playing_flag)
	{
		sprintf(buf,"Standby/%d.wav",list);
		Start_Play(buf);
		
		list++;
		if(list > 7)
			list = 0;
	}
}

void Key_Test(void)
{
	static u8 Key_count[4];
		
	if(Key1)
	{
		if(Key_count[0] == 0)
		{
			Ctl_Data.Key_Pflag |= key_LL;				//点动标记
		}
		
		if(Key_count[0] >= KEY_LONG)
		{
			Ctl_Data.Key_Lflag |= key_LL;				//长按标记
		}
		else
		{
			Key_count[0] += 10;
		}
	}
	else
	{
		Key_count[0] = 0;
		Ctl_Data.Key_Pflag &= ~key_LL;
		Ctl_Data.Key_Lflag &= ~key_LL;
	}
	
	if(Key2)
	{
		if(Key_count[1] == 0)
		{
			Ctl_Data.Key_Pflag |= key_LR;
		}
		
		if(Key_count[1] >= KEY_LONG)
		{
			Ctl_Data.Key_Lflag |= key_LR;
		}
		else
		{
			Key_count[1] += 10;
		}
	}
	else
	{
		Key_count[1] = 0;
		Ctl_Data.Key_Pflag &= ~key_LR;
		Ctl_Data.Key_Lflag &= ~key_LR;
	}
	
	if(Key3)
	{
		if(Key_count[2] == 0)
		{
			Ctl_Data.Key_Pflag |= key_RL;
		}
		
		if(Key_count[2] >= KEY_LONG)
		{
			Ctl_Data.Key_Lflag |= key_RL;
		}
		else
		{
			Key_count[2] += 10;
		}
	}
	else
	{
		Key_count[2] = 0;
		Ctl_Data.Key_Pflag &= ~key_RL;
		Ctl_Data.Key_Lflag &= ~key_RL;
	}
	
	if(Key4)
	{
		if(Key_count[3] == 0)
		{
			Ctl_Data.Key_Pflag |= key_RR;
		}
		
		if(Key_count[3] >= KEY_LONG)
		{
			Ctl_Data.Key_Lflag |= key_RR;
		}
		else
		{
			Key_count[3] += 10;
		}
	}
	else
	{
		Key_count[3] = 0;
		Ctl_Data.Key_Pflag &= ~key_RR;
		Ctl_Data.Key_Lflag &= ~key_RR;
	}
}
void StrToHex(BYTE *pbDest, BYTE *pbSrc, int nLen)
{
char h1,h2;
BYTE s1,s2;
int i;

for (i=0; i<nLen; i++)
{
h1 = pbSrc[2*i];
h2 = pbSrc[2*i+1];

s1 = toupper(h1) - 0x30;
if (s1 > 9) 
s1 -= 7;

s2 = toupper(h2) - 0x30;
if (s2 > 9) 
s2 -= 7;

pbDest[i] = s1*16 + s2;
}
}

