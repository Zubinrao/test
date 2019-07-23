#include "WavPlay.h"
#include "Control.h"
#include "stdio.h"
#include "bsp.h"
#include <string.h>

extern OS_TCB    Music_Play_CtrTCB;  		//���ֲ��������ջ

FATFS 		fs;
FRESULT 	res_sd;

FIL			file;
UINT		rw;

static uint8_t  	buffer[2][4096];		//�ļ�������
static uint8_t 		bufflag=0;				//�����л���־

WavHead 		wav;
Play_Control	Wav_Play;

/** @brief  ���Դ�Сд���ַ������Һ���
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

/** @brief  �����ƶ�Ŀ¼�µ�ָ����׺���ļ�
  * @param  Filedir - ָ��Ŀ¼(�磺0:/xxx/xxx)
  *	 		Filetype - ָ����׺��(�磺mp3��txt��bmp����׺�������ִ�Сд)
  *			Number - ָ����ҵ����ļ������ı���ָ��
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

/** @brief  ����SD����wav���͸���
  * @param  None
  * @retval None
  */
void WAV_Play(void)
{
	OS_ERR err;
	
	Wav_Play.State_flag |= Playing_flag;					//������ڲ���
	res_sd = f_open(&file,Wav_Play.Play_name,FA_READ);		//���ļ�
	if(res_sd != FR_OK)
	{
		Wav_Play.State_flag &= ~Playing_flag;
		OSFlagPost(&Wav_Play.Flag,Play_End_flag,OS_OPT_POST_FLAG_SET,&err);
		return;
	}
	
	res_sd = f_read(&file,buffer,60,&rw);					//��ȡ�ļ�ͷ
	memcpy(&wav,buffer,sizeof(wav));						//�����ʽ
	
	if(wav.dwSamplesPerSec >= I2S_AudioFreq_Default)
	{
		I2S_Freq_Config(wav.dwSamplesPerSec);				//���ò�����
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
			
			if(Wav_Play.State_flag & PlaySuspend_flag)		//��ͣ����
			{
				OSTaskSuspend(DEF_NULL,&err);
			}
			if((file.fptr==file.obj.objsize) || ((Wav_Play.State_flag & Playing_flag) != Playing_flag))
			{
				f_close(&file);
				I2S_Stop();
				break;										//�˳�����
			}
		}
		else
		{
			f_close(&file);
			I2S_Stop();
			break;
		}
	}
	Wav_Play.State_flag &= ~Playing_flag;					//���ֹͣ����
	OSFlagPost(&Wav_Play.Flag,Play_End_flag,OS_OPT_POST_FLAG_SET,&err);
}

/** @brief  �ȴ����Ž���
  * @param  wait_time - �ȴ���ʱʱ��(ms)
  *			��� wait_time = 0, ��һֱ�ȴ�
  * @retval 1 - ��������
  *         0 - ��ʱ
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

/** @brief  ��ʼ����
  * @param  p - �����ļ������ƺ�·��
  *         p = DEF_NULL ȡ���ݶ�����
  * @retval None
  */
void Start_Play(char *p)
{
	OS_ERR err;
	
	if(p != DEF_NULL)
	{
		sprintf(Wav_Play.Play_name,"%s/%s",Directory_path,p);		//���潫Ҫ���ŵ��ļ�·���Լ�����
		
		if(Wav_Play.State_flag & Playing_flag)						//�ж�ǰһ���Ƿ��ڲ���
		{
			if(Wav_Play.State_flag & PlaySuspend_flag)				//���ǰһ�״�����ͣ����״̬
			{
				Wav_Play.State_flag &= ~PlaySuspend_flag;			//ȡ����ͣ״̬
				OSTaskResume(&Music_Play_CtrTCB,&err);				//�ָ�����
			}
			
			Wav_Play.State_flag &= ~Playing_flag;					//ֹͣ����
			OSFlagPost(&Wav_Play.Flag,DMA1_5_flag,OS_OPT_POST_FLAG_SET,&err);
			
			/* �ȴ�����ֹͣ */
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

/** @brief  ��ͣ����
  * @param  None
  * @retval None
  */
void Suspend_Play(void)
{
	if(Wav_Play.State_flag & Playing_flag)			//ȷ���Ƿ��ڲ�����
	{
		Wav_Play.State_flag |= PlaySuspend_flag;	//�����ͣ
	}
}

/** @brief  ֹͣ����
  * @param  flag
  *         0 - ���õȵ�ֹͣ
  *         1 - �ȵ�ֹͣ���˳�(����ʹ��)
  * @retval None
  */
void Stop_Play(u8 flag)
{
	OS_ERR err;
	
	if(Wav_Play.State_flag & Playing_flag)						//�ж�ǰһ���Ƿ��ڲ���
	{
		if(Wav_Play.State_flag & PlaySuspend_flag)				//���ǰһ�״�����ͣ����״̬
		{
			Wav_Play.State_flag &= ~PlaySuspend_flag;			//ȡ����ͣ״̬
			OSTaskResume(&Music_Play_CtrTCB,&err);				//�ָ�����
		}
		Wav_Play.State_flag &= ~Playing_flag;					//ֹͣ����
		if(flag == 1)
		{
			WaitPlay_End(0);
		}
	}
}

/** @brief  ����Ƿ񲥷��Ƿ����
  * @param  None
  * @retval 1 - ����
  *         0 - ���ڲ���
  */
u8 Detection_idle(void)
{
	if(Wav_Play.State_flag & Playing_flag)
	{
		return 0;
	}
	
	return 1;
}

/** @brief  ���Ÿ����б��е��ļ�
  * @param  list - �ڸ����ļ����е����
  *			list = [0 - 24]		��Ӧ�ĸ����ļ�
  *			     = [30 - 39]	
  *				 = [40 - 59]	
  *              = [60 - 79]	
  *              = [80 - 99]	��Ϸѡ����ʾ����
  *              = [100 - 119]	δʹ��
  *              = [120 - 129]	δʹ��
	*				 = 129      У׼����
  *				 = 252			��Ϸ������Ч
  *				 = 253			�ɷ���ʾ��
  *				 = 254			��ʼ������ʾ
  *              = 255			����������ʾ
  * @retval None
  */
void Play_List(uint8_t list)
{
	u16		Indexes;			//�����б�����
	char	position;			//�ַ��ڶ�Ӧ�����б��г��ֵ�λ��
	char 	str_buf[10];		//�ַ���������
	
	for(Indexes=0;Indexes<Wav_Play.Music_Number;Indexes++)
	{
		/* ���Ҷ�Ӧ�ļ��� */
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
