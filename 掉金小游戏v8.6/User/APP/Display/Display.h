#ifndef __DISPLAY__H
#define __DISPLAY__H

#include "stm32f10x.h"

#define LED_Size		16				//һ���ֵ����С 16*16,����Ϊ8�ı���
#define Display_Number	4				//ʵ����ʾ�ĺ��ָ���
#define Number_Max		100				//�����ʾ���ֵĸ���

typedef struct
{
	u16		List;						//ʵ����ʾ����������
	u16		List_start;					//��һ���ַ���ʼ��
	u16 	List_End;					//��һ���ַ��ڻ�ͼ��Ľ�����
}Dispaly;

extern Dispaly Dy_Ctl;

void Font8x16_test(char *str);

static void Reset_RAMBuf(void);
static void Update_Line(u8 line);
void Update_Display(void);
static void Update_RAMBuf(const u8 flag, const char *str);
void Update_DRAM(u16 List);
void DispStrCH(char *str);

#endif
