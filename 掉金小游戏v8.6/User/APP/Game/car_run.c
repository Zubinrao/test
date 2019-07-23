#include "debug.h"
#include "Control.h"
#include "math.h"
#include "WavPlay.h"
#include "car.h"
#include "shapeBalls.h"
#define angabs(x)     (((x)>0)?(x):-(x))
/***************
	  ͼ��
****************/
#include"shapes.h"

/***************
	��Ϸ����
****************/
GAME_CLASS gameCar;

/***************
	��������
****************/
static void gameCarGaming(void);
static void gameCarInit(void);
static void gameCarTerminate(void);
static void Car_Game(void);
static void initCar();
/***************
	����ʵ��
****************/
GAME_CLASS * gameCarReg(){
	// �����ṹ��
	gameCar.funcGame = gameCarGaming;
	gameCar.funcInit = gameCarInit;
	gameCar.pLogo = shapeBallLogo;
	gameCar.pLogoHL = shapeBallLogoHL;
	gameCar.nameWAV = 152;
  gameCar.funcTerminate = gameCarTerminate;
	Debug_SendMsg("GameTankWar", "Game Struct set.", DEBUG_MSG_INFO);
	// ע�ᵽ״̬��
	gameReg(&gameCar);
	return &gameCar;
}

static void gameCarGaming(){
	// Debug_SendMsg("GameTankWar", "frame", DEBUG_MSG_INFO);
	//generalLoop();
	Car_Game();
}

static void gameCarTerminate()
{
	OS_ERR err;
	Debug_SendMsg("GameTankWar", "Terminate", DEBUG_MSG_INFO);
  generalTerminate();
	Play_List(156);
	OSTimeDly(7000,OS_OPT_TIME_DLY,&err);
}

static void gameCarInit(){
	OS_ERR err;
	Play_List(157);
	OSTimeDly(7000,OS_OPT_TIME_DLY,&err);
	Debug_SendMsg("GameTankWar", "Game Init starts.", DEBUG_MSG_INFO);
  initCar();
}


#define GoldLength 2
#define angabs(x)     (((x)>0)?(x):-(x))
#define Collecterlength 3  //�ս��߿��
#define CollecterPlusMark 10
#define BackMark 5 //����ײ���˻���ֵ
/////////////////////////////////////��
static u8 flood1=2;
static u8 flood2=45;
static u32 stockColor6=0xCD8500; //�ս�����ɫ1
static u8 countCar=0;
static u16 countCreat=0;
static u8 markCC=8;
static u8 markBlue=0;
static u8 markRed=0;
static u8 mark_1=5;//ǰ����ֵ
static u8 mark_2=205;
static u8 mark_3=205;
static u8 mark_7=0;
static u8 mark_8=0;
static u8 musceMark=0;
static u16 markCount=0;
static u8 count_9=80;
static u8 mark_12=1;
static u16 obstacleCount=0;
static u8 mark12=1;
static u8 mark11=1;
static u8 mark13=1;
////////////////////////////////////
////////////////////////////////�ս���
static u32 ColleterColor0=0xff0000; //�ս�����ɫ1
static u32 Colleter1Color0=0xffff00; //�ս�����ɫ1
static u32 Colleter2Color0=0xffff00; //�ս�����ɫ1
static u32 ColleterColor1=0x00ff00; //�ս�����ɫ2
static u32 ColleterColor2=0x993366; //�ս�����ɫ3
static u32 stockColor3=0xcc0000; //�����ɫ3
static u32 stockColor4=0x0000cc; //�������ɫ3
static u32 ColleterWid=2;  //�ս��߿��
///////////////////////////////////������
static u32 BrowColor=0xEEEE11;
static u32 BrowBorderColor=0x3CC4C4;
/////////////////////////////////////������ɫ
static u32 GoldColor=0xEEEE11;
static u8 mark5=1;
static u8 count6=0;
static u8 times=8;
static u8 mark62=0;
static u8 mark63=0;
////////////ȫ�ֱ���/////////
static u8 BrowLeft=20; 
static u8 BrowRight=25;	
static vu8 initMark=1;
static u8 winner=2;//1Ϊ�󷽻�ʤ0Ϊ�ҷ���ʤ
static int CountMark=10;
static u8 update1=0;
static u8 mark2=0;
static int count1=0;//���ƽ����ٶ�
static int moneyMark=0;
static int winnerMove;
static u16 count2=0;//���Ƴ���ִ�д���
static int status1=1;///////��Ϸ״̬
static int Lmode=0;
static int Rmode=0;
static int lastCount=150;
static u8 AudioMark=0;
static u8 AudioMark1=0;
static u8 lightStatus0=1;
static u8 lightStatus1=1;
static u8 mark_musce=0;
static u16 carCount=0;
//////////////Ӯ������
#define winnerColor 0xDD22DD
////////////����//////////////////////////////////
static void money();
static void clearQueue();
static void win();

