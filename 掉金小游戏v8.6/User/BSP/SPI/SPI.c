#include "SPI.h"
#include "Drawing.h"

void SPI_Mode_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;						//SPI1-CS
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	SET_SPI_CS1;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;					//SPI1-SCK
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;					//SPI1-MOSI
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
	SPI_I2S_DMACmd(SPI1,SPI_I2S_DMAReq_Tx,ENABLE);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&SPI1->DR);	 		//设置DMA源
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)LED.RGB;					//内存地址(要传输的变量的指针)
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						//方向：从内存到外设
	DMA_InitStructure.DMA_BufferSize = 0;									//传输大小,暂不设置
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 		//外设地址不增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 		//内存数据单位 8bit
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;							//DMA模式：不循环
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;  					//优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//禁止内存到内存的传输
	
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	DMA_ClearFlag(DMA1_FLAG_TC3);
	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA1_Channel3,DISABLE);
}

void SPI_Send_Data(void)
{
	u8 i;
	
	LED.size = sizeof(LED.RGB);
	
	for(i=0;i<50;i++);
	DMA1_Channel3->CCR &= ~DMA_CCR3_EN;					//关闭DMA通道
	DMA1_Channel3->CMAR = (u32)&LED;
	DMA1_Channel3->CNDTR = LED.size+2;					//传输数据大小,Byte
	DMA1_Channel3->CCR |= DMA_CCR3_EN;					//开启DMA通道
}
