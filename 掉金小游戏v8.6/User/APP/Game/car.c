#include "debug.h"
#include "Control.h"
#include "math.h"
#include "WavPlay.h"
#include "car.h"
#define angabs(x)     (((x)>0)?(x):-(x))
/***************
	  图形
****************/
#include"shapes.h"

/***************
	游戏变量
****************/
GAME_CLASS gameCar;

/***************
	函数声明
****************/
static void gameTankGaming(void);
static void gameTankInit(void);
static void gameTankTerminate(void);
static void Car_Game(void);

/***************
	函数实现
****************/
GAME_CLASS * gameCarReg(){
	// 构建结构体
	gameCar.funcGame = gameTankGaming;
	gameCar.funcInit = gameTankInit;
	gameCar.pLogo = shapeNoneLogo;
	gameCar.pLogoHL = shapeTankLogoHL;
	gameCar.nameWAV = 63;
  gameCar.funcTerminate = gameTankTerminate;
	Debug_SendMsg("GameTankWar", "Game Struct set.", DEBUG_MSG_INFO);
	// 注册到状态机
	gameReg(&gameCar);
	return &gameCar;
}

static void gameTankGaming(){
	// Debug_SendMsg("GameTankWar", "frame", DEBUG_MSG_INFO);
	//generalLoop();
	Car_Game();
}

static void gameTankTerminate()
{
	Debug_SendMsg("GameTankWar", "Terminate", DEBUG_MSG_INFO);
  generalTerminate();
}

static void gameTankInit(){
	Debug_SendMsg("GameTankWar", "Game Init starts.", DEBUG_MSG_INFO);

}










#define GoldLength 2
#define angabs(x)     (((x)>0)?(x):-(x))
#define Collecterlength 3  //收金者宽度
#define CollecterPlusMark 10
#define BackMark 5 //车辆撞击退货阈值
/////////////////////////////////////车
u8 flood1=2;
u8 flood2=45;
u32 stockColor6=0xCD8500; //收金者颜色1
u8 countCar=0;
u16 countCreat=0;
u8 markCC=8;
u8 markBlue=0;
u8 markRed=0;
u8 mark_1=5;//前进阈值
u8 mark_2=205;
u8 mark_3=205;
u8 mark_7=0;
u8 mark_8=0;
u8 musceMark=0;
u16 markCount=0;
u8 count_9=80;
u8 mark_12=1;
u16 obstacleCount=0;
u8 mark12=1;
u8 mark11=1;
u8 mark13=1;
////////////////////////////////////
////////////////////////////////收金器
u32 ColleterColor0=0xff0000; //收金者颜色1
u32 Colleter1Color0=0xffff00; //收金者颜色1
u32 Colleter2Color0=0xffff00; //收金者颜色1
u32 ColleterColor1=0x00ff00; //收金者颜色2
u32 ColleterColor2=0x993366; //收金者颜色3
u32 stockColor3=0xcc0000; //库存颜色3
u32 stockColor4=0x0000cc; //库存者颜色3
u32 ColleterWid=2;  //收金者宽度
///////////////////////////////////金条云
u32 BrowColor=0xEEEE11;
u32 BrowBorderColor=0x3CC4C4;
/////////////////////////////////////金条颜色
u32 GoldColor=0xEEEE11;
u8 mark5=1;
u8 count6=0;
u8 times=8;
u8 mark62=0;
u8 mark63=0;
////////////全局变量/////////
u8 BrowLeft=20; 
u8 BrowRight=25;	
u8 initMark=1;
u8 winner=2;//1为左方获胜0为右方获胜
int CountMark=10;
u8 update1=0;
u8 mark2=0;
int count1=0;//控制金条速度
int moneyMark=0;
int winnerMove;
u16 count2=0;//控制程序执行次数
int status=1;///////游戏状态
int Lmode=0;
int Rmode=0;
int lastCount=150;
u8 AudioMark=0;
u8 AudioMark1=0;
u8 lightStatus0=1;
u8 lightStatus1=1;
u8 mark_musce=0;
u16 carCount=0;
//////////////赢家属性
#define winnerColor 0xDD22DD
////////////队列//////////////////////////////////
void money();
void clearQueue();
void win();

