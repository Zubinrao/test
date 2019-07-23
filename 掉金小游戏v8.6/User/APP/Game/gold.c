#include "debug.h"
#include "Control.h"
#include "math.h"
#include "WavPlay.h"
#include "gold.h"
#include "shapeBalls.h"

#define GoldLength 2
#define angabs(x)     (((x)>0)?(x):-(x))
#define Collecterlength 3  //收金者宽度

#define winnerColor 0xDD22DD
#define sizemark 50
#define updateGold 93
/***************
	  图形
****************/
#include"shapes.h"

/***************
	游戏变量
****************/
GAME_CLASS gameGold;
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
u16 count11=0;
u8 times=6;
u8 mark62=0;
u8 mark63=0;
u8 markP=1;
////////////全局变量/////////
u8 CollecterPlusMark=25;
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
u8 last1=10;
/***************
	函数声明
****************/
static void gameTankGaming(void);
static void gameTankInit(void);

static void gameTankTerminate(void);


static void Piano_Game(void);
void money();
void clearQueue();
void win();
void init();
/***************
	函数实现
****************/
GAME_CLASS * gameGoldReg(){
	// 构建结构体
	gameGold.funcGame = gameTankGaming;
	gameGold.funcInit = gameTankInit;
	gameGold.pLogo = shapeBallLogo;
	gameGold.pLogoHL = shapeBallLogoHL;
	gameGold.nameWAV = 95;
  gameGold.funcTerminate = gameTankTerminate;
	Debug_SendMsg("GameTankWar", "Game Struct set.", DEBUG_MSG_INFO);
	// 注册到状态机
	gameReg(&gameGold);
	return &gameGold;
}

static void gameTankGaming(){
	// Debug_SendMsg("GameTankWar", "frame", DEBUG_MSG_INFO);
	//generalLoop();
	Piano_Game();
}

static void gameTankTerminate()
{
	OS_ERR err;
	Debug_SendMsg("GameTankWar", "Terminate", DEBUG_MSG_INFO);
  generalTerminate();
		Play_List(156);
		OSTimeDly(7000,OS_OPT_TIME_DLY,&err);
}

static void gameTankInit(){
	Debug_SendMsg("GameTankWar", "Game Init starts.", DEBUG_MSG_INFO);
	OS_ERR err;
	Play_List(157);
	init();
	OSTimeDly(7000,OS_OPT_TIME_DLY,&err);
}




















////////////队列//////////////////////////////////
typedef struct Gold *GNode; /*定义队列的每个节点的类型*/
struct Gold{
	u8 markWay;
	u8 x;
	u8 y;
}gold;

typedef struct Queue
{
    struct Gold * array;                //数组作为队列
    int front;                  //队列的头索引
    int tail;                   //队列的尾索引
	  int size;
}QUEUE,*PQUEUE;

//函数声明
u8 Isempty(PQUEUE);
u8 Isfull(PQUEUE);



//初始化队列
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


GNode creatCar1(u8 x,u8 y,u8 wayMark){ //x，y金条初始位置 ,wayMark金条掉落方向
	//struct Gold	gNode;
	gold.x=x;
	gold.y=y;
	gold.markWay=wayMark;
	return &gold;
	
}
QUEUE Q;
//////////////////////////////
void initGoldQueue(){
	    Init_queue(&Q);
	}
//////////////////////////////
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
}collecter_1={1,0,1,3,3,0,0},
 collecter_2={1,45,1,3,3,0,0};
 //////////////////创造金子
 void createGolds(){	     
        GNode g1;
	       int x=0;
	       if(winner==1){
				g1=creatCar1(BrowLeft+1,rand() % 8,1);
	      EnQueue(&Q,g1);//x送进队列
				}
				else if(winner==0){
	      g1=creatCar1(BrowRight-1,rand() % 8,0);
        EnQueue(&Q,g1);//x送进队列
				}
				else {
					if(count6++>10){
						count6=0;
				    	x=(int)DMP_Data.Pitch/2;
					}	
				if(x>3){
				g1=creatCar1(BrowLeft+10,rand() % 8,1);
	      EnQueue(&Q,g1);//x送进队列
					}		
				else if(x<-3){
				g1=creatCar1(BrowRight-10,rand() % 8,0);
        EnQueue(&Q,g1);//x送进队列
				}
				else{
				g1=creatCar1(BrowLeft+10,rand() % 8,1);
	      EnQueue(&Q,g1);//x送进队列
				g1=creatCar1(BrowRight-10,rand() % 8,0);
        EnQueue(&Q,g1);//x送进队列
				}
	      
				}
				
	 
}
 //计数器，控制界面刷新速度
