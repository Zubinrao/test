/************************
STM32 GUI Library 1.2

Built by Billy Liu

*************************/

#include "gui.h"
#include "Drawing.h"
#include <stdlib.h>
#include "debug.h"
//#include "os.h"

/***************
	数据结构
****************/

G_CLASS_LIST_NODE * gclassList;			//G类链表，全局所有注册的图形都在这里。
G_CLASS_LIST_NODE * gclassCloneList;	//克隆的图形元素，析构时不需要析构其中的shape list
HOOK_NODE * hookList;	//钩子函数挂在列表
HOOK_NODE * customHookList;	//用户自定义钩子函数链表

const u32 LOADING[48][8] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00000000, 0x00000000, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00000000, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00006DE2, 0x00000000, 0x00006DE2, 0x00006DE2, 0x00006DE2, 0x00000000, 0x00000000, 0x00006DE2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};

void movementXHook(G_CLASS * pGclass);
void movementYHook(G_CLASS * pGclass);
void rotationHook(G_CLASS * pGclass);
void flpudHook(G_CLASS * pGclass);
void flplrHook(G_CLASS * pGclass);
void checkList();
/***************
	函数实现
****************/

/** @brief  GUI初始化
  * @param  None
  * @retval None
  */
void guiInit(){
	gclassList = (G_CLASS_LIST_NODE*)0;
	gclassCloneList = (G_CLASS_LIST_NODE*)0;
	hookList = (HOOK_NODE*)0;
	customHookList = (HOOK_NODE*)0;
	Debug_SendMsg("GUI","GUI lists initialization finished.", DEBUG_MSG_INFO);
}

/** @brief  新建G类，同时注册到gclassList里面
  * @param  None
  * @retval None
  */
G_CLASS * newGclass(int16_t SzX, int16_t SzY, int16_t LocX, int16_t LocY){
	// 建立G类结构体变量
	
	G_CLASS * pGclass = (G_CLASS *)malloc(sizeof(G_CLASS));
	// Debug_SendMsg("GUI","G class initialization finished.", DEBUG_MSG_INFO);
	pGclass -> LocXlu = LocX;
	pGclass -> LocYlu = LocY;
	pGclass -> SzX = SzX;
	pGclass -> SzY = SzY;
	pGclass -> LocXrd = LocX + SzX - 1;
	pGclass -> LocYrd = LocY + SzY - 1;
	pGclass -> directionX = 1;	// x轴运动方向，布尔值
	pGclass -> directionY = 1;	// y轴运动方向，布尔值
	pGclass -> flplr = 0;		// 左右翻转标志，布尔值
	pGclass -> flpud = 0;		// 上下翻转标志，布尔值
	pGclass -> flplrFlag = 0;	// 左右反转动画标志，布尔值
	pGclass -> flpudFlag = 0;	// 上下翻转动画标志，布尔值
	pGclass -> facing = 0;		// 朝向：后两位二进制决定，00b：上，01b：右，10b：下，11b：左
	pGclass -> rotation = 0;	// 旋转方向，0：顺时针，1：逆时针
	pGclass -> hide = 0;		// 显示与隐藏，0：隐藏，1：显示
	pGclass -> brightness = 1;
	// Debug_SendMsg("GUI","G class initialization finished.", DEBUG_MSG_INFO);
	G_CLASS_LIST_NODE * pGclassNode = (G_CLASS_LIST_NODE *)malloc(sizeof(G_CLASS_LIST_NODE));
	pGclassNode -> pGclass = pGclass;
	pGclassNode -> next = (G_CLASS_LIST_NODE *)0;
	if(gclassList == (G_CLASS_LIST_NODE *)0){
		gclassList = pGclassNode;
	}else{
		G_CLASS_LIST_NODE * pSearch = gclassList;
		while(pSearch -> next != (G_CLASS_LIST_NODE *)0){
			pSearch = pSearch -> next;
		}
		pSearch -> next = pGclassNode;
	}
	Debug_SendMsg("GUI","G class initialization finished.", DEBUG_MSG_INFO);

	pGclass -> pCurShape = (SHAPE_MAP_NODE *)0;
	pGclass -> pShapes = (SHAPE_MAP_NODE *)0;
	
	return pGclass;
}

