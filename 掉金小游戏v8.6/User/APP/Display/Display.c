#include "display.h"
#include "in_output.h"
#include "Control.h"
#include "Font.h"
#include "ff.h"
#include "os.h"
#include <string.h>

static FRESULT		res;
static FIL			file;
static UINT			br;

Dispaly Dy_Ctl;

char 		Font_Buf[32];																	//�����������ݻ�����
char 		D_RAM[LED_Size][Display_Number*(LED_Size/8)];									//��ʾ��
char		RAM_Buf[LED_Size][LED_Size/8*Number_Max+Display_Number*(LED_Size/8)*2];			//��ͼ��
OS_MUTEX    Display_mutex;

/*��������λ��ͼ���Դ�
 *���룺
 *�����
*/
static void Reset_RAMBuf(void)
{
	Dy_Ctl.List = LED_Size*Display_Number;
	Dy_Ctl.List_start = Dy_Ctl.List;
	Dy_Ctl.List_End = Dy_Ctl.List_start;
	memset(RAM_Buf,0,sizeof(RAM_Buf));
}

/*������������
 *���룺line - �ڼ���
 *�����void
*/
static void Update_Line(u8 line)
{
	u16 L=0x0001 << line;
	u8 i=0;
	
	CLKC_L;
	RCLK_L;
	if(line != 0xff)
	{
		for(i=0;i<16;i++)
		{
			CLKR_L;
			DI(L & (0x8000 >> i));
			CLKR_H;
		}
	}
	else
	{
		for(i=0;i<16;i++)
		{
			CLKR_L;
			DI(0 & (0x8000 >> i));
			CLKR_H;
		}
	}
	RCLK_H;
	RCLK_L;
}

/*���������µ�����ʾ����
 *���룺void
 *�����void
*/
void Update_Display(void)
{
	OS_ERR err;
	u16 i,j,l;
	
	CLKR_L;
	RCLK_L;
	for(l=0;l<LED_Size;l++)
	{
		for(i=0;i<(Display_Number*(LED_Size/8));i++)
		{
			for(j=0;j<8;j++)
			{
				CLKC_L;
				if(D_RAM[l][i] & (0x80 >> j))
					DI(0);
				else
					DI(1);
				CLKC_H;
			}
		}
		RCLK_H;
		RCLK_L;
		
		Update_Line(l);
		OSTimeDly(1,OS_OPT_TIME_DLY,&err);
	}
}

/*�����������Դ��Ӧ������
 *���룺List - �ڻ�ͼ�����ʼ��
 *�����void
*/
void Update_DRAM(u16 List)
{
	u16 i,j;
	u16 List_H = List/8;
	u16 List_L = List%8;
	char Buf;
	
	for(i=0;i<LED_Size;i++)
	{
		for(j=0;j<Display_Number*(LED_Size/8);j++)
		{
			Buf = RAM_Buf[i][List_H+j] << List_L;
			Buf |= RAM_Buf[i][List_H+j+1] >> (8-List_L);
			D_RAM[i][j] = Buf;
		}
	}
}

/*���������»�ͼ������
 *���룺flag = 1:��������Ϊ˫�ַ�(GB2312)
			 = 0:��������Ϊ���ַ�(ASCLL)
		str - ָ���ַ���ָ��
 *�����void
*/
static void Update_RAMBuf(const u8 flag, const char *str)
{
	u16 i;
	
	for(i=0;i<LED_Size;i++)
	{
		if(flag)
			memcpy(&RAM_Buf[i][Dy_Ctl.List_End/8],str+i*2,2);
		else
			RAM_Buf[i][Dy_Ctl.List_End/8] = ASCLL_Font[*str-32][i];
	}
	if(flag)
		Dy_Ctl.List_End += LED_Size;
	else
		Dy_Ctl.List_End += 8;
}

/*�������ڻ�ͼ��������ʾ���ַ���
 *���룺str - �ַ���ָ��
 *�����void
*/
void DispStrCH(char *str)
{
	OS_ERR err;
	u16	i;
	unsigned int pos;
	u8 Code_H,Code_L;
	
	OSMutexPend ((OS_MUTEX  *)&Display_mutex,
				 (OS_TICK    )0,
				 (OS_OPT     )OS_OPT_PEND_BLOCKING,
				 (CPU_TS    *)0,
				 (OS_ERR    *)&err);
	
	i=0;
	do
	{
		res = f_open(&file,"1:/font/GB2312_16x16.FON",FA_READ);
		i++;
	}while((res != FR_OK) || (i >= 100));
	
	Reset_RAMBuf();
	while(*str != '\0')
	{
		if(*str >= 127)
		{
			Code_H = *str;
			Code_L = *(str+1);
			pos=((Code_H-0xa1)*94+Code_L-0xa1)*32;
			
			i=0;
			do
			{
				res = f_lseek(&file,pos);
				res = f_read(&file,Font_Buf,32,&br);
				i++;
			}while((res != FR_OK) || (i >= 100));
			Update_RAMBuf(1,(char*)Font_Buf);
			str += 2;
		}
		else
		{
			Update_RAMBuf(0,str);
			str++;
		}
	}
	f_close(&file);
	
	OSMutexPost ((OS_MUTEX  *)&Display_mutex,
				 (OS_OPT     )OS_OPT_POST_NONE,
				 (OS_ERR    *)&err);
}
