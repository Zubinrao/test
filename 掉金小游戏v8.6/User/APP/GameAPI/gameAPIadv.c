#include "gameAPIadv.h"
#include "debug.h"

// #define GETMAT(r,c)	(*((curGameInfo -> mat) + (r) * MATLEN + (c)))
#define GETMAT(r,c)	((curGameInfo -> mat)[r][c])
#define GETGLIST(IND) ((curGameInfo -> glist)[(IND)])
#define GETGLISTTMP(IND) (glistTmp[1 - (IND)])
GAME_INFO * curGameInfo;


// void readmat(r, c){
	// return *((curGameInfo -> mat) + r * MATLEN + c);
// }

void scenSetup(u8 scenIndex){
	int matIte;
	G_CLASS * glistTmp[GLIST_TMP_LEN];
	for(matIte = 0; matIte < curGameInfo -> matSize; matIte ++){
		if(GETMAT(matIte, MAT_SCEN_IND) == scenIndex){
			if(matIte > 0){
				if(GETMAT(matIte, MAT_EVNT_IND) == GETMAT(matIte - 1, MAT_EVNT_IND)){
					continue;
				}
			}
			int tmp = GETMAT(matIte, MAT_EVNT_TYPE);
			switch(tmp){
				case EVTP_TMR:
					eventTimerSetup(matIte);
					break;
				case EVTP_CLL:
					eventCollisionSetup(matIte);
					break;
				case EVTP_CNT:
					eventCounterSetup(matIte);
					break;
				case EVTP_EXT:
					eventExternalSetup(matIte);
					break;
				case EVTP_NONE:
					action(matIte, glistTmp);
					break;
			}
		}
	}
}

void eventCounterSetup(u8 matIte){
	EVENT_COUNTER eventCnt;
	eventCnt.cnt = (u16)GETMAT(matIte, MAT_CNT_CNT);
	eventCnt.cnt_top = (u16)GETMAT(matIte, MAT_CNT_CNT_TOP);
	eventCnt.triger_side = (u8)GETMAT(matIte, MAT_CNT_TRIG_ERSIDE);
	eventCnt.callBack = callBackCNT;
	GETMAT(matIte, MAT_EVNT_PTR) = (GAME_DEFINE_TYPE)appendECTL(eventCnt);
}

void eventTimerSetup(u8 matIte){
	EVENT_TIMER eventTmr;
	eventTmr.pGclass = (G_CLASS *)GETMAT(matIte, MAT_TMR_PGCLASS);
	eventTmr.cnt = (u16)GETMAT(matIte, MAT_TMR_CNT);
	eventTmr.cnt_rst = (u16)GETMAT(matIte, MAT_TMR_CNT_RST);
	eventTmr.once_flag = (u8)GETMAT(matIte, MAT_TMR_ONCE_FLAG);
	eventTmr.callBack = callBackTMR;
	GETMAT(matIte, MAT_EVNT_PTR) = (GAME_DEFINE_TYPE)appendETML(eventTmr);
}

void eventExternalSetup(u8 matIte){
	EVENT_EXT eventExt;
	eventExt.monitoring = (u8 (*)(struct eventExternal *))GETMAT(matIte, MAT_EXT_MONITOR);
	eventExt.callBack = callBackEXT;
	GETMAT(matIte, MAT_EVNT_PTR) = (GAME_DEFINE_TYPE)appendEEXL(eventExt);
}

void eventCollisionSetup(u8 matIte){
	EVENT_COLLISION eventCol;
	eventCol.pGclass1 = (G_CLASS *)GETMAT(matIte, MAT_CL_PGCLASS1);
	eventCol.pGclass2 = (G_CLASS *)GETMAT(matIte, MAT_CL_PGCLASS2);
	eventCol.callBack = callBackCLL;
	GETMAT(matIte, MAT_EVNT_PTR) = (GAME_DEFINE_TYPE)appendECLL(eventCol);
}

void switchScenTo(u8 scenIndex){
	scenSetup(scenIndex);
}


void callBackCNT(EVENT_COUNTER * event){
	u8 matIte, matIte2;
	G_CLASS * glistTmp[GLIST_TMP_LEN];
	for(matIte = 0; matIte < curGameInfo -> matSize; matIte++){
		if(event == (EVENT_COUNTER *)GETMAT(matIte, MAT_EVNT_PTR)){
			for(matIte2 = matIte; matIte2 < curGameInfo -> matSize; matIte2 ++){
				if(GETMAT(matIte2, MAT_EVNT_IND) == GETMAT(matIte, MAT_EVNT_IND)){
					action(matIte2, glistTmp);
				}
			}
			break;
		}
	}
}

