#include "stm32f1xx.h"
#include "bsp.h"
#include "Font.h"
#include "ff.h"

__EXRAM FATFS		fs;
__EXRAM FIL			file;
__EXRAM UINT		bw,br;
__EXRAM FRESULT		res_sd;

char Write_Buf[]="һȥ�����\r\n�̴�����ҡ�\r\n̨ͤ��������\r\n�˾�ʮ֦����";
char Font_Buf[16][2];

void r_w_test(void)
{
	res_sd = f_mount(&fs,"1:",1);						//����SD��
	
	/*�½�һ��text.txt�ļ����Զ�дģʽ�� text.txt �ļ�*/
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

/*���������ڴ�ӡ���� 16*16�����ʽ
 *���룺p-ָ���������ݵ�ָ��
		�������ݸ�ʽҪ��
		1.����Ϊ1
		2.˳�򣬸�λ��ǰ
		3.����ʽ
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

/*���������ڴ�ӡASCLL���� 8*16�����ʽ
 *���룺p-ָ���������ݵ�ָ��
		�������ݸ�ʽҪ��
		1.����Ϊ1
		2.˳�򣬸�λ��ǰ
		3.����ʽ
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
