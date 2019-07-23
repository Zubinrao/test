#include "WavPlay.h"
#include "Control.h"
#include "stdio.h"
#include "bsp.h"
#include <string.h>

extern OS_TCB    Music_Play_CtrTCB;  		//音乐播放任务堆栈

FATFS 		fs;
FRESULT 	res_sd;

FIL			file;
UINT		rw;

static uint8_t  	buffer[2][4096];		//文件缓存区
static uint8_t 		bufflag=0;				//缓存切换标志

WavHead 		wav;
Play_Control	Wav_Play;

/** @brief  忽略大小写的字符串查找函数
  * @param  None
  * @retval None
  */
static const char* strcasestr(const char* str, const char* subStr)
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

/** @brief  遍历制定目录下的指定后缀名文件
  * @param  Filedir - 指定目录(如：0:/xxx/xxx)
  *	 		Filetype - 指定后缀名(如：mp3，txt，bmp，后缀名不区分大小写)
  *			Number - 指向查找到的文件数量的变量指针
  * @retval None
  */
void FatReadDir(const TCHAR* Filedir,const TCHAR* Filetype)
{
	char		buf[10];
	u16			i=0;
	DIR 		dir;
	FIL 		file;
	FILINFO 	fno;
	
	Wav_Play.Music_Number = 0;
	sprintf(buf,".%s",Filetype);
	
	res_sd=f_opendir(&dir,Filedir);
	if(res_sd == FR_OK)
	{
		while(f_readdir(&dir, &fno) == FR_OK)
		{
			if(!fno.fname[0])
				break;
			
			if(strcasestr(fno.fname,buf) != NULL)
			{
				i = strlen(fno.fname);
				if(i <= Name_Max)
				{
					memcpy(&Wav_Play.Music_List[Wav_Play.Music_Number][0],fno.fname,i);
					
					Wav_Play.Music_Number++;
					if(Wav_Play.Music_Number >= List_Max) 
						break;
				}
			}
		}
	}
	f_close(&file);
}

/** @brief  播放SD卡中wav类型歌曲
  * @param  None
  * @retval None
  */
void WAV_Play(void)
{
	OS_ERR err;
	
	Wav_Play.State_flag |= Playing_flag;					//标记正在播放
	res_sd = f_open(&file,Wav_Play.Play_name,FA_READ);		//打开文件
	if(res_sd != FR_OK)
	{
		Wav_Play.State_flag &= ~Playing_flag;
		OSFlagPost(&Wav_Play.Flag,Play_End_flag,OS_OPT_POST_FLAG_SET,&err);
		return;
	}
	
	res_sd = f_read(&file,buffer,60,&rw);					//读取文件头
	memcpy(&wav,buffer,sizeof(wav));						//整理格式
	
	if(wav.dwSamplesPerSec >= I2S_AudioFreq_Default)
	{
		I2S_Freq_Config(wav.dwSamplesPerSec);				//设置采样率
	}
	else
	{
		Wav_Play.State_flag &= ~Playing_flag;
		OSFlagPost(&Wav_Play.Flag,Play_End_flag,OS_OPT_POST_FLAG_SET,&err);
		return;
	}
	
	OSFlagPost(&Wav_Play.Flag,DMA1_5_flag,OS_OPT_POST_FLAG_SET,&err);
	while(DEF_TRUE)
	{
		res_sd = f_read(&file,&buffer[bufflag][0],sizeof(buffer)/2, &rw);
		if(res_sd == SD_OK)
		{
			OSFlagPend(&Wav_Play.Flag,DMA1_5_flag,0,OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,0,&err);
			DMA_I2S_Send_Data((u32)&buffer[bufflag][0], rw/2);
			bufflag = 1 -bufflag;
			
			if(Wav_Play.State_flag & PlaySuspend_flag)		//暂停播放
			{
				OSTaskSuspend(DEF_NULL,&err);
			}
			if((file.fptr==file.obj.objsize) || ((Wav_Play.State_flag & Playing_flag) != Playing_flag))
			{
				f_close(&file);
				I2S_Stop();
				break;										//退出播放
			}
		}
		else
		{
			f_close(&file);
			I2S_Stop();
			break;
		}
	}
	Wav_Play.State_flag &= ~Playing_flag;					//标记停止播放
	OSFlagPost(&Wav_Play.Flag,Play_End_flag,OS_OPT_POST_FLAG_SET,&err);
}

/** @brief  等待播放结束
  * @param  wait_time - 等待超时时间(ms)
  *			如果 wait_time = 0, 则一直等待
  * @retval 1 - 正常结束
  *         0 - 超时
  */
u8 WaitPlay_End(u16 wait_time)
{
	OS_ERR err;
	
	OSFlagPend((OS_FLAG_GRP	*)&Wav_Play.Flag,
			   (OS_FLAGS	 )Play_End_flag,
			   (OS_TICK		 )wait_time,
			   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND,
			   (CPU_TS		 )0,
			   (OS_ERR		*)&err);
	
	if(err == OS_ERR_NONE)
		return 1;
	else
		return 0;
}

