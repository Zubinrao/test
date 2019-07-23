#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/***BSPÅäÖÃ£ºRS-232***/

/***¸ú×Ù/µ÷ÊÔÅäÖÃ***/
#if 0
#define  TRACE_LEVEL_OFF              0
#define  TRACE_LEVEL_INFO             1
#define  TRACE_LEVEL_DEBUG            2
#endif

#define  APP_TRACE_LEVEL              TRACE_LEVEL_INFO
#define  APP_TRACE                    BSP_Ser_Printf

#define  APP_TRACE_INFO(x)            ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DEBUG(x)           ((APP_TRACE_LEVEL >= TRACE_LEVEL_DEBUG) ? (void)(APP_TRACE x) : (void)0)


#endif

