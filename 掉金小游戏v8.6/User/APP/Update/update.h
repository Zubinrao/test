#ifndef __UPDATE__H
#define __UPDATE__H

#include "stm32f10x.h"

typedef struct
{
	u16			package;				//正在传输第几包
	u16			file_size;				//有效数据大小
	u32 		package_size ;//升级文件总大小
	u8			end_flag;				//结束标志 0-继续传输 1-传输结束
	
}file_transfer;

//extern u8 updata_stata;
static void Request_packets(u16 package);
void Program_download(void);
void Data_verification(void);

extern	file_transfer	update;

#endif
