#include "PCM1770.h"

/**
  * @brief  Delay ������ʱ
  * @param  Delay Num
  * @retval None
  */
static void Delay(u32 Num)
{
	vu32 Count = Num*4;
	
	while (--Count);	
}

/**
  * @brief  PCM1770 Init��ʼ��PCM1770�õ������ţ�����λоƬ
  * @param  None
  * @retval None
  */
void PCM1770_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//Ӳ����λ
	PCM_PD_L;
	Delay(1000);
	PCM_PD_H;
	Delay(1000);

	PCM_MS_H;
	PCM_MC_H;
	PCM_MD_H;
}

/**
  * @brief  ��PCM1770д���������
  * @param  Reg:�Ĵ�������
	*		Data:Ҫд�������
  * @retval None
  */
void PCM_WriteData(const u8 Reg, const u8 Data)
{
	u16 TrasferData, i; 
	
	TrasferData = Data;
	TrasferData |= (Reg<<8)&0xff00;

	PCM_MS_L;			//Ƭѡ
	Delay(10);
	for (i = 0; i < 16; i++)
	{
		PCM_MC_L;
		if (TrasferData&0x8000)
		{
			PCM_MD_H;
		}
		else
		{
			PCM_MD_L;	
		}
		TrasferData <<=1;
		Delay(10);			//�������ȶ�
		PCM_MC_H;			//������д��
		Delay(10);			//�ȴ��ӻ�������		
	}
	PCM_MC_H;
	PCM_MD_H;	
	PCM_MS_H;				//�ͷ�Ƭѡ
	Delay(10);
}

void PCM1770_VolumeSet(vu8 vol)
{
	PCM_WriteData(0x01, vol);//���������������
	Delay(100);
	PCM_WriteData(0x02, vol);//�����Ҷ�������  
}
