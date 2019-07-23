#include "tank_war.h"
#include "debug.h"
#include "Control.h"
#include "math.h"
#include "WavPlay.h"
/***************
	  图形
****************/
#include"shapes.h"
/***************
	  Macro
****************/
#define BULLET_FLY_CNT_RST 3
#define MISSILE_FLY_CNT_RST 2
#define FIRE_RATE_LV1 80
#define SUPER_WEPON_NUM 2
#define ANGLE_TH 3
#define TOP_SCORE 3
#define TANK_MARCH_STEP 4
#define TANK_MARCH_ROUND 5
#define LEVEL0_RED_X 3
#define LEVEL0_Blue_X 42
#define LEVEL1_RED_X 13
#define LEVEL1_Blue_X 30 
#define LEVEL2_RED_X 30
#define LEVEL2_Blue_X 16
/***************
	游戏变量
****************/
GAME_CLASS gameTankWar;
static u8 level = 1;
static u8 BlueScore = 0;
static u8 RedScore = 0;
static u8 scen = 1;
/***************
	G类指针
****************/

static G_CLASS * pGclassTankRed;
static G_CLASS * pGclassTankBlue;
static G_CLASS * pGclassMissile;
static G_CLASS * pGclassBullet;
static G_CLASS * pGclassBomb;
static G_CLASS * pGclassBlow;
static G_CLASS * pGclassOne;
static G_CLASS * pGclassZero;
static G_CLASS * pGclassTwo;
static G_CLASS * pGclassThree;
static G_CLASS * pGclassVs;

static G_CLASS * pGclassLand1;
static G_CLASS * pGclassLand2;
static G_CLASS * pGclassMarine1;
static G_CLASS * pGclassMarine2;
static G_CLASS * pGclassSea1;
static G_CLASS * pGclassSea2;
static G_CLASS * pGclassBlueScore;
static G_CLASS * pGclassRedScore;
static G_CLASS * pGclassSuperCloned = (G_CLASS *)0;
static G_CLASS * pGclassRedFlag;
static G_CLASS * pGclassGreenFlag;

static EVENT_TIMER * pEventTmrBulletFire;
static EVENT_COUNTER * pEventCntIntro1End;
static EVENT_EXT * pEventGameKey;
static EVENT_EXT * pEventGameAngle;
static EVENT_COUNTER * pEventCntIntro2End;

/***************
	函数声明
****************/
static void gameTankGaming(void);
static void gameTankInit(void);
static void intro1(void);
static void tmrIntro1Animation(EVENT_TIMER * event);
static void cntIntro1End(EVENT_COUNTER * event);
static void Game(void);
static void tmrFireCallBack(EVENT_TIMER * event);
static u8 KeyMonitor(EVENT_EXT * event);
static void KeyGameCallBack(EVENT_EXT * event);
static u8 angleMonitor(EVENT_EXT * event);
static void angleCallBack(EVENT_EXT * event);
static u8 redTankBulletMonitor(EVENT_TIMER * event);
static u8 blueTankBulletMonitor(EVENT_TIMER * event);
static void redTankBulletCallBack(EVENT_TIMER * event);
static void extRedTankBulletCallBack(EVENT_TIMER * event);
static void tmrBulletRedTankFlyCallBack(EVENT_TIMER * event);
static void tmrBulletBlueTankFlyCallBack(EVENT_TIMER * event);
static void colTankCallBack(EVENT_COLLISION * event);
static void Intro2(void);
static void tmrIntro2Animation(EVENT_TIMER * event);
static void cntIntro2End(EVENT_COUNTER * event);
static void ShowScores(void);
static void tmrWaitScoreShow(EVENT_TIMER * event);
static void gameTankTerminate(void);

static void LevelUp(void);
static void tmrFlagUp(EVENT_TIMER * event);


/***************
	函数实现
****************/
GAME_CLASS * gameTankWarReg(){
	// 构建结构体
	gameTankWar.funcGame = gameTankGaming;
	gameTankWar.funcInit = gameTankInit;
	gameTankWar.pLogo = shapeTankLogo;
	gameTankWar.pLogoHL = shapeTankLogoHL;
	gameTankWar.nameWAV = 11;
    gameTankWar.funcTerminate = gameTankTerminate;
	Debug_SendMsg("GameTankWar", "Game Struct set.", DEBUG_MSG_INFO);
	// 注册到状态机
	gameReg(&gameTankWar);
	return &gameTankWar;
}

static void gameTankGaming(){
	// Debug_SendMsg("GameTankWar", "frame", DEBUG_MSG_INFO);
	generalLoop();
}

