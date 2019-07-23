#include "GPRS.h"
#include "usart.h"
#include "string.h"
#include "in_output.h"
#include "Control.h"
//#include "flash.h"
#include "NVIC.h"
#include "update.h"
#include "os.h"
//#include "WavPlay.h"
#include "debug.h"

extern OS_TCB    ControlTCB;
int gprsflag=0;

union u82u32{
	u32 len;
	u8 c[4];
};

extern file_transfer	update;

static char 	TX_Buf[100];			//发送数据缓冲区
char 			RX_Buf[100];			//接收数据缓冲区

static u8 	FTP_ENTER_CMD[1]={0xA0};
static u8 	FTP_IPPORT_CMD[18]={0xA1, 0x34, 0x37, 0x2E, 0x31, 0x30, 0x37, 0x2E, 0x32, 0x35, 0x34, 0x2E, 0x32, 0x31, 0x38, 0x3A, 0x32, 0x31};
static u8 	FTP_KEY_CMD[25]={0xA2, 0x41, 0x64, 0x6D, 0x69, 0x6E, 0x69, 0x73, 0x74, 0x72, 0x61, 0x74, 0x6F, 0x72, 0x00, 0x77, 0x75, 0x70, 0x61, 0x69, 0x40, 0x32, 0x30, 0x31, 0x38};
static u8 	FTP_addr_CMD[11]={0xA3, 0x75, 0x70, 0x64, 0x61, 0x74, 0x61, 0x2E, 0x62, 0x69, 0x6E};
static u8 	FTP_download_CMD[5]={0xA4, 0x01, 0x00, 0x00, 0x01};

OS_MUTEX 		SIM900A_mutex;			//声明互斥信号量
OS_FLAG_GRP		SIM900A_Flag;			//事件标志

static u16 P_Version;					//联网查询程序版本
static u16 M_Version;					//联网查询音频文件版本

static uint8_t ftp_message(u8 *message,u8 response,u8 length);
static uint8_t Ftp_link(void);
void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen);
int NetCheck_Ok(void);







int NetCheck_Ok(void)
{
	  int  flag_network = 1;
	  //查看模块手册；
	flag_network=GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_8);
	
	return flag_network;
}


//void ftp_StrToHex(BYTE *pbDest, BYTE *pbSrc, int nLen);
/**
 *@brief	和校验
 *@param	p - 需要校验的数据首地址,退出之前保证指针有效
			size - 需要的数据总大小
 *@retval	校验结果
 */
uint8_t And_Check(char *p,uint16_t size) 
{
	uint16_t i;
	uint8_t check=0;
	
	for(i=0;i<size;i++)
	{
		check += *p;
		p++;
	}
	
	return check;
}

/**
 *@brief	检测数据中的帧信息
 *@param	p - 需要检测的帧首地址,退出之前保证指针有效
			size - 需要的数据总大小
 *@retval	1 - 检测到有效帧
			0 - 未检测到有效帧
 */
static uint8_t Detection_frame(char *p,u8 size)
{
	u8 i;
	
	if(size < frame_length_Min)											//最短帧长度
		return 0;
	
	for(i=0;i<size;i++)
	{
		if((size - i) < frame_length_Min)
			return 0;
		
		if((p[i] == frame_head) && (p[p[i+2]-1+i] == frame_tail))		//检测帧头和帧尾
		{
			if(i != 0)
			{
				memcpy(TX_Buf,p,size);
				memcpy(p,TX_Buf+i,size-i+1);
			}
			if(And_Check(p,p[2]-2) == p[p[2]-2])						//和校验
				return 1;
			else
				return 0;
		}
	}
	return 0;
}

//static uint8_t FTP_Detection_frame(char *p,u8 size)
//{
//	u8 i;
//	
//	
//	for(i=0;i<size;i++)
//	{
//		if((size - i) < frame_length_Min)
//			return 0;
//		
//		if((p[i] == frame_head) && (p[p[i+2]-1+i] == frame_tail))		//检测帧头和帧尾
//		{
//			if(i != 0)
//			{
//				memcpy(TX_Buf,p,size);
//				memcpy(p,TX_Buf+i,size-i+1);
//			}
//			if(And_Check(p,p[2]-2) == p[p[2]-2])						//和校验
//				return 1;
//			else
//				return 0;
//		}
//	}
//	return 0;
//}