typedef struct Gold *GNode; /*������е�ÿ���ڵ������*/
static struct Gold{
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
    struct Gold * array;                //������Ϊ����
    int front;                  //���е�ͷ����
    int tail;                   //���е�β����
	  int size;
}QUEUE,*PQUEUE;
/////////////////////
typedef struct Gold *GNode; /*������е�ÿ���ڵ������*/
static u8 Isempty(PQUEUE);
static u8 Isfull(PQUEUE);
#define sizemark 50
//��ʼ������
static void Init_queue(PQUEUE QCar);
static void EnQueue(PQUEUE QCar,GNode val);
static void DeQueue(PQUEUE QCar,GNode *p_num);
static int GetLength(PQUEUE QCar);
static GNode creatGold(u8 x,u8 y,u8 wayMark);
static void Init_queue(PQUEUE QCar)
{
    QCar->array = (struct Gold *)malloc(sizeof(struct Gold)*sizemark);     //����������еĶ�̬�ռ�
    QCar->front = 0;
    QCar->tail = 0;
	  QCar->size=0;
    return;
}

//���
static void EnQueue(PQUEUE QCar,GNode val)
{
   if(Isfull(QCar))
   {
      // printf("%4d���ʧ�ܣ�����������\n",val);
   }else
   {
       QCar->array[QCar->tail] =(*val);
       QCar->tail = (QCar->tail+1)%sizemark;
		   QCar->size++;
   }
}

//����
static void DeQueue(PQUEUE QCar,GNode *p_num)
{
    if(Isempty(QCar))
    {
        // printf("����ʧ�ܣ�������û��Ԫ�ء�\n");
    }else
    {
        gold= QCar->array[QCar->front];
		  	*p_num=&gold;
        QCar->front = (QCar->front+1)%sizemark;            //ͷ������һ��������������������0��ʼ
			  QCar->size--;
    }
}

static int GetLength(PQUEUE QCar){
	return QCar->size;
}

//�ж϶����Ƿ�Ϊ��
static u8 Isempty(PQUEUE QCar)
{
    if(QCar->front ==  QCar->tail)                 //�Ѷ�ͷ�Ͷ�β��ͬ��Ϊ����Ϊ�յ��ж�����
    {
        return 1;
    }else
    {
        return 0;
    }
}

//�ж϶����Ƿ���
static u8 Isfull(PQUEUE QCar)
{
    if((QCar->tail+1)%sizemark == QCar->front) //�˴����㷨��˼�ǵ�����β�ټ�һ��Ԫ�ؾͺͶ���ͷ�غϵ�ʱ������������     
    {
        return 1;
    }else
    {
        return 0;
    }
}


static GNode creatGold(u8 x,u8 y,u8 wayMark){ //x��y������ʼλ�� ,wayMark�������䷽��
	//struct Gold	gNode;
	gold.x=x;
	gold.y=y;
	gold.markWay=wayMark;
	return &gold;
	
}
/////////////ȫ�ֶ���ָ�� /////
QUEUE QCar;
/////////////////////////////////////////////////

 static void initGoldQueue(){ 
    Init_queue(&QCar);
//    for(i = 0; i < n; ++i) {
//        x = rand() % 100;//����һ��������100�������
//				g1=creatGold();
//			  (*g1).x=x;
//        EnQueue(QCar,*g1);//x�ͽ�����
//    }
//		i=GetLength(QCar);
//    while(notEmpty(QCar)) {//�ж�û�пն���
//        DeQueue(QCar,g1);//������
//			  free(&g1);   
//    }
	}
