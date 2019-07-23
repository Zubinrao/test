#include "gplatform.h"
#include "Control.h"
#include "shapes.h"
#include "WavPlay.h"
#include "debug.h"
#include "tank_war.h"
#include "gold.h"
#include "car.h"
#include "game_ball.h"

#define GSTM_PRESTART 0
#define GSTM_START 16
#define GSTM_I1 1
#define GSTM_I2 2
#define GSTM_I3 3
#define GSTM_I4 4
#define GSTM_G1 5
#define GSTM_G2 6
#define GSTM_G3 7
#define GSTM_G4 8
#define GSTM_E1 9
#define GSTM_E2 10
#define GSTM_E3 11
#define GSTM_E4 12
#define GSTM_IS 13
#define GSTM_SL 14
#define GSTM_ES 15

#define MAX_GAME_NUM 4
#define NONE_GAME_PLAY_LIST_IND 35


GAME_CLASS * GAMES[MAX_GAME_NUM] = {0};
G_CLASS * GameLogos[MAX_GAME_NUM] = {0};
G_CLASS * GameLogosHL[MAX_GAME_NUM] = {0};
int GameNamePlayListIndex[MAX_GAME_NUM] = {NONE_GAME_PLAY_LIST_IND,NONE_GAME_PLAY_LIST_IND,NONE_GAME_PLAY_LIST_IND,NONE_GAME_PLAY_LIST_IND};
EVENT_EXT * pEventSelectAngle = 0;
EVENT_EXT * pEventSelectKey = 0;
u8 GameNum = 0;
u8 GSTM = GSTM_PRESTART;
u8 curSelected = 0;
u8 Game2Go = 0;
u8 endSelectFlag = 0;

void gameReg(GAME_CLASS* pGameClass);
void selectInit(void);
u8 KeySelectMonitor(EVENT_EXT * event);
void KeySelectCallBack(EVENT_EXT * event);
u8 angleSelectMonitor(EVENT_EXT * event);
void angleSelectCallBack(EVENT_EXT * event);
void select(void);
void selectTerminate(void);
void stateSwitch(void);

void gameReg(GAME_CLASS* pGameClass){
	GameNamePlayListIndex[GameNum] = pGameClass -> nameWAV;
	GAMES[GameNum++] = pGameClass;
}