/**
 *@brief	复位SIM900A
 *@param	
 *@retval	0 - 复位失败
			1 - 复位成功
			2 - 模块正常，未执行复位
 */
static void SIM900A_Reset(void)
{
	OS_ERR err;

	SIM900A_RST_L;
	OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
	SIM900A_RST_H;
	OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
//	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
//	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);

//	SIM900A_SendCommand("AT+E=OFF\r","OK");
}

/**
 *@brief	发送SIM900A控制命令
 *@param	command - 控制命令
			response - 响应信息,if(response == NULL)则不等待响应信息
 *@retval	1 - 发送成功
			0 - 发送失败
 */
static uint8_t SIM900A_SendCommand(char *command,char *response)
{
	OS_ERR err;
	
	memset(&usart2.Rx_Buf,0,Receive_number);
	OSFlagPost(&SIM900A_Flag,CMD_flag,OS_OPT_POST_FLAG_CLR,&err);					//清除串口标志
	UART2_Send_Data(command,strlen(command));										//串口发送数据
	
	if(response != NULL)
	{
		/* 等待接收到响应数据 */
		OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
				   (OS_FLAGS	 )CMD_flag,
				   (OS_TICK		 )2000,
				   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
				   (CPU_TS		 )0,
				   (OS_ERR		*)&err);
		
		if(err == OS_ERR_NONE)
		{
			if(strstr(usart2.Rx_Buf,response) != NULL)
				return 1;
			else
				return 0;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		OSTimeDly(50,OS_OPT_TIME_DLY,&err);
		return 1;
	}
}
/**
 *@brief	SIM900A TCP透传连接配置
 *@param    serverip - 服务器IP
			serverport - 服务器端口
 *@retval	1 - 连接成功
			0 - 连接失败
 */
static uint8_t SIM900A_TCP_Link(char * serverip,char * serverport)
{
	OS_ERR err;
	char buf[100];
//	
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);
//	SIM900A_SendCommand("+++\r","a");							//开启透传模式
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);
//	SIM900A_SendCommand("a\r","+ok");							//开启透传模式
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);	
//	SIM900A_SendCommand("AT+WKMOD=NET\r","OK");							//开启透传模式
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);	
//	SIM900A_SendCommand("AT+SOCKAEN=ON\r","OK");							//开启透传模式
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);
//	SIM900A_SendCommand("AT+SOCKASL=LONG\r","OK");							//开启透传模式
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);	
//  sprintf(buf,"AT+SOCKA=\"TCP\",\"%s\",\"%s\"\r",serverip,serverport);
//	SIM900A_SendCommand(buf,"OK");										//配置服务器IP和端口	
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);	
//	SIM900A_SendCommand("AT+Z\r",NULL);							//开启透传模式
//	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);		
//	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);

										//配置服务器IP和端口
	
	/* 等待接收到响应数据 */
	memset(usart2.Rx_Buf,0,Receive_number);
	OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
			   (OS_FLAGS	 )CMD_flag,
			   (OS_TICK		 )5000,
			   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
			   (CPU_TS		 )0,
			   (OS_ERR		*)&err);
	
//	if(err == OS_ERR_NONE)
//	{
//		if(strstr(usart2.Rx_Buf,"CONNECT") == NULL)
//		{
//			SIM900A_TCP_Close();
//			return 0;
//		}
//	}
//	else
//	{
//		return 0;
//	}

//	memset(usart2.Rx_Buf,0,Receive_number);
//	OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
//			   (OS_FLAGS	 )CMD_flag,
//			   (OS_TICK		 )10000,
//			   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
//			   (CPU_TS		 )0,
//			   (OS_ERR		*)&err);
//	
//	if(err == OS_ERR_NONE)
//	{
//		if(strstr(usart2.Rx_Buf,"CLOSED") != NULL)
//		{
//			SIM900A_TCP_Close();
//			return 0;
//		}
//	}
	
	return 1;
}

