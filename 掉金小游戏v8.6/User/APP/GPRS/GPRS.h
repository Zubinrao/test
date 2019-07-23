#ifndef __GPRS__H
#define __GPRS__H

#include "stm32f10x.h"

#define Heartbeat		80000							//�������(s)

#define	LOCALPORT			"2000"					//���ض˿�
#define	SERVERIP			"47.107.254.218"		//��������IP
#define	SERVERPORT			"20180"					//���������˿�

//#define	SERVERIP_Spare		"2070xa3596.iask.in"	//���÷�����IP
//#define	SERVERPORT_Spare	"22782"					//���÷������˿�

#define	SERVERIP_Spare		"47.107.254.218"	//���÷�����IP
#define	SERVERPORT_Spare	"20180"					//���÷������˿�

#define frame_head				0x55			//֡ͷ
#define frame_tail				0xAA			//֡β
#define frame_length_Min		9				//���֡����

#define register_message		0x10			//ע����Ϣ
#define heartbeat_message		0x20			//������Ϣ
#define game_begins				0x30			//��Ϸ��ʼ
#define game_over				0x40			//��Ϸ����
#define query_version			0x50			//��ѯ�������ļ��汾��
#define File_transfer			0xFC			//�����ļ�����
#define date_transfer			0x90			//���ݴ���

#define Register_flag			0x0001			//����ע��ɹ���־
#define CMD_flag				0x0002			//���ڽ��յ�SIM900A��������Ӧ��־
#define Version_flag			0x0004			//�յ��汾��ѯ�ظ�
#define File_transfer_flag		0x0008			//�����ļ���������ظ�

void   SIM900A_Init(void);

uint8_t And_Check(char *p,const uint16_t size);
static uint8_t Detection_frame(char *p,u8 size);
static void SIM900A_Reset(void);
static uint8_t SIM900A_SendCommand(char *command,char *response);
static uint8_t SIM900A_TCP_Link(char * serverip,char * serverport);
static uint8_t SIM900A_TCP_Close(void);
void   Send_Data(char *p,uint16_t size);
static uint8_t Start_registration(void);
void 	Reply_message(u8 message);
static uint8_t File_version_query(void);
void 	Data_processing(void);
uint8_t BBC_Check(char *p,uint16_t size);
extern int gprsflag;

#endif
