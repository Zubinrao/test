#include "app.h"
#include "stdio.h"
#include "debug.h"

extern OS_MUTEX 	SIM900A_mutex;
extern OS_FLAG_GRP	SIM900A_Flag;
extern OS_FLAG_GRP	Update_Flag;
static u8 heartcount=0;
u8 Time_Count = 0; //1s定时器计数
u8 piano_time_count = 0;
OS_TMR		TMR_1s;

int main(void)
{
    OS_ERR err;
	
//	SCB->VTOR=FLASH_BASE | 0x10000;											//中断向量表偏移
    OSInit(&err);                                               			//初始化 uC/OS-III
	
	/* 创建任务 */
    OSTaskCreate((OS_TCB     *)&STARTTCB,                             		//任务控制块地址
                 (CPU_CHAR   *)"App Task Start",                            //任务名称
                 (OS_TASK_PTR ) AppTaskStart,                               //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) START_PRIO,                       		 	//任务的优先级
                 (CPU_STK    *)&StartStk[0],                         		//任务堆栈的基地址
                 (CPU_STK_SIZE) START_STK_SIZE / 10,               			//任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) START_STK_SIZE,                    			//任务堆栈空间
                 (OS_MSG_QTY  ) 0u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型
				 
    OSStart(&err);                                                          //启动多任务管理（交由uC/OS-III控制)
}

static void AppTaskStart ( void *p_arg )
{
	OS_ERR err;
	
	BSP_Init();                                                 			//板级初始化
	CPU_Init();                                                 			//初始化 CPU 组件（时间戳、关中断时间测量和主机名）

#if OS_CFG_STAT_TASK_EN > 0u                                    			//如果使能了统计任务计算只有空闲任务运行时CPU的（最大）容量
    OSStatTaskCPUUsageInit(&err);                               			//（决定 OS_Stat_IdleCtrMax 的值，为后面计算 CPU 使用率使用）。
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN												//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u										//如果使能了时间片轮转调度法
	OSSchedRoundRobinCfg(DEF_ENABLED,10,&err);								//时间片长度=10ms
#endif
	
	/* 创建互斥信号量 */
	OSMutexCreate ((OS_MUTEX  *)&SIM900A_mutex,
                   (CPU_CHAR  *)"SIM900A Mutex",
                   (OS_ERR    *)&err);
	
	/*创建1s定时器*/
	OSTmrCreate((OS_TMR		*)&TMR_1s,
				(CPU_CHAR	*)"TMR 1s",
				(OS_TICK	 )10,
				(OS_TICK	 )10,
				(OS_OPT		 )OS_OPT_TMR_PERIODIC,
				(OS_TMR_CALLBACK_PTR)TMR_1sCallback,
				(void		*)0,
				(OS_ERR		 *)&err);
	
	/* 创建事件标志组 */
	OSFlagCreate((OS_FLAG_GRP	*)&SIM900A_Flag,
				 (CPU_CHAR		*)"SIM900A Flag",
				 (OS_FLAGS		 )0,
				 (OS_ERR		*)&err);
				
	OSFlagCreate((OS_FLAG_GRP	*)&Ctl_Data.Flag,
				 (CPU_CHAR		*)"Play Wav Play",
				 (OS_FLAGS		 )DMA1_7_flag|System_Startup_flag,
				 (OS_ERR		*)&err);
				
	OSFlagCreate((OS_FLAG_GRP	*)&Wav_Play.Flag,
				 (CPU_CHAR		*)"Play Wav Play",
				 (OS_FLAGS		 )DMA1_5_flag,
				 (OS_ERR		*)&err);
	
	OSFlagCreate((OS_FLAG_GRP	*)&Update_Flag,
				 (CPU_CHAR		*)"Update File",
				 (OS_FLAGS		 )0,
				 (OS_ERR		*)&err);
				
	/* 开机控制流程 */
	OSTaskCreate((OS_TCB     *)&ControlTCB,
                 (CPU_CHAR   *)"Control",
                 (OS_TASK_PTR ) AppTaskControl,
                 (void       *) 0,
                 (OS_PRIO     ) Control_PRIO,
                 (CPU_STK    *)&ControlStk[0],
                 (CPU_STK_SIZE) Control_STK_SIZE / 10,
                 (CPU_STK_SIZE) Control_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
	
	/*MPU6050数据采集*/
	OSTaskCreate((OS_TCB     *)&DMPTCB,
                 (CPU_CHAR   *)"DMP",
                 (OS_TASK_PTR ) AppTaskDMP,
                 (void       *) 0,
                 (OS_PRIO     ) DMP_PRIO,
                 (CPU_STK    *)&DMPStk[0],
                 (CPU_STK_SIZE) DMP_STK_SIZE / 10,
                 (CPU_STK_SIZE) DMP_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
	
	/*LED状态更新*/
	OSTaskCreate((OS_TCB     *)&LED_CtrTCB,
                 (CPU_CHAR   *)"LEDCtr",
                 (OS_TASK_PTR ) AppTaskLEDCtr,
                 (void       *) 0,
                 (OS_PRIO     ) LEDCtr_PRIO,
                 (CPU_STK    *)&LED_CtrStk[0],
                 (CPU_STK_SIZE) LEDCtr_STK_SIZE / 10,
                 (CPU_STK_SIZE) LEDCtr_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
				 
	/*触发判断*/
	OSTaskCreate((OS_TCB     *)&Trigger_CtrTCB,
                 (CPU_CHAR   *)"Trigger",
                 (OS_TASK_PTR ) AppTaskTrigger,
                 (void       *) 0,
                 (OS_PRIO     ) Trigger_PRIO,
                 (CPU_STK    *)&Trigger_CtrStk[0],
                 (CPU_STK_SIZE) Trigger_STK_SIZE / 10,
                 (CPU_STK_SIZE) Trigger_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
				 
	/*音乐播放*/
	OSTaskCreate((OS_TCB     *)&Music_Play_CtrTCB,
                 (CPU_CHAR   *)"Music Play",
                 (OS_TASK_PTR ) AppTaskMusicPlay,
                 (void       *) 0,
                 (OS_PRIO     ) Music_Play_PRIO,
                 (CPU_STK    *)&Music_Play_CtrStk[0],
                 (CPU_STK_SIZE) Music_Play_STK_SIZE / 10,
                 (CPU_STK_SIZE) Music_Play_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
				 
	/*GPRS接收数据处理*/
								 
	OSTaskCreate((OS_TCB     *)&Serial_port_CtrTCB,
                 (CPU_CHAR   *)"GPRS",
                 (OS_TASK_PTR ) AppTaskSerial_port,
                 (void       *) 0,
                 (OS_PRIO     ) Serial_port_PRIO,
                 (CPU_STK    *)&Serial_port_CtrStk[0],
                 (CPU_STK_SIZE) Serial_port_STK_SIZE / 10,
                 (CPU_STK_SIZE) Serial_port_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
		
	/*按键扫描任务*/
	OSTaskCreate((OS_TCB     *)&KEYTCB,
                 (CPU_CHAR   *)"Key Test",
                 (OS_TASK_PTR ) AppTaskKey,
                 (void       *) 0,
                 (OS_PRIO     ) KEY_PRIO,
                 (CPU_STK    *)&KEYStk[0],
                 (CPU_STK_SIZE) KEY_STK_SIZE / 10,
                 (CPU_STK_SIZE) KEY_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
				 
	/*ADC数据处理任务*/
	OSTaskCreate((OS_TCB     *)&ADCTCB,
                 (CPU_CHAR   *)"ADC data processing",
                 (OS_TASK_PTR ) AppTaskADC,
                 (void       *) 0,
                 (OS_PRIO     ) ADC_PRIO,
                 (CPU_STK    *)&ADCStk[0],
                 (CPU_STK_SIZE) KEY_STK_SIZE / 10,
                 (CPU_STK_SIZE) KEY_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
	
	OSTaskDel(&STARTTCB,&err);
}

static void AppTaskControl(void *p_arg)
{
	OS_ERR err;
	OS_FLAGS	flags;
	
	while(DEF_TRUE)
	{
		/* 等待相应事件发生 */
		OSFlagPend((OS_FLAG_GRP *)&Ctl_Data.Flag,
				   (OS_FLAGS	 )Game_Begins|Game_Over_flag|SIM900A_RST_flag|System_Startup_flag,
				   (OS_TICK 	 )0,
				   (OS_OPT  	 )OS_OPT_PEND_FLAG_SET_ANY,
				   (CPU_TS 		*)0,
				   (OS_ERR 		*)&err);
		
		switch(err)
		{
			case OS_ERR_NONE:
				flags = OSFlagPendGetFlagsRdy(&err);
				System_Control(flags);
				OSFlagPost(&Ctl_Data.Flag,flags,OS_OPT_POST_FLAG_CLR,&err);
				break;
			case OS_ERR_TIMEOUT:
				break;
			default:
				break;
		}
	}
}

static void AppTaskDMP(void *p_arg)
{
	OS_ERR err;
	
	while(DEF_TRUE)
	{
		/* 等待DMP数据中断信号 */
		OSTaskSemPend((OS_TICK		)0,
					  (OS_OPT		)OS_OPT_PEND_BLOCKING,
					  (CPU_TS	   *)0,
					  (OS_ERR	   *)&err);
		
		Read_DMP();
	}
}

static void AppTaskTrigger(void *p_arg)
{
	OS_ERR err;
	OSFlagPend((OS_FLAG_GRP *)&Ctl_Data.Flag,
				   (OS_FLAGS	 )System_Startup_finished_flag,
				   (OS_TICK 	 )0,
				   (OS_OPT  	 )OS_OPT_PEND_FLAG_SET_ANY,
				   (CPU_TS 		*)0,
				   (OS_ERR 		*)&err);
	Debug_SendMsg("APP", "wait to startup finished.", DEBUG_MSG_INFO);
	//pgTankWar -> funcInit();
	while(DEF_TRUE)
	{
		//pgTankWar -> funcGame();
		gamePlatformLoop();
		OSTimeDly(20,OS_OPT_TIME_DLY,&err);
	}
}

static void AppTaskLEDCtr(void *p_arg)
{
	OS_ERR err;
	
	while(DEF_TRUE)
	{
		OSTaskSemPend((OS_TICK	 )0,
					  (OS_OPT	 )OS_OPT_PEND_BLOCKING,
					  (CPU_TS	*)0,
					  (OS_ERR	*)&err);
		
		CLR_SPI_CS1;
		SPI_Send_Data();
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);
		SET_SPI_CS1;
	}
}

static void AppTaskMusicPlay(void *p_arg)
{
	OS_ERR	err;
	while(DEF_TRUE)
	{
//		if(Ctl_Data.updata_state==0)
//		{
			FatReadDir(Directory_path,"wav");
			while(DEF_TRUE)
			{
				OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);
				WAV_Play();
			}
//		}
		OSTimeDly(20,OS_OPT_TIME_DLY,&err);
	}
}

static void AppTaskSerial_port(void *p_arg)
{
	OS_ERR err;
	
	while(DEF_TRUE)
	{
		OSTaskSemPend((OS_TICK		)0,
					  (OS_OPT		)OS_OPT_PEND_BLOCKING,
					  (CPU_TS	   *)0,
					  (OS_ERR	   *)&err);
		
		switch(Ctl_Data.Com_Mode)
		{
			case 0:
				Data_processing();
				break;
			case 1:
				Data_verification();
				break;
			default:
				break;
		}
	}
}

static void AppTaskKey(void *p_arg)
{
	OS_ERR	err;
	
	while(DEF_TRUE)
	{
		Key_Test();
		OSTimeDly(10,OS_OPT_TIME_DLY,&err);
	}
}

static void AppTaskADC(void *p_arg)
{
	OS_ERR	err;
	
	while(DEF_TRUE)
	{
		ADC_Data_Sorting();
		OSTimeDly(10,OS_OPT_TIME_DLY,&err);
	}
}


static void TMR_1sCallback(OS_TMR *p_tmr, void *p_arg)
{
	
	OS_ERR err;
	piano_time_count++;
	Time_Count++;
	// Debug_SendMsg("APP", "TMR_task", DEBUG_MSG_INFO);
	// Debug_SendMsg("APP", "TMR_task", DEBUG_MSG_INFO);
	if(Ctl_Data.Game_State == 0x01)
	{
		if(Ctl_Data.Count >= (Ctl_Data.Game_Time*60))
		{
			/* 游戏结束 */
			Debug_SendMsg("APP", "Time Up!", DEBUG_MSG_INFO);
			OSFlagPost(&Ctl_Data.Flag,Game_Over_flag,OS_OPT_POST_FLAG_SET,&err);
			Ctl_Data.Count = 0;
		}
		Ctl_Data.Count++;
		char str[50];
		sprintf(str, "%d / %d", (int)Ctl_Data.Count, (int)Ctl_Data.Game_Time*60);
		Debug_SendMsg("APP", str, DEBUG_MSG_INFO);
	}	
	heartcount++;
if(gprsflag&&(heartcount>5))
 {
		heartcount=0;
		Reply_message(heartbeat_message);			//应答
		Ctl_Data.HeartSendTime++;
		Ctl_Data.Heartbeat_count = 0;			//清除心跳时间
		Ctl_Data.Heartbeat_flag = 1;			//监测到心跳
	 if(NetCheck_Ok()==1)
		Debug_SendMsg("APP", "NetErr", DEBUG_MSG_INFO);
	 else
		Debug_SendMsg("APP", "NetOk", DEBUG_MSG_INFO);
		char str[50];
		sprintf(str, "%d",(int)Ctl_Data.HeartSendTime);
		Debug_SendMsg("APP_Send_heart/HeartSendTime=",str, DEBUG_MSG_INFO);
 }
 if((NetCheck_Ok()==1)&&(gprsflag==1))
 {
		Ctl_Data.NetErrTime++;
 }
 else
 {
		Ctl_Data.NetErrTime=0;
 }
 
 if(Ctl_Data.NetErrTime>40)
 {
	 Ctl_Data.NetErrTime=0;
	 gprsflag=0;
	 Debug_SendMsg("APP", "NetReset", DEBUG_MSG_INFO);
	 SIM900A_Init();
	 
 }
 
 if(Ctl_Data.HeartSendTime>15)
 {
	 Ctl_Data.HeartSendTime=0;
	 Debug_SendMsg("APP","SoftReset", DEBUG_MSG_INFO);
	 SoftReset();
 }
	 
}