/**
 *@brief	关闭TCP链接
 *@param	
 *@retval	1 - 关闭成功
			0 - 关闭失败
 */
static uint8_t SIM900A_TCP_Close(void)
{
	SIM900A_SendCommand("AT+CIPCLOSE=1\r","CLOSE OK");
	return 1;
}

/**
 *@brief	GPRS发送数据
 *@param	p - 发送数据的首地址,发送数据过程中保证指针一直有效
			size - 发送数据总大小
 *@retval	1 - 发送成功
			0 - 发送失败
 */
void Send_Data(char *p,uint16_t size)
{
	OS_ERR err;
	
	OSMutexPend ((OS_MUTEX  *)&SIM900A_mutex,
				 (OS_TICK    )0,
				 (OS_OPT     )OS_OPT_PEND_BLOCKING,
				 (CPU_TS    *)0,
				 (OS_ERR    *)&err);
	
	UART2_Send_Data(p,size);
	
	OSMutexPost ((OS_MUTEX  *)&SIM900A_mutex,
				 (OS_OPT     )OS_OPT_POST_NONE,
				 (OS_ERR    *)&err);  
}


/**
 *@brief	开机注册
 *@param	
 *@retval	1 - 注册成功
			0 - 注册失败
 */
static uint8_t Start_registration(void)
{
	OS_ERR err;
	SIM900A_Init();
	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
	Reply_message(register_message);				//发送注册信息
	Debug_SendMsg("Start_registration", "CMD_Send!", DEBUG_MSG_INFO);
	OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
			   (OS_FLAGS	 )Register_flag,
			   (OS_TICK		 )10000,
			   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
			   (CPU_TS		 )0,
			   (OS_ERR		*)&err);
	
	if(err == OS_ERR_NONE)
		return 1;
	else
		return 0;
}

/**
 *@brief	应答信息
 *@param	message - 信息代码
 *@retval	
 */
void Reply_message(u8 message)
{
	TX_Buf[0] = frame_head;
	TX_Buf[1] = message;
	TX_Buf[2] = 0x09;
	TX_Buf[3] = Ctl_Data.Class >> 8;
	TX_Buf[4] = Ctl_Data.Class;
	TX_Buf[5] = Ctl_Data.ID >> 8;
	TX_Buf[6] = Ctl_Data.ID;
	TX_Buf[7] = And_Check(TX_Buf,7);
	TX_Buf[8] = frame_tail;
	
	Send_Data(TX_Buf,9);
}


/**
 *@brief	文件版本查询
 *@param	
 *@retval	1 - 查询成功
 *          0 - 查询失败
 */
