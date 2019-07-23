#include "PCM1770.h"

/**
  * @brief  Delay 短暂延时
  * @param  Delay Num
  * @retval None
  */
static void Delay(u32 Num)
{
	vu32 Count = Num*4;
	
	while (--Count);	
}

/**
  * @brief  PCM1770 Init初始化PCM1770用到的引脚，并复位芯片
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

	//硬件复位
	PCM_PD_L;
	Delay(1000);
	PCM_PD_H;
	Delay(1000);

	PCM_MS_H;
	PCM_MC_H;
	PCM_MD_H;
}

/**
  * @brief  向PCM1770写入控制数据
  * @param  Reg:寄存器索引
	*		Data:要写入的数据
  * @retval None
  */
void PCM_WriteData(const u8 Reg, const u8 Data)
{
	u16 TrasferData, i; 
	
	TrasferData = Data;
	TrasferData |= (Reg<<8)&0xff00;

	PCM_MS_L;			//片选
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
		Delay(10);			//等数据稳定
		PCM_MC_H;			//上升沿写入
		Delay(10);			//等待从机读数据		
	}
	PCM_MC_H;
	PCM_MD_H;	
	PCM_MS_H;				//释放片选
	Delay(10);
}

void PCM1770_VolumeSet(vu8 vol)
{
	PCM_WriteData(0x01, vol);//调整左耳机整音量
	Delay(100);
	PCM_WriteData(0x02, vol);//调整右耳机音量  
}