/** @brief  向G类添加图型
  * @param  None
  * @retval None
  */

void addShape(G_CLASS * pGclass, const u32 **pMap){
	//复制name和map
	
	SHAPE_MAP_NODE * pShape = (SHAPE_MAP_NODE*)malloc(sizeof(SHAPE_MAP_NODE));
	pShape -> pMap = pMap;
	pShape -> next = (SHAPE_MAP_NODE*)0;
	Debug_SendMsg("GUI","Shape added.", DEBUG_MSG_INFO);
	if(pGclass -> pShapes == (SHAPE_MAP_NODE*)0){
		//如果是第一个图形，则当前图形指向它
		
		pGclass -> pShapes = pShape;
		pGclass -> pCurShape = pShape;
		
	}else{
		//向链尾添加
		SHAPE_MAP_NODE * pSearch = pGclass -> pShapes;
		while(pSearch -> next != (SHAPE_MAP_NODE*)0){
			pSearch = pSearch -> next;
		}
		pSearch -> next = pShape;
	}
	
}

/** @brief  改变形状
  * @param  None
  * @retval None
  */

void changeShape(G_CLASS * pGclass, const u32 **pMap){
	//SHAPE_MAP_NODE * pSearch = pGclass -> pShapes;
	/*
	char * str[50];
	sprintf(str, "adds : %x | %x", (int)(pSearch -> pMap), (int)pMap);
	while(pSearch -> pMap != pMap){
		pSearch = pSearch -> next;
		char * str[50];
		sprintf(str, "adds : %x | %x", (int)(pSearch -> pMap), (int)pMap);
		Debug_SendMsg("GUI", str, DEBUG_MSG_INFO);
		if(pSearch == (SHAPE_MAP_NODE*)0){
			return;
		}
	}
	*/
	pGclass -> pCurShape -> pMap = pMap;
}

/** @brief  克隆G类
  * @param  None
  * @retval None
  */
G_CLASS * cloneGclass(G_CLASS * paramGclass){
	// 建立G类结构体变量
	G_CLASS * pGclass = (G_CLASS *)malloc(sizeof(G_CLASS));
	pGclass -> pCurShape = paramGclass -> pCurShape;	// 当前形状结构体指针
	pGclass -> pShapes = paramGclass -> pShapes;	// 所有角色形状的链表
	pGclass -> LocXlu = paramGclass -> LocXlu;		// 左上点坐标
	pGclass -> LocYlu = paramGclass -> LocYlu;		// 左上点坐标
	pGclass -> LocXrd = paramGclass -> LocXrd;		// 右下点坐标
	pGclass -> LocYrd = paramGclass -> LocYrd;		// 右下点坐标
	pGclass -> SzX = paramGclass -> SzX;		// 大小x轴
	pGclass -> SzY = paramGclass -> SzY;		// 大小y轴
	pGclass -> directionX = paramGclass -> directionX;	// x轴运动方向，1:正方向，2：负方向
	pGclass -> directionY = paramGclass -> directionY;	// y轴运动方向，1:正方向，2：负方向
	pGclass -> flplr = paramGclass -> flplr;		// 左右翻转标志，布尔值
	pGclass -> flpud = paramGclass -> flpud;		// 上下翻转标志，布尔值
	pGclass -> flplrFlag = paramGclass -> flplrFlag;	// 左右反转动画标志，布尔值
	pGclass -> flpudFlag = paramGclass -> flpudFlag;	// 上下翻转动画标志，布尔值
	pGclass -> facing = paramGclass -> facing;		// 朝向：后两位二进制决定，00b：上，01b：右，10b：下，11b：左
	pGclass -> rotation = paramGclass -> rotation;	// 旋转方向，0：顺时针，1：逆时针
	pGclass -> hide = paramGclass -> hide;		// 显示与隐藏，0：隐藏，1：显示
	pGclass -> brightness = paramGclass -> brightness;
	
	G_CLASS_LIST_NODE * pGclassNode = (G_CLASS_LIST_NODE *)malloc(sizeof(G_CLASS_LIST_NODE));
	pGclassNode -> pGclass = pGclass;
	pGclassNode -> next = (G_CLASS_LIST_NODE *)0;
	if(gclassCloneList == (G_CLASS_LIST_NODE *)0){
		gclassCloneList = pGclassNode;
	}else{
		G_CLASS_LIST_NODE * pSearch = gclassCloneList;
		while(pSearch -> next != (G_CLASS_LIST_NODE *)0){
			pSearch = pSearch -> next;
		}
		pSearch -> next = pGclassNode;
	}
	// checkList();	
	return pGclass;
}