static uint8_t File_version_query(void)
{
	OS_ERR err;
	
	Reply_message(0x50);
	OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
			   (OS_FLAGS	 )Version_flag,
			   (OS_TICK		 )5000,
			   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
			   (CPU_TS		 )0,
			   (OS_ERR		*)&err);
	
	if(err == OS_ERR_NONE)
	{
		M_Version = ((u16)usart2.Rx_Buf[8] << 8) || (usart2.Rx_Buf[7]);
		P_Version = ((u16)usart2.Rx_Buf[10] << 8)|| (usart2.Rx_Buf[9]);
		
		P_Version=0;
		if(P_Version != 0)
		{
			u8 i=0;
			u8 ftp_flag=1;

//				Ctl_Data.Game_State = 0;					//停止游戏
//				Ctl_Data.Heartbeat_flag = 0;			//禁用心跳功能
//				Ctl_Data.Trigger_flag = DISABLE;	//禁止角度触发
//				Ctl_Data.Com_Mode = 1;						//进入文件传输命令模式

//			Reply_message(0x80);						//通知服务器进入文件传输模式
			Play_List(255);
			OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
			
//			for(i = 0; i < 2; i++){
//				sprintf(str, "%x ||", bbc_test[i]);
//				Debug_SendMsg("File_transfer",str,DEBUG_MSG_INFO);
//			}
//			bbc_src[0]=BBC_Check(bbc_test,2);
//			HexToStr(str,bbc_src,1);
//			Debug_SendMsg("BBC",str,DEBUG_MSG_INFO);
//			
//					
//			if(BBC_Check(bbc_test,2)==0xA9)
//				Debug_SendMsg("BBC","bbcok",DEBUG_MSG_INFO);
//			else
//				Debug_SendMsg("BBC","bbcfail",DEBUG_MSG_INFO);
				
			gprsflag=0;
							Ctl_Data.Game_State = 0;					//停止游戏
				Ctl_Data.Heartbeat_flag = 0;			//禁用心跳功能
	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
//			while(ftp_flag)
//			{
//				
//				if(Ftp_link()==1)
//				{
//					ftp_flag=0;
////					Play_List(20);
////					OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
//				}
//				else
//				{
////					Play_List(22);
////					OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
//					ftp_flag=1;
//				
//				}
//					
//				
////				i++;
////				if(i=3)
////				{
////					break;
////					
////				}
//				OSTimeDly(200,OS_OPT_TIME_DLY,&err);
//			}
				Ctl_Data.Com_Mode = 1;						//进入文件传输命令模式
				Ctl_Data.Game_State = 0;					//停止游戏
				Ctl_Data.Heartbeat_flag = 0;			//禁用心跳功能
				Ctl_Data.Trigger_flag = DISABLE;	//禁止角度触发
				
				Program_download();
				Ctl_Data.Com_Mode = 0;						//进入文件传输命令模式
//				OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
//					   (OS_FLAGS	 )File_transfer_flag,
//					   (OS_TICK		 )5000,
//					   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
//					   (CPU_TS		 )0,
//					   (OS_ERR		*)&err);
//			
			
//			if(err == OS_ERR_NONE)
//			{
////				Stop_Play(1);											//停止音乐
//				Ctl_Data.Game_State = 0;					//停止游戏
//				Ctl_Data.Heartbeat_flag = 0;			//禁用心跳功能
//				Ctl_Data.Trigger_flag = DISABLE;	//禁止角度触发
//				Ctl_Data.Com_Mode = 1;						//进入文件传输命令模式
//				Program_download();								//下载程序文件
//			}
//			else
//			{
//				
//			}

		}
		else
		{
			Ctl_Data.updata_state=0;
		}
		
		if(M_Version  != 0)
		{
		}
		return 1;
	}
	return 0;
}

/**
 *@brief	SIM900A 启动初始化
 *@param	mode - 连接模式
 *          0 - 复位重连
 *          1 - 重新注册网络
 *          2 - 重新连接服务器
 *          3 - 重新注册
 *@retval	1 - 成功
			0 - 失败
 */
void SIM900A_Init(void)
{
	
	OS_ERR err;
	u8 i=0;
		
	SIM900A_Reset();

//	i=0;
//	while(SIM900A_TCP_Link(SERVERIP,SERVERPORT) == 0)		//链接服务器
//	{
//		i++;
//		if(i >= 20)
//		{
//			i=0;
//			while(SIM900A_TCP_Link(SERVERIP_Spare,SERVERPORT_Spare) == 0)		//链接备用服务器
//			{
//				i++;
//				if(i >= 20)
//				{
//					i=0;
//					SoftReset();
//				}
//				OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
//			}
//			break;
//		}
//		OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
//	}
//等待模块启动，连接服务器
while(NetCheck_Ok()==1)
{
	Debug_SendMsg("Net_link","fail",DEBUG_MSG_INFO);
	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
}
Debug_SendMsg("Net_link","OK",DEBUG_MSG_INFO);
	while(Start_registration() == 0)								//注册终端
	{
		
		i++;
		if(0)
		{
			i=0;
			Debug_SendMsg("Registration_Overtime","SoftReset",DEBUG_MSG_INFO);
			SoftReset();
		}
		OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
	}
  if(Start_registration() == 1)
	{
		gprsflag=1;
	}
OSTimeDly(5000,OS_OPT_TIME_DLY,&err);

	//	File_version_query();							//查询是否需要升级程序和文件
	
	Ctl_Data.Heartbeat_count = 0;
	Ctl_Data.Heartbeat_flag = 1;
}

