/* Game APIs */
#include "gameapi.h"
#include <stdlib.h>
#include "Control.h"
#include "debug.h"
/***************
	数据结构
****************/
GAME_STM gameStateMachine;	//状态机
EVNT_CLSN_NODE * eventCollisionListHead;	// 碰撞事件链表
EVNT_TMR_NODE * eventTimerListHead;			// 计时器事件链表
EVNT_CNT_NODE * eventCounterListHead;		// 计数器事件链表
EVNT_EXT_NODE * eventExternalListHead;		// 外部事件链表

/***************
	函数实现
****************/


//通用算法
u8 max(u8 a, u8 b){
	if(a>b){
		return a;
	}else{
		return b;
	}
}

u8 min(u8 a, u8 b){
	if(a>b){
		return b;
	}else{
		return a;
	}
}

// 游戏API开机初始化
/** @brief  初始化
  * @param  None
  * @retval None
  */
void gameAPIInit(){
	// 初始化状态机
	
	// 初始化事件链表指针
	eventCollisionListHead = (EVNT_CLSN_NODE*)0;
	eventTimerListHead = (EVNT_TMR_NODE*)0;
	eventCounterListHead = (EVNT_CNT_NODE*)0;
	eventExternalListHead = (EVNT_EXT_NODE*)0;
	gameStateMachine.gameList = (GAME_NODE*)0;
	gameStateMachine.currentGame = (GAME_NODE*)0;
	Debug_SendMsg("GameAPI","event lists initialization finished.", DEBUG_MSG_INFO);
	// 初始化GUI
	guiInit();
}

/** @brief  游戏注册函数，注册到游戏切换状态机中【下一版弃用】
  * @param  None
  * @retval None
  */

void gameReg(GAME_CLASS* pGameClass){
	
	GAME_NODE * pGameNode = (GAME_NODE*)malloc(sizeof(GAME_NODE));
	pGameNode -> pGameClass = pGameClass;
	pGameNode -> next = (GAME_NODE*)0;
	GAME_NODE * pSearch = gameStateMachine.gameList;
	//向链表添加节点
	if(pSearch == (GAME_NODE*)0){	//	如果链表为空
		gameStateMachine.gameList = pGameNode;
	}else{	//如果链表不为空
		// 查找链表尾
		while(pSearch -> next != (GAME_NODE *)0){
			pSearch = pSearch -> next;
		}
		pSearch -> next = pGameNode;
	}
	
}

// 添加事件
/** @brief  添加碰撞事件
  * @param  碰撞事件结构体
  * @retval 链表中碰撞事件的指针
  */
EVENT_COLLISION * appendECLL(EVENT_COLLISION eventCollision){
	//复制事件
	EVENT_COLLISION * pEVT = (EVENT_COLLISION*)malloc(sizeof(EVENT_COLLISION));
	pEVT -> pGclass1 = eventCollision.pGclass1;
	pEVT -> pGclass2 = eventCollision.pGclass2;
	pEVT -> callBack = eventCollision.callBack;
	//建立链表节点
	EVNT_CLSN_NODE * pEVTNode = (EVNT_CLSN_NODE*)malloc(sizeof(EVNT_CLSN_NODE));
	pEVTNode -> pEventCollision = pEVT;
	pEVTNode -> next = (EVNT_CLSN_NODE*)0;
	EVNT_CLSN_NODE * pSearch = eventCollisionListHead;
	//向链表添加节点
	if(pSearch == (EVNT_CLSN_NODE*)0){	//	如果链表为空
		eventCollisionListHead = pEVTNode;
	}else{	//如果链表不为空
		// 查找链表尾
		while(pSearch -> next != (EVNT_CLSN_NODE *)0){
			pSearch = pSearch -> next;
		}
		pSearch -> next = pEVTNode;
	}
	return pEVT;
}


/** @brief  向串口输出所有时间事件链表
  * @param  None
  * @retval None
  */