// 有限状态机
void gamePlatformLoop(){
	switch(GSTM){
		case GSTM_PRESTART : 
			Debug_SendMsg("GPlatform", "GSTM = GSTM_PRESTART", DEBUG_MSG_INFO);
			break;
		case GSTM_START : 
			Debug_SendMsg("GPlatform", "GSTM = GSTM_START", DEBUG_MSG_INFO);
			gameAPIInit();
			gameTankWarReg();
			gameCarReg();
		  gameGoldReg();
			PainoReg();
			break;
		case GSTM_I1 : // 游戏初始化
			Debug_SendMsg("GPlatform", "GSTM = GSTM_I1", DEBUG_MSG_INFO);
            if(GAMES[0] != (GAME_CLASS*)0){
                GAMES[0] -> funcInit();
			}else{
				GSTM = GSTM_IS;
			}
			break;
		case GSTM_G1 : // 游戏运行循环
            //Debug_SendMsg("GPlatform", "GSTM = GSTM_G1", DEBUG_MSG_INFO);
            if(GAMES[0] != (GAME_CLASS*)0){
                GAMES[0] -> funcGame();
			}else{
				GSTM = GSTM_IS;
			}
			break;
		case GSTM_E1 : // 游戏退出
			Debug_SendMsg("GPlatform", "GSTM = GSTM_E1", DEBUG_MSG_INFO);
            if(GAMES[0] != (GAME_CLASS*)0){
                GAMES[0] -> funcTerminate();
			}else{
				GSTM = GSTM_IS;
			}
			break;
		case GSTM_I2 : 
            if(GAMES[1] != (GAME_CLASS*)0){
                GAMES[1] -> funcInit();
			}else{
				GSTM = GSTM_IS;
			}
			break;
		case GSTM_G2 : 
            if(GAMES[1] != (GAME_CLASS*)0){
                GAMES[1] -> funcGame();
			}else{
				GSTM = GSTM_IS;
			}
			break;
		case GSTM_E2 : 
            if(GAMES[1] != (GAME_CLASS*)0){
                GAMES[1] -> funcTerminate();
			}else{
				GSTM = GSTM_IS;
			}
			break;
		case GSTM_I3 : 
            if(GAMES[2] != (GAME_CLASS*)0){
                GAMES[2] -> funcInit();
			}else{
				GSTM = GSTM_IS;
			}
			break;
		case GSTM_G3 : 
            if(GAMES[2] != (GAME_CLASS*)0){
                GAMES[2] -> funcGame();
			}else{
				GSTM = GSTM_IS;
			}
			break;
		case GSTM_E3 : 
            if(GAMES[2] != (GAME_CLASS*)0){
                GAMES[2] -> funcTerminate();
			}else{
				GSTM = GSTM_IS;
			}
			break;
		case GSTM_I4 : 
            if(GAMES[3] != (GAME_CLASS*)0){
                GAMES[3] -> funcInit();
			}else{
				GSTM = GSTM_IS;
			}
			break;
		case GSTM_G4 : 
            if(GAMES[3] != (GAME_CLASS*)0){
                GAMES[3] -> funcGame();
			}else{
				GSTM = GSTM_IS;
			}
			break;
		case GSTM_E4 : 
            if(GAMES[3] != (GAME_CLASS*)0){
                GAMES[3] -> funcTerminate();
			}else{
				GSTM = GSTM_IS;
			}
			break;
		case GSTM_IS :
			Debug_SendMsg("GPlatform", "GSTM = GSTM_IS", DEBUG_MSG_INFO);				
			selectInit();
			break;
		case GSTM_SL : 
			select();
			break;
		case GSTM_ES :
			Debug_SendMsg("GPlatform", "GSTM = GSTM_ES", DEBUG_MSG_INFO);			
			selectTerminate();
			break;
	}
	stateSwitch();
}

void selectInit(){
	int i;
	for(i = 0; i < MAX_GAME_NUM; i++){
		if(GAMES[i] != (GAME_CLASS*)0){
			char str[50];
			sprintf(str, "Game %d logo show.", i);
			Debug_SendMsg("GPlatform", str, DEBUG_MSG_INFO);
			GameLogos[i] = newGclass(8,8,5 + i*10,0);
			addShape(GameLogos[i], GAMES[i] -> pLogo);
			GameLogos[i] -> hide = 1;
			GameLogosHL[i] = newGclass(8,8,5 + i*10,0);
			addShape(GameLogosHL[i], GAMES[i] -> pLogoHL);
			GameLogosHL[i] -> hide = 0;
		}else{
			char str[50];
			sprintf(str, "Game %d is not registered", i);
			Debug_SendMsg("GPlatform", str, DEBUG_MSG_INFO);	
			GameLogos[i] = newGclass(8,8,5 + i*10,0);
			addShape(GameLogos[i], shapeNoneLogo);
			GameLogos[i] -> hide = 1;
			GameLogosHL[i] = newGclass(8,8,5 + i*10,0);
			addShape(GameLogosHL[i], shapeNoneLogoHL);
			GameLogosHL[i] -> hide = 0;
		}
	}
	Debug_SendMsg("GPlatform", "Events Init.", DEBUG_MSG_INFO);
	// 角度外部事件
	EVENT_EXT eventExt;
	eventExt.monitoring = angleSelectMonitor;
	eventExt.callBack = angleSelectCallBack;
	pEventSelectAngle = appendEEXL(eventExt);
	// 按键外部事件
	eventExt.monitoring = KeySelectMonitor;
	eventExt.callBack = KeySelectCallBack;
	pEventSelectKey = appendEEXL(eventExt);
}

u8 KeySelectMonitor(EVENT_EXT * event){
	//侦测按键
	if(Ctl_Data.Key_Pflag){
		return 1;
	}else{
		return 0;
	}
}