/** @brief  清空G类列表和克隆G类列表，游戏结束时使用
  * @param  None
  * @retval None
  */
void clearGclass(){
	G_CLASS_LIST_NODE * pSearch = gclassList;
	Debug_SendMsg("GUI", "Start destruct G class list.", DEBUG_MSG_INFO);
	while(gclassList != (G_CLASS_LIST_NODE *)0){
		gclassList = gclassList -> next;
		destructGclassNode(pSearch);
		pSearch = gclassList;
	}
	
	pSearch = gclassCloneList;
	Debug_SendMsg("GUI", "Start destruct G class cloned list.", DEBUG_MSG_INFO);
	while(gclassCloneList != (G_CLASS_LIST_NODE *)0){
		gclassCloneList = gclassCloneList -> next;
		destructClonedGclassNode(pSearch);
		pSearch = gclassCloneList;
	}
}

/** @brief  删除G类列表节点
  * @param  None
  * @retval None
  */
void destructGclassNode(G_CLASS_LIST_NODE * pGclassNode){
	destructGclass(pGclassNode-> pGclass);
	free(pGclassNode);
	pGclassNode = (G_CLASS_LIST_NODE*)0;
}

/** @brief  删除克隆G类列表节点
  * @param  None
  * @retval None
  */
void destructClonedGclassNode(G_CLASS_LIST_NODE * pGclassNode){
	destructClonedGclass(pGclassNode-> pGclass);
	free(pGclassNode);
	pGclassNode = (G_CLASS_LIST_NODE*)0;
}

/** @brief  删除克隆G类列表某一节点
  * @param  None
  * @retval None
  */
u8 cutOffClonedGclassNode(G_CLASS * pGclassCloned){
	G_CLASS_LIST_NODE * pSearch = gclassCloneList;
	if(pSearch == (G_CLASS_LIST_NODE *)0){
		return 1;
	}
	if(pSearch -> pGclass == pGclassCloned){
		gclassCloneList = pSearch -> next;
		free(pSearch -> pGclass);
		free(pSearch);
		pSearch = (G_CLASS_LIST_NODE *)0;
	}else{
		while(pSearch-> next -> pGclass != pGclassCloned){
			
			if(pSearch -> next == (G_CLASS_LIST_NODE *)0){
				return 1;
			}
			pSearch = pSearch -> next;
		}
		G_CLASS_LIST_NODE * pTmp = pSearch -> next;
		pSearch -> next = pSearch -> next -> next;
		free(pTmp -> pGclass);
		free(pTmp);
		pTmp = (G_CLASS_LIST_NODE *)0;
	}
	pGclassCloned = (G_CLASS*)0;
	// checkList();
	return 0;
}

/** @brief  删除G类
  * @param  None
  * @retval None
  */
void destructGclass(G_CLASS * pGclass){
	SHAPE_MAP_NODE* pSearch = pGclass -> pShapes;
	while(pSearch != (SHAPE_MAP_NODE*)0){
		pGclass -> pShapes = pGclass -> pShapes -> next;
		destructShapeListNode(pSearch);
		pSearch = pGclass -> pShapes;
	}
	pGclass -> pCurShape = (SHAPE_MAP_NODE*)0;
	free(pGclass);
	pGclass = (G_CLASS *)0;
}