void checkETML(){
	EVNT_TMR_NODE * pSearch = eventTimerListHead;
	if(pSearch != (EVNT_TMR_NODE*)0){	//	如果链表为空
		// 查找链表尾
		while(pSearch -> next != (EVNT_TMR_NODE *)0){
			char * str[50];
			sprintf(str, "check address : %x", (int)(pSearch -> next));
			Debug_SendMsg("GameAPI", str, DEBUG_MSG_INFO);
			pSearch = pSearch -> next;
		}
		char * str[50];
		sprintf(str, "check tial : %x", (int)(pSearch -> next));
		Debug_SendMsg("GameAPI", str, DEBUG_MSG_INFO);
	}
}

/** @brief  添加计时器事件
  * @param  计时器事件结构体
  * @retval 链表中计时器事件的指针
  */
EVENT_TIMER *  appendETML(EVENT_TIMER eventTimer){
	//复制事件
	EVENT_TIMER * pEVT = (EVENT_TIMER*)malloc(sizeof(EVENT_TIMER));
	pEVT -> pGclass = eventTimer.pGclass;
	pEVT -> cnt = eventTimer.cnt;
	pEVT -> cnt_rst = eventTimer.cnt_rst;
	pEVT -> once_flag = eventTimer.once_flag;
	pEVT -> callBack = eventTimer.callBack;
	//建立链表节点
	EVNT_TMR_NODE * pEVTNode = (EVNT_TMR_NODE*)malloc(sizeof(EVNT_TMR_NODE));
	pEVTNode -> pEventTimer = pEVT;
	pEVTNode -> next = (EVNT_TMR_NODE*)0;
	
	//向链表添加节点
	EVNT_TMR_NODE * pSearch = eventTimerListHead;
	if(pSearch == (EVNT_TMR_NODE*)0){	//	如果链表为空
		eventTimerListHead = pEVTNode;
	}else{	//如果链表不为空
		// 查找链表尾
		while(pSearch -> next != (EVNT_TMR_NODE *)0){
			pSearch = pSearch -> next;
		}
		pSearch -> next = pEVTNode;
	}
	return pEVT;
}

/** @brief  添加计数器事件
  * @param  计数器事件结构体
  * @retval 指向链表中计数器事件的指针
  */
EVENT_COUNTER *  appendECTL(EVENT_COUNTER eventCounter){
	//复制事件
	EVENT_COUNTER * pEVT = (EVENT_COUNTER*)malloc(sizeof(EVENT_COUNTER));
	pEVT -> cnt = eventCounter.cnt;
	pEVT -> cnt_top = eventCounter.cnt_top;
	pEVT -> triger_side = eventCounter.triger_side;
	pEVT -> callBack = eventCounter.callBack;
	//建立链表节点
	EVNT_CNT_NODE * pEVTNode = (EVNT_CNT_NODE*)malloc(sizeof(EVNT_CNT_NODE));
	pEVTNode -> pEventCounter = pEVT;
	pEVTNode -> next = (EVNT_CNT_NODE*)0;
	EVNT_CNT_NODE * pSearch = eventCounterListHead;
	//向链表添加节点
	if(pSearch == (EVNT_CNT_NODE*)0){	//	如果链表为空
		eventCounterListHead = pEVTNode;
	}else{	//如果链表不为空
		// 查找链表尾
		while(pSearch -> next != (EVNT_CNT_NODE *)0){
			pSearch = pSearch -> next;
		}
		pSearch -> next = pEVTNode;
	}
	return pEVT;
}

/** @brief  添加外部事件
  * @param  外部事件定义结构体
  * @retval 链表中外部事件结构体
  */
EVENT_EXT *  appendEEXL(EVENT_EXT eventExternal){
	//复制事件
	EVENT_EXT * pEVT = (EVENT_EXT*)malloc(sizeof(EVENT_EXT));
	pEVT -> pGclass = eventExternal.pGclass;
	pEVT -> monitoring = eventExternal.monitoring;
	pEVT -> callBack = eventExternal.callBack;
	//建立链表节点
	EVNT_EXT_NODE * pEVTNode = (EVNT_EXT_NODE*)malloc(sizeof(EVNT_EXT_NODE));
	pEVTNode -> pEventExternal = pEVT;
	pEVTNode -> next = (EVNT_EXT_NODE*)0;
	EVNT_EXT_NODE * pSearch = eventExternalListHead;
	//向链表添加节点
	if(pSearch == (EVNT_EXT_NODE*)0){	//	如果链表为空
		eventExternalListHead = pEVTNode;
	}else{	//如果链表不为空
		// 查找链表尾
		while(pSearch -> next != (EVNT_EXT_NODE *)0){
			pSearch = pSearch -> next;
		}
		pSearch -> next = pEVTNode;
	}
	return pEVT;
}