void viewFlushCounter(){
	
}
////////////////////掉金速度动态设置
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
void controlGolds(struct CollectGold *id,struct CollectGold *id2){
	GNode g;
	int i=0,x=0;
	
	if(count1++>CountMark){
		BrowColor=(u32)rand();//////改变云的颜色
		i=GetLength(&Q);
		count1=0;
		if(i<5){
			count11++;
		 createGolds();
		}		
		///////////////////////////设置掉金概率40%
		else if(rand()%10>times)
			if(i<60)//设置金条数量
				{
				count11++;
			  createGolds();
	}
		 
			
			/////////////
		 i=GetLength(&Q);
		
	for(;x<i;x++){
		DeQueue(&Q,&g);//金条出队列	
		
		////////////////////情况一(游戏结束后，控制获胜者的金条)
		if(winner!=2){
		 if(winner==1){//////////左边收金
			if(((*g).x+Collecterlength)>=25){
				// freeGold(g);
			}
			else{
				(*g).x++;
				EnQueue(&Q,g);
			}
		}	
		else{
			if((*g).x<=20){
				// freeGold(g);
			}
			else{
				(*g).x--;
				EnQueue(&Q,g);
			}
		}
	}	
		////////////////////情况二
		else{			 
		if((*g).markWay==1){//////////左边收金
			if(((*id).x+Collecterlength)==(*g).x&&(*g).y>=(*id).y&&(*g).y<=((*id).y+ColleterWid)&&Lmode==0){
				(*id).score++; //收金器计分
				(*id).workPosition++;
				// freeGold(g);
				  mark63=1;
				if((*id).workPosition>CollecterPlusMark){
					(*id).x+=2;
					mark62=1;
					// Play_List(62);
					(*id).workPosition=0;//根据计分增加收金器的位置
				}
			}
			else if(((*id).x+Collecterlength)==(*g).x&&(*g).y>=(*id).y&&(*g).y<=((*id).y+ColleterWid+2)&&Lmode==1){
				(*id).score++; //收金器计分
				(*id).workPosition++;
				mark63=1;
				// freeGold(g);
				if((*id).workPosition>CollecterPlusMark){
					(*id).x+=2;
					mark62=1;
					 //Play_List(62);
					(*id).workPosition=0;//根据计分增加收金器的位置
				}
			}
			else if(((*id).x+Collecterlength-1)>=(*g).x){
			//	freeGold(g);
			}
			else{
				(*g).x--;
				EnQueue(&Q,g);
			}
		}	
		else{
					if(((*id2).x)-1==(*g).x&&(*g).y>=(*id2).y&&(*g).y<=((*id2).y+ColleterWid)&&Rmode==0){
				(*id2).score++; //收金器计分
			   mark63=1;
				(*id2).workPosition++;
				// freeGold(g);
				if((*id2).workPosition>CollecterPlusMark){
					(*id2).x-=2;
					mark62=1;
					// Play_List(62);
					(*id2).workPosition=0;//根据计分增加收金器的位置
				}
			}
			else if(((*id2).x)-1==(*g).x&&(*g).y>=(*id2).y&&(*g).y<=((*id2).y+ColleterWid+2)&&Rmode==1){
				(*id2).score++; //收金器计分
				 mark63=1;
				(*id2).workPosition++;
			//	 freeGold(g);
				if((*id2).workPosition>CollecterPlusMark){
					(*id2).x-=2;
					mark62=1;
					 //Play_List(62);
					(*id2).workPosition=0;//根据计分增加收金器的位置
				}
			}
			else if(((*id2).x)<(*g).x){
			//	freeGold(g);
			}
			else{
				(*g).x++;
				EnQueue(&Q,g);
			}
		}
	}
	}
	if(status==1){
	if(lastCount++>200)
		lastCount=100;
	if(lastCount<20)
		return;
	if(mark62==1){
		Play_List(62);
		mark62=0;
		lastCount=5;
	}
	else if(mark63==1){
		mark63=0;
		switch(rand()%5){
			case 0:
				Play_List(90);
			break;
			case 1:
				Play_List(91);
			break;
			case 2:
				Play_List(91);
			break;
			case 3:
				Play_List(93);
			break;
			default:
				Play_List(63);
			break;
		}
		
		lastCount=18;
	}
}
	}
}
//////////控制金条速度
void controlSudu(){
	OS_ERR err;
	
	CountMark=10-(collecter_1).x;
	if(CountMark<=4&&last1==10){
		 last1=1;
		 Play_List(83);
    OSTimeDly(3000,OS_OPT_TIME_DLY,&err);	
	}
	
	if(CountMark<4)
		CountMark=4;
	
	
}
 ////////////绘制金子