static void gameTankTerminate()
{
	OS_ERR err;
    generalTerminate();
	Play_List(156);
	OSTimeDly(7000,OS_OPT_TIME_DLY,&err);
	pGclassTankRed = 0;
	pGclassTankBlue = 0;
	pGclassMissile = 0;
    pGclassBullet = 0;
    pGclassBomb = 0;
    pGclassBlow = 0;
    pGclassOne = 0;
    pGclassZero = 0;
    pGclassTwo = 0;
    pGclassThree = 0;
    pGclassVs = 0;

    pGclassLand1 = 0;
    pGclassLand2 = 0;
    pGclassMarine1 = 0;
    pGclassMarine2 = 0;
    pGclassSea1 = 0;
    pGclassSea2 = 0;
    pGclassBlueScore = 0;
    pGclassRedScore = 0;
    pGclassSuperCloned = (G_CLASS *)0;
    pGclassRedFlag = 0;
    pGclassGreenFlag = 0;

    pEventTmrBulletFire = 0;
    pEventCntIntro1End = 0;
    pEventGameKey = 0;
    pEventGameAngle = 0;
    pEventCntIntro2End = 0;
}

static void gameTankInit(){
	
	OS_ERR err;
	Play_List(157);
	OSTimeDly(7000,OS_OPT_TIME_DLY,&err);
	Debug_SendMsg("GameTankWar", "Game Init starts.", DEBUG_MSG_INFO);
	level = 1;
	BlueScore = 0;
	RedScore = 0;
	// land
	pGclassLand1 = newGclass(8,8,0,0);
	addShape(pGclassLand1, shapeLand);
	pGclassLand1 -> hide = 0;
	pGclassLand1 -> brightness = 1;

	pGclassLand2 = newGclass(8,8,40,0);
	addShape(pGclassLand2, shapeLand);
	pGclassLand2 -> hide = 0;
	pGclassLand2 -> brightness = 1;
	
	//Marine
	pGclassMarine1 = newGclass(8,8,8,0);
	addShape(pGclassMarine1, shapeMarine);
	pGclassMarine1 -> hide = 0;
	pGclassMarine1 -> brightness = 1;
	setFlpud(pGclassMarine1, GUI_FLP_POS);
	pGclassMarine2 = newGclass(8,8,32,0);
	addShape(pGclassMarine2, shapeMarine);
	pGclassMarine2 -> hide = 0;
	pGclassMarine2 -> brightness = 1;
	
	//Sea
	pGclassSea1 = newGclass(8,8,16,0);
	addShape(pGclassSea1, shapeSea);
	pGclassSea1 -> hide = 0;
	pGclassSea1 -> brightness = 1;
	pGclassSea2 = newGclass(8,8,24,0);
	addShape(pGclassSea2, shapeSea);
	pGclassSea2 -> hide = 0;
	pGclassSea2  -> brightness = 1;
	
	
	// missile
	pGclassMissile = newGclass(3,3,0,0);
	addShape(pGclassMissile, shapeMissile);
	pGclassMissile -> hide = 0;
	pGclassMissile  -> brightness = 0.3;
	// tanks
	pGclassTankRed = newGclass(3,3,3,0);
	addShape(pGclassTankRed, shapeTankRed);
	pGclassTankRed -> hide = 0;
	pGclassTankRed  -> brightness = 0.3;
	setFlpud(pGclassTankRed, GUI_FLP_POS);
	
	pGclassTankBlue = newGclass(3,3,42,4);
	addShape(pGclassTankBlue, shapeTankBlue);
	pGclassTankBlue -> hide = 0;
	pGclassTankBlue  -> brightness = 0.3;
	
	//Flags
	pGclassRedFlag = newGclass(8,8,40,0);
	addShape(pGclassRedFlag, shapeFlagRed);
	pGclassRedFlag -> hide = 0;
	setFlpud(pGclassRedFlag, GUI_FLP_POS);
	
	pGclassGreenFlag = newGclass(8,8,0,0);
	addShape(pGclassGreenFlag, shapeFlagGreen);
	pGclassGreenFlag -> hide = 0;
	setFlpud(pGclassGreenFlag, GUI_FLP_POS);
	// bullet
	pGclassBullet = newGclass(3,3,12,0);
	addShape(pGclassBullet, shapeBullet);
	pGclassBullet -> hide = 0;
	pGclassBullet  -> brightness = 0.3;
	
	// checkList();
	// Bomb
	// pGclassBomb = newGclass(3,3,12,0);
	// addShape(pGclassBomb, shapeBomb);
	// pGclassBomb -> hide = 0;
	// Blow
	// pGclassBlow = newGclass(5,5,12,0);
	// addShape(pGclassBlow, shapeBlow);
	// pGclassBlow -> hide = 0;
	
	// number
	pGclassOne = newGclass(7,4,12,0);
	addShape(pGclassOne, shapeOne);
	pGclassOne -> hide = 0;
	
	pGclassZero = newGclass(7,4,12,0);
	addShape(pGclassZero, shapeZero);
	pGclassZero -> hide = 0;
	
	pGclassTwo = newGclass(7,4,12,0);
	addShape(pGclassTwo, shapeTwo);
	pGclassTwo -> hide = 0;
	
	pGclassThree = newGclass(7,4,12,0);
	addShape(pGclassThree, shapeThree);
	pGclassThree -> hide = 0;
	
	pGclassVs = newGclass(1,4, 24,2);
	addShape(pGclassVs, shapeVS);
	pGclassVs -> hide = 0;
	
	intro1();
	
}