// 删除某个事件, 返回0成功删除，返回1未找到该事件
/** @brief  删除一个碰撞事件
  * @param  要删除的碰撞事件结构体指针
  * @retval 1：未找到该事件结构体；
			0：删除成功
  */
u8 cutOffECLLNode(EVENT_COLLISION * pEventCollision){
	EVNT_CLSN_NODE *pSearch = eventCollisionListHead;
	if(pSearch == (EVNT_CLSN_NODE *)0){
		return 1;
	}
	if(pSearch -> pEventCollision == pEventCollision){
		eventCollisionListHead = pSearch -> next;
		free(pSearch -> pEventCollision);
		free(pSearch);
		pSearch = (EVNT_CLSN_NODE *)0;
	}else{
		while(pSearch-> next -> pEventCollision != pEventCollision){
			pSearch = pSearch -> next;
			if(pSearch -> next == (EVNT_CLSN_NODE *)0){
				return 1;
			}
		}
		EVNT_CLSN_NODE * pTmp = pSearch -> next;
		pSearch -> next = pSearch -> next -> next;
		free(pTmp -> pEventCollision);
		free(pTmp);
		pTmp = (EVNT_CLSN_NODE *)0;
	}
	pEventCollision = (EVENT_COLLISION *)0;
	
	return 0;
}

/** @brief  通过查询pGclass1，删除一个碰撞事件
  * @param  要删除的碰撞事件结构体中G类指针
  * @retval 1：未找到该事件结构体；
			0：删除成功
  */
u8 cutOffECLLNodeByGclass1(G_CLASS * pGclass){
	EVNT_CLSN_NODE *pSearch = eventCollisionListHead;
	if(pSearch == (EVNT_CLSN_NODE *)0){
		return 1;
	}
	if(pSearch -> pEventCollision -> pGclass1 == pGclass){
		eventCollisionListHead = pSearch -> next;
		free(pSearch -> pEventCollision);
		free(pSearch);
		pSearch = (EVNT_CLSN_NODE *)0;
	}else{
		while(pSearch-> next -> pEventCollision -> pGclass1 != pGclass){
			pSearch = pSearch -> next;
			if(pSearch -> next == (EVNT_CLSN_NODE *)0){
				return 1;
			}
		}
		EVNT_CLSN_NODE * pTmp = pSearch -> next;
		pSearch -> next = pSearch -> next -> next;
		free(pTmp -> pEventCollision);
		free(pTmp);
		pTmp = (EVNT_CLSN_NODE *)0;
	}
	pGclass = (EVENT_COLLISION *)0;
	
	return 0;
}

/** @brief  删除一个计数器事件
  * @param  要删除的计数器事件结构体指针
  * @retval 1：未找到该事件结构体；
			0：删除成功
  */
u8 cutOffECTLNode(EVENT_COUNTER * pEventCounter){
	EVNT_CNT_NODE *pSearch = eventCounterListHead;
	if(pSearch == (EVNT_CNT_NODE *)0){
		return 1;
	}
	if(pSearch -> pEventCounter == pEventCounter){
		eventCounterListHead = pSearch -> next;
		free(pSearch -> pEventCounter);
		free(pSearch);
		pSearch = (EVNT_CNT_NODE *)0;
	}else{
		while(pSearch-> next -> pEventCounter != pEventCounter){
			pSearch = pSearch -> next;
			if(pSearch -> next == (EVNT_CNT_NODE *)0){
				return 1;
			}
		}
		EVNT_CNT_NODE * pTmp = pSearch -> next;
		pSearch -> next = pSearch -> next -> next;
		free(pTmp -> pEventCounter);
		free(pTmp);
		pTmp = (EVNT_CNT_NODE *)0;
	}
	pEventCounter = (EVENT_COUNTER *)0;
	
	return 0;
}

/** @brief  删除一个计时器事件
  * @param  要删除的计时器事件结构体指针
  * @retval 1：未找到该事件结构体；
			0：删除成功
  */
