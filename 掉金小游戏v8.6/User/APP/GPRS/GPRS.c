#include "GPRS.h"
#include "usart.h"
#include "string.h"
#include "in_output.h"
#include "Control.h"
//#include "flash.h"
#include "NVIC.h"
#include "update.h"
#include "os.h"
//#include "WavPlay.h"
#include "debug.h"

extern OS_TCB    ControlTCB;
int gprsflag=0;

union u82u32{
	u32 len;
	u8 c[4];
};

extern file_transfer	update;

static char 	TX_Buf[100];			//�������ݻ�����
char 			RX_Buf[100];			//�������ݻ�����

static u8 	FTP_ENTER_CMD[1]={0xA0};
static u8 	FTP_IPPORT_CMD[18]={0xA1, 0x34, 0x37, 0x2E, 0x31, 0x30, 0x37, 0x2E, 0x32, 0x35, 0x34, 0x2E, 0x32, 0x31, 0x38, 0x3A, 0x32, 0x31};
static u8 	FTP_KEY_CMD[25]={0xA2, 0x41, 0x64, 0x6D, 0x69, 0x6E, 0x69, 0x73, 0x74, 0x72, 0x61, 0x74, 0x6F, 0x72, 0x00, 0x77, 0x75, 0x70, 0x61, 0x69, 0x40, 0x32, 0x30, 0x31, 0x38};
static u8 	FTP_addr_CMD[11]={0xA3, 0x75, 0x70, 0x64, 0x61, 0x74, 0x61, 0x2E, 0x62, 0x69, 0x6E};
static u8 	FTP_download_CMD[5]={0xA4, 0x01, 0x00, 0x00, 0x01};

OS_MUTEX 		SIM900A_mutex;			//���������ź���
OS_FLAG_GRP		SIM900A_Flag;			//�¼���־

static u16 P_Version;					//������ѯ����汾
static u16 M_Version;					//������ѯ��Ƶ�ļ��汾

static uint8_t ftp_message(u8 *message,u8 response,u8 length);
static uint8_t Ftp_link(void);
void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen);
int NetCheck_Ok(void);







int NetCheck_Ok(void)
{
	  int  flag_network = 1;
	  //�鿴ģ���ֲ᣻
	flag_network=GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_8);
	
	return flag_network;
}


//void ftp_StrToHex(BYTE *pbDest, BYTE *pbSrc, int nLen);
/**
 *@brief	��У��
 *@param	p - ��ҪУ��������׵�ַ,�˳�֮ǰ��ָ֤����Ч
			size - ��Ҫ�������ܴ�С
 *@retval	У����
 */
uint8_t And_Check(char *p,uint16_t size) 
{
	uint16_t i;
	uint8_t check=0;
	
	for(i=0;i<size;i++)
	{
		check += *p;
		p++;
	}
	
	return check;
}

/**
 *@brief	��������е�֡��Ϣ
 *@param	p - ��Ҫ����֡�׵�ַ,�˳�֮ǰ��ָ֤����Ч
			size - ��Ҫ�������ܴ�С
 *@retval	1 - ��⵽��Ч֡
			0 - δ��⵽��Ч֡
 */
static uint8_t Detection_frame(char *p,u8 size)
{
	u8 i;
	
	if(size < frame_length_Min)											//���֡����
		return 0;
	
	for(i=0;i<size;i++)
	{
		if((size - i) < frame_length_Min)
			return 0;
		
		if((p[i] == frame_head) && (p[p[i+2]-1+i] == frame_tail))		//���֡ͷ��֡β
		{
			if(i != 0)
			{
				memcpy(TX_Buf,p,size);
				memcpy(p,TX_Buf+i,size-i+1);
			}
			if(And_Check(p,p[2]-2) == p[p[2]-2])						//��У��
				return 1;
			else
				return 0;
		}
	}
	return 0;
}

//static uint8_t FTP_Detection_frame(char *p,u8 size)
//{
//	u8 i;
//	
//	
//	for(i=0;i<size;i++)
//	{
//		if((size - i) < frame_length_Min)
//			return 0;
//		
//		if((p[i] == frame_head) && (p[p[i+2]-1+i] == frame_tail))		//���֡ͷ��֡β
//		{
//			if(i != 0)
//			{
//				memcpy(TX_Buf,p,size);
//				memcpy(p,TX_Buf+i,size-i+1);
//			}
//			if(And_Check(p,p[2]-2) == p[p[2]-2])						//��У��
//				return 1;
//			else
//				return 0;
//		}
//	}
//	return 0;
//}


