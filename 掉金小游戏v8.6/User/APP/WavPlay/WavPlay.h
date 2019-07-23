#ifndef __WAVPLAY__H
#define __WAVPLAY__H

#include "stm32f10x.h"
#include "os.h"
#include "ff.h"

#define	Directory_path  	"1:/music"	//��Ÿ�����Ŀ¼·��
#define List_Max			255			//����ŵĸ����ļ�������
#define Name_Max			20			//һ�׸���������ֽ���

/* WAV�ļ�ͷ����ʽ */
typedef struct
{
	uint8_t		riff[4];					// = "RIFF"
	uint32_t	size_8;						// ���¸���ַ��ʼ���ļ�β�����ֽ��
	uint8_t		wave[4];					// = "WAVE"
	
	uint8_t		fmt[4];						// = "fmt "
	uint32_t	fmtSize;					// ��һ���ṹ��Ĵ�С(һ��Ϊ16)
	uint16_t	wFormatTag;					// ���뷽ʽ,һ��Ϊ1
	uint16_t	wChannels;					// ͨ������������Ϊ1��������Ϊ2
	uint32_t	dwSamplesPerSec;			// ������
	uint32_t	dwAvgBytesPerSec;			// ÿ���ֽ���(= ������ �� ÿ���������ֽ���)
	uint16_t	wBlockAlign;				// ÿ���������ֽ���(=����������/8*ͨ����)
	uint16_t	wBitsPerSample;				// ����������(ÿ��������Ҫ��bit��)
																			   
	uint8_t		data[4];					// = "data"
	uint32_t	datasize;					// �����ݳ���
} WavHead;

/* State_flag Bit definition*/
#define Playing_flag		0x01
#define PlaySuspend_flag	0x02

/* Flag Bit definition*/
#define DMA1_5_flag				0x0001		//DMA1_5���ݴ�����ɱ�־
#define Play_End_flag			0x0002		//���Ž����¼���־

typedef struct
{
	uint8_t		State_flag;							//����״̬��־
	char		Play_name[Name_Max*2];				//�����ļ������ּ�·��
	u8			Play_Number;						//���ڲ��ŵĸ������
	
	char		Music_List[List_Max][Name_Max];		//�ļ���������20���ֽ�
	u16			Music_Number;						//���ҵ��ĸ����ļ�����
	
	OS_FLAG_GRP	Flag;								//�¼���־
}Play_Control;

extern FATFS 		fs;
extern FRESULT 		res_sd;

extern Play_Control	Wav_Play;

static const char* strcasestr(const char* str, const char* subStr);
void FatReadDir(const TCHAR* Filedir,const TCHAR* Filetype);
void WAV_Play(void);
u8 WaitPlay_End(u16 wait_time);
void Start_Play(char *p);
void Suspend_Play(void);
void Stop_Play(u8 flag);
u8 Detection_idle(void);
void Play_List(uint8_t list);

#endif