/** @brief  删除G类
  * @param  None
  * @retval None
  */
void destructClonedGclass(G_CLASS * pGclass){
	free(pGclass);
	pGclass = (G_CLASS *)0;
}

/** @brief  删除形状列表节点
  * @param  None
  * @retval None
  */
void destructShapeListNode(SHAPE_MAP_NODE * pShape){
	free(pShape);
}

/** @brief  设置形状的速度X轴
  * @param  None
  * @retval None
  */
// void setVelocityX(G_CLASS * pGclass, 
				// u8 directionX,			
				// u16 CntRstValue){	// 计数器重载值，0为不移动
	// pGclass -> directionX = directionX;
	// pGclass -> pMovementXHook -> pHook -> cnt_rst = CntRstValue;
// }

/** @brief  设置形状的速度Y轴
  * @param  None
  * @retval None
  */
// void setVelocityY(G_CLASS * pGclass, 
				// u8 directionY,			
				// u16 CntRstValue){	// 计数器重载值，0为不移动
	// pGclass -> directionY = directionY;
	// pGclass -> pMovementYHook -> pHook -> cnt_rst = CntRstValue;
// }

/** @brief  设置形状的旋转动画
  * @param  None
  * @retval None
  */
// void setRotation(G_CLASS * pGclass, 
				// u8 rotation,		
				// u16 CntRstValue){	// 计数器重载值，0为不移动
	// pGclass -> rotation = rotation;
	// pGclass -> pRotationHook -> pHook -> cnt_rst = CntRstValue;
// }

/** @brief  设置形状的翻转动画
  * @param  None
  * @retval None
  */
// void setFlplrFlag(G_CLASS * pGclass, 
				// u8 flplrFlag,		
				// u16 CntRstValue){	// 计数器重载值，0为不移动
	// pGclass -> flplrFlag = flplrFlag;
	// pGclass -> pFlplrHook -> pHook -> cnt_rst = CntRstValue;
// }

/** @brief  设置形状的翻转动画
  * @param  None
  * @retval None
  */
// void setFlpudFlag(G_CLASS * pGclass, 
				// u8 flpudFlag,		
				// u16 CntRstValue){	// 计数器重载值，0为不移动
	// pGclass -> flpudFlag = flpudFlag;
	// pGclass -> pFlpudHook -> pHook -> cnt_rst = CntRstValue;
// }

/** @brief  左右反转
  * @param  None
  * @retval None
  */
void flprltg(G_CLASS * pGclass){
	pGclass -> flplr  = ~pGclass->flplr;
}

/** @brief  上下反转
  * @param  None
  * @retval None
  */
void flpudtg(G_CLASS * pGclass){
	pGclass -> flpud = ~pGclass -> flpud;
}

/** @brief  设定左右反转
  * @param  None
  * @retval None
  */
void setFlprl(G_CLASS * pGclass, u8 value){
	pGclass -> flplr = value;
}

/** @brief  设定上下反转
  * @param  None
  * @retval None
  */
void setFlpud(G_CLASS * pGclass, u8 value){
	pGclass -> flpud = value;
}

/** @brief  Y轴移动
  * @param  None
  * @retval None
  */
void moveY(G_CLASS * pGclass, int16_t value){
	pGclass -> LocYlu += value;
	pGclass -> LocYrd += value;
}

/** @brief  X轴移动
  * @param  None
  * @retval None
  */
void moveX(G_CLASS * pGclass, int16_t value){
	pGclass -> LocXlu += value;
	pGclass -> LocXrd += value;
}

/** @brief  X轴移动
  * @param  None
  * @retval None
  */
void rotate(G_CLASS * pGclass, u8 value){
	pGclass -> facing += value;	// 旋转
	pGclass -> facing &= 0x03;	// 取最后两位
}