typedef struct Gold *GNode; /*定义队列的每个节点的类型*/
struct Gold{
	u8 markWay;
	u8 x;
	u8 y;
	u8 wight;
	u8 length;
	u32 color;
	u8 mark;
	u8 id;
}gold;
typedef struct Queue
{
    struct Gold * array;                //数组作为队列
    int front;                  //队列的头索引
    int tail;                   //队列的尾索引
	  int size;
}QUEUE,*PQUEUE;
/////////////////////
typedef struct Gold *GNode; /*定义队列的每个节点的类型*/
u8 Isempty(PQUEUE);
u8 Isfull(PQUEUE);
#define sizemark 50
//初始化队列
static void Init_queue(PQUEUE Q);
static void EnQueue(PQUEUE Q,GNode val);
static void DeQueue(PQUEUE Q,GNode *p_num);
static int GetLength(PQUEUE Q);
static GNode creatGold(u8 x,u8 y,u8 wayMark);
void Init_queue(PQUEUE Q)
{
    Q->array = (struct Gold *)malloc(sizeof(struct Gold)*sizemark);     //申请数组队列的动态空间
    Q->front = 0;
    Q->tail = 0;
	  Q->size=0;
    return;
}

//入队
void EnQueue(PQUEUE Q,GNode val)
{
   if(Isfull(Q))
   {
      // printf("%4d入队失败！队列已满。\n",val);
   }else
   {
       Q->array[Q->tail] =(*val);
       Q->tail = (Q->tail+1)%sizemark;
		   Q->size++;
   }
}

//出队
void DeQueue(PQUEUE Q,GNode *p_num)
{
    if(Isempty(Q))
    {
        // printf("出队失败！队列中没有元素。\n");
    }else
    {
        gold= Q->array[Q->front];
		  	*p_num=&gold;
        Q->front = (Q->front+1)%sizemark;            //头索引加一，如果到达最大索引，从0开始
			  Q->size--;
    }
}

int GetLength(PQUEUE Q){
	return Q->size;
}

//判断队列是否为空
u8 Isempty(PQUEUE Q)
{
    if(Q->front ==  Q->tail)                 //把队头和队尾相同作为队列为空的判断条件
    {
        return 1;
    }else
    {
        return 0;
    }
}

//判断队列是否满
u8 Isfull(PQUEUE Q)
{
    if((Q->tail+1)%sizemark == Q->front) //此处的算法意思是当队列尾再加一个元素就和队列头重合的时候代表队列已满     
    {
        return 1;
    }else
    {
        return 0;
    }
}


GNode creatGold(u8 x,u8 y,u8 wayMark){ //x，y金条初始位置 ,wayMark金条掉落方向
	//struct Gold	gNode;
	gold.x=x;
	gold.y=y;
	gold.markWay=wayMark;
	return &gold;
	
}
/////////////全局队列指针 /////
QUEUE Q;
/////////////////////////////////////////////////

 void initGoldQueue(){ 
    Init_queue(&Q);
//    for(i = 0; i < n; ++i) {
//        x = rand() % 100;//产生一个不大于100的随机数
//				g1=creatGold();
//			  (*g1).x=x;
//        EnQueue(Q,*g1);//x送进队列
//    }
//		i=GetLength(Q);
//    while(notEmpty(Q)) {//判断没有空队列
//        DeQueue(Q,g1);//出队列
//			  free(&g1);   
//    }
	}
//////////////////////////////


////金条云
//struct Brow{
// 
//}brow{}；
//收金器定义
struct CollectGold
{
	int id;
	u8 x;
	u8 y;
	u8 length;
	u8 wid;
	u8 score;
	u8 workPosition;
}collecter_1={1,0,2,3,3,0,0},
 collecter_2={1,45,2,3,3,0,0};

 
 

 
////////参数初始化
static void init(){
 
 ColleterWid=2;  //收金者宽度

////////////全局变量/////////	
 initMark=1;
 status=1;///////游戏状态
	collecter_1.x=0;
	collecter_1.y=2;
	collecter_1.length=3;
	collecter_1.wid=3;
	collecter_1.score=0;
	collecter_1.workPosition=0;
	collecter_2.x=45;
	collecter_2.y=2;
	collecter_2.length=3;
	collecter_2.wid=3;
	collecter_2.score=0;
	collecter_2.workPosition=0;
 flood1=2;
 flood2=45;
 stockColor6=0xCD8500; //收金者颜色1
 countCar=0;
 countCreat=0;
 markCC=8;
 markBlue=0;
 markRed=0;
 mark_1=5;
 mark_2=205;
 mark_3=205;
 mark_7=0;
 mark_8=0;
 mark_12=1;
 markCount=0;
 count_9=80;
 obstacleCount=0;
 mark_musce=0;
 mark12=1;
 mark11=1;
 mark13=1;
 carCount=0;
} 
 //计数器，控制界面刷新速度