void callBackCLL(EVENT_COLLISION * event){
	u8 matIte, matIte2;
	G_CLASS * glistTmp[GLIST_TMP_LEN];
	for(matIte = 0; matIte < curGameInfo -> matSize; matIte++){
		if(event == (EVENT_COLLISION *)GETMAT(matIte, MAT_EVNT_PTR)){
			for(matIte2 = matIte; matIte2 < curGameInfo -> matSize; matIte2 ++){
				if(GETMAT(matIte2, MAT_EVNT_IND) == GETMAT(matIte, MAT_EVNT_IND)){
					action(matIte2, glistTmp);
				}
			}
			break;
		}
	}
}


void callBackTMR(EVENT_TIMER * event){
	u8 matIte, matIte2;
	G_CLASS * glistTmp[GLIST_TMP_LEN];
	for(matIte = 0; matIte < curGameInfo -> matSize; matIte++){
		if(event == (EVENT_TIMER *)GETMAT(matIte, MAT_EVNT_PTR)){
			for(matIte2 = matIte; matIte2 < curGameInfo -> matSize; matIte2 ++){
				if(GETMAT(matIte2, MAT_EVNT_IND) == GETMAT(matIte, MAT_EVNT_IND)){
					action(matIte2, glistTmp);
				}
			}
			break;
		}
	}
}

void callBackEXT(EVENT_EXT * event){
	u8 matIte, matIte2;
	G_CLASS * glistTmp[GLIST_TMP_LEN];
	for(matIte = 0; matIte < curGameInfo -> matSize; matIte++){
		if(event == (EVENT_EXT *)GETMAT(matIte, MAT_EVNT_PTR)){
			for(matIte2 = matIte; matIte2 < curGameInfo -> matSize; matIte2 ++){
				if(GETMAT(matIte2, MAT_EVNT_IND) == GETMAT(matIte, MAT_EVNT_IND)){
					action(matIte2, glistTmp);
				}
			}
			break;
		}
	}
}