static void intro1(){
	Debug_SendMsg("GameTankWar", "Intro1 start", DEBUG_MSG_INFO);
	scen = 1;
	// outputLists();
	EVENT_TIMER eventTmr;
	eventTmr.cnt = 20;
	eventTmr.cnt_rst = 20;
	eventTmr.once_flag = EVENT_TIMER_MULT;
	eventTmr.callBack = tmrIntro1Animation;
	appendETML(eventTmr);
	
	EVENT_COUNTER eventCnt;
	eventCnt.cnt = 0;
	eventCnt.cnt_top = 3;
	eventCnt.triger_side = EVENT_COUNTER_TRG_LAGER;
	eventCnt.callBack = cntIntro1End;
	pEventCntIntro1End = appendECTL(eventCnt);
	pGclassLand1 -> hide = 0;
	pGclassLand2 -> hide = 0;
	pGclassMarine1 -> hide = 0;
	pGclassMarine2 -> hide = 0;
	pGclassSea1 -> hide = 0;
	pGclassSea2 -> hide = 0;
	Play_List(3);
}

static void tmrIntro1Animation(EVENT_TIMER * event){
	Debug_SendMsg("GameTankWar", "Intro1 Animation", DEBUG_MSG_INFO);
	pEventCntIntro1End -> cnt ++ ;
	switch(pEventCntIntro1End -> cnt){
		case 1:
			// land
			pGclassLand1 -> hide = 1;
			pGclassLand2 -> hide = 1;
			pGclassTankBlue -> hide =1;
			pGclassTankRed -> hide =1;
			setLocation(pGclassTankBlue, LEVEL0_Blue_X, 4);
			setLocation(pGclassTankRed, LEVEL0_RED_X, 0);
			Debug_SendMsg("GameTankWar", "Land show", DEBUG_MSG_INFO);
			break;
		case 2:
			//Marine
			pGclassMarine1 -> hide = 1;
			pGclassMarine2 -> hide = 1;
			Debug_SendMsg("GameTankWar", "Marine show", DEBUG_MSG_INFO);
			break;
		case 3:
			//Sea
			pGclassSea1 -> hide = 1;
			pGclassSea2 -> hide = 1;
			Debug_SendMsg("GameTankWar", "Sea show", DEBUG_MSG_INFO);
			cutOffETMLNode(event);
			break;
	}
}

static void cntIntro1End(EVENT_COUNTER * event){
	Debug_SendMsg("GameTankWar", "Intro1 Ends.", DEBUG_MSG_INFO);
	cutOffECTLNode(event);
	Game();
}

static void Game(){
	Debug_SendMsg("GameTankWar", "Game starts.", DEBUG_MSG_INFO);
	setLocation(pGclassTankBlue, LEVEL0_Blue_X, 4);
	setLocation(pGclassTankRed, LEVEL0_RED_X, 0);
	// Events for bullet
	EVENT_TIMER eventTmr;
	eventTmr.cnt = FIRE_RATE_LV1;
	eventTmr.cnt_rst = FIRE_RATE_LV1;
	eventTmr.once_flag = EVENT_TIMER_MULT;
	eventTmr.callBack = tmrFireCallBack;
	pEventTmrBulletFire = appendETML(eventTmr);
	
	// 按键外部事件
	EVENT_EXT eventExt;
	eventExt.monitoring = KeyMonitor;
	eventExt.callBack = KeyGameCallBack;
	pEventGameKey = appendEEXL(eventExt);
	// 角度外部事件
	eventExt.monitoring = angleMonitor;
	eventExt.callBack = angleCallBack;
	pEventGameAngle = appendEEXL(eventExt);
}

