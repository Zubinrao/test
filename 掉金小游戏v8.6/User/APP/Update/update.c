#include "update.h"
#include "GPRS.h"
#include "Control.h"
#include "NVIC.h"
#include "os.h"
#include "ff.h"
#include "usart.h"
//#include "ioi2c.h"
#include "WavPlay.h"
#include "debug.h"

file_transfer	update;
OS_FLAG_GRP		Update_Flag;			//事件标志
static char 	TX_Buf[300];			//发送数据缓冲区
static FIL 		fnew;

static FRESULT 	res;
static UINT 	bw;
u8 updata_stata=1;
//static void Request_packets(u16 package)
//{
//	u8 Test_Buf[13]={0}; 
//	
//	TX_Buf[0] = 0x55;
//	TX_Buf[1] = 0xFC;
//	TX_Buf[2] = 0xAA;
//	TX_Buf[3] = 0x00;
//	TX_Buf[4] = 0x09;
//	TX_Buf[5] = 0x01;
//	TX_Buf[6] = 0xA4;
//	TX_Buf[7] = 0x01;
//	TX_Buf[8] = 0x00;
//	
//	TX_Buf[9] = package>>8;
//	TX_Buf[10] = package;
////	TX_Buf[9] = 0x00;
////	TX_Buf[10] = 0x01;
////	TX_Buf[11] =0xAF;
//	TX_Buf[11] = BBC_Check(TX_Buf,11);
//	Send_Data(TX_Buf,12);
//	
////	HexToStr(Test_Buf,TX_Buf,12);
////	Debug_SendMsg("Request_packets",Test_Buf,DEBUG_MSG_INFO);
//}

static void Request_packets(u16 package)
{
	TX_Buf[0] = frame_head;
	TX_Buf[1] = 0x90;
	TX_Buf[2] = 0x0B;
	TX_Buf[3] = Ctl_Data.Class >> 8;
	TX_Buf[4] = Ctl_Data.Class;
	TX_Buf[5] = Ctl_Data.ID >> 8;
	TX_Buf[6] = Ctl_Data.ID;
	TX_Buf[7] = package >> 8;
	TX_Buf[8] = package;
	TX_Buf[9] = And_Check(TX_Buf,9);
	TX_Buf[10] = frame_tail;
	Send_Data(TX_Buf,11);
}

void Program_download(void)
{
	OS_ERR err;
	u8	i=0;
	CPU_SR_ALLOC();
	res = f_open(&fnew, "1:/update/update.bin.download", FA_CREATE_ALWAYS | FA_WRITE );
	res = f_sync(&fnew);
	if ( res == FR_OK )
	{
		update.package = 0;
		update.end_flag = 0;
		while(update.end_flag == 0)
		{	
			Request_packets(update.package);
			OSFlagPend((OS_FLAG_GRP	*)&Update_Flag,
					   (OS_FLAGS	 )0x0001,
					   (OS_TICK		 )2000,
					   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
					   (CPU_TS		 )0,
					   (OS_ERR		*)&err);
			if(err == OS_ERR_NONE)
			{ 
					OS_CRITICAL_ENTER();//进入临界区	
					res = f_write(&fnew, &usart2.Rx_Buf[5],update.file_size, &bw);			
					res = f_sync(&fnew);
					OS_CRITICAL_EXIT();	//退出临界区
					if(res == FR_OK)
					{
						update.package++;
						i=0;
					}
			}
			else
			{
				if(i >= 6)
				{
					
					Debug_SendMsg("Program_download","SoftReset", DEBUG_MSG_INFO);
					SoftReset();
				}
				i++;
				OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
			}
		}
		res = f_close(&fnew);
		res = f_rename("1:/update/update.bin.download","1:/update/update.bin");
		Ctl_Data.updata_state=0;
	}
	else
	{
		Debug_SendMsg("Program_download_SDOpenfail","SoftReset", DEBUG_MSG_INFO);
		SoftReset();
	}
}