void action(u8 matIte, G_CLASS ** glistTmp){
	int tmp = GETMAT(matIte, MAT_CBTP);
	switch(tmp){
		case CBTP_GC_CLONE:
			if(GETMAT(matIte, MAT_CBPR1) < -GLIST_TMP_LEN || GETMAT(matIte, MAT_CBPR1) >= GLIST_LEN){
				char str[60];
				sprintf(str, "G List Index Is Out Of Range At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				break;
			}
			if(GETMAT(matIte, MAT_CBPR2) < -GLIST_TMP_LEN || GETMAT(matIte, MAT_CBPR2) >= GLIST_LEN){
				char str[60];
				sprintf(str, "G List Index Is Out Of Range At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				break;
			}
			if(GETMAT(matIte, MAT_CBPR1) < 0){
				if(GETMAT(matIte, MAT_CBPR2) < 0){
					GETGLISTTMP(GETMAT(matIte, MAT_CBPR1)) = cloneGclass(GETGLISTTMP(GETMAT(matIte, MAT_CBPR2)));
				}else{
					GETGLISTTMP(GETMAT(matIte, MAT_CBPR1)) = cloneGclass(GETGLIST(GETMAT(matIte, MAT_CBPR2)));
				}
				
			}else{
				if(GETMAT(matIte, MAT_CBPR2) < 0){
					GETGLIST(GETMAT(matIte, MAT_CBPR1)) = cloneGclass(GETGLISTTMP(GETMAT(matIte, MAT_CBPR2)));
				}else{
					GETGLIST(GETMAT(matIte, MAT_CBPR1)) = cloneGclass(GETGLIST(GETMAT(matIte, MAT_CBPR2)));
				}
			}
			
			break;
		case CBTP_GC_SHOW:
			if(GETMAT(matIte, MAT_CBPR1) < -GLIST_TMP_LEN || GETMAT(matIte, MAT_CBPR1) >= GLIST_LEN){
				char str[60];
				sprintf(str, "G List Index Is Out Of Range At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				break;
			}
			if(GETMAT(matIte, MAT_CBPR1) < 0){
				GETGLISTTMP(GETMAT(matIte, MAT_CBPR1)) -> hide = 1;
			}else{
				GETGLIST(GETMAT(matIte, MAT_CBPR1)) -> hide = 1;
			}
			break;
		case CBTP_GC_HIDE:
			if(GETMAT(matIte, MAT_CBPR1) < -GLIST_TMP_LEN || GETMAT(matIte, MAT_CBPR1) >= GLIST_LEN){
				char str[60];
				sprintf(str, "G List Index Is Out Of Range At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				break;
			}
			if(GETMAT(matIte, MAT_CBPR1) < 0){
				GETGLISTTMP(GETMAT(matIte, MAT_CBPR1)) -> hide = 0;
			}else{
				GETGLIST(GETMAT(matIte, MAT_CBPR1)) -> hide = 0;
			}
			break;
		case CBTP_GC_MVX:
			if(GETMAT(matIte, MAT_CBPR1) < -GLIST_TMP_LEN || GETMAT(matIte, MAT_CBPR1) >= GLIST_LEN){
				char str[60];
				sprintf(str, "G List Index Is Out Of Range At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				break;
			}
			if(GETMAT(matIte, MAT_CBPR1) < 0){
				moveX(GETGLISTTMP(GETMAT(matIte, MAT_CBPR1)), (int16_t)GETMAT(matIte, MAT_CBPR2));
			}else{
				moveX(GETGLIST(GETMAT(matIte, MAT_CBPR1)), (int16_t)GETMAT(matIte, MAT_CBPR2));
			}
			break;
		case CBTP_GC_MVY:
			if(GETMAT(matIte, MAT_CBPR1) < -GLIST_TMP_LEN || GETMAT(matIte, MAT_CBPR1) >= GLIST_LEN){
				char str[60];
				sprintf(str, "G List Index Is Out Of Range At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				break;
			}
			if(GETMAT(matIte, MAT_CBPR1) < 0){
				moveY(GETGLISTTMP(GETMAT(matIte, MAT_CBPR1)), (int16_t)GETMAT(matIte, MAT_CBPR2));
			}else{
				moveY(GETGLIST(GETMAT(matIte, MAT_CBPR1)), (int16_t)GETMAT(matIte, MAT_CBPR2));
			}
			break;
		case CBTP_GC_NEW:
			if(GETMAT(matIte, MAT_CBPR1) < -GLIST_TMP_LEN || GETMAT(matIte, MAT_CBPR1) >= GLIST_LEN){
				char str[60];
				sprintf(str, "G List Index Is Out Of Range At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				break;
			}
			if(GETMAT(matIte, MAT_CBPR1) < 0){
				char str[80];
				sprintf(str, "New G Class Can Not Be Saved In Temporary G List. At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
			}else{
				GETGLIST(GETMAT(matIte, MAT_CBPR1)) = newGclass((int16_t)GETMAT(matIte, MAT_CBPR2), (int16_t)GETMAT(matIte, MAT_CBPR3), (int16_t)GETMAT(matIte, MAT_CBPR4), (int16_t)GETMAT(matIte, MAT_CBPR5));
			}
			break;
		case CBTP_GC_FLPLR:
			if(GETMAT(matIte, MAT_CBPR1) < -GLIST_TMP_LEN || GETMAT(matIte, MAT_CBPR1) >= GLIST_LEN){
				char str[60];
				sprintf(str, "G List Index Is Out Of Range At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				break;
			}
			if(GETMAT(matIte, MAT_CBPR1) < 0){
				flprltg(GETGLISTTMP(GETMAT(matIte, MAT_CBPR1)));
			}else{
				flprltg(GETGLIST(GETMAT(matIte, MAT_CBPR1)));
			}
			break;
		case CBTP_GC_FLPLR2:
			if(GETMAT(matIte, MAT_CBPR1) < -GLIST_TMP_LEN || GETMAT(matIte, MAT_CBPR1) >= GLIST_LEN){
				char str[60];
				sprintf(str, "G List Index Is Out Of Range At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				break;
			}
			if(GETMAT(matIte, MAT_CBPR1) < 0){
				setFlprl(GETGLISTTMP(GETMAT(matIte, MAT_CBPR1)), (u8)GETMAT(matIte, MAT_CBPR2));
			}else{
				setFlprl(GETGLIST(GETMAT(matIte, MAT_CBPR1)), (u8)GETMAT(matIte, MAT_CBPR2));
			}
			break;
		case CBTP_GC_FLPUD:
			if(GETMAT(matIte, MAT_CBPR1) < -GLIST_TMP_LEN || GETMAT(matIte, MAT_CBPR1) >= GLIST_LEN){
				char str[60];
				sprintf(str, "G List Index Is Out Of Range At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				break;
			}
			if(GETMAT(matIte, MAT_CBPR1) < 0){
				flpudtg(GETGLISTTMP(GETMAT(matIte, MAT_CBPR1)));
			}else{
				flpudtg(GETGLIST(GETMAT(matIte, MAT_CBPR1)));
			}
			break;
		case CBTP_GC_FLPUD2:
			if(GETMAT(matIte, MAT_CBPR1) < -GLIST_TMP_LEN || GETMAT(matIte, MAT_CBPR1) >= GLIST_LEN){
				char str[60];
				sprintf(str, "G List Index Is Out Of Range At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				break;
			}
			if(GETMAT(matIte, MAT_CBPR1) < 0){
				setFlpud(GETGLISTTMP(GETMAT(matIte, MAT_CBPR1)), (u8)GETMAT(matIte, MAT_CBPR2));
			}else{
				setFlpud(GETGLIST(GETMAT(matIte, MAT_CBPR1)), (u8)GETMAT(matIte, MAT_CBPR2));
			}
			break;
		case CBTP_GC_STLOC:
			if(GETMAT(matIte, MAT_CBPR1) < -GLIST_TMP_LEN || GETMAT(matIte, MAT_CBPR1) >= GLIST_LEN){
				char str[60];
				sprintf(str, "G List Index Is Out Of Range At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				break;
			}
			if(GETMAT(matIte, MAT_CBPR1) < 0){
				setLocation(GETGLISTTMP(GETMAT(matIte, MAT_CBPR1)), (int16_t)GETMAT(matIte, MAT_CBPR2), (int16_t)GETMAT(matIte, MAT_CBPR3));
			}else{
				setLocation(GETGLIST(GETMAT(matIte, MAT_CBPR1)), (int16_t)GETMAT(matIte, MAT_CBPR2), (int16_t)GETMAT(matIte, MAT_CBPR3));
			}
			break;
		case CBTP_SCORE_LEFT_INC:
			if(curGameInfo -> scoreMode == SCORE_MODE_MARCH){
				if(curGameInfo -> scoreRight == 0){
					curGameInfo -> scoreLeft ++;
				}else{
					curGameInfo -> scoreRight --;
				}
				if(curGameInfo -> scoreLeft == curGameInfo -> score2Win){
					switchScenTo(GETMAT(matIte, MAT_CBPR1));
				}
			}else if(curGameInfo -> scoreMode == SCORE_MODE_NORMAL){
				curGameInfo -> scoreLeft ++;
				if(curGameInfo -> scoreLeft == curGameInfo -> score2Win){
					switchScenTo(GETMAT(matIte, MAT_CBPR1));
				}
			}
			break;
		case CBTP_SCORE_LEFT_DEC:
			if(curGameInfo -> scoreMode == SCORE_MODE_MARCH){
				char str[80];
				sprintf(str, "Can Not Use CBTP_SCORE_LEFT_DEC In March Mode. At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
			}else if(curGameInfo -> scoreMode == SCORE_MODE_NORMAL){
				curGameInfo -> scoreLeft --;
				if(curGameInfo -> scoreLeft == curGameInfo -> score2Win){
					switchScenTo(GETMAT(matIte, MAT_CBPR1));
				}
			}
			break;
		case CBTP_SCORE_RIGHT_INC:
			if(curGameInfo -> scoreMode == SCORE_MODE_MARCH){
				if(curGameInfo -> scoreLeft == 0){
					curGameInfo -> scoreRight ++;
				}else{
					curGameInfo -> scoreLeft --;
				}
				if(curGameInfo -> scoreRight == curGameInfo -> score2Win){
					switchScenTo(GETMAT(matIte, MAT_CBPR1));
				}
			}else if(curGameInfo -> scoreMode == SCORE_MODE_NORMAL){
				curGameInfo -> scoreRight ++;
				if(curGameInfo -> scoreRight == curGameInfo -> score2Win){
					switchScenTo(GETMAT(matIte, MAT_CBPR1));
				}
			}
			break;
		case CBTP_SCORE_RIGHT_DEC:
			if(curGameInfo -> scoreMode == SCORE_MODE_MARCH){
				char str[80];
				sprintf(str, "Can Not Use CBTP_SCORE_RIGHT_DEC In March Mode. At Mat Index [%d]", (int)matIte);
				Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
			}else if(curGameInfo -> scoreMode == SCORE_MODE_NORMAL){
				curGameInfo -> scoreRight --;
				if(curGameInfo -> scoreRight == curGameInfo -> score2Win){
					switchScenTo(GETMAT(matIte, MAT_CBPR1));
				}
			}
			break;
		case CBTP_SCEN_SWITCH:
			switchScenTo(GETMAT(matIte, MAT_CBPR1));
			break;
		case CBTP_GC_DEL:
			if(GETMAT(matIte, MAT_CBPR1) == GC_DELMD_GLIST){
				if(cutOffClonedGclassNode(GETGLIST(GETMAT(matIte, MAT_CBPR2)))){
					char str[60];
					sprintf(str, "Fail to delete G class. At Mat Index [%d]", (int)matIte);
					Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				}
			}else if(GETMAT(matIte, MAT_CBPR1) == GC_DELMD_CLSN1){
				u8 matIteTmp;
				int16_t tmpInd = -1;
				for(matIteTmp = matIte; matIteTmp > 0; matIteTmp--){
					if(GETMAT(matIteTmp, MAT_EVNT_IND) != GETMAT(matIteTmp - 1, MAT_EVNT_IND)){
						tmpInd = matIteTmp;
						break;
					}
				}
				if(tmpInd == -1 && matIteTmp == 0){
					tmpInd = 0;
				}
				if(cutOffClonedGclassNode(((EVENT_COLLISION*)GETMAT(tmpInd, MAT_EVNT_PTR)) -> pGclass1)){
					char str[60];
					sprintf(str, "Fail to delete G class. At Mat Index [%d]", (int)matIte);
					Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				}
			}else if(GETMAT(matIte, MAT_CBPR1) == GC_DELMD_CLSN2){
				u8 matIteTmp;
				int16_t tmpInd = -1;
				for(matIteTmp = matIte; matIteTmp > 0; matIteTmp--){
					if(GETMAT(matIteTmp, MAT_EVNT_IND) != GETMAT(matIteTmp - 1, MAT_EVNT_IND)){
						tmpInd = matIteTmp;
						break;
					}
				}
				if(tmpInd == -1 && matIteTmp == 0){
					tmpInd = 0;
				}
				if(cutOffClonedGclassNode(((EVENT_COLLISION*)GETMAT(tmpInd, MAT_EVNT_PTR)) -> pGclass2)){
					char str[60];
					sprintf(str, "Fail to delete G class. At Mat Index [%d]", (int)matIte);
					Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				}
			}else if(GETMAT(matIte, MAT_CBPR1) == GC_DELMD_TMRGC){
				u8 matIteTmp;
				int16_t tmpInd = -1;
				for(matIteTmp = matIte; matIteTmp > 0; matIteTmp--){
					if(GETMAT(matIteTmp, MAT_EVNT_IND) != GETMAT(matIteTmp - 1, MAT_EVNT_IND)){
						tmpInd = matIteTmp;
						break;
					}
				}
				if(tmpInd == -1 && matIteTmp == 0){
					tmpInd = 0;
				}
				if(cutOffClonedGclassNode(((EVENT_TIMER*)GETMAT(tmpInd, MAT_EVNT_PTR)) -> pGclass)){
					char str[60];
					sprintf(str, "Fail to delete G class. At Mat Index [%d]", (int)matIte);
					Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				}
			}
			break;
		case CBTP_CUSTOM:
			//GETMAT(matIte, )
			break;
		case CBTP_EVNT_STUP:
		{
			u8 tmpInd;
			for(tmpInd = 0; tmpInd < curGameInfo -> matSize; tmpInd ++){
				if(GETMAT(tmpInd, MAT_EVNT_IND) == GETMAT(matIte, MAT_CBPR1)){
					if(GETMAT(tmpInd, MAT_SCEN_IND) == 0){
						if(tmpInd > 0){
							if(GETMAT(tmpInd, MAT_EVNT_IND) == GETMAT(tmpInd - 1, MAT_EVNT_IND)){
								Debug_SendMsg("GameAPIAdvance", "Fail to find event head when setup event", DEBUG_MSG_ERROR);
								break;
							}
						}
						switch(GETMAT(tmpInd, MAT_EVNT_TYPE)){
							case EVTP_TMR:
								eventTimerSetup(tmpInd);
								break;
							case EVTP_CLL:
								eventCollisionSetup(tmpInd);
								break;
							case EVTP_CNT:
								eventCounterSetup(tmpInd);
								break;
							case EVTP_EXT:
								eventExternalSetup(tmpInd);
								break;
							case EVTP_NONE:
							{
								char str[60];
								sprintf(str, "Try to setup a None Type Event. At Mat Index [%d]", (int)matIte);
								Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
							}
							break;
						}
					}else{
						char str[60];
						sprintf(str, "Try to setup a Dependent Event. At Mat Index [%d]", (int)matIte);
						Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
					}
					break;
				}
				break;
			}
		}
			break;
		case CBTP_EVNT_DEL:
		{
			u8 tmpInd=0;
			for(tmpInd = 0; tmpInd < curGameInfo -> matSize; tmpInd ++){
				if(GETMAT(tmpInd, MAT_EVNT_IND) == GETMAT(matIte, MAT_CBPR1)){
					if(tmpInd > 0){
						if(GETMAT(tmpInd, MAT_EVNT_IND) == GETMAT(tmpInd - 1, MAT_EVNT_IND)){
							Debug_SendMsg("GameAPIAdvance", "Fail to find event head when delete event", DEBUG_MSG_ERROR);
							break;
						}
					}
					switch(GETMAT(tmpInd, MAT_EVNT_TYPE)){
						case EVTP_TMR:
							cutOffETMLNode((EVENT_TIMER*)GETMAT(tmpInd, MAT_EVNT_PTR));
							GETMAT(tmpInd, MAT_EVNT_PTR) = 0;
							break;
						case EVTP_CLL:
							cutOffECLLNode((EVENT_COLLISION*)GETMAT(tmpInd, MAT_EVNT_PTR));
							GETMAT(tmpInd, MAT_EVNT_PTR) = 0;
							break;
						case EVTP_CNT:
							cutOffECTLNode((EVENT_COUNTER*)GETMAT(tmpInd, MAT_EVNT_PTR));
							GETMAT(tmpInd, MAT_EVNT_PTR) = 0;
							break;
						case EVTP_EXT:
							cutOffEEXLNode((EVENT_EXT*)GETMAT(tmpInd, MAT_EVNT_PTR));
							GETMAT(tmpInd, MAT_EVNT_PTR) = 0;
							break;
						case EVTP_NONE:
						{
							char str[60];
							sprintf(str, "Try to delete a None Type Event. At Mat Index [%d]", (int)matIte);
							Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
						}
						break;
					}
				}
				break;
			}
		}
			break;
		case CBTP_SUICIDE:
		{
			u8 matIteTmp;
			int16_t tmpInd = -1;
			for(matIteTmp = matIte; matIteTmp > 0; matIteTmp--){
				if(GETMAT(matIteTmp, MAT_EVNT_IND) != GETMAT(matIteTmp - 1, MAT_EVNT_IND)){
					tmpInd = matIteTmp;
					break;
				}
			}
			if(tmpInd == -1 && matIteTmp == 0){
				tmpInd = 0;
			}
			switch(GETMAT(tmpInd, MAT_EVNT_TYPE)){
				case EVTP_TMR:
					cutOffETMLNode((EVENT_TIMER*)GETMAT(tmpInd, MAT_EVNT_PTR));
					GETMAT(tmpInd, MAT_EVNT_PTR) = 0;
					break;
				case EVTP_CLL:
					cutOffECLLNode((EVENT_COLLISION*)GETMAT(tmpInd, MAT_EVNT_PTR));
					GETMAT(tmpInd, MAT_EVNT_PTR) = 0;
					break;
				case EVTP_CNT:
					cutOffECTLNode((EVENT_COUNTER*)GETMAT(tmpInd, MAT_EVNT_PTR));
					GETMAT(tmpInd, MAT_EVNT_PTR) = 0;
					break;
				case EVTP_EXT:
					cutOffEEXLNode((EVENT_EXT*)GETMAT(tmpInd, MAT_EVNT_PTR));
					GETMAT(tmpInd, MAT_EVNT_PTR) = 0;
					break;
				case EVTP_NONE:
				{
					char str[60];
					sprintf(str, "Try to delete a None Type Event. At Mat Index [%d]", (int)matIte);
					Debug_SendMsg("GameAPIAdvance", str, DEBUG_MSG_ERROR);
				}
					break;
			}
		}
			break;
	}
}