static void viewFlushCounter();
 void viewFlushCounter(){
	
}
////////////////////掉金速度动态设置
static void setCountMark();
 void setCountMark(){
	int angle=angabs(DMP_Data.Angular_Velocity)/10;
	if(angle<0)
		angle=-angle;
		CountMark=10-angle;
}
///////////////////判断结束？
u8 over(struct CollectGold *id,struct CollectGold *id2){
	 
		
}
 ////////////金条控制策略

//////////控制金条速度
static void controlSudu(){
	CountMark=10-(collecter_1).x;
	if(CountMark<7)
		CountMark=7;
}
////////////////////////////////////////////////////////////////
static void creatObstacle(struct CollectGold *id,struct CollectGold *id2){
	//GNode g2,g3;
	int x=rand()%3;
	 obstacleCount++;
	 if(mark_3>20){
	// g2=(GNode)malloc(sizeof(gold));
		 if(x==1)
	     gold.y=(*id).y;
		 else
			 gold.y=rand()%6;
	gold.color=(u32)rand();
	gold.x=35;
	gold.markWay=1;
	gold.id=x;
	 EnQueue(&Q,&gold);//x送进队列
	}
	 if(mark_2>20){
	 
		 if(x==1)
	     gold.y=(*id2).y;
		 else
			 gold.y=rand()%6;
	gold.color=(u32)rand();
	gold.x=10;
	gold.markWay=0;
	gold.id=x;
	 EnQueue(&Q,&gold);//x送进队列
	}
}
static void drawObstacle(){
	    GNode g;
	    u8 i=GetLength(&Q);
	    u8 xx=0,x;
	//////////////////////////////////画中心线
	Draw_LineY(24,0,7,(u32)rand());
	Draw_LineY(23,0,7,(u32)rand());
	//Draw_LineY(24,0,7,(u32)rand());
	for(;xx<i;xx++){
		  DeQueue(&Q,&g);
		if((*g).markWay==1&&(*g).x<=20){

			if((*g).id==0){
				Draw_LineY((*g).x,(*g).y,(*g).y+ColleterWid,0xDA70D6);
				Draw_LineY((*g).x+2,(*g).y,(*g).y+ColleterWid,0xDA70D6);
				Draw_LineX((*g).x,(*g).x+ColleterWid,(*g).y+1,(*g).color);
			}
			else if((*g).id==1){
				Draw_LineY((*g).x,(*g).y,(*g).y+ColleterWid,(*g).color);
				Draw_LineY((*g).x+2,(*g).y,(*g).y+ColleterWid,(*g).color);
				Draw_LineY((*g).x+1,(*g).y,(*g).y+ColleterWid,(*g).color);
				
			}
			else if((*g).id==2){
			  Draw_LineY((*g).x,(*g).y,(*g).y+ColleterWid,(u32)rand());
				Draw_LineY((*g).x+2,(*g).y,(*g).y+ColleterWid,(u32)rand());
				Draw_LineY((*g).x+1,(*g).y,(*g).y+ColleterWid,(u32)rand());
			}
		  
			
		}
		else if((*g).markWay==0&&(*g).x>=26){
	
			if((*g).id==2){
				Draw_LineY((*g).x,(*g).y,(*g).y+ColleterWid,0xDA70D6);
				Draw_LineY((*g).x+2,(*g).y,(*g).y+ColleterWid,0xDA70D6);
				Draw_LineX((*g).x,(*g).x+ColleterWid,(*g).y+1,(*g).color);
			}
			else if((*g).id==1){
			 Draw_LineY((*g).x,(*g).y,(*g).y+ColleterWid,(*g).color);
				Draw_LineY((*g).x+2,(*g).y,(*g).y+ColleterWid,(*g).color);
				Draw_LineY((*g).x+1,(*g).y,(*g).y+ColleterWid,(*g).color);
			
			}
			else if((*g).id==0){
			 Draw_LineY((*g).x,(*g).y,(*g).y+ColleterWid,(u32)rand());
				Draw_LineY((*g).x+2,(*g).y,(*g).y+ColleterWid,(u32)rand());
				Draw_LineY((*g).x+1,(*g).y,(*g).y+ColleterWid,(u32)rand());
			}
		  
		}
		  EnQueue(&Q,g);
	}
	    
}