static void tmrFireCallBack(EVENT_TIMER * event){
	if(scen != 1) return;
	level = Ctl_Data.Count/60 + 1;
    if(level > 3){
        level = 3;
    }
	// clone
	int16_t bulletX, bulletY;
	int i;
	for(i = 0; i < level; i++){
		Debug_SendMsg("GameTankWar", "Tank Fire!", DEBUG_MSG_INFO);
		bulletX = pGclassTankRed -> LocXlu + 2 + 2 * i;
		bulletY = pGclassTankRed -> LocYlu;
		G_CLASS * pGclassBulletCloned = cloneGclass(pGclassBullet);
		pGclassBulletCloned -> LocXlu = bulletX;
		pGclassBulletCloned -> LocYlu = bulletY;
		pGclassBulletCloned -> LocXrd = bulletX + pGclassBulletCloned -> SzX -1;
		pGclassBulletCloned -> LocYrd = bulletY + pGclassBulletCloned -> SzY -1;
		pGclassBulletCloned -> hide = 1;
		
		// set movement
		EVENT_TIMER eventTmr;
		eventTmr.pGclass = pGclassBulletCloned;
		eventTmr.cnt = BULLET_FLY_CNT_RST;
		eventTmr.cnt_rst = BULLET_FLY_CNT_RST;
		eventTmr.once_flag = EVENT_TIMER_MULT;
		eventTmr.callBack = tmrBulletRedTankFlyCallBack;
		appendETML(eventTmr);
		
		// 碰撞
		EVENT_COLLISION eventCol;
		eventCol.pGclass1 = pGclassBulletCloned;
		eventCol.pGclass2 = pGclassTankBlue;
		eventCol.callBack = colTankCallBack;
		appendECLL(eventCol);
		
	}
	
	// clone
	for(i = 0; i < level; i++){
		bulletX = pGclassTankBlue -> LocXlu - 2 - 2 * i;
		bulletY = pGclassTankBlue -> LocYlu;
		
		G_CLASS * pGclassBulletCloned = cloneGclass(pGclassBullet);
		pGclassBulletCloned -> LocXlu = bulletX;
		pGclassBulletCloned -> LocYlu = bulletY;
		pGclassBulletCloned -> LocXrd = bulletX + pGclassBulletCloned -> SzX -1;
		pGclassBulletCloned -> LocYrd = bulletY + pGclassBulletCloned -> SzY -1;
		pGclassBulletCloned -> hide = 1;
		
		// set movement
		EVENT_TIMER eventTmr;
		eventTmr.pGclass = pGclassBulletCloned;
		eventTmr.cnt = BULLET_FLY_CNT_RST;
		eventTmr.cnt_rst = BULLET_FLY_CNT_RST;
		eventTmr.once_flag = EVENT_TIMER_MULT;
		eventTmr.callBack = tmrBulletBlueTankFlyCallBack;
		appendETML(eventTmr);
		
		// 碰撞
		EVENT_COLLISION eventCol;
		eventCol.pGclass1 = pGclassBulletCloned;
		eventCol.pGclass2 = pGclassTankRed;
		eventCol.callBack = colTankCallBack;
		appendECLL(eventCol);
		
	}
	// outputLists();
	if(Detection_idle()){
		Play_List(2);
	}
	
}

static u8 KeyMonitor(EVENT_EXT * event){
	if(Ctl_Data.Key_Pflag){
        return 1;
    }
	return 0;
}

static void KeyGameCallBack(EVENT_EXT * event){
	if(Ctl_Data.Key_Pflag & key_LL){
		Debug_SendMsg("GameTankWar", "key_LL", DEBUG_MSG_INFO);
		if(pGclassTankRed -> LocYlu > -1){
			moveY(pGclassTankRed, -1);
		}
		Ctl_Data.Key_Pflag = Ctl_Data.Key_Pflag & ~key_LL;
	}
	if(Ctl_Data.Key_Pflag & key_LR){
		Debug_SendMsg("GameTankWar", "key_LR", DEBUG_MSG_INFO);
		if(pGclassTankRed -> LocYlu < 6){
			moveY(pGclassTankRed, 1);
		}
		Ctl_Data.Key_Pflag = Ctl_Data.Key_Pflag & ~key_LR;
	}
	if(Ctl_Data.Key_Pflag & key_RL){
		Debug_SendMsg("GameTankWar", "key_RL", DEBUG_MSG_INFO);
		if(pGclassTankBlue -> LocYlu < 6){
			moveY(pGclassTankBlue, 1);
		}
		Ctl_Data.Key_Pflag = Ctl_Data.Key_Pflag & ~key_RL;
	}
	if(Ctl_Data.Key_Pflag & key_RR){
		Debug_SendMsg("GameTankWar", "key_RR", DEBUG_MSG_INFO);
		if(pGclassTankBlue -> LocYlu > -1){
			moveY(pGclassTankBlue, -1);
		}
		Ctl_Data.Key_Pflag = Ctl_Data.Key_Pflag & ~key_RR;
	}
	
}

static u8 angleMonitor(EVENT_EXT * event){
	static int angle = 0;
	static int angleLast = 0;
	u8 res;
	if(Ctl_Data.payment == 0){
		return 0;
	}
	angle = (int)abs((int)DMP_Data.Pitch);
	if(angle == ANGLE_TH && angleLast < angle){
		res = 1;
	}else{
		res = 0;
	}
	angleLast = angle;
	return res;
}