/**
 *@brief	��λSIM900A
 *@param	
 *@retval	0 - ��λʧ��
			1 - ��λ�ɹ�
			2 - ģ��������δִ�и�λ
 */
static void SIM900A_Reset(void)
{
	OS_ERR err;

	SIM900A_RST_L;
	OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
	SIM900A_RST_H;
	OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
//	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
//	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);

//	SIM900A_SendCommand("AT+E=OFF\r","OK");
}

/**
 *@brief	����SIM900A��������
 *@param	command - ��������
			response - ��Ӧ��Ϣ,if(response == NULL)�򲻵ȴ���Ӧ��Ϣ
 *@retval	1 - ���ͳɹ�
			0 - ����ʧ��
 */
static uint8_t SIM900A_SendCommand(char *command,char *response)
{
	OS_ERR err;
	
	memset(&usart2.Rx_Buf,0,Receive_number);
	OSFlagPost(&SIM900A_Flag,CMD_flag,OS_OPT_POST_FLAG_CLR,&err);					//������ڱ�־
	UART2_Send_Data(command,strlen(command));										//���ڷ�������
	
	if(response != NULL)
	{
		/* �ȴ����յ���Ӧ���� */
		OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
				   (OS_FLAGS	 )CMD_flag,
				   (OS_TICK		 )2000,
				   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
				   (CPU_TS		 )0,
				   (OS_ERR		*)&err);
		
		if(err == OS_ERR_NONE)
		{
			if(strstr(usart2.Rx_Buf,response) != NULL)
				return 1;
			else
				return 0;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		OSTimeDly(50,OS_OPT_TIME_DLY,&err);
		return 1;
	}
}
/**
 *@brief	SIM900A TCP͸����������
 *@param    serverip - ������IP
			serverport - �������˿�
 *@retval	1 - ���ӳɹ�
			0 - ����ʧ��
 */
static uint8_t SIM900A_TCP_Link(char * serverip,char * serverport)
{
	OS_ERR err;
	char buf[100];
//	
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);
//	SIM900A_SendCommand("+++\r","a");							//����͸��ģʽ
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);
//	SIM900A_SendCommand("a\r","+ok");							//����͸��ģʽ
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);	
//	SIM900A_SendCommand("AT+WKMOD=NET\r","OK");							//����͸��ģʽ
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);	
//	SIM900A_SendCommand("AT+SOCKAEN=ON\r","OK");							//����͸��ģʽ
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);
//	SIM900A_SendCommand("AT+SOCKASL=LONG\r","OK");							//����͸��ģʽ
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);	
//  sprintf(buf,"AT+SOCKA=\"TCP\",\"%s\",\"%s\"\r",serverip,serverport);
//	SIM900A_SendCommand(buf,"OK");										//���÷�����IP�Ͷ˿�	
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);	
//	SIM900A_SendCommand("AT+Z\r",NULL);							//����͸��ģʽ
//	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);		
//	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);

										//���÷�����IP�Ͷ˿�
	
	/* �ȴ����յ���Ӧ���� */
	memset(usart2.Rx_Buf,0,Receive_number);
	OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
			   (OS_FLAGS	 )CMD_flag,
			   (OS_TICK		 )5000,
			   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
			   (CPU_TS		 )0,
			   (OS_ERR		*)&err);
	
//	if(err == OS_ERR_NONE)
//	{
//		if(strstr(usart2.Rx_Buf,"CONNECT") == NULL)
//		{
//			SIM900A_TCP_Close();
//			return 0;
//		}
//	}
//	else
//	{
//		return 0;
//	}

//	memset(usart2.Rx_Buf,0,Receive_number);
//	OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
//			   (OS_FLAGS	 )CMD_flag,
//			   (OS_TICK		 )10000,
//			   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
//			   (CPU_TS		 )0,
//			   (OS_ERR		*)&err);
//	
//	if(err == OS_ERR_NONE)
//	{
//		if(strstr(usart2.Rx_Buf,"CLOSED") != NULL)
//		{
//			SIM900A_TCP_Close();
//			return 0;
//		}
//	}
	
	return 1;
}

/**
 *@brief	�ر�TCP����
 *@param	
 *@retval	1 - �رճɹ�
			0 - �ر�ʧ��
 */
