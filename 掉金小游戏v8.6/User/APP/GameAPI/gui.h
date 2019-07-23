/************************
STM32 GUI Library 1.2

Built by Billy Liu

*************************/
#ifndef __GUI_H
#define __GUI_H
#include "stm32f10x.h"

#define GUI_FACEING_UP 0
#define GUI_FACEING_RIGHT 1
#define GUI_FACEING_DOWN 2
#define GUI_FACEING_LEFT 3
#define GUI_DIRCT_POS 1
#define GUI_DIRCT_NEG 2
#define GUI_FLP_POS 1
#define GUI_FLP_NEG 0
#define GUI_FLP_FLAG_POS 1
#define GUI_FLP_FLAG_NEG 0
#define GUI_ROTT_CLKW 0	// clockwise
#define GUI_ROTT_ICLW 1	// inversed clockwise

typedef struct shapeMap{
	const u32 ** pMap;
	//char * name;
	struct shapeMap * next;
}SHAPE_MAP_NODE;

struct hookNode;
typedef struct{
	SHAPE_MAP_NODE * pCurShape;	// 当前形状结构体指针
	SHAPE_MAP_NODE * pShapes;	// 所有角色形状的链表
	
	int16_t 	LocXlu;		// 左上点坐标
	int16_t 	LocYlu;		// 左上点坐标
	int16_t 	LocXrd;		// 右下点坐标
	int16_t 	LocYrd;		// 右下点坐标
	u8	SzX;		// 大小x轴
	u8	SzY;		// 大小y轴
	u8	directionX;	// x轴运动方向，1:正方向，2：负方向
	u8	directionY;	// y轴运动方向，1:正方向，2：负方向
	// struct hookNode * pMovementXHook;	// 移动钩子函数
	// struct hookNode * pMovementYHook;	// 移动钩子函数
	
	u8	flplr;		// 左右翻转标志，布尔值
	u8	flpud;		// 上下翻转标志，布尔值
	u8	flplrFlag;	// 左右反转动画标志，布尔值
	u8	flpudFlag;	// 上下翻转动画标志，布尔值
	// struct hookNode * pFlplrHook;	// 左右翻转钩子函数
	// struct hookNode * pFlpudHook;	// 移动钩子函数
	
	u8	facing;		// 朝向：后两位二进制决定，00b：上，01b：右，10b：下，11b：左
	u8	rotation;	// 旋转方向，0：顺时针，1：逆时针
	// struct hookNode * pRotationHook;	// 左右翻转钩子函数
	float brightness;
	u8	hide;		// 显示与隐藏，0：隐藏，1：显示
}G_CLASS;

typedef struct gclassListNode{
	G_CLASS * pGclass;
	struct gclassListNode * next;
}G_CLASS_LIST_NODE;

typedef struct{
	G_CLASS * pGclass;
	u16 cnt_rst;
	u16 cnt;
	void (*hookfunc)(G_CLASS *);
}ANIMATION_HOOK;

typedef struct hookNode{
	ANIMATION_HOOK * pHook;
	struct hookNode * next;
}HOOK_NODE;

void guiInit(void);
G_CLASS * newGclass(int16_t SzX, int16_t SzY, int16_t LocX, int16_t LocY);
void addShape(G_CLASS * pGclass, const u32 **pMap);
void changeShape(G_CLASS * pGclass, const u32 **pMap);
G_CLASS * cloneGclass(G_CLASS * paramGclass);
void delHook(ANIMATION_HOOK* pHook);
void delHookNode(HOOK_NODE* pHookNode);
void clearGclass(void);
void destructGclassNode(G_CLASS_LIST_NODE * pGclassNode);
u8 cutOffClonedGclassNode(G_CLASS * pGclassCloned);
void destructGclass(G_CLASS * pGclass);
void destructShapeListNode(SHAPE_MAP_NODE * pShape);
void setVelocityX(G_CLASS * pGclass, 
				u8 directionX,			
				u16 CntRstValue);	// 计数器重载值，0为不移动
void setVelocityY(G_CLASS * pGclass, 
				u8 directionY,			
				u16 CntRstValue);
void setRotation(G_CLASS * pGclass, 
				u8 rotation,		
				u16 CntRstValue);
void setFlplrFlag(G_CLASS * pGclass, 
				u8 flplrFlag,		
				u16 CntRstValue);
void setFlpudFlag(G_CLASS * pGclass, 
				u8 flpudFlag,		
				u16 CntRstValue);
void flprltg(G_CLASS * pGclass);
void flpudtg(G_CLASS * pGclass);
void setFlprl(G_CLASS * pGclass, u8 value);
void setFlpud(G_CLASS * pGclass, u8 value);
void moveY(G_CLASS * pGclass, int16_t value);
void moveX(G_CLASS * pGclass, int16_t value);
void rotate(G_CLASS * pGclass, u8 value);
void appendCustomHook(ANIMATION_HOOK * paramHook);
void destructCustomHooks(void);
void checkListHeads(void);	// Debug用
u32 getGclassPoint(G_CLASS * pGclass, int16_t x, int16_t y);
void Led_lane(void);
void rendStartUp(void);
void setLocation(G_CLASS * pGclass, int16_t LocXlu, int16_t LocYlu);
void destructClonedGclassNode(G_CLASS_LIST_NODE * pGclassNode);
void destructClonedGclass(G_CLASS * pGclass);
#endif