void setLocation(G_CLASS * pGclass, int16_t LocXlu, int16_t LocYlu){
	// char str[50];
	// sprintf(str, "Location to : %d, %d", (int)LocXlu, (int)LocYlu);
	// Debug_SendMsg("GUI", str, DEBUG_MSG_INFO);
	pGclass -> LocXlu = LocXlu;
	pGclass -> LocYlu = LocYlu;
	
	pGclass -> LocXrd = pGclass -> LocXlu + pGclass -> SzX - 1;
	pGclass -> LocYrd = pGclass -> LocYlu + pGclass -> SzY - 1;
}

// /** @brief  X轴移动钩子函数，挂在游戏主循环上
  // * @param  None
  // * @retval None
  // */
// void movementXHook(G_CLASS * pGclass){
	// Debug_SendMsg("GUI", "move X!", DEBUG_MSG_INFO);
	// if(pGclass -> directionX == 1){
		// moveX(pGclass, 1);
	// }else{
		// moveX(pGclass, -1);
	// }
// }
// /** @brief  Y轴移动钩子函数，挂在游戏主循环上
  // * @param  None
  // * @retval None
  // */
// void movementYHook(G_CLASS * pGclass){
	// if(pGclass -> directionY == 1){
		// moveY(pGclass, 1);
	// }else{
		// moveY(pGclass, -1);
	// }
// }
// /** @brief  旋转钩子函数，挂在游戏主循环上
  // * @param  None
  // * @retval None
  // */
// void rotationHook(G_CLASS * pGclass){
	// if(pGclass -> rotation == 0){
		// rotate(pGclass, 1);
	// }else{
		// rotate(pGclass, 3);
	// }
// }

// /** @brief  上下翻转钩子函数，挂在游戏主循环上
  // * @param  None
  // * @retval None
  // */
// void flpudHook(G_CLASS * pGclass){
	// if(pGclass -> flpudFlag == 1){
		// flpudtg(pGclass);
	// }
// }

// /** @brief  左右翻转钩子函数，挂在游戏主循环上
  // * @param  None
  // * @retval None
  // */	
// void flplrHook(G_CLASS * pGclass){
	// if(pGclass -> flplrFlag == 1){
		// flplrtg(pGclass);
	// }
// }

/** @brief  渲染画面
  * @param  None
  * @retval None
  */

void render(){
	// checkList();
	Clear_screen();
	// Debug_SendMsg("GUI","==========render frame =========", DEBUG_MSG_INFO);
	// Debug_SendMsg("GUI","Orig. List: ", DEBUG_MSG_INFO);
	G_CLASS_LIST_NODE* pSearch = gclassList;
	while(pSearch != (G_CLASS_LIST_NODE*)0){
		if(pSearch -> pGclass -> hide == 1){
			int16_t itex,itey;
			// Debug_SendMsg("GUI","-----G Class-----", DEBUG_MSG_INFO);
			for(itex = 0; itex < pSearch -> pGclass -> SzX; itex++){
				for(itey = 0; itey < pSearch -> pGclass -> SzY; itey++){
					if(getGclassPoint(pSearch -> pGclass, itex,itey) != 0 
						&& 0 <= pSearch -> pGclass -> LocXlu + itex  
						&& pSearch -> pGclass -> LocXlu + itex <= LED_X
						&& pSearch -> pGclass -> LocYlu + itey <= LED_Y
						&& 0 <= pSearch -> pGclass -> LocYlu + itey){
						Draw_Point_b(pSearch -> pGclass -> LocXlu + itex, pSearch -> pGclass -> LocYlu + itey, getGclassPoint(pSearch -> pGclass, itex,itey), 0, pSearch -> pGclass -> brightness);
						// char strtmp[50];
						// sprintf(strtmp, "%x (%d ,%d)", (int)(*((u32*)(pSearch -> pGclass -> pCurShape -> pMap) + itex * pSearch -> pGclass -> SzX + itey)), pSearch -> pGclass -> LocXlu + itex, pSearch -> pGclass -> LocYlu + itey);
						// Debug_SendMsg("GUI", strtmp, DEBUG_MSG_INFO);
					}
				}
			}
		}
		pSearch = pSearch -> next;
	}
	// Debug_SendMsg("GUI","Cloned List: ", DEBUG_MSG_INFO);
	pSearch = gclassCloneList;
	while(pSearch != (G_CLASS_LIST_NODE*)0){
		if(pSearch -> pGclass -> hide == 1){
			// Debug_SendMsg("GUI","-----G Clone-----", DEBUG_MSG_INFO);
			int16_t itex,itey;
			for(itex = 0; itex < pSearch -> pGclass -> SzX; itex++){
				for(itey = 0; itey < pSearch -> pGclass -> SzY; itey++){
					if(getGclassPoint(pSearch -> pGclass, itex,itey) != 0
						&& 0 <= pSearch -> pGclass -> LocXlu + itex  
						&& pSearch -> pGclass -> LocXlu + itex <= LED_X
						&& pSearch -> pGclass -> LocYlu + itey <= LED_Y
						&& 0 <= pSearch -> pGclass -> LocYlu + itey){
						Draw_Point_b(pSearch -> pGclass -> LocXlu + itex, pSearch -> pGclass -> LocYlu + itey, getGclassPoint(pSearch -> pGclass, itex,itey), 0, pSearch -> pGclass -> brightness);
						// char strtmp[50];
						// sprintf(strtmp, "%x (%d ,%d)", (int)(*((u32*)(pSearch -> pGclass -> pCurShape -> pMap) + itex * pSearch -> pGclass -> SzX + itey)), pSearch -> pGclass -> LocXlu + itex, pSearch -> pGclass -> LocYlu + itey);
						// Debug_SendMsg("GUI", strtmp, DEBUG_MSG_INFO);
					}
				}
			}
		}
		pSearch = pSearch -> next;
	}
	Led_lane();
	Update_Layer();
	
}

