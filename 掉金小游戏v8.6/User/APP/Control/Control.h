#ifndef __Control_H
#define __Control_H

#include "stm32f10x.h"
#include "os.h"
#include "ff.h"
#include "gplatform.h"

/********************* ��Ϸģʽ *********************/
#define TANK_GAME				1
#define Marbles					2
#define snakes					3
#define PIANO						4

/********************Flag�¼���־λ����**********************/
#define DMA1_7_flag				0x0001		//DMA1_7���ݴ�����ɱ�־
#define Game_Begins				0x0002		//��Ϸ������־
#define Game_Over_flag			0x0004		//��Ϸ������־
#define GameOver_Answer_flag	0x0008		//��Ϸ�����ظ���־
#define SIM900A_RST_flag		0x0010		//SIM900A��λ��־
#define System_Startup_flag		0x0020		//ϵͳ������־
#define System_Startup_finished_flag		0x0040		//ϵͳ������־

/*****************Standby_mode����ģʽ�������**************
Standby_mode = 0 - ������
Standby_mode = 1 - �����
Standby_mode = 2 - ��β��
***********************************************************/

/********************������־λ����**********************/
#define key_LL		0x01
#define key_LR		0x02
#define key_RL		0x04
#define key_RR		0x08

#define KEY_LONG	200			//����ʱ����ֵ(ms)

typedef struct
{
	u16			P_version;				//ϵͳ����汾��
	u16			M_version;				//��Ƶ�ļ��汾��
}System;

typedef struct
{
	CPU_TS		P_run_time;				//��������ʱ��
	FRESULT		Fatfs;					//�ļ�ϵͳ״̬
	u8			volume;
	
	u8			Com_Mode;				//����ģʽ 0-��������ģʽ 1-�ļ���������
	float 		Origin_Offset;			//������У׼ֵ
	float		EQ_K;
	u16			ID;						//�ն�ID��
	u16			Class;					//�ն�����
	u16			Game_Time;				//��Ϸʱ��(min)
	u8			Game_State;				//��Ϸ״̬ 0x01-��Ϸ��ʼ��0x00-��Ϸ����
	u8			Trigger_flag;			//DISABLE-��ֹ��������ENABLE-����������
	u8 			Standby_mode;			//����ģʽ
	u32			Count;					//��Ϸ����ʱ�����(s)
	u32			Heartbeat_count;		//�����������
	u8			Heartbeat_flag;			//������־
	u8			Mode;					//ģʽ
	
	u8			TH_RL;					//���ΰ��ұ���������,�����������ΰ����
	u8			TH_RR;					//���ΰ��ұ���������
	u8			TH_LL;					//���ΰ������������
	u8			TH_LR;					//���ΰ������������
	
	u8			Key_Pflag;				//�����㶯��־(ʹ������ֶ����)
	u8			Key_Lflag;				//����������־
	u8			updata_state;				//����״̬
	u8			Game_init_flag;				//����/ֹͣ��Ϸ��־  1��������Ϸ  0��ֹͣ��Ϸ
	u8			Game_exit;				//��;�˳���Ϸ  1���˳�ѯ��  0����ѯ��
	u8			Trigger_schedule;
	u8			payment;
	u8      NetErrTime;
	u8      HeartSendTime;

	OS_FLAG_GRP	Flag;					//�¼���־
}Control;
typedef struct
{
	float 	Pitch;						//������
	float	Roll;						//������
	float	Yaw;						//�����
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