static void angleCallBack(EVENT_EXT * event){
	if(pGclassSuperCloned == (G_CLASS*)0){
		int16_t bulletX, bulletY;
		// RedFire
		bulletX = pGclassTankRed -> LocXlu + 2;
		bulletY = pGclassTankRed -> LocYlu;
		pGclassSuperCloned = cloneGclass(pGclassMissile);
		pGclassSuperCloned -> LocXlu = bulletX;
		pGclassSuperCloned -> LocYlu = bulletY;
		pGclassSuperCloned -> LocXrd = bulletX + pGclassSuperCloned -> SzX -1;
		pGclassSuperCloned -> LocYrd = bulletY + pGclassSuperCloned -> SzY -1;
		pGclassSuperCloned -> hide = 1;
		setFlpud(pGclassSuperCloned, GUI_FLP_POS);
		
		// set movement
		EVENT_TIMER eventTmr;
		eventTmr.pGclass = pGclassSuperCloned;
		eventTmr.cnt = MISSILE_FLY_CNT_RST;
		eventTmr.cnt_rst = MISSILE_FLY_CNT_RST;
		eventTmr.once_flag = EVENT_TIMER_MULT;
		eventTmr.callBack = tmrBulletRedTankFlyCallBack;
		appendETML(eventTmr);
		
		// 碰撞
		EVENT_COLLISION eventCol;
		eventCol.pGclass1 = pGclassSuperCloned;
		eventCol.pGclass2 = pGclassTankBlue;
		eventCol.callBack = colTankCallBack;
		appendECLL(eventCol);
		
		// BlueFire
		bulletX = pGclassTankBlue -> LocXlu - 2;
		bulletY = pGclassTankBlue -> LocYlu;
		pGclassSuperCloned = cloneGclass(pGclassMissile);
		pGclassSuperCloned -> LocXlu = bulletX;
		pGclassSuperCloned -> LocYlu = bulletY;
		pGclassSuperCloned -> LocXrd = bulletX + pGclassSuperCloned -> SzX -1;
		pGclassSuperCloned -> LocYrd = bulletY + pGclassSuperCloned -> SzY -1;
		pGclassSuperCloned -> hide = 1;
		
		// set movement
		eventTmr.pGclass = pGclassSuperCloned;
		eventTmr.cnt = MISSILE_FLY_CNT_RST;
		eventTmr.cnt_rst = MISSILE_FLY_CNT_RST;
		eventTmr.once_flag = EVENT_TIMER_MULT;
		eventTmr.callBack = tmrBulletBlueTankFlyCallBack;
		appendETML(eventTmr);
		
		// 碰撞
		eventCol.pGclass1 = pGclassSuperCloned;
		eventCol.pGclass2 = pGclassTankRed;
		eventCol.callBack = colTankCallBack;
		appendECLL(eventCol);
		Play_List(22);
	}
}

static void tmrBulletRedTankFlyCallBack(EVENT_TIMER * event){
	
	moveX(event -> pGclass, 1);
	
	if(redTankBulletMonitor(event)){
		cutOffECLLNodeByGclass1(event -> pGclass);
		redTankBulletCallBack(event);
		cutOffETMLNode(event);
	}
	if(scen != 1){
		cutOffECLLNodeByGclass1(event -> pGclass);
		redTankBulletCallBack(event);
		cutOffETMLNode(event);

	}
	// Debug_SendMsg("GameTankWar", "Flying", DEBUG_MSG_INFO);
}

static void tmrBulletBlueTankFlyCallBack(EVENT_TIMER * event){
	
	moveX(event -> pGclass, -1);
	
	if(blueTankBulletMonitor(event)){
		cutOffECLLNodeByGclass1(event -> pGclass);
		redTankBulletCallBack(event);
		cutOffETMLNode(event);
	}
	if(scen != 1){
		cutOffECLLNodeByGclass1(event -> pGclass);
		redTankBulletCallBack(event);
		cutOffETMLNode(event);
		outputLists();
		checkList();
	}
	// Debug_SendMsg("GameTankWar", "Flying", DEBUG_MSG_INFO);
}

static u8 redTankBulletMonitor(EVENT_TIMER * event){
	if(event -> pGclass -> LocXlu > 47){
		return 1;
	}else{
		return 0;
	}
}

static u8 blueTankBulletMonitor(EVENT_TIMER * event){
	if(event -> pGclass -> LocXlu < -1){
		return 1;
	}else{
		return 0;
	}
}

static void redTankBulletCallBack(EVENT_TIMER * event){
	cutOffClonedGclassNode(event -> pGclass);
	if(pGclassSuperCloned == event -> pGclass){
		pGclassSuperCloned = (G_CLASS*)0;
	}
}

