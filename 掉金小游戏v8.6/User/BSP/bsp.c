#include <bsp.h>

/*****�Ĵ���******/
#define  DWT_CR      *(CPU_REG32 *)0xE0001000
#define  DWT_CYCCNT  *(CPU_REG32 *)0xE0001004
#define  DEM_CR      *(CPU_REG32 *)0xE000EDFC
#define  DBGMCU_CR   *(CPU_REG32 *)0xE0042004


#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)


/*
*������	��BSP_Init
*����	����ʼ���İ弶֧�ְ�
*����   ��
*����	����
*���	����
*/
void  BSP_Init (void)
{
	Systick_Init();
	IIC_Init();
	MPU6050_initialize();
	DMP_Init();
	IO_Init();
	TIM_Init();
	EXTI_Config();
	PCM1770_Init();
	I2S_Bus_Init();
	USART_Config();
	SPI_Mode_Init();
	ADC_Mcode_init();
	X9313W_Init();
	NVIC_Config();
	iwdg_init();
	
	Ctl_Data.Fatfs = f_mount(&fs,"1:",1);			//����SD��
//	Ctl_Data.updata_state = 1;
}

/*��ʼ��������CPUʱ�����ʱ����*/
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
    DEM_CR         |= (CPU_INT32U)DEM_CR_TRCENA;                /* Enable Cortex-M3's DWT CYCCNT reg.                   */
    DWT_CYCCNT      = (CPU_INT32U)0u;
    DWT_CR         |= (CPU_INT32U)DWT_CR_CYCCNTENA;

    CPU_TS_TmrFreqSet(SystemCoreClock);
}
#endif

/*��ȡ��ǰʱ���ֵCPU��ʱ������*/
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    return ((CPU_TS_TMR)DWT_CYCCNT);
}
#endif