static uint8_t SIM900A_TCP_Close(void)
{
	SIM900A_SendCommand("AT+CIPCLOSE=1\r","CLOSE OK");
	return 1;
}

/**
 *@brief	GPRS��������
 *@param	p - �������ݵ��׵�ַ,�������ݹ����б�ָ֤��һֱ��Ч
			size - ���������ܴ�С
 *@retval	1 - ���ͳɹ�
			0 - ����ʧ��
 */
void Send_Data(char *p,uint16_t size)
{
	OS_ERR err;
	
	OSMutexPend ((OS_MUTEX  *)&SIM900A_mutex,
				 (OS_TICK    )0,
				 (OS_OPT     )OS_OPT_PEND_BLOCKING,
				 (CPU_TS    *)0,
				 (OS_ERR    *)&err);
	
	UART2_Send_Data(p,size);
	
	OSMutexPost ((OS_MUTEX  *)&SIM900A_mutex,
				 (OS_OPT     )OS_OPT_POST_NONE,
				 (OS_ERR    *)&err);  
}


/**
 *@brief	����ע��
 *@param	
 *@retval	1 - ע��ɹ�
			0 - ע��ʧ��
 */
static uint8_t Start_registration(void)
{
	OS_ERR err;
	SIM900A_Init();
	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
	Reply_message(register_message);				//����ע����Ϣ
	Debug_SendMsg("Start_registration", "CMD_Send!", DEBUG_MSG_INFO);
	OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
			   (OS_FLAGS	 )Register_flag,
			   (OS_TICK		 )10000,
			   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
			   (CPU_TS		 )0,
			   (OS_ERR		*)&err);
	
	if(err == OS_ERR_NONE)
		return 1;
	else
		return 0;
}

/**
 *@brief	Ӧ����Ϣ
 *@param	message - ��Ϣ����
 *@retval	
 */
void Reply_message(u8 message)
{
	TX_Buf[0] = frame_head;
	TX_Buf[1] = message;
	TX_Buf[2] = 0x09;
	TX_Buf[3] = Ctl_Data.Class >> 8;
	TX_Buf[4] = Ctl_Data.Class;
	TX_Buf[5] = Ctl_Data.ID >> 8;
	TX_Buf[6] = Ctl_Data.ID;
	TX_Buf[7] = And_Check(TX_Buf,7);
	TX_Buf[8] = frame_tail;
	
	Send_Data(TX_Buf,9);
}


/**
 *@brief	�ļ��汾��ѯ
 *@param	
 *@retval	1 - ��ѯ�ɹ�
 *          0 - ��ѯʧ��
 */