/** @brief  开始播放
  * @param  p - 播放文件的名称和路径
  *         p = DEF_NULL 取消暂定播放
  * @retval None
  */
void Start_Play(char *p)
{
	OS_ERR err;
	
	if(p != DEF_NULL)
	{
		sprintf(Wav_Play.Play_name,"%s/%s",Directory_path,p);		//保存将要播放的文件路径以及名称
		
		if(Wav_Play.State_flag & Playing_flag)						//判断前一首是否在播放
		{
			if(Wav_Play.State_flag & PlaySuspend_flag)				//如果前一首处于暂停播放状态
			{
				Wav_Play.State_flag &= ~PlaySuspend_flag;			//取消暂停状态
				OSTaskResume(&Music_Play_CtrTCB,&err);				//恢复任务
			}
			
			Wav_Play.State_flag &= ~Playing_flag;					//停止播放
			OSFlagPost(&Wav_Play.Flag,DMA1_5_flag,OS_OPT_POST_FLAG_SET,&err);
			
			/* 等待播放停止 */
			OSFlagPend((OS_FLAG_GRP	*)&Wav_Play.Flag,
					   (OS_FLAGS	 )Play_End_flag,
					   (OS_TICK		 )0,
					   (OS_OPT	 	 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
					   (CPU_TS		 )0,
					   (OS_ERR		*)&err);
		}
		
		OSFlagPost(&Wav_Play.Flag,Play_End_flag,OS_OPT_POST_FLAG_CLR,&err);
		OSTaskSemPost(&Music_Play_CtrTCB,OS_OPT_POST_NO_SCHED,&err);
	}
	else
	{
		if(Wav_Play.State_flag & Playing_flag)
		{
			if(Wav_Play.State_flag & PlaySuspend_flag)
			{
				Wav_Play.State_flag &= ~PlaySuspend_flag;
				OSTaskResume(&Music_Play_CtrTCB,&err);
			}
		}
	}
}

/** @brief  暂停播放
  * @param  None
  * @retval None
  */
void Suspend_Play(void)
{
	if(Wav_Play.State_flag & Playing_flag)			//确认是否在播放中
	{
		Wav_Play.State_flag |= PlaySuspend_flag;	//标记暂停
	}
}

/** @brief  停止播放
  * @param  flag
  *         0 - 不用等到停止
  *         1 - 等到停止才退出(谨慎使用)
  * @retval None
  */
void Stop_Play(u8 flag)
{
	OS_ERR err;
	
	if(Wav_Play.State_flag & Playing_flag)						//判断前一首是否在播放
	{
		if(Wav_Play.State_flag & PlaySuspend_flag)				//如果前一首处于暂停播放状态
		{
			Wav_Play.State_flag &= ~PlaySuspend_flag;			//取消暂停状态
			OSTaskResume(&Music_Play_CtrTCB,&err);				//恢复任务
		}
		Wav_Play.State_flag &= ~Playing_flag;					//停止播放
		if(flag == 1)
		{
			WaitPlay_End(0);
		}
	}
}

/** @brief  检测是否播放是否空闲
  * @param  None
  * @retval 1 - 空闲
  *         0 - 正在播放
  */
u8 Detection_idle(void)
{
	if(Wav_Play.State_flag & Playing_flag)
	{
		return 0;
	}
	
	return 1;
}

/** @brief  播放歌曲列表中的文件
  * @param  list - 在歌曲文件名中的序号
  *			list = [0 - 24]		对应的歌曲文件
  *			     = [30 - 39]	
  *				 = [40 - 59]	
  *              = [60 - 79]	
  *              = [80 - 99]	游戏选择提示语音
  *              = [100 - 119]	未使用
  *              = [120 - 129]	未使用
	*				 = 129      校准语音
  *				 = 252			游戏结束音效
  *				 = 253			缴费提示音
  *				 = 254			开始语音提示
  *              = 255			结束语音提示
  * @retval None
  */
void Play_List(uint8_t list)
{
	u16		Indexes;			//歌曲列表索引
	char	position;			//字符在对应歌曲列表中出现的位置
	char 	str_buf[10];		//字符串缓冲区
	
	for(Indexes=0;Indexes<Wav_Play.Music_Number;Indexes++)
	{
		/* 查找对应文件名 */
		for(position=0;position<Name_Max;position++)
		{
			if(Wav_Play.Music_List[Indexes][position] == '-')
			{
				memcpy(str_buf,&Wav_Play.Music_List[Indexes][0],position);
				str_buf[position] = ' ';
				if(atoi(str_buf) == list)
				{
					Wav_Play.Play_Number = list;
					Start_Play(&Wav_Play.Music_List[Indexes][0]);
					return;
				}
			}
		}
	}
}