//void Program_download(void)
//{
//	OS_ERR err;
//	
//	CPU_SR_ALLOC();
//	static u8 	FTP_Exit_CMD[8]={0x55, 0xFC, 0xAA, 0x00, 0x05, 0x01, 0xA0, 0xA7};
//	u8	i=0;
//	u16 package_number=0;
//	package_number=ceil(update.package_size/256);//分包数量
//	update.package = 1;
//	update.end_flag = 0;
//	
//	Debug_SendMsg("Program_download","download.....",DEBUG_MSG_INFO);
//	res = f_open(&fnew, "1:/update/update.bin.download", FA_CREATE_ALWAYS | FA_WRITE );
//	res = f_sync(&fnew);
//		
//	while(update.end_flag==0)
//		{

//			Request_packets(update.package);
//			Debug_SendMsg("Program_download","package++",DEBUG_MSG_INFO);
//			
//			OSFlagPend((OS_FLAG_GRP	*)&Update_Flag,
//					   (OS_FLAGS	 )0x0001,
//					   (OS_TICK		 )5000,
//					   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
//					   (CPU_TS		 )0,
//					   (OS_ERR		*)&err);
//			
//			if(err == OS_ERR_NONE)
//			{ 
//				Debug_SendMsg("Program_download","no error",DEBUG_MSG_INFO);
////				
////				OS_CRITICAL_ENTER();//进入临界区	
////					res = f_write(&fnew, &usart2.Rx_Buf[5],update.file_size, &bw);
////					res = f_sync(&fnew);
//				
////				OS_CRITICAL_EXIT();	//退出临界区

//				
//			if((usart2.Rx_Buf[8]==usart2.Rx_Buf[10])&&(usart2.Rx_Buf[9]==usart2.Rx_Buf[11])&&(usart2.Rx_Buf[0]=0x55))
//				{
//					Debug_SendMsg("Data_verification","end_flag",DEBUG_MSG_INFO);
//					res = f_close(&fnew);
//					res = f_rename("1:/update/update.bin.download","1:/update/update.bin");
//					break;
//				}
//				
//					usart2.Rx_Buf[4]=0;
//					usart2.Rx_Buf[6] =0; 
//					usart2.Rx_Buf[7] =0;
//						
//				if(res == FR_OK)
//					{	
//						update.package++;	
//					}
////				update.package++;
//			}
//			else
//			{
//			
//				Debug_SendMsg("Program_download","error",DEBUG_MSG_INFO);
//			}
////			if(update.end_flag ==0)
////				update.file_size=256;
////			else
////				update.file_size=update.package_size-(update.package-1)*256;
//			
//		
////		OSTimeDly(50,OS_OPT_TIME_DLY,&err);
//		}
//		
//		
//		Send_Data(FTP_Exit_CMD,7);
//		
//		
////		HexToStr();
//		Debug_SendMsg("Program_download","over",DEBUG_MSG_INFO);
//	
////	res=FR_OK;
////	if ( res == FR_OK )
////	{
////		
//////		while(update.end_flag == 0)
////		while(update.package !=package_number)
////		{	
////			Request_packets(update.package);
////			Debug_SendMsg("Program_download","package++",DEBUG_MSG_INFO);
////			
////			OSFlagPend((OS_FLAG_GRP	*)&Update_Flag,
////					   (OS_FLAGS	 )0x0001,
////					   (OS_TICK		 )10000,
////					   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
////					   (CPU_TS		 )0,
////					   (OS_ERR		*)&err);
////			
////			if(update.end_flag ==0)
////				update.file_size=256;
////			else
////				update.file_size=update.package_size-(update.package-1)*256;
////			
////			if(err == OS_ERR_NONE)
////			{ 
////				Debug_SendMsg("Program_download","4",DEBUG_MSG_INFO);
////					OS_CRITICAL_ENTER();//进入临界区	
////				Debug_SendMsg("Program_download","4",DEBUG_MSG_INFO);
////					res = f_write(&fnew, &usart2.Rx_Buf[5],update.file_size, &bw);
////					Debug_SendMsg("Program_download","4",DEBUG_MSG_INFO);
////					res = f_sync(&fnew);
////				Debug_SendMsg("Program_download","4",DEBUG_MSG_INFO);
////					OS_CRITICAL_EXIT();	//退出临界区
////				Debug_SendMsg("Program_download","1",DEBUG_MSG_INFO);
////					if(res == FR_OK)
////					{
////						Debug_SendMsg("Program_download","2",DEBUG_MSG_INFO);
////						update.package++;
////						i=0;
////						
////					}
////			}
////			else
////			{
////				
////				if(i >= 3)
////				{
////					Debug_SendMsg("Program_download","3",DEBUG_MSG_INFO);
////					SoftReset();
////				}
////				i++;
////				OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
////			}
////		}
////		res = f_close(&fnew);
////		res = f_rename("1:/update/update.bin.download","1:/update/update.bin");
////		Debug_SendMsg("Program_download","OK",DEBUG_MSG_INFO);
////		Ctl_Data.updata_state=0;
////	}
////	else
////	{
////		Debug_SendMsg("Program_download","5",DEBUG_MSG_INFO);
////		SoftReset();
////	}
//}

