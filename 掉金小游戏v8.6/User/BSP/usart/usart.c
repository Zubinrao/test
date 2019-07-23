#include "usart.h"
#include "os.h"
#include "Control.h"
#include <string.h>
#include "GPRS.h"
#include "update.h"
#include "debug.h"
#include "in_output.h"
USART_Rx_TypeDef	usart2;


/*
*Function name��USART_Config()
*Notes��		��ʼ���ڽӿ�
*Input:			NO
*Output:		NO
*/
void USART2_Send(char *p,uint16_t size)
{
	while(size--)
	{
		USART_SendData(USART2, *p);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		p++;
	}
}

void USART_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE);
	
	/********** USART GPIO config **********/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_9;							//USART2 Tx (PA2)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10;							//USART2 Rx (PA3)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = USART2_Baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART2, ENABLE);
	USART_Cmd(USART1, ENABLE);
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	
	/*DMA���ʹ�������*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART2_Data_ADDRESS;	 		//����DMAԴ��UART2���ݼĴ�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)0;							//�ڴ��ַ(Ҫ����ı�����ָ��),�ݲ�����
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						//���򣺴��ڴ浽����
	DMA_InitStructure.DMA_BufferSize = 0;									//�����С,�ݲ�����
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 		//�����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //�������ݵ�λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 		//�ڴ����ݵ�λ 8bit
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;							//DMAģʽ����ѭ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  					//���ȼ�����
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//��ֹ�ڴ浽�ڴ�Ĵ���
	
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);
	DMA_ClearFlag(DMA1_FLAG_TC7);
	DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE);
	
	/*DMA���մ�������*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART2_Data_ADDRESS;	 		//����DMAԴ��UART2���ݼĴ�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&usart2.Rx_Buf;				//�ڴ��ַ(Ҫ����ı�����ָ��)
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;						//���򣺴��ڴ浽����
	DMA_InitStructure.DMA_BufferSize = Receive_number;						//�����С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 		//�����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //�������ݵ�λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 		//�ڴ����ݵ�λ 8bit
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;							//DMAģʽ����ѭ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  					//���ȼ�����
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//��ֹ�ڴ浽�ڴ�Ĵ���
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel6,ENABLE);
	USART3_Init();
//	USART2_Send("test ok\r\n",sizeof("test ok\r\n")-1);
	USART3_Send("test ok\r\n",20);	
}
//int NetCheck_Ok(void)
//{
//	  int  flag_network = 1;
//	  //�鿴ģ���ֲ᣻
//	flag_network=GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_8);
//	
//	return flag_network;
//}

//void LTE_Init(void)
//{
//	OS_ERR err;
//	u8 i=0;
//	SIM900A_Reset();
//	while(Start_registration() == 0)								//ע���ն�
//	{
//		
//		i++;
//		if(i >= 10)
//		{
//			i=0;
//			SoftReset();
//		}
//		OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
//	}
//  if(Start_registration() == 1)
//	{
//		gprsflag=1;
//	}

//	File_version_query();							//��ѯ�Ƿ���Ҫ����������ļ�
//	
//	Ctl_Data.Heartbeat_count = 0;
//	Ctl_Data.Heartbeat_flag = 1;
//}

//int FtpLink(void)
//{
	
	
	
//    int  i = 0;	
//		char FTPEnter[20] ={0};
//		char FTPEnter_OK[20] = [0];
//		char FTP_USR[20] ={0};
//		char FTP_USR_OK[20] = [0];
//		char enterFtp[20] ={0};
//		char FTP_USR[20] ={0};
//		char
//		USART2_Send(FTPEnter,8);
//		whilie(i<50)
//		{
//				if(strcmp(rcv,FTPEnter_OK)==1)
//						break;
//				delay_ms(200);
//				i++;
//		}
//		if (i>=50)
//			return 0;
//		else
//		{
//				i=0;
//				memset(rcv,0,sizeof(rcv));
//		}
//		
//				USART2_Send(FTPEnter,8);
//		whilie(i<50)
//		{
//				if(strcmp(rcv,FTPEnter_OK)==1)
//						break;
//				delay_ms(200);
//				i++;
//		}
//		if (i>=50)
//			return 0;
//		else
//		{
//				i=0;
//				memset(rcv,0,sizeof(rcv));
//		}
//		
//				USART2_Send(FTPEnter,8);
//		whilie(i<50)
//		{
//				if(strcmp(rcv,FTPEnter_OK)==1)
//						break;
//				delay_ms(200);
//				i++;
//		}
//		if (i>=50)
//			return 0;
//		else
//		{
//				i=0;
//				memset(rcv,0,sizeof(rcv));
//		}
//		
//				USART2_Send(FTPEnter,8);
//		whilie(i<50)
//		{
//				if(strcmp(rcv,FTPEnter_OK)==1)
//						break;
//				delay_ms(200);
//				i++;
//		}
//		if (i>=50)
//			return 0;
//		else
//		{
//				i=0;
//				memset(rcv,0,sizeof(rcv));
//		};
//		return 1;
	
//}

//int FtpDownload(void)
//{
//		    int  i = 0;	
//		char FTPEnter[20] ={0};
//		char FTPEnter_OK[20] = [0];
//		
//		return 1;
//}


void USART3_Init(){
		GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
		USART_InitTypeDef USART_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3��GPIOBʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��USART3��GPIOBʱ��
    USART_DeInit(USART3);  //��λ����3
    //USART3_TX   PB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��Pb10
    //USART3_RX	  PB11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //��ʼ��PB11
 
    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
 
    //USART ��ʼ������
    USART_InitStructure.USART_BaudRate = 115200;//һ������Ϊ115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART3, &USART_InitStructure); //��ʼ������
 
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//����USART3�����ж�
    USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���
}

void USART3_Send(char *p,uint16_t size)
{
	while(size--)
	{
		USART_SendData(USART3, *p);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		p++;
	}
}




void USART1_Send(char *p,uint16_t size)
{
	while(size--)
	{
		USART_SendData(USART1, *p);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		p++;
	}
}

/**
 *@Notes��		����DMA�ڴ�ָ���Լ����ݴ�С
 *@Input:		void
 *@Output:
 */
