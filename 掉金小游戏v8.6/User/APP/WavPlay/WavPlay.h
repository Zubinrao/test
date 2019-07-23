#ifndef __WAVPLAY__H
#define __WAVPLAY__H

#include "stm32f10x.h"
#include "os.h"
#include "ff.h"

#define	Directory_path  	"1:/music"	//´æ·Å¸èÇúµÄÄ¿Â¼Â·¾¶
#define List_Max			255			//×î´ó´æ·ÅµÄ¸èÇúÎÄ¼şÃûÊıÁ¿
#define Name_Max			20			//Ò»Ê×¸èÇúÃû×î´ó×Ö½ÚÊı

/* WAVÎÄ¼şÍ·²¿¸ñÊ½ */
typedef struct
{
	uint8_t		riff[4];					// = "RIFF"
	uint32_t	size_8;						// ´ÓÏÂ¸öµØÖ·¿ªÊ¼µ½ÎÄ¼şÎ²µÄ×Ü×Ö½ÚÊ
	uint8_t		wave[4];					// = "WAVE"
	
	uint8_t		fmt[4];						// = "fmt "
	uint32_t	fmtSize;					// ÏÂÒ»¸ö½á¹¹ÌåµÄ´óĞ¡(Ò»°ãÎª16)
	uint16_t	wFormatTag;					// ±àÂë·½Ê½,Ò»°ãÎª1
	uint16_t	wChannels;					// Í¨µÀÊı£¬µ¥ÉùµÀÎª1£¬Á¢ÌåÉùÎª2
	uint32_t	dwSamplesPerSec;			// ²ÉÑùÂÊ
	uint32_t	dwAvgBytesPerSec;			// Ã¿Ãë×Ö½ÚÊı(= ²ÉÑùÂÊ ¡Á Ã¿¸ö²ÉÑùµã×Ö½ÚÊı)
	uint16_t	wBlockAlign;				// Ã¿¸ö²ÉÑùµã×Ö½ÚÊı(=Á¿»¯±ÈÌØÊı/8*Í¨µÀÊı)
	uint16_t	wBitsPerSample;				// Á¿»¯±ÈÌØÊı(Ã¿¸ö²ÉÑùĞèÒªµÄbitÊı)
																			   
	uint8_t		data[4];					// = "data"
	uint32_t	datasize;					// ´¿Êı¾İ³¤¶È
} WavHead;

/* State_flag Bit definition*/
#define Playing_flag		0x01
#define PlaySuspend_flag	0x02

/* Flag Bit definition*/
#define DMA1_5_flag				0x0001		//DMA1_5Êı¾İ´«ÊäÍê³É±êÖ¾
#define Play_End_flag			0x0002		//²¥·Å½áÊøÊÂ¼ş±êÖ¾

typedef struct
{
	uint8_t		State_flag;							//²¥·Å×´Ì¬±êÖ¾
	char		Play_name[Name_Max*2];				//²¥·ÅÎÄ¼şµÄÃû×Ö¼°Â·¾¶
	u8			Play_Number;						//ÕıÔÚ²¥·ÅµÄ¸èÇúĞòºÅ
	
	char		Music_List[List_Max][Name_Max];		//ÎÄ¼şÃû²»³¬¹ı20¸ö×Ö½Ú
	u16			Music_Number;						//²éÕÒµ½µÄ¸èÇúÎÄ¼şÊıÁ¿
	
	OS_FLAG_GRP	Flag;								//ÊÂ¼ş±êÖ¾
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