//void Data_verification(void)
//{
//	OS_ERR err;
//  u16 BBC_Check_Rx_Buf;
//	Debug_SendMsg("Data_verification","DATA",DEBUG_MSG_INFO);
//	if((usart2.Rx_Buf[4] == 0x0A) && (usart2.Rx_Buf[6] == 0xA4)&& (usart2.Rx_Buf[7] == 0x01))
//	{
//		Debug_SendMsg("Data_verification","OK",DEBUG_MSG_INFO);
//		
//		BBC_Check_Rx_Buf = (u16) usart2.Rx_Buf[usart2.Rx_Number-1];
//		
//		
//		if(BBC_Check(usart2.Rx_Buf,usart2.Rx_Number-1) == BBC_Check_Rx_Buf)
//		{
////			if(usart2.Rx_Buf[1] == date_transfer)
////			{
//////		
//			Debug_SendMsg("Data_verification","BBC_Check_OK",DEBUG_MSG_INFO);
////			if((usart2.Rx_Buf[8]==usart2.Rx_Buf[10])&&(usart2.Rx_Buf[9]==usart2.Rx_Buf[11]))
////			{
////				update.end_flag = 1;//文件传输标志位 0：还有下一包 1：传输完成
////			Debug_SendMsg("Data_verification","end_flag",DEBUG_MSG_INFO);
////			}
////				update.file_size = usart2.Rx_Buf[4] *256 + usart2.Rx_Buf[3];
//				OSFlagPost(&Update_Flag,0x0001,OS_OPT_POST_FLAG_SET,&err);
////			}
//		}
//	}
//}


void Data_verification(void)
{
	OS_ERR err;
  u16 And_Check_Rx_Buf;
	if((usart2.Rx_Buf[0] == frame_head) && (usart2.Rx_Buf[usart2.Rx_Number - 1] == frame_tail))
	{
		And_Check_Rx_Buf = (u16) usart2.Rx_Buf[usart2.Rx_Number-2];
		if(And_Check(usart2.Rx_Buf,usart2.Rx_Number-2) == And_Check_Rx_Buf)
		{
			if(usart2.Rx_Buf[1] == date_transfer)
			{
				update.end_flag = usart2.Rx_Buf[2];//文件传输标志位 0：还有下一包 1：传输完成
				update.file_size = usart2.Rx_Buf[4] *256 + usart2.Rx_Buf[3];
				OSFlagPost(&Update_Flag,0x0001,OS_OPT_POST_FLAG_SET,&err);
			}
		}
	}
}

