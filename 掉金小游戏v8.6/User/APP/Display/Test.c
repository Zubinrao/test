#include "stm32f1xx.h"
#include "bsp.h"
#include "Font.h"
#include "ff.h"

__EXRAM FATFS		fs;
__EXRAM FIL			file;
__EXRAM UINT		bw,br;
__EXRAM FRESULT		res_sd;

char Write_Buf[]="一去二三里，\r\n烟村四五家。\r\n亭台六七座，\r\n八九十枝花。";
char Font_Buf[16][2];

void r_w_test(void)
{
	res_sd = f_mount(&fs,"1:",1);						//挂载SD卡
	
	/*新建一个text.txt文件，以读写模式打开 text.txt 文件*/
	res_sd = f_open(&file,"1:text.txt",FA_READ|FA_WRITE|FA_CREATE_ALWAYS);
	res_sd = f_sync(&file);
	res_sd = f_write(&file,Write_Buf,sizeof(Write_Buf),&bw);
	res_sd = f_sync(&file);
	if(res_sd != FR_OK)
	{
		while(1);
	}
	f_close(&file);
}

/*描述：串口打印字体 16*16字体格式
 *输入：p-指向字体数据的指针
		字体数据格式要求：
		1.亮点为1
		2.顺向，高位在前
		3.逐行式
*/
void Font16x16_test(char *str)
{
	char i,j,k;
	unsigned int pos=((*str++-0xa1)*94+*str-0xa1)*32;
	
	res_sd = f_mount(&fs,"1:",1);
	res_sd = f_open(&file,"1:/GB2312_16x16.FON",FA_READ);
	res_sd = f_lseek(&file,pos);
	res_sd = f_read(&file,Font_Buf,32,&br);
	f_close(&file);
	
	for(i=0;i<16;i++)
	{
		for(k=0;k<2;k++)
		{
			for(j=1;j<=8;j++)
			{
				if(Font_Buf[i][k] & 1<<(8-j))
					printf("*");
				else
					printf(" ");
			}
		}
		printf("\n");
	}
}

/*描述：串口打印ASCLL字体 8*16字体格式
 *输入：p-指向字体数据的指针
		字体数据格式要求：
		1.亮点为1
		2.顺向，高位在前
		3.逐行式
*/
void Font8x16_test(char *str)
{
	char i,j;
	
	for(i=0;i<16;i++)
	{
		for(j=1;j<=8;j++)
		{
			if(ASCLL_Font[*str-32][i] & 1<<(8-j))
				printf("*");
			else
				printf(" ");
		}
		printf("\n");
	}
}