static void colTankCallBack(EVENT_COLLISION * event){
	Debug_SendMsg("GameTankWar", "collide.", DEBUG_MSG_INFO);
	
	// cutOffEEXLNode(pEventGameKey);
	// cutOffEEXLNode(pEventGameAngle);
	// cutOffETMLNode(pEventTmrBulletFire);
	cutOffClonedGclassNode(event -> pGclass1);
	cutOffETMLNodeByGclass(event -> pGclass1);
	if(pGclassSuperCloned == event -> pGclass1){
		pGclassSuperCloned = (G_CLASS*)0;
	}
	cutOffECLLNode(event);
	Play_List(1);
	//Intro2();	// 显示分数
	if(event -> pGclass2 == pGclassTankBlue){
		if(BlueScore == 0){
			RedScore ++;
			setLocation(pGclassTankRed, pGclassTankRed -> LocXlu + TANK_MARCH_STEP, pGclassTankRed -> LocYlu);
		}else{
			BlueScore --;
			setLocation(pGclassTankBlue, pGclassTankBlue -> LocXlu + TANK_MARCH_STEP, pGclassTankBlue -> LocYlu);
		}
	}else{
		if(RedScore == 0){
			BlueScore ++;
			setLocation(pGclassTankBlue, pGclassTankBlue -> LocXlu - TANK_MARCH_STEP, pGclassTankBlue -> LocYlu);
		}else{
			RedScore --;
			setLocation(pGclassTankRed, pGclassTankRed -> LocXlu - TANK_MARCH_STEP, pGclassTankRed -> LocYlu);
		}
	}
	LevelUp();
}

static void LevelUp(){
	Debug_SendMsg("GameTankWar", "Level Up!", DEBUG_MSG_INFO);
	if(BlueScore == TANK_MARCH_ROUND){
		Debug_SendMsg("GameTankWar", "Blue Win!", DEBUG_MSG_INFO);
		Play_List(19);
		pGclassGreenFlag -> hide = 1;
		pGclassTankBlue -> hide = 0;
		pGclassTankRed -> hide = 0;
		cutOffEEXLNode(pEventGameKey);
		cutOffEEXLNode(pEventGameAngle);
		cutOffETMLNode(pEventTmrBulletFire);
		EVENT_TIMER eventTmr;
		eventTmr.cnt = 100;
		eventTmr.cnt_rst = 100;
		eventTmr.once_flag = EVENT_TIMER_ONCE;
		eventTmr.callBack = tmrFlagUp;
		appendETML(eventTmr);
	}
	
	if(RedScore == TANK_MARCH_ROUND){
		Debug_SendMsg("GameTankWar", "Red Win!", DEBUG_MSG_INFO);
		Play_List(19);
		pGclassRedFlag -> hide = 1;
		pGclassTankBlue -> hide = 0;
		pGclassTankRed -> hide = 0;
		cutOffEEXLNode(pEventGameKey);
		cutOffEEXLNode(pEventGameAngle);
		cutOffETMLNode(pEventTmrBulletFire);
		EVENT_TIMER eventTmr;
		eventTmr.cnt = 100;
		eventTmr.cnt_rst = 100;
		eventTmr.once_flag = EVENT_TIMER_ONCE;
		eventTmr.callBack = tmrFlagUp;
		appendETML(eventTmr);
	}
	// switch(BlueScore){
		// case 0:
			// Debug_SendMsg("GameTankWar", "Blue Level Up to 0!", DEBUG_MSG_INFO);
			// setLocation(pGclassTankBlue, LEVEL0_Blue_X, pGclassTankBlue -> LocYlu);
			// break;
		// case 1:
			// Debug_SendMsg("GameTankWar", "Blue Level Up to 1!", DEBUG_MSG_INFO);
			// setLocation(pGclassTankBlue, LEVEL1_Blue_X, pGclassTankBlue -> LocYlu);
			// break;
		// case 2:
			// Debug_SendMsg("GameTankWar", "Blue Level Up to 2!", DEBUG_MSG_INFO);
			// setLocation(pGclassTankBlue, LEVEL2_Blue_X, pGclassTankBlue -> LocYlu);
			// break;
		// case 3:
			// Debug_SendMsg("GameTankWar", "Blue Win!", DEBUG_MSG_INFO);
			// Play_List(19);
			// pGclassGreenFlag -> hide = 1;
			// pGclassTankBlue -> hide = 0;
			// pGclassTankRed -> hide = 0;
			// cutOffEEXLNode(pEventGameKey);
			// cutOffEEXLNode(pEventGameAngle);
			// cutOffETMLNode(pEventTmrBulletFire);
			// EVENT_TIMER eventTmr;
			// eventTmr.cnt = 100;
			// eventTmr.cnt_rst = 100;
			// eventTmr.once_flag = EVENT_TIMER_ONCE;
			// eventTmr.callBack = tmrFlagUp;
			// appendETML(eventTmr);
			// 显示旗子
			
			// break;
	// }
	// switch(RedScore){
		// case 0:
			// Debug_SendMsg("GameTankWar", "Red Level Up to 0!", DEBUG_MSG_INFO);
			// setLocation(pGclassTankRed, LEVEL0_RED_X, pGclassTankRed -> LocYlu);
			// break;
		// case 1:
			// Debug_SendMsg("GameTankWar", "Red Level Up to 1!", DEBUG_MSG_INFO);
			// setLocation(pGclassTankRed, LEVEL1_RED_X, pGclassTankRed -> LocYlu);
			// break;
		// case 2:
			// Debug_SendMsg("GameTankWar", "Red Level Up to 2!", DEBUG_MSG_INFO);
			// setLocation(pGclassTankRed, LEVEL2_RED_X, pGclassTankRed -> LocYlu);
			// break;
		// case 3:
			// Debug_SendMsg("GameTankWar", "Red Win!", DEBUG_MSG_INFO);
			// Play_List(19);
			// pGclassRedFlag -> hide = 1;
			// pGclassTankBlue -> hide = 0;
			// pGclassTankRed -> hide = 0;
			// cutOffEEXLNode(pEventGameKey);
			// cutOffEEXLNode(pEventGameAngle);
			// cutOffETMLNode(pEventTmrBulletFire);
			// EVENT_TIMER eventTmr;
			// eventTmr.cnt = 100;
			// eventTmr.cnt_rst = 100;
			// eventTmr.once_flag = EVENT_TIMER_ONCE;
			// eventTmr.callBack = tmrFlagUp;
			// appendETML(eventTmr);
			// // 显示旗子
			
			// break;
	// }
	Debug_SendMsg("GameTankWar", "Level Up finished!", DEBUG_MSG_INFO);
}