u8 cutOffETMLNode(EVENT_TIMER * pEventTimer){
	EVNT_TMR_NODE *pSearch = eventTimerListHead;
	if(pSearch == (EVNT_TMR_NODE *)0){
		return 1;
	}
	
	if(pSearch -> pEventTimer == pEventTimer){
		eventTimerListHead = pSearch -> next;
		free(pSearch -> pEventTimer);
		free(pSearch);
		pSearch = (EVNT_TMR_NODE *)0;
	}else{
		while(pSearch-> next -> pEventTimer != pEventTimer){
			pSearch = pSearch -> next;
			if(pSearch -> next == (EVNT_TMR_NODE *)0){
				return 1;
			}
		}
		EVNT_TMR_NODE * pTmp = pSearch -> next;
		pSearch -> next = pSearch -> next -> next;
		free(pTmp -> pEventTimer);
		free(pTmp);
		pTmp = (EVNT_TMR_NODE *)0;
	}
	pEventTimer = (EVENT_TIMER*)0;
	
	return 0;
}

/** @brief  通过查询pGclass，删除一个计时器事件
  * @param  要删除的计时器事件结构体中G类的指针
  * @retval 1：未找到该事件结构体；
			0：删除成功
  */
u8 cutOffETMLNodeByGclass(G_CLASS * pGclass){
	EVNT_TMR_NODE *pSearch = eventTimerListHead;
	if(pSearch == (EVNT_TMR_NODE *)0){
		return 1;
	}
	if(pSearch -> pEventTimer -> pGclass == pGclass){
		eventTimerListHead = pSearch -> next;
		free(pSearch -> pEventTimer);
		free(pSearch);
		pSearch = (EVNT_TMR_NODE *)0;
	}else{
		while(pSearch-> next -> pEventTimer -> pGclass != pGclass){
			pSearch = pSearch -> next;
			if(pSearch -> next == (EVNT_TMR_NODE *)0){
				return 1;
			}
		}
		EVNT_TMR_NODE * pTmp = pSearch -> next;
		pSearch -> next = pSearch -> next -> next;
		free(pTmp -> pEventTimer);
		free(pTmp);
		pTmp = (EVNT_TMR_NODE *)0;
	}
	pGclass = (EVENT_TIMER*)0;
	return 0;
}

/** @brief  删除一个外部事件
  * @param  要删除的外部事件结构体指针
  * @retval 1：未找到该事件结构体；
			0：删除成功
  */
u8 cutOffEEXLNode(EVENT_EXT * pEventExternal){
	EVNT_EXT_NODE * pSearch = eventExternalListHead;
	if(pSearch == (EVNT_EXT_NODE *)0){
		return 1;
	}
	if(pSearch -> pEventExternal == pEventExternal){
		eventExternalListHead = pSearch -> next;
		free(pSearch -> pEventExternal);
		free(pSearch);
		pSearch = (EVNT_EXT_NODE *)0;
	}else{
		while(pSearch-> next -> pEventExternal != pEventExternal){
			pSearch = pSearch -> next;
			if(pSearch -> next == (EVNT_EXT_NODE *)0){
				return 1;
			}
		}
		EVNT_EXT_NODE * pTmp = pSearch -> next;
		pSearch -> next = pSearch -> next -> next;
		free(pTmp -> pEventExternal);
		free(pTmp);
		pTmp = (EVNT_EXT_NODE *)0;
	}
	pEventExternal = (EVENT_EXT*)0;
	return 0;
}

// 析构事件链表
/** @brief  析构碰撞事件链表
  * @param  None
  * @retval None
  */
void destructECLL(){
	EVNT_CLSN_NODE *pSearch = eventCollisionListHead;
	while(eventCollisionListHead != (EVNT_CLSN_NODE *)0){
		eventCollisionListHead = eventCollisionListHead -> next;
		free(pSearch -> pEventCollision);
		free(pSearch);
		pSearch = eventCollisionListHead;
	}
}
/** @brief  析构计数器事件链表
  * @param  None
  * @retval None
  */
void destructECTL(){
	EVNT_CNT_NODE *pSearch = eventCounterListHead;
	while(eventCounterListHead != (EVNT_CNT_NODE *)0){
		eventCounterListHead = eventCounterListHead -> next;
		free(pSearch -> pEventCounter);
		free(pSearch);
		pSearch = eventCounterListHead;
	}
}