static uint8_t File_version_query(void)
{
	OS_ERR err;
	
	Reply_message(0x50);
	OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
			   (OS_FLAGS	 )Version_flag,
			   (OS_TICK		 )5000,
			   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
			   (CPU_TS		 )0,
			   (OS_ERR		*)&err);
	
	if(err == OS_ERR_NONE)
	{
		M_Version = ((u16)usart2.Rx_Buf[8] << 8) || (usart2.Rx_Buf[7]);
		P_Version = ((u16)usart2.Rx_Buf[10] << 8)|| (usart2.Rx_Buf[9]);
		
		P_Version=0;
		if(P_Version != 0)
		{
			u8 i=0;
			u8 ftp_flag=1;

//				Ctl_Data.Game_State = 0;					//ֹͣ��Ϸ
//				Ctl_Data.Heartbeat_flag = 0;			//������������
//				Ctl_Data.Trigger_flag = DISABLE;	//��ֹ�Ƕȴ���
//				Ctl_Data.Com_Mode = 1;						//�����ļ���������ģʽ

//			Reply_message(0x80);						//֪ͨ�����������ļ�����ģʽ
			Play_List(255);
			OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
			
//			for(i = 0; i < 2; i++){
//				sprintf(str, "%x ||", bbc_test[i]);
//				Debug_SendMsg("File_transfer",str,DEBUG_MSG_INFO);
//			}
//			bbc_src[0]=BBC_Check(bbc_test,2);
//			HexToStr(str,bbc_src,1);
//			Debug_SendMsg("BBC",str,DEBUG_MSG_INFO);
//			
//					
//			if(BBC_Check(bbc_test,2)==0xA9)
//				Debug_SendMsg("BBC","bbcok",DEBUG_MSG_INFO);
//			else
//				Debug_SendMsg("BBC","bbcfail",DEBUG_MSG_INFO);
				
			gprsflag=0;
							Ctl_Data.Game_State = 0;					//ֹͣ��Ϸ
				Ctl_Data.Heartbeat_flag = 0;			//������������
	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
//			while(ftp_flag)
//			{
//				
//				if(Ftp_link()==1)
//				{
//					ftp_flag=0;
////					Play_List(20);
////					OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
//				}
//				else
//				{
////					Play_List(22);
////					OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
//					ftp_flag=1;
//				
//				}
//					
//				
////				i++;
////				if(i=3)
////				{
////					break;
////					
////				}
//				OSTimeDly(200,OS_OPT_TIME_DLY,&err);
//			}
				Ctl_Data.Com_Mode = 1;						//�����ļ���������ģʽ
				Ctl_Data.Game_State = 0;					//ֹͣ��Ϸ
				Ctl_Data.Heartbeat_flag = 0;			//������������
				Ctl_Data.Trigger_flag = DISABLE;	//��ֹ�Ƕȴ���
				
				Program_download();
				Ctl_Data.Com_Mode = 0;						//�����ļ���������ģʽ
//				OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
//					   (OS_FLAGS	 )File_transfer_flag,
//					   (OS_TICK		 )5000,
//					   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
//					   (CPU_TS		 )0,
//					   (OS_ERR		*)&err);
//			
			
//			if(err == OS_ERR_NONE)
//			{
////				Stop_Play(1);											//ֹͣ����
//				Ctl_Data.Game_State = 0;					//ֹͣ��Ϸ
//				Ctl_Data.Heartbeat_flag = 0;			//������������
//				Ctl_Data.Trigger_flag = DISABLE;	//��ֹ�Ƕȴ���
//				Ctl_Data.Com_Mode = 1;						//�����ļ���������ģʽ
//				Program_download();								//���س����ļ�
//			}
//			else
//			{
//				
//			}

		}
		else
		{
			Ctl_Data.updata_state=0;
		}
		
		if(M_Version  != 0)
		{
		}
		return 1;
	}
	return 0;
}

/**
 *@brief	SIM900A ������ʼ��
 *@param	mode - ����ģʽ
 *          0 - ��λ����
 *          1 - ����ע������
 *          2 - �������ӷ�����
 *          3 - ����ע��
 *@retval	1 - �ɹ�
			0 - ʧ��
 */
void SIM900A_Init(void)
{
	
	OS_ERR err;
	u8 i=0;
		
	SIM900A_Reset();

//	i=0;
//	while(SIM900A_TCP_Link(SERVERIP,SERVERPORT) == 0)		//���ӷ�����
//	{
//		i++;
//		if(i >= 20)
//		{
//			i=0;
//			while(SIM900A_TCP_Link(SERVERIP_Spare,SERVERPORT_Spare) == 0)		//���ӱ��÷�����
//			{
//				i++;
//				if(i >= 20)
//				{
//					i=0;
//					SoftReset();
//				}
//				OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
//			}
//			break;
//		}
//		OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
//	}
//�ȴ�ģ�����������ӷ�����
while(NetCheck_Ok()==1)
{
	Debug_SendMsg("Net_link","fail",DEBUG_MSG_INFO);
	OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
}
Debug_SendMsg("Net_link","OK",DEBUG_MSG_INFO);
	while(Start_registration() == 0)								//ע���ն�
	{
		
		i++;
		if(0)
		{
			i=0;
			Debug_SendMsg("Registration_Overtime","SoftReset",DEBUG_MSG_INFO);
			SoftReset();
		}
		OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
	}
  if(Start_registration() == 1)
	{
		gprsflag=1;
	}
OSTimeDly(5000,OS_OPT_TIME_DLY,&err);

	//	File_version_query();							//��ѯ�Ƿ���Ҫ����������ļ�
	
	Ctl_Data.Heartbeat_count = 0;
	Ctl_Data.Heartbeat_flag = 1;
}