static void contorlVehicle(struct CollectGold *id,struct CollectGold *id2){
	  ///////////
//	GNode g;
//	int i=0,x=0;
//	
//	if(count1++>CountMark){
//		BrowColor=(u32)rand();//////改变云的颜色
//		i=GetLength(Q);
//		count1=0;
//		if(i<5){
//		 createGolds();
//	
//		}		
//		///////////////////////////设置掉金概率40%
//		else if(rand()%10>times)
//			if(i<60)//设置金条数量
//		   createGolds();
//			/////////////
//		 i=GetLength(Q);
//		
//	for(;x<i;x++){
//		DeQueue(Q,&g);//金条出队列	
	///////////////
	      OS_ERR err;
				GNode g1;
				u8 i=GetLength(&Q);
				u8 x_car=0;
		if(countCar++>markCC){
		   countCar=0;

		    if(countCreat++>5){
					if(i<=2){
						carCount++;
					 creatObstacle(&collecter_1,&collecter_2);
					markCount++;
				}
		     else if(rand()%10>5){
					countCreat=0;
					 carCount++;
					creatObstacle(&collecter_1,&collecter_2);
					markCount++;
				}					
			}
			i=GetLength(&Q);
		
			for(;x_car<i;x_car++){
				 
				  DeQueue(&Q,&g1);
				  if((*g1).markWay==1){
						if(mark_3++>50)
						  mark_3=150;
				  if(mark_3>20){
						if((*g1).mark==1){
							
						  continue;
						}	
					if(markRed>mark_1){
						markRed=0;
				//	 flood1--;
						(*id).x++;
						if((*id).x==25){
						status=6;
							break;
						}
							
					}
					if(((*id).x+Collecterlength)>=(*g1).x+1&&((*id).x+Collecterlength)<=(*g1).x+2&&abs(((*g1).y-(*id).y))<1){
						if(mark_7++>=BackMark){//撞击退后阈值
							 mark_7=0;
							(*id).x--;
						}
						if((*id).x-flood1==2&&mark12==1){
							mark_musce=3;
						   mark12=0;
						}
						
						if((*id).x<=flood1){
						  status=2;
							mark_musce=5;
							break;
						}
						  mark_3=0;
						  markRed=0;
						 (*g1).mark=1; 
						  musceMark=1;
						  EnQueue(&Q,g1);
							continue;
					}
					else if(((*id).x)>=(*g1).x&&((*id).x)<=(*g1).x+2&&abs(((*g1).y-(*id).y))<1){
						if(mark_7++>=BackMark){//撞击退后阈值
								 mark_7=0;
								(*id).x--;
							}
						if((*id).x-flood1==2&&mark12==1){
							mark_musce=3;
						   mark12=0;
						}
							if((*id).x<=flood1){
								status=2;
								mark_musce=5;
								break;
							}
								mark_3=0;
								markRed=0;
							 (*g1).mark=1; 
							  musceMark=1;
								EnQueue(&Q,g1);
								continue;
					}
					
					if((*g1).x--<=flood1){
							markRed++;
					  	
							continue;
						}
					}  
				}
				else if((*g1).markWay==0){
					 if(mark_2++>50)
						  mark_2=150;
					 if(mark_2>20){
						 ////////////////
		       if((*g1).mark==1){
						   
							 continue;
						 }
					 if(markBlue>mark_1){
						markBlue=0;
					//	 flood2++;
						 (*id2).x--;
						 if((*id2).x==25){
						  status=7;
							 break;
						 }
							 
					}
					if(((*id2).x+Collecterlength)>=(*g1).x+1&&((*id2).x+Collecterlength)<=(*g1).x+2&&abs(((*g1).y-(*id2).y))<1){
						if(mark_8++>=BackMark){//撞击退后阈值
							 mark_8=0;
							(*id2).x++;
						}
						if(flood2-(*id2).x==2&&mark11==1){
							mark_musce=3;
						  mark11=0;
						}
						if(((*id2).x+2)>=flood2){
						  status=3;
							mark_musce=5;
							break;
						} 
						 mark_2=0;
						 markBlue=0;
					  (*g1).mark=1;
						 musceMark=1;
						 EnQueue(&Q,g1);
							continue;
					}
					else if(((*id2).x)>=(*g1).x&&((*id2).x)<=(*g1).x+2&&abs(((*g1).y-(*id2).y))<1){
						if(mark_8++>=BackMark){//撞击退后阈值
							 mark_8=0;
							(*id2).x++;
						}
						if(flood2-(*id2).x==2&&mark11==1){
							mark_musce=3;
						  mark11=0;
						}
							
						if(((*id2).x+2)>=flood2){
						  status=3;
							mark_musce=5;
							break;
						} 
						 mark_2=0;
						 markBlue=0;
						 musceMark=1;
					  (*g1).mark=1;
						 EnQueue(&Q,g1);
							continue;
					}
					if((*g1).x++>=flood2){
							markBlue++;
						 
							continue;
						}
					  	} 
				}
					EnQueue(&Q,g1);
			}	
			if(count_9>250)
				count_9=80;
			if(count_9++<10);
			
			else if(musceMark==1){
			//爆炸声
				count_9=2;
				musceMark=0;
				switch(rand()%5){
					case 0:
					  	Play_List(50);	
					break;
					case 1:
							Play_List(51);	
					break;
					case 2:
							Play_List(56);	
					break;
					case 3:
							Play_List(52);	
					break;
					case 4:
							Play_List(53);	
					break;
					default :
						 Play_List(56);	
					break;
				}
				
			}
			else if(musceMark==2){
			//赛车声音2
			
			}
			else if(musceMark==3){
			//赛车声音1
				count_9=0;
				musceMark=0;
				Play_List(54);	
			}
			
//			if(musceMark==0&&count_9==80){
//				Play_List(80);				
//			}
			 
		}
}
static void drawVehicle(struct CollectGold *id,struct CollectGold *id2){
     OS_ERR err; 
	   int color=(u32)rand();
   	  if(mark_12==1){
			 (*id).x++;
			 (*id2).x--;
			 OSTimeDly(300,OS_OPT_TIME_DLY,&err);
			 if((*id).x==8)
				 mark_12=0;
			}
      
	    Draw_LineY((*id).x,(*id).y,(*id).y+ColleterWid,color);
		  Draw_LineY((*id).x+2,(*id).y,(*id).y+ColleterWid,color);
		  Draw_LineX((*id).x,(*id).x+ColleterWid,(*id).y+1,0xff0000);
	    //Draw_Point(20,4,0x00ffff,0);
	
			Draw_LineY((*id2).x,(*id2).y,(*id2).y+ColleterWid,color);
			Draw_LineY((*id2).x+2,(*id2).y,(*id2).y+ColleterWid,color);
	    Draw_LineX((*id2).x,(*id2).x+ColleterWid,(*id2).y+1,0x00ff00);
	
}
static void drawFlood(struct CollectGold *id,struct CollectGold *id2){
	      	OS_ERR err; 
				int x1,x2,x3;
	   //   u32 x4=(u32)rand();
	///////////用障碍物数量控制洪水高度
	      if(obstacleCount>20){
					obstacleCount=0;
					flood1++;
					flood2--;
				}
				if(status==2){//左边输
					for(x3=flood1;x3<25;x3++){
						OSTimeDly(200,OS_OPT_TIME_DLY,&err);
						clearQueue();
						for(x1=0;x1<8;x1++){
							x2=rand()%3;
							Draw_LineX(0,x3+x2,x1,stockColor6);	
						}
						Update_Layer();
					}	
					OSTimeDly(10000,OS_OPT_TIME_DLY,&err);
					status=7;
				}
				else if(status==3){//左边输
					for(x3=flood2;x3>25;x3--){
						OSTimeDly(200,OS_OPT_TIME_DLY,&err);
						clearQueue();
						for(x1=0;x1<8;x1++){
							x2=rand()%3;
							Draw_LineX(x3-x2,50,x1,stockColor6);
						}
						Update_Layer();
					}	
					status=6;
				}
				else
				for(x1=0;x1<8;x1++){
					x2=rand()%3;
					Draw_LineX(0,flood1+x2,x1,stockColor6);	
					Draw_LineX(flood2-x2,50,x1,stockColor6);
				}
				
}

 //绘制收金器
