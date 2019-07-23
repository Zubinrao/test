#ifndef __UPDATE__H
#define __UPDATE__H

#include "stm32f10x.h"

typedef struct
{
	u16			package;				//���ڴ���ڼ���
	u16			file_size;				//��Ч���ݴ�С
	u32 		package_size ;//�����ļ��ܴ�С
	u8			end_flag;				//������־ 0-�������� 1-�������
	
}file_transfer;

//extern u8 updata_stata;
static void Request_packets(u16 package);
void Program_download(void);
void Data_verification(void);

extern	file_transfer	update;

#endif
