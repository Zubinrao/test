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


                                             /* --------------------------------- ���� ------------------------------ */
#define OS_CFG_APP_HOOKS_EN             0u   /* Enable (1) or Disable (0) ���Ӻ�������                                */
#define OS_CFG_ARG_CHK_EN               0u   /* Enable (1) or Disable (0) ������鹦��                                */
#define OS_CFG_CALLED_FROM_ISR_CHK_EN   0u   /* Enable (1) or Disable (0) �жϵ���ϵͳ��������                        */
#define OS_CFG_DBG_EN                   1u   /* Enable (1)          debug ����                                        */
#define OS_CFG_ISR_POST_DEFERRED_EN     0u   /* Enable (1) or Disable (0) ���жϻ��������������ٽ��                  */
#define OS_CFG_OBJ_TYPE_CHK_EN          0u   /* Enable (1) or Disable (0) �������ͼ�⹦��                            */
#define OS_CFG_TS_EN                    1u   /* Enable (1) or Disable (0) ʱ���                                      */

#define OS_CFG_PEND_MULTI_EN            0u   /* Enable (1) or Disable (0) ���¼��ȴ�����                              */

#define OS_CFG_PRIO_MAX                64u   /* ������ȼ����� (see OS_PRIO data type)                                */

#define OS_CFG_SCHED_LOCK_TIME_MEAS_EN  0u   /* ������ʱ���⹦��                                                    */
#define OS_CFG_SCHED_ROUND_ROBIN_EN     0u   /* ʱ��Ƭ��ת����                                                        */
#define OS_CFG_STK_SIZE_MIN            64u   /* �����ջ����Сֵ                                                      */


                                             /* -------------------------------- �¼����� --------------------------- */
#define OS_CFG_FLAG_EN                  1u   /* Enable (1) or Disable (0) �¼���־����                                */
#define OS_CFG_FLAG_DEL_EN              0u   /* �¼���־ɾ������                                OSFlagDel()           */
#define OS_CFG_FLAG_MODE_CLR_EN         0u   /* ����ȴ��¼���־����                                                  */
#define OS_CFG_FLAG_PEND_ABORT_EN       0u   /* ��ֹ��־�¼��ȴ�����                            OSFlagPendAbort()     */


                                             /* ------------------------------- �ڴ���� ---------------------------- */
#define OS_CFG_MEM_EN                   0u   /* Enable (1) or Disable (0) �ڴ������                                */

                                             /* ------------------------------ �����ź��� --------------------------- */
#define OS_CFG_MUTEX_EN                 1u   /* Enable (1) or Disable (0) ����������                                  */
#define OS_CFG_MUTEX_DEL_EN             1u   /* �����ź���ɾ������                               OSMutexDel()         */
#define OS_CFG_MUTEX_PEND_ABORT_EN      1u   /* ��ֹ�����ź����ȴ�����                           OSMutexPendAbort()   */


                                             /* ------------------------------ ��Ϣ���� ----------------------------- */
#define OS_CFG_Q_EN                     0u   /* Enable (1) or Disable (0) ��Ϣ���й���                                */
#define OS_CFG_Q_DEL_EN                 0u   /* ����ɾ������                                     OSQDel()             */
#define OS_CFG_Q_FLUSH_EN               0u   /* ��Ϣ����ˢ�¹���                                 OSQFlush()           */
#define OS_CFG_Q_PEND_ABORT_EN          0u   /* ��ֹ���еȴ�����                                 OSQPendAbort()       */


                                             /* ------------------------------- �ź��� ------------------------------ */
#define OS_CFG_SEM_EN                   1u   /* Enable (1) or Disable (0) �ź�������                                  */
#define OS_CFG_SEM_DEL_EN               1u   /* �ź���ɾ������                                   OSSemDel()           */
#define OS_CFG_SEM_PEND_ABORT_EN        1u   /* ��ֹ�ź����ȴ�����                               OSSemPendAbort()     */
#define OS_CFG_SEM_SET_EN               1u   /* �ź������ù���                                   OSSemSet()           */


                                             /* ------------------------------ ������� ----------------------------- */
#define OS_CFG_STAT_TASK_EN             1u   /* Enable (1) or Disable(0) ͳ������						              */
#define OS_CFG_STAT_TASK_STK_CHK_EN     1u   /* ͳ�������ջ��⹦��                                                  */

#define OS_CFG_TASK_CHANGE_PRIO_EN      0u   /* ���ȼ����ڹ���                                   OSTaskChangePrio()   */
#define OS_CFG_TASK_DEL_EN              1u   /* ����ɾ������                                     OSTaskDel()          */
#define OS_CFG_TASK_Q_EN                0u   /* ������Ϣ���й���                                 OSTaskQXXXX()        */
#define OS_CFG_TASK_Q_PEND_ABORT_EN     0u   /* ��ֹ������Ϣ���еȴ�����                         OSTaskQPendAbort()   */
#define OS_CFG_TASK_PROFILE_EN          1u   /* ������ϸ״̬����                                                      */
#define OS_CFG_TASK_REG_TBL_SIZE        1u   /* �������⹦�ܼĴ���                                                    */
#define OS_CFG_TASK_SEM_PEND_ABORT_EN   0u   /* ��ֹ�����ź����ȴ�����                           OSTaskSemPendAbort() */
#define OS_CFG_TASK_SUSPEND_EN          1u   /* ������ʱ��ֹ�ͻָ�����           OSTaskSuspend() and OSTaskResume()   */


                                             /* ----------------------------- ʱ����� ------------------------------ */
#define OS_CFG_TIME_DLY_HMSM_EN         0u   /* ʱ����ʱ��������                                 OSTimeDlyHMSM()      */
#define OS_CFG_TIME_DLY_RESUME_EN       0u   /* ʱ����ʱȡ������                                 OSTimeDlyResume()    */


                                             /* ---------------------------- ��ʱ������ ----------------------------- */
#define OS_CFG_TMR_EN                   1u   /* Enable (1) or Disable (0) ��ʱ������                                  */
#define OS_CFG_TMR_DEL_EN               0u   /* Enable (1) or Disable (0) ��ʱ��ɾ������         OSTmrDel()           */

#endif