static void tmrFlagUp(EVENT_TIMER * event){
	Debug_SendMsg("GameTankWar", "Flag Up Ends.", DEBUG_MSG_INFO);
	//cutOffETMLNode(event);
	BlueScore = 0;
	RedScore = 0;
	pGclassGreenFlag -> hide = 0;
	pGclassRedFlag -> hide = 0;
	intro1();
}


static void Intro2(){
	Debug_SendMsg("GameTankWar", "Intro2.", DEBUG_MSG_INFO);
	// 动画计时器
	//scen = 2;
	EVENT_TIMER eventTmr;
	eventTmr.cnt = 20;
	eventTmr.cnt_rst = 20;
	eventTmr.once_flag = EVENT_TIMER_MULT;
	eventTmr.callBack = tmrIntro2Animation;
	appendETML(eventTmr);
	
	// 过场完成计数器
	EVENT_COUNTER eventCnt;
	eventCnt.cnt = 0;
	eventCnt.cnt_top = 3;
	eventCnt.triger_side = EVENT_COUNTER_TRG_LAGER;
	eventCnt.callBack = cntIntro2End;
	pEventCntIntro2End = appendECTL(eventCnt);
	
}

static void tmrIntro2Animation(EVENT_TIMER * event){
	pEventCntIntro2End -> cnt ++ ;
	switch(pEventCntIntro2End -> cnt){
		case 1:
			// land
			pGclassLand1 -> hide = 0;
			pGclassLand2 -> hide = 0;
			pGclassTankBlue -> hide =0;
			pGclassTankRed -> hide =0;
			Debug_SendMsg("GameTankWar", "Land hide", DEBUG_MSG_INFO);
			break;
		case 2:
			//Marine
			pGclassMarine1 -> hide = 0;
			pGclassMarine2 -> hide = 0;
			Debug_SendMsg("GameTankWar", "Marine hide", DEBUG_MSG_INFO);
			break;
		case 3:
			//Sea
			pGclassSea1 -> hide = 0;
			pGclassSea2 -> hide = 0;
			Debug_SendMsg("GameTankWar", "Sea hide", DEBUG_MSG_INFO);
			cutOffETMLNode(event);
			break;
	}
}

static void cntIntro2End(EVENT_COUNTER * event){
	Debug_SendMsg("GameTankWar", "Intro2 Ends.", DEBUG_MSG_INFO);
	cutOffECTLNode(event);
	Game();
}