/** @brief  析构计时器事件链表
  * @param  None
  * @retval None
  */
void destructETML(){
	EVNT_TMR_NODE *pSearch = eventTimerListHead;
	while(eventTimerListHead != (EVNT_TMR_NODE *)0){
		eventTimerListHead = eventTimerListHead -> next;
		free(pSearch -> pEventTimer);
		free(pSearch);
		pSearch = eventTimerListHead;
	}
}

/** @brief  析构外部事件链表
  * @param  None
  * @retval None
  */
void destructEEXL(){
	EVNT_EXT_NODE *pSearch = eventExternalListHead;
	while(eventExternalListHead != (EVNT_EXT_NODE *)0){
		eventExternalListHead = eventExternalListHead -> next;
		free(pSearch -> pEventExternal);
		free(pSearch);
		pSearch = eventExternalListHead;
	}
}

// 事件监测
/** @brief  碰撞见监测
  * @param  None
  * @retval 1：触发
			0：未触发
  */
u8 isCollided(EVENT_COLLISION * ecl){
	u8 collide = 0;
	if(ecl -> pGclass1 -> hide == 0 || ecl -> pGclass2 -> hide == 0){	// 如果隐藏，则不检测碰撞，即未碰撞
		return collide;
	}
	if((ecl -> pGclass1 -> LocXlu > ecl -> pGclass2 -> LocXrd)
		||(ecl -> pGclass1 -> LocXrd < ecl -> pGclass2 -> LocXlu)
		||(ecl -> pGclass1 -> LocYlu > ecl -> pGclass2 -> LocYrd)
		||(ecl -> pGclass1 -> LocYrd < ecl -> pGclass2 -> LocYlu)){
		// 未碰撞
	}else{
		// 边框重合，进一步判断
		u8 ovlpXlu = max(ecl -> pGclass1 -> LocXlu, ecl -> pGclass2 -> LocXlu);
		u8 ovlpYlu = max(ecl -> pGclass1 -> LocYlu, ecl -> pGclass2 -> LocYlu);
		u8 ovlpXrd = min(ecl -> pGclass1 -> LocXrd, ecl -> pGclass2 -> LocXrd);
		u8 ovlpYrd = min(ecl -> pGclass1 -> LocYrd, ecl -> pGclass2 -> LocYrd);
		u8 itex,itey;
		for(itex = ovlpXlu; itex <= ovlpXrd; itex++){
			for(itey = ovlpYlu; itey <= ovlpYrd; itey++){
				if(getGclassPoint(ecl -> pGclass1, itex - ecl -> pGclass1 -> LocXlu, itey - ecl -> pGclass1 -> LocYlu) != 0
					&& getGclassPoint(ecl -> pGclass2, itex - ecl -> pGclass2 -> LocXlu, itey - ecl -> pGclass2 -> LocYlu) != 0){
					// 碰撞
					collide = 1;
					break;
				}
			}
		}
	}
	//如果碰撞调用回调函数
	if(collide != 0){
		ecl -> callBack(ecl);
	}
	return collide;
}

/** @brief  计时器监测
  * @param  计时器事件结构体指针
  * @retval 1：触发
			0：未触发
  */
u8 isTimeUp(EVENT_TIMER * pEventTimer){
	(pEventTimer -> cnt)--;
	if(pEventTimer -> cnt == 0){
		if(pEventTimer -> once_flag == EVENT_TIMER_MULT){	// 重复触发模式
			pEventTimer -> cnt = pEventTimer -> cnt_rst;	// 重载计时器
			pEventTimer -> callBack(pEventTimer);	// 调用回调函数
			return 1;
		}else{
			pEventTimer -> callBack(pEventTimer);	// 调用回调函数
			cutOffETMLNode(pEventTimer);			// 销毁当前计时器
			return 1;
		}
	}
	return 0;
}

/** @brief  碰撞监测
  * @param  碰撞事件结构体指针
  * @retval 1：触发
			0：未触发
  */