/** @brief  运行所有钩子函数
  * @param  None
  * @retval None
  */

void runHooks(){
	// 运行系统钩子
	HOOK_NODE* pSearch = hookList;
	while(pSearch != (HOOK_NODE*)0){
		if(pSearch -> pHook -> cnt_rst != 0){
			pSearch -> pHook -> cnt --;
			if(pSearch -> pHook -> cnt <= 0){
				pSearch -> pHook -> cnt = pSearch -> pHook -> cnt_rst;
				pSearch -> pHook -> hookfunc(pSearch -> pHook -> pGclass);
			}
		}
	}
	
	// 运行自定义钩子
	pSearch = customHookList;
	while(pSearch != (HOOK_NODE*)0){
		if(pSearch -> pHook -> cnt_rst != 0){
			pSearch -> pHook -> cnt --;
			if(pSearch -> pHook -> cnt <= 0){
				pSearch -> pHook -> cnt = pSearch -> pHook -> cnt_rst;
				pSearch -> pHook -> hookfunc(pSearch -> pHook -> pGclass);
			}
		}
	}
}

/** @brief  添加自定义钩子函数
  * @param  None
  * @retval None
  */
  
void appendCustomHook(ANIMATION_HOOK * paramHook){
	ANIMATION_HOOK * pHook = (ANIMATION_HOOK*)malloc(sizeof(ANIMATION_HOOK));
	*pHook = *paramHook; 
	HOOK_NODE * pHookNode = (HOOK_NODE*)malloc(sizeof(HOOK_NODE));
	pHookNode -> pHook = pHook;
	pHookNode -> next =(HOOK_NODE*)0;
	if(customHookList == (HOOK_NODE *)0){
		customHookList = pHookNode;
	}else{
		HOOK_NODE * pSearch = customHookList;
		while(pSearch -> next != (HOOK_NODE *)0){
			pSearch = pSearch -> next;
		}
		pSearch -> next = pHookNode;
	}
	// 不向G类注册自定义钩子函数。
}

/** @brief  析构所有自定义钩子
  * @param  None
  * @retval None
  */
void destructCustomHooks(){
	HOOK_NODE * pSearch = customHookList;
	while(pSearch != (HOOK_NODE *)0){
		customHookList = customHookList -> next;
		free(pSearch -> pHook);
		free(pSearch);
		pSearch = customHookList;
	}
}

