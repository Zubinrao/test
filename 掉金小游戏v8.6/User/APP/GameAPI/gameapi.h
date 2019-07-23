#ifndef __GAMEAPI_H
#define __GAMEAPI_H
#include "stm32f10x.h"
#include "gui.h"
#define GS_INIT 0	// Game State：Initailization
#define GS_PLAY 1	// Game State：Playing
#define GS_INTERVAL 2	// Game State：Interval
#define GS_SELET 3	// Game State：Selection
#define GS_PAYING 4 // Game State: waiting for paying

#define EVENT_COUNTER_TRG_LAGER 0	// 计数器触发方向大于等于
#define EVENT_COUNTER_TRG_SMALLER 1	// 计数器触发方向小于等于
#define EVENT_TIMER_ONCE 1	// 计时器只触发一次
#define EVENT_TIMER_MULT 0	// 计时器触发多次


/***************
	数据结构体
****************/

// 游戏类
typedef struct{
	//void (*funcReg)(void);	// 注册游戏
	void (*funcInit)(void);
	void (*funcGame)(void);	// 在这里面，游戏必须要调用generalLoop()函数
	void (*funcTerminate)(void);
    const u32 ** pLogo;	// 当前形状结构体指针
    const u32 ** pLogoHL;	// 所有角色形状的链表
	int nameWAV;
} GAME_CLASS;


// 游戏注册结构体
// typedef struct gameListNode{
	// GAME_CLASS * pGameClass;
	// struct gameListNode * next;
// }GAME_NODE;

// 游戏状态机
// typedef struct{
	// u8	gameState;	// Game State
	// u8	gameStateNext;	// Next Game State - if gameState != gameStateNext then go from gameState to gameStateNext
	// GAME_NODE * currentGame;
	// GAME_NODE * gameList;
// } GAME_STM;	// game state machine

// 碰撞事件
typedef struct eventCollision{
	G_CLASS * pGclass1;
	G_CLASS * pGclass2;
    int8_t state;
	void (*callBack)(struct eventCollision *);
}EVENT_COLLISION;

typedef struct eventCollisionListNode{
	EVENT_COLLISION * pEventCollision;
	struct eventCollisionListNode * next;
}EVNT_CLSN_NODE;

// 计时器事件
typedef struct eventTimer{
	G_CLASS * pGclass;
	u16 cnt;		// 计数器
	u16 cnt_rst;	// 计数器重载值
	u8 once_flag;	// 一次性事件标志
	void (*callBack)(struct eventTimer *);
}EVENT_TIMER;

typedef struct eventTimerListNode{
	EVENT_TIMER * pEventTimer;
	struct eventTimerListNode * next;
}EVNT_TMR_NODE;

// 计数器事件
typedef struct eventCounter{
	u16 cnt;
	u16 cnt_top;
	u8 triger_side;	// 触发方向：0：>=；1：<=
	void (*callBack)(struct eventCounter *);
}EVENT_COUNTER;

typedef struct eventCounterListNode{
	EVENT_COUNTER * pEventCounter;
	struct eventCounterListNode * next;
}EVNT_CNT_NODE;

// 外部事件
typedef struct eventExternal{
	G_CLASS * pGclass;
	u8 (*monitoring)(struct eventExternal *);	// 如果事件触发返回1
	void (*callBack)(struct eventExternal *);
}EVENT_EXT;

typedef struct eventExternalNode{
	EVENT_EXT * pEventExternal;
	struct eventExternalNode * next;
}EVNT_EXT_NODE;

/***************
	数据结构声明
****************/
// extern GAME_STM gameStateMachine;	//状态机
// extern EVNT_CLSN_NODE * eventCollisionListHead;	// 碰撞事件链表
// extern EVNT_TMR_NODE * eventTimerListHead;			// 计时器事件链表
// extern EVNT_CNT_NODE * eventCounterListHead;		// 计数器事件链表
// extern EVNT_EXT_NODE * eventExternalListHead;		// 外部事件链表

/***************
	函数声明
****************/
int16_t max(int16_t a, int16_t b);
int16_t min(int16_t a, int16_t b);
void gameAPIInit(void);
void gameReg(GAME_CLASS* pGameClass);
EVENT_COLLISION * appendECLL(EVENT_COLLISION eventCollision);
EVENT_TIMER * appendETML(EVENT_TIMER eventTimer);
EVENT_COUNTER * appendECTL(EVENT_COUNTER eventCounter);
EVENT_EXT * appendEEXL(EVENT_EXT eventExternal);
u8 cutOffECLLNode(EVENT_COLLISION * pEventCollision);
u8 cutOffECLLNodeByGclass1(G_CLASS * pGclass);
u8 cutOffECLLNodeByGclass2(G_CLASS * pGclass);
u8 cutOffECTLNode(EVENT_COUNTER * pEventCounter);
u8 cutOffETMLNode(EVENT_TIMER * pEventTimer);
u8 cutOffETMLNodeByGclass(G_CLASS * pGclass);
u8 cutOffEEXLNode(EVENT_EXT * pEventExternal);
void destructECLL(void);
void destructECTL(void);
void destructETML(void);
void destructEEXL(void);
u8 isCollided(EVENT_COLLISION * ecl);
u8 isTimeUp(EVENT_TIMER * pEventTimer);
u8 isCounterOver(EVENT_COUNTER * pEventCounter);
void eventScaner(void);
void generalLoop(void);
void checkETML(void);
void generalTerminate(void);
void outputLists(void);
void selectInit(void);
void select(void);
#endif
