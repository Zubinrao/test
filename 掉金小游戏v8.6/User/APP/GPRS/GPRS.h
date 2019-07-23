#ifndef __GPRS__H
#define __GPRS__H

#include "stm32f10x.h"

#define Heartbeat		80000							//心跳间隔(s)

#define	LOCALPORT			"2000"					//本地端口
#define	SERVERIP			"47.107.254.218"		//主服务器IP
#define	SERVERPORT			"20180"					//主服务器端口

//#define	SERVERIP_Spare		"2070xa3596.iask.in"	//备用服务器IP
//#define	SERVERPORT_Spare	"22782"					//备用服务器端口

#define	SERVERIP_Spare		"47.107.254.218"	//备用服务器IP
#define	SERVERPORT_Spare	"20180"					//备用服务器端口

#define frame_head				0x55			//帧头
#define frame_tail				0xAA			//帧尾
#define frame_length_Min		9				//最短帧长度

#define register_message		0x10			//注册信息
#define heartbeat_message		0x20			//心跳信息
#define game_begins				0x30			//游戏开始
#define game_over				0x40			//游戏结束
#define query_version			0x50			//查询服务器文件版本号
#define File_transfer			0xFC			//程序文件传输
#define date_transfer			0x90			//数据传输

#define Register_flag			0x0001			//开机注册成功标志
#define CMD_flag				0x0002			//串口接收到SIM900A的命令响应标志
#define Version_flag			0x0004			//收到版本查询回复
#define File_transfer_flag		0x0008			//进入文件传输命令回复

void   SIM900A_Init(void);

uint8_t And_Check(char *p,const uint16_t size);
static uint8_t Detection_frame(char *p,u8 size);
static void SIM900A_Reset(void);
static uint8_t SIM900A_SendCommand(char *command,char *response);
static uint8_t SIM900A_TCP_Link(char * serverip,char * serverport);
static uint8_t SIM900A_TCP_Close(void);
void   Send_Data(char *p,uint16_t size);
static uint8_t Start_registration(void);
void 	Reply_message(u8 message);
static uint8_t File_version_query(void);
void 	Data_processing(void);
uint8_t BBC_Check(char *p,uint16_t size);
extern int gprsflag;

#endif