void Data_processing(void)
{
	OS_ERR err;
	int i;
	char str[50];
//	Debug_SendMsg("File_transfer","==================",DEBUG_MSG_INFO);
//	for(i = 0; i < usart2.Rx_Number; i++)
//	{
//		sprintf(&str[i], "%x", usart2.Rx_Buf[i]);
//		//Debug_SendMsg("File_transfer",str,DEBUG_MSG_INFO);
//	}
//	
	if(Detection_frame(usart2.Rx_Buf,usart2.Rx_Number)||(usart2.Rx_Buf[4]==0x0A))		//�ж��Ƿ�Ϊ�������´��Ŀ�������
	{
		switch(usart2.Rx_Buf[1])
		{
			case register_message:							//�յ�����ע��Ӧ��
				OSFlagPost(&SIM900A_Flag,Register_flag,OS_OPT_POST_FLAG_SET,&err);
				Debug_SendMsg("Data_processing","register_message", DEBUG_MSG_INFO);
				break;
			case heartbeat_message:							//�յ�������Ϣ
//				Reply_message(heartbeat_message);			//Ӧ��
//				Ctl_Data.Heartbeat_count = 0;			//�������ʱ��
//				Ctl_Data.Heartbeat_flag = 1;			//��⵽����
			Ctl_Data.HeartSendTime=0;
			char str[50];
			sprintf(str, "%d",(int)Ctl_Data.HeartSendTime);
			Debug_SendMsg("Data_processing_Rec_heart/HeartSendTime=",str, DEBUG_MSG_INFO);
				break;
			case game_begins:								//�յ���Ϸ��ʼ��Ϣ
				memcpy(RX_Buf,&usart2.Rx_Buf[0],usart2.Rx_Buf[2]);
				Reply_message(game_begins);
			
				OSFlagPost(&Ctl_Data.Flag,Game_Begins,OS_OPT_POST_FLAG_SET,&err);
			Debug_SendMsg("Data_processing","game_begins", DEBUG_MSG_INFO);
				break;
			case game_over:									//�յ���Ϸ�����ظ�
				OSFlagPost(&Ctl_Data.Flag,GameOver_Answer_flag,OS_OPT_POST_FLAG_SET,&err);
			Debug_SendMsg("Data_processing","game_over", DEBUG_MSG_INFO);
				break;
			case query_version:								//�յ��汾��ѯ�ظ�
				OSFlagPost(&SIM900A_Flag,Version_flag,OS_OPT_POST_FLAG_SET,&err);
				break;
			case File_transfer:
				OSFlagPost(&SIM900A_Flag,File_transfer_flag,OS_OPT_POST_FLAG_SET,&err);
				break;
			default:
				/*�����������ⲿ����*/
				OSFlagPost(&SIM900A_Flag,CMD_flag,OS_OPT_POST_FLAG_SET,&err);
				break;
		}
	}
	else													//������Ϣ���ⲿ����
	{
		OSFlagPost(&SIM900A_Flag,CMD_flag,OS_OPT_POST_FLAG_SET,&err);
	}
}

uint8_t BBC_Check(char *p,uint16_t size)
{
	uint8_t out;//���ڱ�������� 
	uint16_t i=0;
	out=*p;
	p++;
	for (i=0;i<size-1;i++) 
	{
		out^=*p;
		p++;
	} 
	return out;
}

/*
ftp����
����0���ɹ�
����1��ʧ��
*/
static uint8_t ftp_message(u8 *message,u8 response,u8 length)
{
	OS_ERR err;
	u8 ftp_frame_head[3]={0x55,0xFC,0xAA};
	u8 i=0;
//	length=sizeof(message);
	
	TX_Buf[0] = 0x55;
	TX_Buf[1] = 0xFC;
	TX_Buf[2] = 0xAA;
	
	TX_Buf[3] = 0x00;
	TX_Buf[4] = length+4;
	TX_Buf[5] = 0x01;
	
	
	for(i=0;i<length;i++)
	{
		TX_Buf[i+6]=*message;
		message++;
	}
	
	TX_Buf[length+6]=BBC_Check(TX_Buf,length+6);
	Send_Data(TX_Buf,length+7);

	if(response != NULL)
	{
		/* �ȴ����յ���Ӧ���� */
		OSFlagPend((OS_FLAG_GRP	*)&SIM900A_Flag,
				 (OS_FLAGS	 )File_transfer_flag,
				 (OS_TICK		 )10000,
				 (OS_OPT		 )OS_OPT_PEND_FLAG_SET_AND|OS_OPT_PEND_FLAG_CONSUME,
				 (CPU_TS		 )0,
				 (OS_ERR		*)&err);
		
		
		
		if(err == OS_ERR_NONE)
		{
			if(usart2.Rx_Buf[7]==response)
			{	
				return 1;
			}
				
			else
			{
				return 0;
			}
				
		}
		else
		{
			return 0;
		}
	}
	else
	{
		OSTimeDly(50,OS_OPT_TIME_DLY,&err);
		return 1;
	}
}

