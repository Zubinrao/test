#include "Lookup.h"
#include <string.h>
#include <stdio.h>

/*
*忽略大小写的字符串查找函数
*/
const char* strcasestr(const char* str, const char* subStr)
{
    int len = strlen(subStr);
    if(len == 0)
    {
        return NULL;
    }

    while(*str)
    {
        if(strncasecmp(str, subStr, len) == 0)
        {
            return str;
        }
        str++;
    }
    return NULL;
}

/*
功能描述：查找指定目录下的指定后缀名文件数量
函数参数：Filedir - 指定目录(如：0:/xxx/xxx)
		  Filetype - 指定后缀名(如：mp3，txt，bmp，后缀名不区分大小写)
		  Number - 指向查找到的文件数量的变量
*/
char FatReadDir(const TCHAR* Filedir,const TCHAR* Filetype,UINT* Number)
{
	char		buf[10];
	u16 		i;
	DIR 		dir;
	FIL 		file;
	FILINFO 	fno;
	
	sprintf(buf,".%s",Filetype);
//	sprintf(fno.fname,"%s/%slist.txt",Filedir,Filetype);
//	res_sd=f_open(&file,fno.fname,FA_WRITE|FA_CREATE_ALWAYS);
	
	res_sd=f_opendir(&dir,Filedir);
	if(res_sd == FR_OK)
	{
		while(f_readdir(&dir, &fno) == FR_OK)
		{
			if(!fno.fname[0])
				break;								//文件读取完成
			
			if(strcasestr(fno.fname,buf) != NULL)
			{
				i = strlen(fno.fname);
				
				(*Number)++;
				if((*Number) >= 0xFFFF) 
					break;
			}
		}
	}
	else
	{
		return res_sd;
	}
	f_close(&file);
	
	return 0;
}