void drawGold(){
	int i=GetLength(&Q),x;
	GNode g2;	
	for(x=0;x<i;x++){
		DeQueue(&Q,&g2);//金条出队列
		if(update1==1)
		GoldColor=(u32)rand();
		if(winner!=2){
			if((*g2).markWay==1) //1左边 0右边；
		{
			 Draw_LineX((*g2).x,(*g2).x+GoldLength,(*g2).y,GoldColor);	
		}
		else if((*g2).markWay==0){
			 Draw_LineX((*g2).x-GoldLength,(*g2).x,(*g2).y,GoldColor);
		}
		}
		else if((*g2).markWay==1&&(*g2).x<=BrowLeft+1) //1左边 0右边；
		{
			 Draw_LineX((*g2).x,(*g2).x+GoldLength,(*g2).y,GoldColor);	
		}
		else if((*g2).markWay==0&&(*g2).x>=BrowRight-1){
			 Draw_LineX((*g2).x-GoldLength,(*g2).x,(*g2).y,GoldColor);
		}
		EnQueue(&Q,g2);//金条进队列
	}
}
 //绘制收金器
void drawCollectGold(struct CollectGold *id,struct CollectGold *id2){
	
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
void controlCollecter(struct CollectGold *id1,struct CollectGold *id){
		/*检测是否需要左右移动位置*/
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
void creatBrow(){
	if(status==1){
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
//判断输赢
void winners(){
	 
	 if((collecter_1).x+Collecterlength>=BrowLeft-8){ //左边赢
		  winner=1;
			 status=2;
			winnerMove=2;
		}
		else if(((collecter_2).x)<=BrowRight+1+8){ //右边赢
			winner=0;
			status=2;
			winnerMove=50;
		}	
}
//游戏模式判断
void judgeMode(){

	if(collecter_1.x>=4&&Lmode==0){
			Colleter1Color0=0xffff00;
			Lmode=1;//左边升级模式
		 
	}
	if(collecter_2.x<=41&&Rmode==0){
			Colleter2Color0=0xffff00;
			Rmode=1;//右边收金桶升级
		 
	}
		
}
////////参数初始化
void init(){
	 ColleterColor0=0xff0000; //收金者颜色1
	 Colleter1Color0=0xff0000; //收金者颜色1
	 Colleter2Color0=0xff0000; //收金者颜色1
	 ColleterColor1=0x00ff00; //收金者颜色2
	 ColleterColor2=0x993366; //收金者颜色3
	 stockColor3=0xcc0000; //库存颜色3
	 stockColor4=0x0000cc; //库存者颜色3
	 ColleterWid=2;  //收金者宽度
	///////////////////////////////////金条云
	 BrowColor=0xEEEE11;
	 BrowBorderColor=0x3CC4C4;
	/////////////////////////////////////金条颜色
	 GoldColor=0xEEEE11;
	////////////全局变量/////////
	 BrowLeft=20; 
	 BrowRight=25;	
	 initMark=1;
	 winner=2;//1为左方获胜0为右方获胜
	 CountMark=5;
	 count1=0;//控制金条速度
	 moneyMark=0;
	 count2=0;//控制程序执行次数
	 status=1;///////游戏状态
	 Lmode=0;
	 Rmode=0;
	 AudioMark=0;
	 AudioMark1=0;
	collecter_1.x=0;
	collecter_1.y=1;
	collecter_1.length=3;
	collecter_1.wid=3;
	collecter_1.score=0;
	collecter_1.workPosition=0;
	collecter_2.x=45;
	collecter_2.y=1;
	collecter_2.length=3;
	collecter_2.wid=3;
	collecter_2.score=0;
	collecter_2.workPosition=0;
  lightStatus0=1;
  lightStatus1=1;
	update1=0;
	mark2=0;
	mark62=0;
	mark63=0;
	times=6;
	lastCount=150;
	markP=1;
	last1=10;
	count11=0;
	CollecterPlusMark=25;
}
void yuzhi(){
	    OS_ERR err;
if(collecter_1.x==BrowLeft-6&&markP==1){
			markP=0;
	    Play_List(80);						
			OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
}
else if(collecter_2.x==BrowRight+5&&markP==1){
			markP=0;
			Play_List(81);						
			OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
}
}
//////////////////主函数
void Piano_Game(void)
{
	OS_ERR err; 
	GNode g2;	
	 if(count11==updateGold&&mark2==0){
	    Play_List(66);
		  update1=1;
		  CollecterPlusMark=45;
		  mark2=1;
		  times=3;
		OSTimeDly(4000,OS_OPT_TIME_DLY,&err);
	 }
	 if(AudioMark1==0)
	 if(Rmode==1){
		 AudioMark1=1;
		 times=3;
		 Play_List(67);
		OSTimeDly(2500,OS_OPT_TIME_DLY,&err);
	 }
	 if(AudioMark==0){
	 if(Lmode==1){
		AudioMark=1;
		
		Play_List(68);
		OSTimeDly(2500,OS_OPT_TIME_DLY,&err);
	}
	}
	
	if(initMark==1){ ///初始化只运行一次
		Clear_screen();
		DispStrCH(10,"Gold",0xffFF00,0);
		Update_Layer();
		Play_List(65);						//播放游戏开始音效
		OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
		if(mark5==1){
			mark5=0;
			initGoldQueue();
		}	
		initMark=0;
		Play_List(64);	
	}
	///////////////////////
	if(status==1){
	judgeMode();
	yuzhi();
  Clear_screen();
	//setCountMark();
	controlSudu();
	controlCollecter(&collecter_1,&collecter_2);
	controlGolds(&collecter_1,&collecter_2);
	judgeMode();
	winners();//f分出胜负后status=2；
	drawGold();
	creatBrow();
	drawCollectGold(&collecter_1,&collecter_2);
}
	else if(status==2){
		CountMark=10;
	if(winner==1){
		OSTimeDly(50,OS_OPT_TIME_DLY,&err);		
		BrowLeft=0;
	  Clear_screen();
		//drawGold();
		drawCollectGold(&collecter_1,&collecter_2);
		creatBrow();
		Update_Layer();
	  Play_List(58);
    OSTimeDly(2000,OS_OPT_TIME_DLY,&err);		
	}
	else {
		OSTimeDly(50,OS_OPT_TIME_DLY,&err);	
		Clear_screen();
		//drawGold();
		drawCollectGold(&collecter_1,&collecter_2);
	  creatBrow();
		Update_Layer();
		BrowRight=50;
	  Play_List(59);
   OSTimeDly(2000,OS_OPT_TIME_DLY,&err);		
	}
	 clearQueue();
	   status=3;
	
	}		
	else if(status==3){
			 money();
		}
	else if(status==4){
		win();
		OSTimeDly(100,OS_OPT_TIME_DLY,&err);
	}
	else if(status==5){
			clearQueue();
		  Play_List(156);	
	  	init();
    //  OSFlagPost(&Ctl_Data.Flag,Game_Over_flag,OS_OPT_POST_FLAG_SET,&err);			
			OSTimeDly(3000,OS_OPT_TIME_DLY,&err);
		}
	Update_Layer();

}
/////////////////获胜者撒金条-提示音：土豪发钱啦or红包来啦！
void money(){
	  GNode g2;	
	  Clear_screen();
	  if(count2==30)
		Play_List(84);
    if(count2++>=1200){
		  count2=0;
			status=4;
		}
		controlGolds(&collecter_1,&collecter_2);
		drawGold();    
}
////////////////////////////////////////////////////////清空队列
void clearQueue(){
	GNode g2;	
	createGolds();
	while(GetLength(&Q)!=1){
		DeQueue(&Q,&g2);//金条出队列
		//freeGold(g2);
	}
	createGolds();
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
			if(winnerMove>12)
				 winnerMove=12;
			DispStrCH(winnerMove,"WIN",winnerColor,0);
		}		
		else {
			if(winnerMove<=15)
				 winnerMove=15;
		  DispStrCH(winnerMove,"WIN",winnerColor,0);
		}
}