void KeySelectCallBack(EVENT_EXT * event){
		//Ctl_Data.payment=1;//取消扫码
	if(0){//Ctl_Data.payment ==0
		Debug_SendMsg("GPlatform", "Please Scan the QR code.", DEBUG_MSG_INFO);
		Play_List(253);
	}else{
   	Ctl_Data.payment=1;
		if(GAMES[curSelected] == (GAME_CLASS*)0){
			Play_List(24);
			Debug_SendMsg("GPlatform", "Game does not exist.", DEBUG_MSG_INFO);
		}else{
			char str[50];
			sprintf(str, "Go to game %d.", (int)curSelected);
			Debug_SendMsg("GPlatform", str, DEBUG_MSG_INFO);
			Game2Go = curSelected;
            endSelectFlag = 1;
            //GSTM = GSTM_ES;
		}
	}
	Ctl_Data.Key_Pflag = 0;
}

u8 angleSelectMonitor(EVENT_EXT * event){
	return 1;
}

void angleSelectCallBack(EVENT_EXT * event){
	int i;
	static u8 lastSelected = 4;
	curSelected=abs((int)DMP_Data.Pitch)/2;
	if(curSelected > 3){
		curSelected = 3;
	}
	for(i = 0; i < MAX_GAME_NUM; i++){
		if(i == curSelected){
			GameLogos[i] -> hide = 0;
			GameLogosHL[i] -> hide = 1;
		}else{
			GameLogos[i] -> hide = 1;
			GameLogosHL[i] -> hide = 0;
		}
	}
	if(lastSelected != curSelected){
		Play_List(GameNamePlayListIndex[curSelected]);
	}
	lastSelected = curSelected;
}

void select(){
	generalLoop();
}

void selectTerminate(){
	if(Game2Go >= GameNum){
		return;
	}else{
		// 清除所有链表
		clearGclass();
		destructEEXL();
	}
    Debug_SendMsg("GPlatform", "Finished select termination.", DEBUG_MSG_INFO);
}

void stateSwitch(){
    static u8 paymentLast = 0;
	switch(GSTM){
		case GSTM_PRESTART : 
			GSTM = GSTM_START;
			break;
		case GSTM_START : 
			GSTM = GSTM_IS;
			break;
		case GSTM_I1 : 
			GSTM = GSTM_G1;
			break;
		case GSTM_G1 : 
			if(Ctl_Data.payment == 0){
				GSTM = GSTM_E1;
			}
			break;
		case GSTM_E1 : 
			GSTM = GSTM_IS;
			break;
		case GSTM_I2 : 
			GSTM = GSTM_G2;
			break;
		case GSTM_G2 : 
			if(Ctl_Data.payment == 0){
				GSTM = GSTM_E2;
			}
			break;
		case GSTM_E2 : 
			GSTM = GSTM_IS;
			break;
		case GSTM_I3 : 
			GSTM = GSTM_G3;
			break;
		case GSTM_G3 : 
			if(Ctl_Data.payment == 0){
				GSTM = GSTM_E3;
			}
			break;
		case GSTM_E3 : 
			GSTM = GSTM_IS;
			break;
		case GSTM_I4 : 
			GSTM = GSTM_G4;
			break;
		case GSTM_G4 : 
			if(Ctl_Data.payment == 0){
				GSTM = GSTM_E4;
			}
			break;
		case GSTM_E4 : 
			GSTM = GSTM_IS;
			break;
		case GSTM_IS : 
			GSTM = GSTM_SL;
			break;
		case GSTM_SL : 
        if(endSelectFlag != 0){
            GSTM = GSTM_ES;
            endSelectFlag = 0;
        }
			break;
		case GSTM_ES : 
            if(Game2Go >= GameNum){
                GSTM = GSTM_SL;
            }else{
                // 切换状态
                switch(Game2Go){
                    case 0 :
                        GSTM = GSTM_I1;
                        break;
                    case 1 :
                        GSTM = GSTM_I2;
                        break;
                    case 2 :
                        GSTM = GSTM_I3;
                        break;
                    case 3 :
                        GSTM = GSTM_I4;
                        break;
                    default:
                        GSTM = GSTM_IS;
                }
            }
			break;
	}
	paymentLast = Ctl_Data.payment;
}
