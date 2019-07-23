#ifndef __DISPLAY__H
#define __DISPLAY__H

#include "stm32f10x.h"

#define LED_Size		16				//一个字点阵大小 16*16,必须为8的倍数
#define Display_Number	4				//实际显示的汉字个数
#define Number_Max		100				//最多显示汉字的个数

typedef struct
{
	u16		List;						//实际显示区域总列数
	u16		List_start;					//上一个字符起始行
	u16 	List_End;					//上一个字符在绘图层的结束行
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