void Data_processing(void)
{
	OS_ERR err;
	int i;
	char str[50];
//	Debug_SendMsg("File_transfer","==================",DEBUG_MSG_INFO);
//	for(i = 0; i < usart2.Rx_Number; i++)
//	{
//		sprintf(&str[i], "%x", usart2.Rx_Buf[i]);
//		//Debug_SendMsg("File_transfer",str,DEBUG_MSG_INFO);
//	}
//	
	if(Detection_frame(usart2.Rx_Buf,usart2.Rx_Number)||(usart2.Rx_Buf[4]==0x0A))		//判断是否为服务器下传的控制命令
	{
		switch(usart2.Rx_Buf[1])
		{
			case register_message:							//收到开机注册应答
				OSFlagPost(&SIM900A_Flag,Register_flag,OS_OPT_POST_FLAG_SET,&err);
				Debug_SendMsg("Data_processing","register_message", DEBUG_MSG_INFO);
				break;
			case heartbeat_message:							//收到心跳信息
//				Reply_message(heartbeat_message);			//应答
//				Ctl_Data.Heartbeat_count = 0;			//清除心跳时间
//				Ctl_Data.Heartbeat_flag = 1;			//监测到心跳
			Ctl_Data.HeartSendTime=0;
			char str[50];
			sprintf(str, "%d",(int)Ctl_Data.HeartSendTime);
			Debug_SendMsg("Data_processing_Rec_heart/HeartSendTime=",str, DEBUG_MSG_INFO);
				break;
			case game_begins:								//收到游戏开始信息
				memcpy(RX_Buf,&usart2.Rx_Buf[0],usart2.Rx_Buf[2]);
				Reply_message(game_begins);
			
				OSFlagPost(&Ctl_Data.Flag,Game_Begins,OS_OPT_POST_FLAG_SET,&err);
			Debug_SendMsg("Data_processing","game_begins", DEBUG_MSG_INFO);
				break;
			case game_over:									//收到游戏结束回复
				OSFlagPost(&Ctl_Data.Flag,GameOver_Answer_flag,OS_OPT_POST_FLAG_SET,&err);
			Debug_SendMsg("Data_processing","game_over", DEBUG_MSG_INFO);
				break;
			case query_version:								//收到版本查询回复
				OSFlagPost(&SIM900A_Flag,Version_flag,OS_OPT_POST_FLAG_SET,&err);
				break;
			case File_transfer:
				OSFlagPost(&SIM900A_Flag,File_transfer_flag,OS_OPT_POST_FLAG_SET,&err);
				break;
			default:
				/*其它命名交外部处理*/
				OSFlagPost(&SIM900A_Flag,CMD_flag,OS_OPT_POST_FLAG_SET,&err);
				break;
		}
	}
	else													//其它信息交外部处理
	{
		OSFlagPost(&SIM900A_Flag,CMD_flag,OS_OPT_POST_FLAG_SET,&err);
	}
}

uint8_t BBC_Check(char *p,uint16_t size)
{
	uint8_t out;//用于保存异或结果 
	uint16_t i=0;
	out=*p;
	p++;
	for (i=0;i<size-1;i++) 
	{
		out^=*p;
		p++;
	} 
	return out;
}

/*
ftp链接
返回0：成功
返回1：失败
*/
static uint8_t ftp_message(u8 *message,u8 response,u8 length)
{
	OS_ERR err;
	u8 ftp_frame_head[3]={0x55,0xFC,0xAA};
	u8 i=0;
//	length=sizeof(message);
	
	TX_Buf[0] = 0x55;
	TX_Buf[1] = 0xFC;
	TX_Buf[2] = 0xAA;
	
	TX_Buf[3] = 0x00;
	TX_Buf[4] = length+4;
	TX_Buf[5] = 0x01;
	
	
	for(i=0;i<length;i++)
	{
		TX_Buf[i+6]=*message;
		message++;
	}
	
	TX_Buf[length+6]=BBC_Check(TX_Buf,length+6);
	Send_Data(TX_Buf,length+7);

	if(response != NULL)
	{
		/* 等待接收到响应数据 */
		OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
				 (OS_FLAGS	 )File_transfer_flag,
				 (OS_TICK		 )10000,
				 (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
				 (CPU_TS		 )0,
				 (OS_ERR		*)&err);
		
		
		
		if(err == OS_ERR_NONE)
		{
			if(usart2.Rx_Buf[7]==response)
			{	
				return 1;
			}
				
			else
			{
				return 0;
			}
				
		}
		else
		{
			return 0;
		}
	}
	else
	{
		OSTimeDly(50,OS_OPT_TIME_DLY,&err);
		return 1;
	}
}