static uint8_t Ftp_link()
{
	OS_ERR err;
	char str[50];
	char src[4];
	u8 p_size[4];
	u8 i=0;
	
	
	u8 hl_bit[2]={0};
	u8 buf[4]={0};
	u8 usarttest[20]={0};
		
	union u82u32 cvter;

	
	
	while(ftp_message(FTP_ENTER_CMD,0x01,1)==0)
	{ 
		i++;
		if(i>3)
			return 0;
		Debug_SendMsg("Ftp_link","ENTER",DEBUG_MSG_INFO);
		OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
	}
	i=0;
	while(ftp_message(FTP_IPPORT_CMD,0x01,18)==0)
	{ i++;
		if(i>3)
			return 0;
		Debug_SendMsg("Ftp_link","IPPORT",DEBUG_MSG_INFO);
		OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
	}
	
	i=0;
	while(ftp_message(FTP_KEY_CMD,0x01,25)==0)
	{ i++;
		if(i>3)
			return 0;
		Debug_SendMsg("Ftp_link","KEY",DEBUG_MSG_INFO);
		OSTimeDly(500,OS_OPT_TIME_DLY,&err);
	}
	
	i=0;
	
	while(ftp_message(FTP_addr_CMD,0x01,11)==0)
	{ i++;
		if(i>3)
			return 0;
		Debug_SendMsg("Ftp_link","addr",DEBUG_MSG_INFO);
		OSTimeDly(500,OS_OPT_TIME_DLY,&err);
	}
	
//	ftp_message(FTP_ENTER_CMD,0x01,1);
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);
//	ftp_message(FTP_IPPORT_CMD,0x01,18);
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);
//	ftp_message(FTP_KEY_CMD,0x01,25);
//	OSTimeDly(500,OS_OPT_TIME_DLY,&err);

//	if(ftp_message(FTP_addr_CMD,01,11)==1)//��ַ���óɹ��򷵻������ļ������ֽ���
//	{
		
		Play_List(35);
		OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
		HexToStr(usarttest,usart2.Rx_Buf,20);
		Debug_SendMsg("BBCusart2",usarttest,DEBUG_MSG_INFO);
		
		src[0]=usart2.Rx_Buf[8];
		src[1]=usart2.Rx_Buf[9];
		src[2]=usart2.Rx_Buf[10];
		src[3]=usart2.Rx_Buf[11];
		HexToStr(str,src,4);
		Debug_SendMsg("BBC1",str,DEBUG_MSG_INFO);
		
		cvter.c[0] = usart2.Rx_Buf[11];
		cvter.c[1] = usart2.Rx_Buf[10];
		cvter.c[2] = usart2.Rx_Buf[9];
		cvter.c[3] = usart2.Rx_Buf[8];

		update.package_size=cvter.len;
		return 1;
//	}
//	else
//	{
//		return 0;
//	}

}

/*
// C prototype : void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - ���Ŀ���ַ���
//	[IN] pbSrc - ����16����������ʼ��ַ
//	[IN] nLen - 16���������ֽ���
// return value: 
// remarks : ��16������ת��Ϊ�ַ���
*/
void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen)
{
char	ddl,ddh;
int i;

for (i=0; i<nLen; i++)
{
ddh = 48 + pbSrc[i] / 16;
ddl = 48 + pbSrc[i] % 16;
if (ddh > 57) ddh = ddh + 7;
if (ddl > 57) ddl = ddl + 7;
pbDest[i*2] = ddh;
pbDest[i*2+1] = ddl;
}

pbDest[nLen*2] = '\0';
}
/*
// C prototype : void StrToHex(BYTE *pbDest, BYTE *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - ���������
//	[IN] pbSrc - �ַ���
//	[IN] nLen - 16���������ֽ���(�ַ����ĳ���/2)
// return value: 
// remarks : ���ַ���ת��Ϊ16������
*/
void ftp_StrToHex(BYTE *pbDest, BYTE *pbSrc, int nLen)
{
char h1,h2;
BYTE s1,s2;
int i;

for (i=0; i<nLen; i++)
{
h1 = pbSrc[2*i];
h2 = pbSrc[2*i+1];

s1 = toupper(h1) - 0x30;
if (s1 > 9) 
s1 -= 7;

s2 = toupper(h2) - 0x30;
if (s2 > 9) 
s2 -= 7;

pbDest[i] = s1*16 + s2;
}
}
