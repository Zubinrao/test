#ifndef __Control_H
#define __Control_H

#include "stm32f10x.h"
#include "os.h"
#include "ff.h"
#include "gplatform.h"

/********************* 游戏模式 *********************/
#define TANK_GAME				1
#define Marbles					2
#define snakes					3
#define PIANO						4

/********************Flag事件标志位定义**********************/
#define DMA1_7_flag				0x0001		//DMA1_7数据传输完成标志
#define Game_Begins				0x0002		//游戏启动标志
#define Game_Over_flag			0x0004		//游戏结束标志
#define GameOver_Answer_flag	0x0008		//游戏结束回复标志
#define SIM900A_RST_flag		0x0010		//SIM900A复位标志
#define System_Startup_flag		0x0020		//系统启动标志
#define System_Startup_finished_flag		0x0040		//系统启动标志

/*****************Standby_mode待机模式相关配置**************
Standby_mode = 0 - 呼吸灯
Standby_mode = 1 - 间隔亮
Standby_mode = 2 - 拖尾灯
***********************************************************/

/********************按键标志位定义**********************/
#define key_LL		0x01
#define key_LR		0x02
#define key_RL		0x04
#define key_RR		0x08

#define KEY_LONG	200			//长按时间阈值(ms)

typedef struct
{
	u16			P_version;				//系统程序版本号
	u16			M_version;				//音频文件版本号
}System;

typedef struct
{
	CPU_TS		P_run_time;				//程序运行时间
	FRESULT		Fatfs;					//文件系统状态
	u8			volume;
	
	u8			Com_Mode;				//命名模式 0-基本命令模式 1-文件更新命令
	float 		Origin_Offset;			//传感器校准值
	float		EQ_K;
	u16			ID;						//终端ID号
	u16			Class;					//终端类型
	u16			Game_Time;				//游戏时间(min)
	u8			Game_State;				//游戏状态 0x01-游戏开始，0x00-游戏结束
	u8			Trigger_flag;			//DISABLE-禁止触发程序，ENABLE-允许触发程序
	u8 			Standby_mode;			//待机模式
	u32			Count;					//游戏运行时间计数(s)
	u32			Heartbeat_count;		//心跳间隔计数
	u8			Heartbeat_flag;			//心跳标志
	u8			Mode;					//模式
	
	u8			TH_RL;					//跷跷板右边左手油门,控制器在跷跷板左边
	u8			TH_RR;					//跷跷板右边右手油门
	u8			TH_LL;					//跷跷板左边左手油门
	u8			TH_LR;					//跷跷板左边右手油门
	
	u8			Key_Pflag;				//按键点动标志(使用完后手动清除)
	u8			Key_Lflag;				//按键长按标志
	u8			updata_state;				//升级状态
	u8			Game_init_flag;				//继续/停止游戏标志  1：继续游戏  0：停止游戏
	u8			Game_exit;				//中途退出游戏  1：退出询问  0：不询问
	u8			Trigger_schedule;
	u8			payment;
	u8      NetErrTime;
	u8      HeartSendTime;

	OS_FLAG_GRP	Flag;					//事件标志
}Control;
typedef struct
{
	float 	Pitch;						//俯仰角
	float	Roll;						//翻滚角
	float	Yaw;						//航向角
	short   gyro[3];
	short	Angular_Velocity;
}MPU;

extern	Control	Ctl_Data;
extern	MPU		DMP_Data;
extern  System	System_Data;
extern	GAME_CLASS * pgTankWar;

void System_Control(OS_FLAGS flag);
static void Control_Init(void);
static void Calibration(void);
void Trigger_judgment(void);
void Mode_Swicth(u8 mode);
static void Standby_effect(void);
static void Standby_music(void);
void 	Key_Test(void);

#endif