static uint8_t Ftp_link()
{
	OS_ERR err;
	char str[50];
	char src[4];
	u8 p_size[4];
	u8 i=0;
	
	
	u8 hl_bit[2]={0};
	u8 buf[4]={0};
	u8 usarttest[20]={0};
		
	union u82u32 cvter;

	
	
	while(ftp_message(FTP_ENTER_CMD,0x01,1)==0)
	{ 
		i++;
		if(i>3)
			return 0;
		Debug_SendMsg("Ftp_link","ENTER",DEBUG_MSG_INFO);
		OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
	}
	i=0;
	while(ftp_message(FTP_IPPORT_CMD,0x01,18)==0)
	{ i++;
		if(i>3)
			return 0;
		Debug_SendMsg("Ftp_link","IPPORT",DEBUG_MSG_INFO);
		OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
	}
	
	i=0;
	while(ftp_message(FTP_KEY_CMD,0x01,25)==0)
	{ i++;
		if(i>3)
			return 0;
		Debug_SendMsg("Ftp_link","KEY",DEBUG_MSG_INFO);
		OSTimeDly(500,OS_OPT_TIME_DLY,&err);
	}
	
	i=0;
	
	while(ftp_message(FTP_addr_CMD,0x01,11)==0)
	{ i++;
		if(i>3)
			return 0;
		Debug_SendMsg("Ftp_link","addr",DEBUG_MSG_INFO);
		OSTimeDly(500,OS_OPT_TIME_DLY,&err);
	}
	
//	ftp_message(FTP_ENTER_CMD,0x01,1);
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);
//	ftp_message(FTP_IPPORT_CMD,0x01,18);
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);
//	ftp_message(FTP_KEY_CMD,0x01,25);
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);

//	if(ftp_message(FTP_addr_CMD,01,11)==1)//地址设置成功则返回升级文件的总字节数
//	{
		
		Play_List(35);
		OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
		HexToStr(usarttest,usart2.Rx_Buf,20);
		Debug_SendMsg("BBCusart2",usarttest,DEBUG_MSG_INFO);
		
		src[0]=usart2.Rx_Buf[8];
		src[1]=usart2.Rx_Buf[9];
		src[2]=usart2.Rx_Buf[10];
		src[3]=usart2.Rx_Buf[11];
		HexToStr(str,src,4);
		Debug_SendMsg("BBC1",str,DEBUG_MSG_INFO);
		
		cvter.c[0] = usart2.Rx_Buf[11];
		cvter.c[1] = usart2.Rx_Buf[10];
		cvter.c[2] = usart2.Rx_Buf[9];
		cvter.c[3] = usart2.Rx_Buf[8];

		update.package_size=cvter.len;
		return 1;
//	}
//	else
//	{
//		return 0;
//	}

}

/*
// C prototype : void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - 存放目标字符串
//	[IN] pbSrc - 输入16进制数的起始地址
//	[IN] nLen - 16进制数的字节数
// return value: 
// remarks : 将16进制数转化为字符串
*/
void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen)
{
char	ddl,ddh;
int i;

for (i=0; i<nLen; i++)
{
ddh = 48 + pbSrc[i] / 16;
ddl = 48 + pbSrc[i] % 16;
if (ddh > 57) ddh = ddh + 7;
if (ddl > 57) ddl = ddl + 7;
pbDest[i*2] = ddh;
pbDest[i*2+1] = ddl;
}

pbDest[nLen*2] = '\0';
}
/*
// C prototype : void StrToHex(BYTE *pbDest, BYTE *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - 输出缓冲区
//	[IN] pbSrc - 字符串
//	[IN] nLen - 16进制数的字节数(字符串的长度/2)
// return value: 
// remarks : 将字符串转化为16进制数
*/
void ftp_StrToHex(BYTE *pbDest, BYTE *pbSrc, int nLen)
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