static void drawCollectGold(struct CollectGold *id,struct CollectGold *id2){
	
	  ///////////////左边收金者 
	    if(Lmode==0){
			Draw_LineY((*id).x,0,8,ColleterColor2);
	    Draw_LineY((*id).x,(*id).y,(*id).y+ColleterWid,Colleter1Color0);
			Draw_LineY((*id).x+1,(*id).y,(*id).y+ColleterWid,Colleter1Color0);
		  Draw_LineY((*id).x+2,(*id).y,(*id).y+ColleterWid,ColleterColor1);
			}
			else if(Lmode==1){
 
				Colleter1Color0=(u32)rand();
					
			Draw_LineY((*id).x,0,8,ColleterColor2);
	    Draw_LineY((*id).x,(*id).y,(*id).y+ColleterWid+2,Colleter1Color0);
			Draw_LineY((*id).x+1,(*id).y,(*id).y+ColleterWid+2,Colleter1Color0);
		  Draw_LineY((*id).x+2,(*id).y,(*id).y+ColleterWid+2,ColleterColor1);
			}
	    if((*id).x>0){
				int j=1;
				Draw_LineX(0,(*id).x-j,0,stockColor3);	
				Draw_LineX(0,(*id).x-j,1,stockColor3);	
				Draw_LineX(0,(*id).x-j,2,stockColor3);	
				Draw_LineX(0,(*id).x-j,3,stockColor3);	
				Draw_LineX(0,(*id).x-j,4,stockColor3);	
				Draw_LineX(0,(*id).x-j,5,stockColor3);	
				Draw_LineX(0,(*id).x-j,6,stockColor3);	
				Draw_LineX(0,(*id).x-j,7,stockColor3);	
			}
		
		 ///////////////右边收金者 
			if(Rmode==0){
			Draw_LineY((*id2).x,(*id2).y,(*id2).y+ColleterWid,ColleterColor1);
			Draw_LineY((*id2).x+1,(*id2).y,(*id2).y+ColleterWid,Colleter2Color0);
		  Draw_LineY((*id2).x+2,0,8,ColleterColor2);
			Draw_LineY((*id2).x+2,(*id2).y,(*id2).y+ColleterWid,Colleter2Color0);
			}
			else if(Rmode==1){
				
				Colleter2Color0=(u32)rand();
			Draw_LineY((*id2).x,(*id2).y,(*id2).y+ColleterWid+2,ColleterColor1);
			Draw_LineY((*id2).x+1,(*id2).y,(*id2).y+ColleterWid+2,Colleter2Color0);
		  Draw_LineY((*id2).x+2,0,8,ColleterColor2);
			Draw_LineY((*id2).x+2,(*id2).y,(*id2).y+ColleterWid+2,Colleter2Color0);
			}
			 if((*id2).x>0){
				 int x=3;
				 Draw_LineX((*id2).x+x,50,0,stockColor4);	
				 Draw_LineX((*id2).x+x,50,1,stockColor4);
				 Draw_LineX((*id2).x+x,50,2,stockColor4);
				 Draw_LineX((*id2).x+x,50,3,stockColor4);
				 Draw_LineX((*id2).x+x,50,4,stockColor4);
				 Draw_LineX((*id2).x+x,50,5,stockColor4);
				 Draw_LineX((*id2).x+x,50,6,stockColor4);
				 Draw_LineX((*id2).x+x,50,7,stockColor4);
			}

}
//收金器运动控制
static void controlCollecter(struct CollectGold *id,struct CollectGold *id1){
//	if(count6++>20){
//		count6=0;
//	if(Ctl_Data.Key_Pflag)
//	 {
//		switch(Ctl_Data.Key_Pflag)
//			{
//				case key_RL:
//					if((*id).y<5&&Lmode==0)
//						(*id).y++;
//					else if((*id).y<3&&Lmode==1)
//						(*id).y++;
//					break;
//				case key_RR:
//					if((*id).y>0)
//				   (*id).y--;
//					break;
//				case key_LL:
//				if((*id1).y<5&&Rmode==0)
//						(*id1).y++;
//				else if((*id1).y<3&&Rmode==1)
//						(*id1).y++;
//					break;
//				case key_LR:
//					if((*id1).y>0)
//				   (*id1).y--;
//					break;
//				default:
//					break;
//			}	
// }
//	 }
		/*检测是否需要左右移动坦克位置*/
		if(Ctl_Data.Key_Pflag || Ctl_Data.Key_Lflag)
		{
		/*点动控制*/
			if(Ctl_Data.Key_Pflag)
			{
				if(Ctl_Data.Key_Pflag & key_LL)
				{
					Ctl_Data.Key_Pflag &= ~key_LL;			//清除按键
					if((*id1).y>0)
				   (*id1).y--;
				
				}
				else if(Ctl_Data.Key_Pflag & key_LR)
				{
					Ctl_Data.Key_Pflag &= ~key_LR;
					if((*id1).y<5&&Rmode==0)
						(*id1).y++;
				else if((*id1).y<3&&Rmode==1)
						(*id1).y++;
					
				}
				
				if(Ctl_Data.Key_Pflag & key_RL)
				{
					Ctl_Data.Key_Pflag &= ~key_RL;			//清除按键
					if((*id).y<5&&Lmode==0)
						(*id).y++;
					else if((*id).y<3&&Lmode==1)
						(*id).y++;
				}
				else if(Ctl_Data.Key_Pflag & key_RR)
				{
					Ctl_Data.Key_Pflag &= ~key_RR;			//清除按键
					if((*id).y>0)
				   (*id).y--;
				}
			}
			/*长按控制*/
			else if(Ctl_Data.Key_Lflag)
			{
				if(Ctl_Data.Key_Lflag & key_LL)
				{
					if((*id1).y>0)
				   (*id1).y--;
					
				}
				else if(Ctl_Data.Key_Lflag & key_LR)
				{
						if((*id1).y<5&&Rmode==0)
						(*id1).y++;
				else if((*id1).y<3&&Rmode==1)
						(*id1).y++;
				}
				
				if(Ctl_Data.Key_Lflag & key_RL)
				{
					if((*id).y<5&&Lmode==0)
						(*id).y++;
					else if((*id).y<3&&Lmode==1)
						(*id).y++;
				}
				else if(Ctl_Data.Key_Lflag & key_RR)
				{
					if((*id).y>0)
				   (*id).y--;
				}
			}
				
			
		}
}
//绘制金条云
static void creatBrow(){
	if(status==1||status==2){
	//外边界
	
	Draw_LineY(BrowLeft,0,8,BrowBorderColor);
	//内部绘制
	Draw_LineY(BrowLeft+1,0,8,BrowColor);
	Draw_LineY(BrowLeft+2,0,8,BrowColor);
	Draw_LineY(BrowLeft+3,0,8,BrowColor);
	Draw_LineY(BrowLeft+4,0,8,BrowColor);	
	//外边界
  Draw_LineY(BrowRight,0,8,BrowBorderColor);
	}
}
static float coefficient;		//系数
u8		P_Now;				//现在位置
u8		P_Last;				//上一时刻位置
u32 Piano_color[7]={0XFF0000,0XFF7F00,0XFFFF00,0X00FF00,0X00FFFF,0X0000FF,0X8B00FF};

