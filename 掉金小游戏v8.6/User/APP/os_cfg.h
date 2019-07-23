/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2010; Micrium, Inc.; Weston, FL
*                          All rights reserved.  Protected by international copyright laws.
*
*                                                  CONFIGURATION  FILE
*
* File    : OS_CFG.H
* By      : JJL
* Version : V3.01.2
*
* LICENSING TERMS:
* ---------------
*               uC/OS-III is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
************************************************************************************************************************
*/

#ifndef OS_CFG_H
#define OS_CFG_H


                                             /* --------------------------------- 杂项 ------------------------------ */
#define OS_CFG_APP_HOOKS_EN             0u   /* Enable (1) or Disable (0) 钩子函数功能                                */
#define OS_CFG_ARG_CHK_EN               0u   /* Enable (1) or Disable (0) 参数检查功能                                */
#define OS_CFG_CALLED_FROM_ISR_CHK_EN   0u   /* Enable (1) or Disable (0) 中断调用系统函数功能                        */
#define OS_CFG_DBG_EN                   1u   /* Enable (1)          debug 功能                                        */
#define OS_CFG_ISR_POST_DEFERRED_EN     0u   /* Enable (1) or Disable (0) 关中断或锁调度锁保护临界段                  */
#define OS_CFG_OBJ_TYPE_CHK_EN          0u   /* Enable (1) or Disable (0) 对象类型检测功能                            */
#define OS_CFG_TS_EN                    1u   /* Enable (1) or Disable (0) 时间戳                                      */

#define OS_CFG_PEND_MULTI_EN            0u   /* Enable (1) or Disable (0) 多事件等待功能                              */

#define OS_CFG_PRIO_MAX                64u   /* 最大优先级数量 (see OS_PRIO data type)                                */

#define OS_CFG_SCHED_LOCK_TIME_MEAS_EN  0u   /* 调度锁时间检测功能                                                    */
#define OS_CFG_SCHED_ROUND_ROBIN_EN     0u   /* 时间片轮转功能                                                        */
#define OS_CFG_STK_SIZE_MIN            64u   /* 任务堆栈的最小值                                                      */


                                             /* -------------------------------- 事件管理 --------------------------- */
#define OS_CFG_FLAG_EN                  1u   /* Enable (1) or Disable (0) 事件标志功能                                */
#define OS_CFG_FLAG_DEL_EN              0u   /* 事件标志删除功能                                OSFlagDel()           */
#define OS_CFG_FLAG_MODE_CLR_EN         0u   /* 清除等待事件标志功能                                                  */
#define OS_CFG_FLAG_PEND_ABORT_EN       0u   /* 终止标志事件等待功能                            OSFlagPendAbort()     */


                                             /* ------------------------------- 内存管理 ---------------------------- */
#define OS_CFG_MEM_EN                   0u   /* Enable (1) or Disable (0) 内存管理功能                                */

                                             /* ------------------------------ 互斥信号量 --------------------------- */
#define OS_CFG_MUTEX_EN                 1u   /* Enable (1) or Disable (0) 互斥量功能                                  */
#define OS_CFG_MUTEX_DEL_EN             1u   /* 互斥信号量删除功能                               OSMutexDel()         */
#define OS_CFG_MUTEX_PEND_ABORT_EN      1u   /* 终止互斥信号量等待功能                           OSMutexPendAbort()   */


                                             /* ------------------------------ 消息队列 ----------------------------- */
#define OS_CFG_Q_EN                     0u   /* Enable (1) or Disable (0) 消息队列功能                                */
#define OS_CFG_Q_DEL_EN                 0u   /* 队列删除功能                                     OSQDel()             */
#define OS_CFG_Q_FLUSH_EN               0u   /* 消息队列刷新功能                                 OSQFlush()           */
#define OS_CFG_Q_PEND_ABORT_EN          0u   /* 终止队列等待功能                                 OSQPendAbort()       */


                                             /* ------------------------------- 信号量 ------------------------------ */
#define OS_CFG_SEM_EN                   1u   /* Enable (1) or Disable (0) 信号量功能                                  */
#define OS_CFG_SEM_DEL_EN               1u   /* 信号量删除功能                                   OSSemDel()           */
#define OS_CFG_SEM_PEND_ABORT_EN        1u   /* 终止信号量等待功能                               OSSemPendAbort()     */
#define OS_CFG_SEM_SET_EN               1u   /* 信号量设置功能                                   OSSemSet()           */


                                             /* ------------------------------ 任务管理 ----------------------------- */
#define OS_CFG_STAT_TASK_EN             1u   /* Enable (1) or Disable(0) 统计任务						              */
#define OS_CFG_STAT_TASK_STK_CHK_EN     1u   /* 统计任务堆栈检测功能                                                  */

#define OS_CFG_TASK_CHANGE_PRIO_EN      0u   /* 优先级调节功能                                   OSTaskChangePrio()   */
#define OS_CFG_TASK_DEL_EN              1u   /* 任务删除功能                                     OSTaskDel()          */
#define OS_CFG_TASK_Q_EN                0u   /* 任务消息队列功能                                 OSTaskQXXXX()        */
#define OS_CFG_TASK_Q_PEND_ABORT_EN     0u   /* 终止任务消息队列等待功能                         OSTaskQPendAbort()   */
#define OS_CFG_TASK_PROFILE_EN          1u   /* 任务详细状态功能                                                      */
#define OS_CFG_TASK_REG_TBL_SIZE        1u   /* 任务特殊功能寄存器                                                    */
#define OS_CFG_TASK_SEM_PEND_ABORT_EN   0u   /* 终止任务信号量等待功能                           OSTaskSemPendAbort() */
#define OS_CFG_TASK_SUSPEND_EN          1u   /* 任务暂时终止和恢复功能           OSTaskSuspend() and OSTaskResume()   */


                                             /* ----------------------------- 时间管理 ------------------------------ */
#define OS_CFG_TIME_DLY_HMSM_EN         0u   /* 时间延时函数功能                                 OSTimeDlyHMSM()      */
#define OS_CFG_TIME_DLY_RESUME_EN       0u   /* 时间延时取消功能                                 OSTimeDlyResume()    */


                                             /* ---------------------------- 定时器管理 ----------------------------- */
#define OS_CFG_TMR_EN                   1u   /* Enable (1) or Disable (0) 定时器功能                                  */
#define OS_CFG_TMR_DEL_EN               0u   /* Enable (1) or Disable (0) 定时器删除功能         OSTmrDel()           */

#endif