static void ShowScores(){
	Debug_SendMsg("GameTankWar", "Scores show.", DEBUG_MSG_INFO);
	pGclassVs -> hide = 1;
	
	// clone
	switch(BlueScore){
		case 0:
			pGclassBlueScore = cloneGclass(pGclassZero);
			pGclassBlueScore -> LocXlu = 26;
			pGclassBlueScore -> LocYlu = 2;
			pGclassBlueScore -> LocXrd = 26 + pGclassBlueScore -> SzX -1;
			pGclassBlueScore -> LocYrd = 2 + pGclassBlueScore -> SzY -1;
			pGclassBlueScore -> hide = 1;
			setFlprl(pGclassBlueScore, GUI_FLP_POS);
			break;
		case 1:
			pGclassBlueScore = cloneGclass(pGclassOne);
			pGclassBlueScore -> LocXlu = 26;
			pGclassBlueScore -> LocYlu = 2;
			pGclassBlueScore -> LocXrd = 26 + pGclassBlueScore -> SzX -1;
			pGclassBlueScore -> LocYrd = 2 + pGclassBlueScore -> SzY -1;
			pGclassBlueScore -> hide = 1;
			setFlprl(pGclassBlueScore, GUI_FLP_POS);
			break;
		case 2:
			pGclassBlueScore = cloneGclass(pGclassTwo);
			pGclassBlueScore -> LocXlu = 26;
			pGclassBlueScore -> LocYlu = 2;
			pGclassBlueScore -> LocXrd = 26 + pGclassBlueScore -> SzX -1;
			pGclassBlueScore -> LocYrd = 2 + pGclassBlueScore -> SzY -1;
			pGclassBlueScore -> hide = 1;
			setFlprl(pGclassBlueScore, GUI_FLP_POS);
			break;
		case 3:	
			pGclassBlueScore = cloneGclass(pGclassThree);
			pGclassBlueScore -> LocXlu = 26;
			pGclassBlueScore -> LocYlu = 2;
			pGclassBlueScore -> LocXrd = 26 + pGclassBlueScore -> SzX -1;
			pGclassBlueScore -> LocYrd = 2 + pGclassBlueScore -> SzY -1;
			pGclassBlueScore -> hide = 1;
			setFlprl(pGclassBlueScore, GUI_FLP_POS);
			break;
	}
	
	switch(RedScore){
		case 0:
			pGclassRedScore = cloneGclass(pGclassZero);
			pGclassRedScore -> LocXlu = 16;
			pGclassRedScore -> LocYlu = 2;
			pGclassRedScore -> LocXrd = 16 + pGclassRedScore -> SzX -1;
			pGclassRedScore -> LocYrd = 2 + pGclassRedScore -> SzY -1;
			pGclassRedScore -> hide = 1;
			setFlpud(pGclassRedScore, GUI_FLP_POS);
			break;
		case 1:
			pGclassRedScore = cloneGclass(pGclassOne);
			pGclassRedScore -> LocXlu = 16;
			pGclassRedScore -> LocYlu = 2;
			pGclassRedScore -> LocXrd = 16 + pGclassRedScore -> SzX -1;
			pGclassRedScore -> LocYrd = 2 + pGclassRedScore -> SzY -1;
			pGclassRedScore -> hide = 1;
			setFlpud(pGclassRedScore, GUI_FLP_POS);
			break;
		case 2:
			pGclassRedScore = cloneGclass(pGclassTwo);
			pGclassRedScore -> LocXlu = 16;
			pGclassRedScore -> LocYlu = 2;
			pGclassRedScore -> LocXrd = 16 + pGclassRedScore -> SzX -1;
			pGclassRedScore -> LocYrd = 2 + pGclassRedScore -> SzY -1;
			pGclassRedScore -> hide = 1;
			setFlpud(pGclassRedScore, GUI_FLP_POS);
			break;
		case 3:	
			pGclassRedScore = cloneGclass(pGclassThree);
			pGclassRedScore -> LocXlu = 16;
			pGclassRedScore -> LocYlu = 2;
			pGclassRedScore -> LocXrd = 16 + pGclassRedScore -> SzX -1;
			pGclassRedScore -> LocYrd = 2 + pGclassRedScore -> SzY -1;
			pGclassRedScore -> hide = 1;
			break;
			setFlpud(pGclassRedScore, GUI_FLP_POS);
	}
	
	if(BlueScore == 3 || RedScore == 3){
		BlueScore = 0;
		RedScore = 0;
		level = 1;
		// event
		EVENT_TIMER eventTmr;
		eventTmr.cnt = 100;
		eventTmr.cnt_rst = 50;
		eventTmr.once_flag = EVENT_TIMER_ONCE;
		eventTmr.callBack = tmrWaitScoreShow;
		appendETML(eventTmr);
		Play_List(19);
	}else{
		// event
		level++;
		Debug_SendMsg("GameTankWar", "level up", DEBUG_MSG_INFO);
		EVENT_TIMER eventTmr;
		eventTmr.cnt = 100;
		eventTmr.cnt_rst = 50;
		eventTmr.once_flag = EVENT_TIMER_ONCE;
		eventTmr.callBack = tmrWaitScoreShow;
		appendETML(eventTmr);
	}
	
	
}

static void tmrWaitScoreShow(EVENT_TIMER * event){
	pGclassVs -> hide = 0;
	cutOffClonedGclassNode(pGclassRedScore);
	cutOffClonedGclassNode(pGclassBlueScore);
	intro1();
}

// callBacks