u8 isCounterOver(EVENT_COUNTER * pEventCounter){
	if(pEventCounter -> triger_side == 0){
		if(pEventCounter -> cnt >= pEventCounter -> cnt_top){
			pEventCounter -> callBack(pEventCounter);
			return 1;
		}else{
			return 0;
		}
	}else{
		if(pEventCounter -> cnt >= pEventCounter -> cnt_top){
			pEventCounter -> callBack(pEventCounter);
			return 1;
		}else{
			return 0;
		}
	}
}

/** @brief  事件扫描监测，扫描所有事件检查是否发生，发生则调用回调函数
  * @param  None
  * @retval 1：触发
			0：未触发
  */
void eventScaner(){
	// outputLists();
	// 扫描碰撞事件
	EVNT_CLSN_NODE * pSearch1 = eventCollisionListHead;
	while(pSearch1 != (EVNT_CLSN_NODE*)0){
		// Debug_SendMsg("GameAPI", "check collision", DEBUG_MSG_INFO);
		EVENT_COLLISION* tmp = pSearch1-> pEventCollision;
		pSearch1 = pSearch1 -> next;
		isCollided(tmp);
		
	}
	// 扫描计数器事件
	EVNT_CNT_NODE * pSearch2 = eventCounterListHead;
	while(pSearch2 != (EVNT_CNT_NODE*)0){
		// Debug_SendMsg("GameAPI", "check counter", DEBUG_MSG_INFO);
		EVENT_COUNTER* tmp = pSearch2 -> pEventCounter;
		pSearch2 = pSearch2 -> next;
		isCounterOver(tmp);
	}
	// outputLists();
	// 扫描计数器事件
	EVNT_TMR_NODE * pSearch3 = eventTimerListHead;
	while(pSearch3 != (EVNT_TMR_NODE*)0){
		// Debug_SendMsg("GameAPI", "check timer", DEBUG_MSG_INFO);
		EVENT_TIMER* tmp = pSearch3-> pEventTimer;
		pSearch3 = pSearch3 -> next;
		isTimeUp(tmp);
	}
	// outputLists();
	// 扫描外部事件
	EVNT_EXT_NODE * pSearch4 = eventExternalListHead;
	while(pSearch4 != (EVNT_EXT_NODE*)0){
		// Debug_SendMsg("GameAPI", "check Ext event", DEBUG_MSG_INFO);
		EVENT_EXT* tmp = pSearch4-> pEventExternal;
		pSearch4 = pSearch4 -> next;
		if(tmp -> monitoring(tmp)){
			tmp -> callBack(tmp);
		}
	}
	
}

/** @brief  通用事件和渲染处理循环
  * @param  None
  * @retval NONE
  */
void generalLoop(){
	render();
	//runHooks();
	eventScaner();
}

/** @brief  向串口输出所有事件链表中各个数据的内存地址
  * @param  None
  * @retval NONE
  */

void outputLists(){
	Debug_SendMsg("GameAPI", "===eventCollisionListHead===", DEBUG_MSG_INFO);
	// 扫描碰撞事件
	EVNT_CLSN_NODE * pSearch1 = eventCollisionListHead;
	while(pSearch1 != (EVNT_CLSN_NODE*)0){
		// Debug_SendMsg("GameAPI", "check collision", DEBUG_MSG_INFO);
		char * str[50];
		sprintf(str, "%x", (int)pSearch1);
		Debug_SendMsg("GameAPI", str, DEBUG_MSG_INFO);
		pSearch1 = pSearch1 -> next;
	}
	Debug_SendMsg("GameAPI", "===eventCounterListHead===", DEBUG_MSG_INFO);
	// 扫描计数器事件
	EVNT_CNT_NODE * pSearch2 = eventCounterListHead;
	while(pSearch2 != (EVNT_CNT_NODE*)0){
		// Debug_SendMsg("GameAPI", "check counter", DEBUG_MSG_INFO);
		char * str[50];
		sprintf(str, "%x", (int)pSearch2);
		Debug_SendMsg("GameAPI", str, DEBUG_MSG_INFO);
		pSearch2 = pSearch2 -> next;
	}
	Debug_SendMsg("GameAPI", "===eventTimerListHead===", DEBUG_MSG_INFO);
	// 扫描计数器事件
	EVNT_TMR_NODE * pSearch3 = eventTimerListHead;
	while(pSearch3 != (EVNT_TMR_NODE*)0){
		// Debug_SendMsg("GameAPI", "check timer", DEBUG_MSG_INFO);
		char * str[50];
		sprintf(str, "%x", (int)pSearch3 -> pEventTimer);
		Debug_SendMsg("GameAPI", str, DEBUG_MSG_INFO);
		pSearch3 = pSearch3 -> next;
	}
	Debug_SendMsg("GameAPI", "===eventExternalListHead===", DEBUG_MSG_INFO);
	// 扫描外部事件
	EVNT_EXT_NODE * pSearch4 = eventExternalListHead;
	while(pSearch4 != (EVNT_EXT_NODE*)0){
		// Debug_SendMsg("GameAPI", "check Ext event", DEBUG_MSG_INFO);
		char * str[50];
		sprintf(str, "%x", (int)pSearch4);
		Debug_SendMsg("GameAPI", str, DEBUG_MSG_INFO);
		pSearch4 = pSearch4 -> next;
	}
}

