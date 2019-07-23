#ifndef __GAMEAPIADV_H
#define __GAMEAPIADV_H
#include "stm32f10x.h"
#include "gui.h"
#include "gameapi.h"

#define CBTP_GC_CLONE			0
#define CBTP_GC_SHOW			1
#define CBTP_GC_HIDE			2
#define CBTP_GC_MVX				3
#define CBTP_GC_MVY				4
#define CBTP_GC_NEW				5
#define CBTP_GC_FLPLR			6
#define CBTP_GC_FLPLR2			7
#define CBTP_GC_FLPUD			8
#define CBTP_GC_FLPUD2			9
#define CBTP_GC_STLOC			10
#define CBTP_SCORE_LEFT_INC		11
#define CBTP_SCORE_LEFT_DEC		12
#define CBTP_SCORE_RIGHT_INC	13
#define CBTP_SCORE_RIGHT_DEC	14
#define CBTP_SCEN_SWITCH 		15
#define EVTP_TMR				16
#define EVTP_CLL				17
#define EVTP_CNT				18
#define EVTP_EXT				19
#define CBTP_GC_DEL				20
#define EVTP_NONE				21
#define CBTP_CUSTOM				22
#define CBTP_EVNT_STUP			23
#define CBTP_SUICIDE			24
#define GC_DELMD_GLIST			25	// G class delete mode : delete G class in GList
#define GC_DELMD_CLSN1			26	// G class delete mode : delete the first G class of collision event
#define GC_DELMD_CLSN2			27	// G class delete mode : delete the second G class of collision event
#define GC_DELMD_TMRGC			28	// G class delete mode : delete G class of timer event
#define CBTP_EVNT_DEL			29
#define CBTP_GC_ADD_SHAPE		30
#define CBTP_PLAY_WAV			31

#define MAT_SCEN_IND			0
#define MAT_EVNT_IND			14
#define MAT_EVNT_TYPE			1
#define MAT_CL_PGCLASS1			2
#define MAT_CL_PGCLASS2			3
#define MAT_TMR_PGCLASS			2
#define MAT_TMR_CNT				3
#define MAT_TMR_CNT_RST			4
#define MAT_TMR_ONCE_FLAG		5	
#define MAT_CNT_CNT				2
#define MAT_CNT_CNT_TOP			3
#define MAT_CNT_TRIG_ERSIDE		4
#define MAT_EXT_MONITOR			2
#define MAT_EVNT_PTR			6
#define MAT_CBTP				7
#define MAT_CBPR1				8	
#define MAT_CBPR2				9
#define MAT_CBPR3				10
#define MAT_CBPR4				11
#define MAT_CBPR5				12
#define MAT_CBPR6 				13

#define MATLEN 15
#define GLIST_LEN 20
#define GLIST_TMP_LEN 20

#define SCORE_MODE_MARCH 0
#define SCORE_MODE_NORMAL 1
typedef int32_t GAME_DEFINE_TYPE;

typedef struct GameInfo{
	G_CLASS * glist[GLIST_LEN];
	GAME_DEFINE_TYPE ** mat;
	u8 matSize;
	u8 scoreLeft;
	u8 scoreRight;
	u8 score2Win;
	u8 scoreMode;
	u8 curScen;
}GAME_INFO;

void scenSetup(u8 scenIndex);
void eventCounterSetup(u8 matIte);
void eventTimerSetup(u8 matIte);
void eventExternalSetup(u8 matIte);
void eventCollisionSetup(u8 matIte);
void switchScenTo(u8 scenIndex);
void callBackCNT(EVENT_COUNTER * event);
void callBackCLL(EVENT_COLLISION * event);
void callBackTMR(EVENT_TIMER * event);
void callBackEXT(EVENT_EXT * event);
void action(u8 matIte, G_CLASS ** glistTmp);


#endif