void checkList(){
	Debug_SendMsg("GUI", "===gclassCloneList===", DEBUG_MSG_INFO);
	
	G_CLASS_LIST_NODE * pSearch = gclassCloneList;
	while(pSearch != (G_CLASS_LIST_NODE *)0){
		char * str[50];
		sprintf(str, "%x", (int)pSearch);
        Debug_SendMsg("GUI", str, DEBUG_MSG_INFO);
		pSearch = pSearch -> next;		
	}
	Debug_SendMsg("GUI", "===gclassList===", DEBUG_MSG_INFO);
	pSearch = gclassList;
	while(pSearch != (G_CLASS_LIST_NODE *)0){
		char * str[50];
		sprintf(str, "%x", (int)pSearch);
		Debug_SendMsg("GUI", str, DEBUG_MSG_INFO);
		pSearch = pSearch -> next;		
	}
	
}

u32 getGclassPoint(G_CLASS * pGclass, int16_t x, int16_t y){
    int16_t xtmp = x, ytmp = y;
	if(pGclass -> flplr){
		ytmp = pGclass -> SzY - 1 -ytmp;
	}
	if(pGclass -> flpud){
		xtmp = pGclass -> SzX - 1 -xtmp;
	}
	return *((u32*)(pGclass -> pCurShape -> pMap) + xtmp * pGclass -> SzY + ytmp);
	
}

const u32 ledlane[48] = {0x00FE5FD0, 0x00FA64EC, 0x00F26AFC, 0x00E770FE, 0x00D976F1, 0x00C97CD8, 0x00B782B6, 0x00A48990, 0x00918F6B, 0x007E964E, 0x006D9C3C, 0x005DA337, 0x004FA942, 0x0044B059, 0x003CB67A, 0x0038BDA0, 0x0037C3C5, 0x003AC9E4, 0x0041CEF8, 0x004BD4FF, 0x0058D9F7, 0x0067DEE2, 0x0079E3C2, 0x008BE79D, 0x009EEB77, 0x00B1EF57, 0x00C4F340, 0x00D4F637, 0x00E3F83C, 0x00EFFA4F, 0x00F8FC6E, 0x00FDFD92, 0x00FFFEB8, 0x00FDFFDA, 0x00F8FFF2, 0x00EFFFFE, 0x00E3FEFC, 0x00D4FDEB, 0x00C3FBCE, 0x00B1F9AA, 0x009EF684, 0x008BF361, 0x0078F047, 0x0067ED39, 0x0058E939, 0x004BE447, 0x0041E062, 0x003ADB85};

void Led_lane(){
	static u32 offset = 0;
	int i;
	offset ++;
	if(offset >= 48){
		offset = 0;
	}
	// Debug_SendMsg("Control", "LED_Lane", DEBUG_MSG_INFO);
	for(i = 0; i < 48; i++){
		Draw_Point(i, 8, ledlane[(i + offset) % 48], 0);
		Draw_Point(i, 9, ledlane[(i + offset) % 48], 0);
		Draw_Point(i, 10, ledlane[(i + offset) % 48], 0);
		Draw_Point(i, 11, ledlane[(i + offset) % 48], 0);
		Draw_Point(i, 12, ledlane[(i + offset) % 48], 0);
		Draw_Point(i, 13, ledlane[(i + offset) % 48], 0);
		Draw_Point(i, 14, ledlane[(i + offset) % 48], 0);
		Draw_Point(i, 15, ledlane[(i + offset) % 48], 0);
		
	}
	// char str[50];
	// sprintf(str, "Color : %x", (int)(((tmpColor +350) & 0x00ff0000) >> 16));
	// Debug_SendMsg("Drawing", str, DEBUG_MSG_INFO);
}

void rendStartUp(){
	int i,j;
	Led_lane();
	for(i = 0; i < 48; i++){
		for(j = 0; j < 8; j++){
			Draw_Point(47 - i,j, LOADING[i][j], 1);
		}
	}
	Update_Layer();
}