//////////////////////////////


////������
//struct Brow{
// 
//}brow{}��
//�ս�������
static struct CollectGold
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

 
 

 
////////������ʼ��
static void initCar(){
 
 ColleterWid=2;  //�ս��߿��

////////////ȫ�ֱ���/////////	
 initMark=1;
 status1=1;///////��Ϸ״̬
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
 stockColor6=0xCD8500; //�ս�����ɫ1
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
 //�����������ƽ���ˢ���ٶ�

 static void viewFlushCounter(){
	
}
////////////////////�����ٶȶ�̬����

 static void setCountMark(){
	int angle=angabs(DMP_Data.Angular_Velocity)/10;
	if(angle<0)
		angle=-angle;
		CountMark=10-angle;
}



//////////���ƽ����ٶ�
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
	 EnQueue(&QCar,&gold);//x�ͽ�����
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
	 EnQueue(&QCar,&gold);//x�ͽ�����
	}
}
static void drawObstacle(){
	    GNode g;
	    u8 i=GetLength(&QCar);
	    u8 xx=0,x;
	//////////////////////////////////��������
	Draw_LineY(24,0,7,(u32)rand());
	Draw_LineY(23,0,7,(u32)rand());
	//Draw_LineY(24,0,7,(u32)rand());
	for(;xx<i;xx++){
		  DeQueue(&QCar,&g);
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
		  EnQueue(&QCar,g);
	}
	    
}
static void judge(struct CollectGold *id,struct CollectGold *id2){
           if((*id).x<=flood1){
								status1=2;
								mark_musce=5;
								return;
							}
					 else if(((*id2).x+2)>=flood2){
						  status1=3;
							mark_musce=5;
					 }
}
static void contorlVehicle(struct CollectGold *id,struct CollectGold *id2){

	      OS_ERR err;
				GNode g1;
				u8 i=GetLength(&QCar);
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
			i=GetLength(&QCar);
		
			for(;x_car<i;x_car++){
				 
				  DeQueue(&QCar,&g1);
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
						status1=6;
							break;
						}
							
					}
					if(((*id).x+Collecterlength)>=(*g1).x+1&&((*id).x+Collecterlength)<=(*g1).x+2&&abs(((*g1).y-(*id).y))<1){
						if(mark_7++>=BackMark){//ײ���˺���ֵ
							 mark_7=0;
							(*id).x--;
						}
						if((*id).x-flood1==2&&mark12==1){
							mark_musce=3;
						   mark12=0;
						}
						
//						if((*id).x<=flood1){
//						  status1=2;
//							mark_musce=5;
//							break;
//						}
						  mark_3=0;
						  markRed=0;
						 (*g1).mark=1; 
						  musceMark=1;
						  EnQueue(&QCar,g1);
							continue;
					}
					else if(((*id).x)>=(*g1).x&&((*id).x)<=(*g1).x+2&&abs(((*g1).y-(*id).y))<1){
						if(mark_7++>=BackMark){//ײ���˺���ֵ
								 mark_7=0;
								(*id).x--;
							}
						if((*id).x-flood1==2&&mark12==1){
							mark_musce=3;
						   mark12=0;
						}
//							if((*id).x<=flood1){
//								status1=2;
//								mark_musce=5;
//								break;
//							}
								mark_3=0;
								markRed=0;
							 (*g1).mark=1; 
							  musceMark=1;
								EnQueue(&QCar,g1);
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
						  status1=7;
							 break;
						 }
							 
					}
					if(((*id2).x+Collecterlength)>=(*g1).x+1&&((*id2).x+Collecterlength)<=(*g1).x+2&&abs(((*g1).y-(*id2).y))<1){
						if(mark_8++>=BackMark){//ײ���˺���ֵ
							 mark_8=0;
							(*id2).x++;
						}
						if(flood2-(*id2).x==2&&mark11==1){
							mark_musce=3;
						  mark11=0;
						}
//						if(((*id2).x+2)>=flood2){
//						  status1=3;
//							mark_musce=5;
//							break;
//						} 
						 mark_2=0;
						 markBlue=0;
					  (*g1).mark=1;
						 musceMark=1;
						 EnQueue(&QCar,g1);
							continue;
					}
					else if(((*id2).x)>=(*g1).x&&((*id2).x)<=(*g1).x+2&&abs(((*g1).y-(*id2).y))<1){
						if(mark_8++>=BackMark){//ײ���˺���ֵ
							 mark_8=0;
							(*id2).x++;
						}
						if(flood2-(*id2).x==2&&mark11==1){
							mark_musce=3;
						  mark11=0;
						}
							
//						if(((*id2).x+2)>=flood2){
//						  status1=3;
//							mark_musce=5;
//							break;
//						} 
						 mark_2=0;
						 markBlue=0;
						 musceMark=1;
					  (*g1).mark=1;
						 EnQueue(&QCar,g1);
							continue;
					}
					if((*g1).x++>=flood2){
							markBlue++;
						 
							continue;
						}
					  	} 
				}
					EnQueue(&QCar,g1);
			}	
			judge(&collecter_1,&collecter_2);
			if(count_9>250)
				count_9=80;
			if(count_9++<10);
			
			else if(musceMark==1){
			//��ը��
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
			//��������2
			
			}
			else if(musceMark==3){
			//��������1
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
	///////////���ϰ����������ƺ�ˮ�߶�
	      if(obstacleCount>20){
					obstacleCount=0;
					flood1++;
					flood2--;
				}
				if(status1==2){//�����
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
					status1=7;
				}
				else if(status1==3){//�����
					for(x3=flood2;x3>25;x3--){
						OSTimeDly(200,OS_OPT_TIME_DLY,&err);
						clearQueue();
						for(x1=0;x1<8;x1++){
							x2=rand()%3;
							Draw_LineX(x3-x2,50,x1,stockColor6);
						}
						Update_Layer();
					}	
					status1=6;
				}
				else
				for(x1=0;x1<8;x1++){
					x2=rand()%3;
					Draw_LineX(0,flood1+x2,x1,stockColor6);	
					Draw_LineX(flood2-x2,50,x1,stockColor6);
				}
				
}

 //�����ս���
static void drawCollectGold(struct CollectGold *id,struct CollectGold *id2){
	
	  ///////////////����ս��� 
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
		
		 ///////////////�ұ��ս��� 
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
//�ս����˶�����
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
		/*����Ƿ���Ҫ�����ƶ�̹��λ��*/
		if(Ctl_Data.Key_Pflag || Ctl_Data.Key_Lflag)
		{
		/*�㶯����*/
			if(Ctl_Data.Key_Pflag)
			{
				if(Ctl_Data.Key_Pflag & key_RR)
				{
					Ctl_Data.Key_Pflag &= ~key_RR;			//�������
					if((*id1).y>0)
				   (*id1).y--;
				
				}
				else if(Ctl_Data.Key_Pflag & key_RL)
				{
					Ctl_Data.Key_Pflag &= ~key_RL;
					if((*id1).y<5&&Rmode==0)
						(*id1).y++;
				else if((*id1).y<3&&Rmode==1)
						(*id1).y++;
					
				}
				
				if(Ctl_Data.Key_Pflag & key_LR)
				{
					Ctl_Data.Key_Pflag &= ~key_LR;			//�������
					if((*id).y<5&&Lmode==0)
						(*id).y++;
					else if((*id).y<3&&Lmode==1)
						(*id).y++;
				}
				else if(Ctl_Data.Key_Pflag & key_LL)
				{
					Ctl_Data.Key_Pflag &= ~key_LL;			//�������
					if((*id).y>0)
				   (*id).y--;
				}
			}
			/*��������*/
			else if(Ctl_Data.Key_Lflag)
			{
				if(Ctl_Data.Key_Lflag & key_RR)
				{
					if((*id1).y>0)
				   (*id1).y--;
					
				}
				else if(Ctl_Data.Key_Lflag & key_RL)
				{
						if((*id1).y<5&&Rmode==0)
						(*id1).y++;
				else if((*id1).y<3&&Rmode==1)
						(*id1).y++;
				}
				
				if(Ctl_Data.Key_Lflag & key_LR)
				{
					if((*id).y<5&&Lmode==0)
						(*id).y++;
					else if((*id).y<3&&Lmode==1)
						(*id).y++;
				}
				else if(Ctl_Data.Key_Lflag & key_LL)
				{
					if((*id).y>0)
				   (*id).y--;
				}
			}
				
			
		}
}
//���ƽ�����
static void creatBrow(){
	if(status1==1||status1==2){
	//��߽�
	
	Draw_LineY(BrowLeft,0,8,BrowBorderColor);
	//�ڲ�����
	Draw_LineY(BrowLeft+1,0,8,BrowColor);
	Draw_LineY(BrowLeft+2,0,8,BrowColor);
	Draw_LineY(BrowLeft+3,0,8,BrowColor);
	Draw_LineY(BrowLeft+4,0,8,BrowColor);	
	//��߽�
  Draw_LineY(BrowRight,0,8,BrowBorderColor);
	}
}
static float coefficient;		//ϵ��
u8		P_Now;				//����λ��
u8		P_Last;				//��һʱ��λ��
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
		markCC=2;
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
		Play_List(85);	//ɳ������Ϯ���ʻС������
		OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
		mark_musce=0;
	}
	else if(mark_musce==2){
		OSTimeDly(400,OS_OPT_TIME_DLY,&err);
		Play_List(86);	//�밴�����а�ť ����С�������ƶ� ����ϰ� ��
		OSTimeDly(10000,OS_OPT_TIME_DLY,&err);
			mark_musce=0;
	}
	else if(mark_musce==3){
		OSTimeDly(400,OS_OPT_TIME_DLY,&err);
		Play_List(87);	//ע�� ע�� �� �ܵ�̫���� ��ʻ��С�� ������ɳ������û �������
		OSTimeDly(5500,OS_OPT_TIME_DLY,&err);
			mark_musce=0;
	}
	else if(mark_musce==4){
		OSTimeDly(400,OS_OPT_TIME_DLY,&err);
		Play_List(88);	//ע�� ע�� С����������ǰ��
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
//////////////////������

void Car_Game(void)
{
	OS_ERR err; 
	
	if(initMark==1){ ///��ʼ��ֻ����һ��
		 
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
	if(status1==1){
   Clear_screen();
		sudu();
		playMusce();
	//setCountMark();
//	controlSudu();
	controlCollecter(&collecter_1,&collecter_2);
//	judgeMode();
//	winners();//f�ֳ�ʤ����status=2��
	contorlVehicle(&collecter_1,&collecter_2);
	
	playMusce();
	drawObstacle();
	drawFlood(&collecter_1,&collecter_2);
	//creatBrow();
	drawVehicle(&collecter_1,&collecter_2);
}

	else if(status1==6){
			OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
			clearQueue();
		  Clear_screen();
			DispStrCH(2,"WIN",0xff0000,0);
		 	Update_Layer();
		  Play_List(57);						
	  	initCar();
		  OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
		//	OSFlagPost(&Ctl_Data.Flag,Game_Over_flag,OS_OPT_POST_FLAG_SET,&err);			
			OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
		}
	else if(status1==7){
			OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
			clearQueue();
		  Clear_screen();
			DispStrCH(25,"WIN",0xff0000,0);
		 	Update_Layer();
		  Play_List(57);						
			Clear_screen();
	  	initCar();
		  OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
		//	OSFlagPost(&Ctl_Data.Flag,Game_Over_flag,OS_OPT_POST_FLAG_SET,&err);			
			OSTimeDly(2000,OS_OPT_TIME_DLY,&err);
		}
		
			
	//////////////////////////////////

	Update_Layer();
//	if((angabs(DMP_Data.Angular_Velocity))>20)
//	{
//		Play_List(36);					//������Ϸ��ʼ��Ч
//	OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
//	Play_List(35);					//������Ϸ��ʼ��Ч
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

////////////////////////////////////////////////////////��ն���
 void clearQueue(){
	GNode g2;	
	while(GetLength(&QCar)!=0){
		DeQueue(&QCar,&g2);//����������
	
	}
}




