u8 location_old=0;
u8 location;
int piano=0;
u32 breath_color=0;
u8 breath_order=0;
static void Piano_Init(void)
{
	piano=0;
	
}
static void key_board(u8 location)
{	
			Draw_LineY(location*6+3,0,7,Piano_color[location]);
			Draw_LineY(location*6+4,0,7,Piano_color[location]);
			Draw_LineY(location*6+5,0,7,Piano_color[location]);
			Draw_LineY(location*6+6,0,7,Piano_color[location]);
			Draw_LineY(location*6+7,0,7,Piano_color[location]);
			Draw_LineY(location*6+8,0,7,Piano_color[location]);
}

static void breath_light(void)
{
	if((0<=breath_color<=255)&&breath_order==0)
	{
		breath_color++;
		if(breath_color==254)
			breath_order=1;
	}
	if((0<=breath_color<=255)&&breath_order==1)
	{
	breath_color--;
	if(breath_color==1)
		breath_order=0;
	}
	Draw_LineX(0,47,9,breath_color);
	Draw_LineX(0,47,10,breath_color);
}


static void sudu(){
	if(markCount==0)
		markCC=7;
	else if(markCount==30)
		markCC=6;
	else if(markCount==50)
		markCC=5;
	else if(markCount==60){
	  mark_musce=4;
		markCount++;
		markCC=4;
	}
		
	else if(markCount==70)
		markCC=3;
	else if(markCount==90){
		markCount++;
		mark_musce=4;
		markCC=1;
	}
else if(markCount==120){
		mark_musce=4;
		markCount++;
		markCC=0;
	}
		
}
static void playMusce(){
	OS_ERR err; 
	if(carCount==4&&mark13==1){
	mark13=0;
  mark_musce=2;
	}

	
	if(mark_musce==1){
		OSTimeDly(400,OS_OPT_TIME_DLY,&err);
		Play_List(85);	//沙尘暴来袭请驾驶小车逃离
		OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
		mark_musce=0;
	}
	else if(mark_musce==2){
		OSTimeDly(400,OS_OPT_TIME_DLY,&err);
		Play_List(86);	//请按动手中按钮 控制小车左右移动 躲避障碍 物
		OSTimeDly(10000,OS_OPT_TIME_DLY,&err);
			mark_musce=0;
	}
	else if(mark_musce==3){
		OSTimeDly(400,OS_OPT_TIME_DLY,&err);
		Play_List(87);	//注意 注意 您 跑得太慢了 驾驶的小车 即将被沙尘暴吞没 请给力点
		OSTimeDly(5500,OS_OPT_TIME_DLY,&err);
			mark_musce=0;
	}
	else if(mark_musce==4){
		OSTimeDly(400,OS_OPT_TIME_DLY,&err);
		Play_List(88);	//注意 注意 小车即将加速前进
		OSTimeDly(3000,OS_OPT_TIME_DLY,&err);
			mark_musce=0;
	}
	else if(mark_musce==5){
		OSTimeDly(400,OS_OPT_TIME_DLY,&err);
		Play_List(89);	//over
		mark_musce=0;
	}
}
static void bigan(){
	Clear_screen();
	DispStrCH(7,"GAME",0x00ff00,0);
	Update_Layer();
}
//////////////////主函数

