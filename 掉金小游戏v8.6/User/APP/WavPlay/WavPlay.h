#ifndef __WAVPLAY__H
#define __WAVPLAY__H

#include "stm32f10x.h"
#include "os.h"
#include "ff.h"

#define	Directory_path  	"1:/music"	//存放歌曲的目录路径
#define List_Max			255			//最大存放的歌曲文件名数量
#define Name_Max			20			//一首歌曲名最大字节数

/* WAV文件头部格式 */
typedef struct
{
	uint8_t		riff[4];					// = "RIFF"
	uint32_t	size_8;						// 从下个地址开始到文件尾的总字节�
	uint8_t		wave[4];					// = "WAVE"
	
	uint8_t		fmt[4];						// = "fmt "
	uint32_t	fmtSize;					// 下一个结构体的大小(一般为16)
	uint16_t	wFormatTag;					// 编码方式,一般为1
	uint16_t	wChannels;					// 通道数，单声道为1，立体声为2
	uint32_t	dwSamplesPerSec;			// 采样率
	uint32_t	dwAvgBytesPerSec;			// 每秒字节数(= 采样率 × 每个采样点字节数)
	uint16_t	wBlockAlign;				// 每个采样点字节数(=量化比特数/8*通道数)
	uint16_t	wBitsPerSample;				// 量化比特数(每个采样需要的bit数)
																			   
	uint8_t		data[4];					// = "data"
	uint32_t	datasize;					// 纯数据长度
} WavHead;

/* State_flag Bit definition*/
#define Playing_flag		0x01
#define PlaySuspend_flag	0x02

/* Flag Bit definition*/
#define DMA1_5_flag				0x0001		//DMA1_5数据传输完成标志
#define Play_End_flag			0x0002		//播放结束事件标志

typedef struct
{
	uint8_t		State_flag;							//播放状态标志
	char		Play_name[Name_Max*2];				//播放文件的名字及路径
	u8			Play_Number;						//正在播放的歌曲序号
	
	char		Music_List[List_Max][Name_Max];		//文件名不超过20个字节
	u16			Music_Number;						//查找到的歌曲文件数量
	
	OS_FLAG_GRP	Flag;								//事件标志
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