void generalTerminate(){
	destructECLL();
	destructECTL();
	destructEEXL();
	destructETML();
	clearGclass();
	selectInit();
	gameStateMachine.currentGame = (GAME_NODE*)0;
}


//[已经弃用]
//selet

GAME_NODE * selectShowSwitcher = 0;
void selectShowSwitch(EVENT_TIMER * event);

void selectInit(){
	Debug_SendMsg("GameAPI","selet init",DEBUG_MSG_INFO);
	GAME_NODE * pSearch = gameStateMachine.gameList;
	while(pSearch != (GAME_NODE *)0){
		pSearch -> pGameClass -> funcSeletShowInit();
		pSearch = pSearch -> next;
	}
	gameStateMachine.gameList -> pGameClass -> funcSeletShow();
	Debug_SendMsg("GameAPI","switcher start",DEBUG_MSG_INFO);
	EVENT_TIMER eventTmr;
	eventTmr.cnt = 200;
	eventTmr.cnt_rst = 200;
	eventTmr.once_flag = EVENT_TIMER_MULT;
	eventTmr.callBack = selectShowSwitch;
	appendETML(eventTmr);
}
//[已经弃用]
void select(){
	if(Ctl_Data.payment == 1){
		// 按照角度调整
		
		// 按任意键进入游戏
		if(Ctl_Data.Key_Pflag){
			
			Ctl_Data.Key_Pflag = 0;
			Debug_SendMsg("GameAPI", "Game Start", DEBUG_MSG_INFO);
			char str[50];
			sprintf(str, "adress : %x", (int)(selectShowSwitcher -> pGameClass -> funcSeletShowTerminate));
			Debug_SendMsg("GameAPI", str, DEBUG_MSG_INFO);
			selectShowSwitcher -> pGameClass -> funcSeletShowTerminate();
			Debug_SendMsg("GameAPI", "Game Start", DEBUG_MSG_INFO);
			outputLists();
			checkList();
			selectShowSwitcher -> pGameClass -> funcInit();
			Debug_SendMsg("GameAPI", "Game Start", DEBUG_MSG_INFO);
			gameStateMachine.currentGame = selectShowSwitcher;
			Debug_SendMsg("GameAPI", "Game Start", DEBUG_MSG_INFO);
		}
		generalLoop();
	}else{
		generalLoop();
	}
}

//[已经弃用]
void selectShowSwitch(EVENT_TIMER * event){
	if(selectShowSwitcher == 0){
		selectShowSwitcher = gameStateMachine.gameList;
	}
	if(Ctl_Data.payment != 1){
		Debug_SendMsg("GameAPI","select switch",DEBUG_MSG_INFO);
		GAME_NODE * pSearch = gameStateMachine.gameList;	
		while(pSearch != (GAME_NODE *)0){
			if(selectShowSwitcher == pSearch){
				pSearch -> pGameClass -> funcSeletShow();
			}else{
				pSearch -> pGameClass -> funcSeletHide();
			}
			pSearch = pSearch -> next;
		}
		if(selectShowSwitcher -> next != 0){
			selectShowSwitcher = selectShowSwitcher -> next;
		}else{
			selectShowSwitcher = gameStateMachine.gameList;
		}
	}
}