void Car_Game(void)
{
	OS_ERR err; 
	
	if(initMark==1){ ///初始化只运行一次
		 
		bigan();
		if(mark5==1){
			mark5=0;
			initGoldQueue();
			
		}	
	//	creatObstacle();
		initMark=0;
		mark_musce=1;	
	}
	///////////////////////
	if(status==1){
   Clear_screen();
		sudu();
		playMusce();
	//setCountMark();
//	controlSudu();
	controlCollecter(&collecter_1,&collecter_2);
//	judgeMode();
//	winners();//f分出胜负后status=2；
	contorlVehicle(&collecter_1,&collecter_2);
	playMusce();
	drawObstacle();
	drawFlood(&collecter_1,&collecter_2);
	//creatBrow();
	drawVehicle(&collecter_1,&collecter_2);
}
	else if(status==5){
			clearQueue();
			OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
		  Clear_screen();
			DispStrCH(10,"END",0xff0000,0);
		 	Update_Layer();
		  Play_List(57);						
			Clear_screen();
	  	init();
		  OSFlagPost(&Ctl_Data.Flag,Game_Over_flag,OS_OPT_POST_FLAG_SET,&err);			
			OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
			
		}
	else if(status==6){
			OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
			clearQueue();
		  Clear_screen();
			DispStrCH(2,"WIN",0xff0000,0);
		 	Update_Layer();
		  Play_List(57);						
			Clear_screen();
	  	init();
			OSFlagPost(&Ctl_Data.Flag,Game_Over_flag,OS_OPT_POST_FLAG_SET,&err);			
			OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
		}
	else if(status==7){
			OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
			clearQueue();
		  Clear_screen();
			DispStrCH(25,"WIN",0xff0000,0);
		 	Update_Layer();
		  Play_List(57);						
			Clear_screen();
	  	init();
		OSFlagPost(&Ctl_Data.Flag,Game_Over_flag,OS_OPT_POST_FLAG_SET,&err);			
			OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
		}
		
			
	//////////////////////////////////

	Update_Layer();
//	if((angabs(DMP_Data.Angular_Velocity))>20)
//	{
//		Play_List(36);					//播放游戏开始音效
//	OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
//	Play_List(35);					//播放游戏开始音效
//		OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
//	}
//	Clear_screen();
//		    OSTimeDly(10,OS_OPT_TIME_DLY,&err);
//	//if(Ctl_Data.Key_Pflag)
//	//{
//		Draw_LineY(0,0,7,0x00ff00);
//	 Draw_Point(20,4,0x00ffff,0);
//	Update_Layer();
//		OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
//	Clear_screen();
//		    OSTimeDly(10,OS_OPT_TIME_DLY,&err);
////	}
}

////////////////////////////////////////////////////////清空队列
 void clearQueue(){
	GNode g2;	
	while(GetLength(&Q)!=0){
		DeQueue(&Q,&g2);//金条出队列
	
	}
}
////////////////显示赢家
 void win(){
////////////////显示winner
	  Clear_screen();
	  if(count2++>=100){
			count2=0;
			status=5;
		}			
			if(winner==1)
				winnerMove++;
			else
				winnerMove--;
		
		if(winner==1){
			if(winnerMove>15)
				 winnerMove=15;
			DispStrCH(winnerMove,"WIN",winnerColor,0);
		}		
		else {
			if(winnerMove<=15)
				 winnerMove=15;
		  DispStrCH(winnerMove,"WIN",winnerColor,0);
		}
}





