void SetDMA_MemoryBaseAddr(void)
{
	DMA1_Channel6->CCR &= ~DMA_CCR6_EN;					//�ر�DMAͨ��
	DMA1_Channel6->CMAR = (u32)&usart2.Rx_Buf;			//�ڴ��ַ
	DMA1_Channel6->CNDTR = Receive_number;				//�������ݴ�С,Byte
	DMA1_Channel6->CCR |= DMA_CCR6_EN;					//����DMAͨ��
}

/*
*Function name��UART2_Send_Data()
*Notes��		����2 DMA ��������
*Input:			p - ���������ݵ��׵�ַ
*				Size - ���������ݵĴ�С
*Output:		NO
*/
void UART2_Send_Data(char *p,u16 Size)
{
	OS_ERR err;
	
	/* �ȴ���һ�����ݷ�����ɱ�־ */
	OSFlagPend((OS_FLAG_GRP	*)&Ctl_Data.Flag,
			   (OS_FLAGS	 )DMA1_7_flag,
			   (OS_TICK		 )1000,
			   (OS_OPT		 )OS_OPT_PEND_FLAG_SET_ANY|OS_OPT_PEND_FLAG_CONSUME,
			   (CPU_TS		*)0,
			   (OS_ERR		*)&err);
	
	if(err == OS_ERR_NONE)
	{
		DMA1_Channel7->CCR &= ~DMA_CCR7_EN;					//�ر�DMAͨ��
		DMA1_Channel7->CMAR = (u32)p;						//�ڴ��ַ
		DMA1_Channel7->CNDTR = Size;						//�������ݴ�С,Byte
		DMA1_Channel7->CCR |= DMA_CCR7_EN;					//����DMAͨ��
		USART2->CR3 |= USART_DMAReq_Tx;						//
	}
}

int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
	return (ch);
}

int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
}
/*********************************************END OF FILE**********************/
