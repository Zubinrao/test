#include "snake.h"
#include "Drawing.h"
#include "Control.h"
#include <stdio.h>
#include <stdlib.h>
#include "bsp.h"
#include "TIM.H"
#include "ADC.h"
#include "WavPlay.h"

#define angabs(x)     (((x)>0)?(x):-(x))
/*�ⲿ�����ģ�����ɼ����ݣ���������������������*/
extern __IO u16 AD_Value[CH_N*Sampling_N];

static int angnum=500;
static int angflag;
 int cycleflag=1;

static int walkcount=0;

int linkclr=0;


static int lvflag;

static int16_t  count[5];					//���ü���	
static  u16	over_count=0;					//��Ϸ������ʱ
u8 	snake_flag=0;				//��Ϸ������־
static u16	SAudio_flag=0x0000;						//��Ч��־
//static void Game_Over(u8 flag);

static u8 fb_flag;

static food f_food;
static food s_food;


static food sfood[8];
static int eatflag;
//�����ͷ��ʼλ��
int initl_x=1;
int initl_y=2;
//�Ҳ���ͷ��ʼλ��
int initr_x=46;
int initr_y=5;

static int lscore=0;
static int rscore=0;
static int eatf_flag=1;
static int eats_flag=1;

static bool diet_flag=0;

static int vel_flag;

//��һλ�õ�
int templ_x;
int templ_y;
int tempr_x;
int tempr_y;


static int gol_flag=3;
static int gor_flag=1;

int llength;
int rlength;
//��
int line_l;
int line_r;
//��
int row_l;
int row_r;

int rfx;
int rfy;
int rfflag;//���괩ǽ��־

int templ_s;
int tempr_s;

char ls[2];
char rs[2];
////��˸��ɫ��־
//bool sflag=0;

snake *lptr;//��ȡָ��
snake *rptr;//��ȡָ��
static snake *lhead;//����ͷ
//static snake *lseven;//����β�����ڶ���
static snake *ltail;//����β
static snake *rhead;//����ͷ
//static snake *rseven;//����β�����ڶ���
static snake *rtail;//����β

snake *new_data;//����Ԫ��
snake *tempp;

int i;

/*����: ��Ϸ��ʼ��Ϣ��ʾ
 *���룺x - λ��
 *�����void
 */
static void Game_Begin(int8_t x)
{
	Clear_screen();
	DispStrCH(x,"GAME3",0x00ff00,0);
}

/*̰���߳�ʼ��*/
void snake_init(void)
{
	OS_ERR err;
//	angnum=500;
	over_count=0;	
	//�����ͷ��ʼλ��
 initl_x=1;
 initl_y=2;
//�Ҳ���ͷ��ʼλ��
 initr_x=46;
 initr_y=5;

 lscore=0;
 rscore=0;
 eatf_flag=1;
 eats_flag=1;
 diet_flag=0;
 gol_flag=3;
 gor_flag=1;
	walkcount=0;
snake_flag=0;
  //��̰���߳�ʼ��
	lhead= (snake * )malloc(sizeof(snake));
	lptr= (snake * )malloc(sizeof(snake));
  lhead->next=lptr;
	lptr->next=NULL;
	lhead->x=initl_x;
	lhead->y=initl_y;
	lhead->color=clrred;
  Draw_Point(lhead->x,lhead->y,lhead->color,0);
	lptr->x=initl_x;
  lptr->y=initl_y-1;		
	lptr->color=clryellow;
	Draw_Point(lptr->x,lptr->y,lptr->color,0);
  new_data=(snake * )malloc(sizeof(snake));
	lptr->next=new_data;
	new_data->next =NULL;
	lptr=new_data ;
	lptr->x=initl_x;
  lptr->y=initl_y-2;		
	lptr->color=clryellow;
	Draw_Point(lptr->x,lptr->y,lptr->color,0);
	//�ҷ�̰���߳�ʼ��
	rhead= (snake * )malloc(sizeof(snake));
	rptr= (snake * )malloc(sizeof(snake));
  rhead->next=rptr;
	rptr->next=NULL;
	rhead->x=initr_x;
	rhead->y=initr_y;
	rhead->color=clrpink;
  Draw_Point(rhead->x,rhead->y,rhead->color,0);
	rptr->x=initr_x;
  rptr->y=initr_y+1;		
	rptr->color=clrazure;
	Draw_Point(rptr->x,rptr->y,rptr->color,0);
	new_data=(snake * )malloc(sizeof(snake));
	rptr->next=new_data;
	new_data->next =NULL;
	rptr=new_data ;
	rptr->x=initr_x;
  rptr->y=initr_y+2;		
	rptr->color=clrazure;
	Draw_Point(rptr->x,rptr->y,rptr->color,0);
	
//	Draw_LineY(0,0,7,clrgreen);
//	Draw_LineY(47,0,7,clrgreen);
//	Update_Layer();
//	OSTimeDly(10,OS_OPT_TIME_DLY,&err);
	for(i=0;i<8;i++)
	{
		sfood[i].x=60;
		sfood[i].y=0;
	}
//	Update_Layer();
//	OSTimeDly(10,OS_OPT_TIME_DLY,&err);
	fb_flag=0;
	vel_flag=50;
	SAudio_flag = 0;
	cycleflag=1;
	eatflag=0;
	lvflag=0;
}

void snake_game(void)
{
	OS_ERR err;
	//snake


	fb_flag++;
	over_count++;
	//�Ƕ��жϣ������ٶ�
	
	if((angabs(DMP_Data.Angular_Velocity))>100)
	{
		vel_flag=5;
	}
	else
		vel_flag=10;
 
//	if(over_count>10000)
//	{
//		lv2flag=1;
//	}
//		if(over_count>10000)
//	{
//		lv2flag=0;
//		lv3flag=1;
//	}
	
	
//	if((angabs(DMP_Data.Pitch)>=0)&&(angabs(DMP_Data.Pitch)<=30)&&vel_flag!=30)
//	{
//		vel_flag=30;
//		angnum=500;
//	}
//	else if((angabs(DMP_Data.Pitch)>30)&&(angabs(DMP_Data.Pitch)<=60)&&vel_flag!=40)
//	{
//		vel_flag=40;
//		angnum=500;
//	}	
//	else if((angabs(DMP_Data.Pitch)>60)&&(angabs(DMP_Data.Pitch)<=90)&&vel_flag!=50)
//	{
//		vel_flag=50;
//		angnum=500;
//	}
//			
//	if(angnum==0)
//	{
//		angnum=500;
//		if((angabs(DMP_Data.Pitch)>=0)&&(angabs(DMP_Data.Pitch)<=30))
//			vel_flag=30;
//		else if((angabs(DMP_Data.Pitch)>30)&&(angabs(DMP_Data.Pitch)<=60))
//			vel_flag=40;
//		else if((angabs(DMP_Data.Pitch)>60)&&(angabs(DMP_Data.Pitch)<=90))
//			vel_flag=50;
//	}
//	else
//		angnum--;
	
//if(Audio_flag & SAudio_bit15)			//����ֹ����������Ч��־λ
//	{
//		if(Detection_idle() == 1)
//		{
//			Audio_flag = 0x0000;			//������������Ч
//		}
//	}
	if(snake_flag == 0)					//��Ϸ��ʼǰ��
	{
		if((SAudio_flag & SAudio_bit0) == 0x0000)
		{
		SAudio_flag=0x0001;
			count[0] = 0;
			count[1] = -37;
			
			if(cycleflag)
			{
			Play_List(36);					//������Ϸ��ʼ��Ч
//	Draw_LineY(10,0,7,clrgreen);
//	Update_Layer();
			OSTimeDly(10,OS_OPT_TIME_DLY,&err);
			}
		}
		else if(SAudio_flag & SAudio_bit0)	//�ȴ���Ϸ��ʼ��Ч�������
		{

			count[0]++;
			if(count[0] >= 3)
			{

				count[0] = 0;
				count[1]++;
				if(cycleflag)
				{
				if(count[1] < 6)
					Game_Begin(count[1]);
				Update_Layer();
			  }
			}
			
			
			
			
			if(Detection_idle() == 1)
			{
				SAudio_flag = 0x0000; 
//				SAudio_flag &= ~SAudio_bit0;			
			  Clear_screen();
		    OSTimeDly(10,OS_OPT_TIME_DLY,&err);
				snake_init();
				linkclr=1;

				snake_flag = 1;
			}
		}
		
	}
	else if(snake_flag == 1)				//��Ϸ��ʼ
	{	
	
		Draw_LineY(0,0,7,clrgreen);
	Draw_LineY(47,0,7,clrgreen);
	Update_Layer();
	
	

	
	
	
	
	//ʳ��λ���������
	randfood();
	
		//�����жϣ��޸ķ���,��ȡ��һλ�õ�
		if(Ctl_Data.Key_Pflag)
	 {
		switch(Ctl_Data.Key_Pflag)
			{
				case key_RL:
					gor_flag-=1;
				  Ctl_Data.Key_Pflag &= ~key_RL;
					break;
				case key_RR:
					gor_flag+=1;
				  Ctl_Data.Key_Pflag &= ~key_RR;
					break;
				case key_LL:
					gol_flag-=1;
				  Ctl_Data.Key_Pflag &= ~key_LL;
					break;
				case key_LR:
					gol_flag+=1;
				  Ctl_Data.Key_Pflag &= ~key_LR;
					break;
				default:
					break;
			}
			if(gor_flag>4)
			{
				gor_flag=1;
			}
			if(gor_flag<1)
			{
				gor_flag=4;
			}
			if(gol_flag>4)
			{
				gol_flag=1;
			}
			if(gol_flag<1)
			{
				gol_flag=4;
			}
	 }
	 
 if(fb_flag>=vel_flag)
 {
	 fb_flag=0;
		 //�˶�����
	switch(gol_flag)
	{
		    case up_f:
				  templ_x=lhead->x;
				  templ_y=lhead->y-1;
					break;
				case down_f:
				  templ_x=lhead->x;
				  templ_y=lhead->y+1;
					break;
				case left_f:
				  templ_x=lhead->x-1;
				  templ_y=lhead->y;
					break;
				case right_f:
				  templ_x=lhead->x+1;
				  templ_y=lhead->y;
					break;
				default:
					break;
	}
	
		switch(gor_flag)
	{
		    case up_f:
				  tempr_x=rhead->x;
				  tempr_y=rhead->y-1;
					break;
				case down_f:
				  tempr_x=rhead->x;
				  tempr_y=rhead->y+1;
					break;
				case left_f:
				  tempr_x=rhead->x-1;
				  tempr_y=rhead->y;
					break;
				case right_f:
				  tempr_x=rhead->x+1;
				  tempr_y=rhead->y;
					break;
				default:
					break;
	}
	//�Ƿ�ǽ
	if(templ_y>7)
	{
		templ_y=0;
	}
	if(templ_y<0)
	{
		templ_y=7;
	}
		if(tempr_y>7)
	{
		tempr_y=0;
	}
	if(tempr_y<0)
	{
		tempr_y=7;
	}

	mself();

if(!diet_flag)
{
	//���Ƿ�Ե�ʳ��
	int lflag=0;
	int rflag=0;
	int tempnum;
	if(lvflag==1)
		tempnum=2;
	else if(lvflag==2)
		tempnum=4;
	else if(lvflag==3)
		tempnum=8;
	for(i=0;i<tempnum;i++)
	{
		if((templ_x==sfood[i].x)&&(templ_y==sfood[i].y))
		{
			lptr= (snake * )malloc(sizeof(snake));
	  	lptr->next =lhead;
		  lptr->x=templ_x;
	  	lptr->y=templ_y;
	  	lptr->color =lhead->color;
	  	lhead->color =0xffff00;
	  	Draw_Point(lhead->x,lhead->y,lhead->color,0);
	  	lhead=lptr;
	  	Draw_Point(lhead->x,lhead->y,lhead->color,0);
	  	lscore++;
	  	eatflag++;
			lflag=1;
			//ȥ���ѳԵ���ʳ��
			sfood[i].x=60;
			sfood[i].y=7;
			break;
		}
	}
	if(!lflag)
	{
		lptr= (snake * )malloc(sizeof(snake));
		lptr->next =lhead;
		lptr->x=templ_x;
		lptr->y=templ_y;
		lptr->color =lhead->color;
		Draw_Point(lhead->x,lhead->y,clryellow,0);
		lhead=lptr;
		tempp=ltail->next;
		ltail->next=NULL;
		Draw_Point(tempp->x,tempp->y,0x000000,0);
		Draw_Point(lhead->x,lhead->y,lhead->color,0);
		free(tempp);
	}
	//�ҷ��Ƿ�Ե�ʳ��
	for(i=0;i<tempnum;i++)
	{
		if((tempr_x==sfood[i].x)&&(tempr_y==sfood[i].y))
		{
      rptr= (snake * )malloc(sizeof(snake));
		  rptr->next =rhead;
	  	rptr->x=tempr_x;
	  	rptr->y=tempr_y;
	  	rptr->color =rhead->color;
	  	rhead->color =clrazure;
	  	Draw_Point(rhead->x,rhead->y,rhead->color,0);
	  	rhead=rptr;
	  	Draw_Point(rhead->x,rhead->y,rhead->color,0);
	  	rscore++;
	  	eatflag++;
			rflag=1;
			//ȥ���ѳԵ���ʳ��
			sfood[i].x=60;
			sfood[i].y=7;
			break;
		}
	}
	if(!rflag)
	{
		rptr= (snake * )malloc(sizeof(snake));
		rptr->next =rhead;
		rptr->x=tempr_x;
		rptr->y=tempr_y;
		rptr->color =rhead->color;
		Draw_Point(rhead->x,rhead->y,clrazure,0);
		rhead=rptr;
		tempp=rtail->next;
		rtail->next=NULL;
		Draw_Point(tempp->x,tempp->y,0x000000,0);
		Draw_Point(rhead->x,rhead->y,rhead->color,0);
		free(tempp);
	}
	walkcount++;
	if(lflag||rflag)
	{
			if(Detection_idle() == 1)
		{
			SAudio_flag = 0x0000;			//������������Ч
      Play_List(38);
		}
		
//		OSTimeDly(300,OS_OPT_TIME_DLY,&err);
	}
	else
	{
					if(Detection_idle() == 1)
		{
			SAudio_flag = 0x0000;			//������������Ч
			
			if(walkcount>=2)
			{
				walkcount=0;
			Play_List(35);
			}
		}
//		Play_List(34);
	}
//	
	Update_Layer();
	OSTimeDly(10,OS_OPT_TIME_DLY,&err);
}
diet_flag=0;
 }
}
	else
		;
 if(over_count>2000)
	{
		snakeover();
		Ctl_Data.Game_exit=0x01;
	}
	
	if((Ctl_Data.Trigger_schedule==1)&&(linkclr==1))
{
	freelink();
	linkclr=0;
	Ctl_Data.Trigger_schedule=0;
}

}

/*************************************************
*��������	������ʳ���������
*��ڲ���	����
*����ֵ		����
*˵��		  ����
**************************************************/
void Random_t(food *a)
{
	static u16	Indexes_t=0;
  static u16	Indexes_max_t = CH_N*Sampling_N;
	(*a).x = AD_Value[Indexes_t]%47;
	Indexes_t++;
	if(Indexes_t == Indexes_max_t)
		Indexes_t = 0;
	(*a).y=AD_Value[Indexes_t]%7;
		Indexes_t++;
	if(Indexes_t == Indexes_max_t)
		Indexes_t = 0;
	
}



/*************************************************
*��������	���ж��Ƿ�ײǽ/�������Լ�/�Ե��Է�β��/�����Ե����Բ����Լ�β��//��ͷ��ҧ/�Ե��Է�
*��ڲ���	����
*����ֵ		����
*˵��		  ��ײǽ��Ե��Լ�/�Է�������������㣬˫��ˢ��λ��
**************************************************/
void mself(void)
{
	OS_ERR err;
	lptr=lhead;
	rptr=rhead;
	//��
	while((lptr->next!=NULL)||(rptr->next!=NULL))
	{
		//��ͷ��ҧ
	if((templ_x==tempr_x)&&(templ_y==tempr_y))
		{
			if(Detection_idle() == 1)
		{
			SAudio_flag = 0x0000;			//������������Ч
			Play_List(22);
		}	
			//ˢ��������
			refresh(1,1);
			diet_flag=1;
			break;
		}
		//�����Ե����Բ����Լ�β��
		if((templ_x==ltail->next->x)&&(templ_y==ltail->next->y))
		{
			break;
//			lptr=lptr->next;
		}
		//ײǽ/�������Լ�/�ԶԷ�
		else if((templ_x==0||templ_x==47)||((templ_x==lptr->x)&&(templ_y==lptr->y))||((templ_x==rptr->x)&&(templ_y==rptr->y)))
		{
			if(Detection_idle() == 1)
		{
			SAudio_flag = 0x0000;			//������������Ч
			Play_List(22);
		}
			//��˸��ˢ��
			diet_flag=1;
			lscore=0;
//			twinkle(lhead);
			lptr=lhead;
			Draw_Point(lptr->x,lptr->y,0x000000,0);
			while(lptr->next!=NULL)
			{
				tempp=lptr;
				lptr=lptr->next;
				Draw_Point(lptr->x,lptr->y,0x000000,0);
				free(tempp);
			}
			free(lptr);
      //��̰���߳�ʼ��
	    lhead= (snake * )malloc(sizeof(snake));
	    lptr= (snake * )malloc(sizeof(snake));
      lhead->next=lptr;
	    lptr->next=NULL;
	    lhead->x=initl_x;
	    lhead->y=initl_y;
	    lhead->color=clrred;
      Draw_Point(lhead->x,lhead->y,lhead->color,0);
	    lptr->x=initl_x;
      lptr->y=initl_y-1;		
	    lptr->color=clryellow;
	    Draw_Point(lptr->x,lptr->y,lptr->color,0);
      new_data=(snake * )malloc(sizeof(snake));
	    lptr->next=new_data;
	    new_data->next =NULL;
    	lptr=new_data ;
	    lptr->x=initl_x;
      lptr->y=initl_y-2;		
	    lptr->color=clryellow;
	    Draw_Point(lptr->x,lptr->y,lptr->color,0);			 
//      Update_Layer();		
//      OSTimeDly(10,OS_OPT_TIME_DLY,&err);			
//			while(1)
//			{
//				;
//			}
			gol_flag=2;
			//�ҷ�̰����ˢ��
			refresh(0,1);
			
			break;
		}
		//�Ե��Է�β��
		else if((templ_x==rtail->x)&&(templ_y==rtail->y))
		{
			diet_flag=1;
			if(Detection_idle() == 1)
		{
			SAudio_flag = 0x0000;			//������������Ч
			Play_List(22);
		}
			if((tempr_x==ltail->x)&&(tempr_y==ltail->y))
			{
//					    Draw_Point(27,7,clrred,0);			 
//      Update_Layer();		
//      OSTimeDly(500,OS_OPT_TIME_DLY,&err);		
				refresh(1,1);
			}
			else
			{
				templ_s=lscore;
				tempr_s=rscore;
				if(rscore<=1)
				{
					;
				}
				else
				{
					rscore=rscore/2;
					lscore=rscore+lscore;
					//̰�������¶���
					lptr=lhead;
					rptr=rhead;
					//�ҷ�
					for(i=0;i<(2+rscore);i++)
					{
						rptr=rptr->next;
					}
					tempp=rptr->next;
					rptr->next=NULL;
					rptr=tempp;
					while(tempp!=NULL)
					{
						tempp=tempp->next;
						Draw_Point(rptr->x,rptr->y,0x000000,0);	
						free(rptr);
						rptr=tempp;
					}
					//��
					tempp=ltail->next;
					for(i=0;i<(lscore-templ_s);i++)
					{
						lptr= (snake * )malloc(sizeof(snake));
					  tempp->next=lptr;
					  lptr->next=NULL;
						tempp=lptr;
					}
					
				}	 
        Update_Layer();		
        OSTimeDly(500,OS_OPT_TIME_DLY,&err);
				refresh(1,1);
			}
			break;
		}
		else
		{
			lptr=lptr->next;
			if(rptr->next ==NULL)
			  ;
		  else
			  rptr=rptr->next ;
		}
	}
	lptr=lhead;
	rptr=rhead;
	//�ҷ�
	while((rptr->next!=NULL)||(lptr->next!=NULL))
	{
		//��ͷ��ҧ
	if((templ_x==tempr_x)&&(templ_y==tempr_y))
		{
			//ˢ��������
//			refresh(1,1);
			break;
		}
		//�����Ե����Բ����Լ�β��
		if((tempr_x==rtail->next->x)&&(tempr_y==rtail->next->y))
		{
//			rptr=rptr->next;
			break;
		}
		//ײǽ/�������Լ�/�ԶԷ�
		else if((tempr_x==0||tempr_x==47)||((tempr_x==lptr->x)&&(tempr_y==lptr->y))||((tempr_x==rptr->x)&&(tempr_y==rptr->y)))
		{
			if(Detection_idle() == 1)
		{
			SAudio_flag = 0x0000;			//������������Ч
			Play_List(22);
		}
			diet_flag=1;
     //���㣬ˢ��
			rscore=0;
//			twinkle(rhead);
			rptr=rhead;
			Draw_Point(rptr->x,rptr->y,0x000000,0);
			while(rptr->next!=NULL)
			{
				tempp=rptr;
				rptr=rptr->next;
				Draw_Point(rptr->x,rptr->y,0x000000,0);
				free(tempp);
			}
			free(rptr);
//			Update_Layer();
//			OSTimeDly(10,OS_OPT_TIME_DLY,&err);
      //�ҷ�̰���߳�ʼ��
	    rhead= (snake * )malloc(sizeof(snake));
	    rptr= (snake * )malloc(sizeof(snake));
      rhead->next=rptr;
	    rptr->next=NULL;
	    rhead->x=initr_x;
	    rhead->y=initr_y;
	    rhead->color=clrpink;
      Draw_Point(rhead->x,rhead->y,rhead->color,0);
	    rptr->x=initr_x;
      rptr->y=initr_y+1;		
	    rptr->color=clrazure;
	    Draw_Point(rptr->x,rptr->y,rptr->color,0);
      new_data=(snake * )malloc(sizeof(snake));
	    rptr->next=new_data;
	    new_data->next =NULL;
    	rptr=new_data ;
	    rptr->x=initr_x;
      rptr->y=initr_y+2;		
	    rptr->color=clrazure;
	    Draw_Point(rptr->x,rptr->y,rptr->color,0);			 
//      Update_Layer();		
//      OSTimeDly(10,OS_OPT_TIME_DLY,&err);			
//			while(1)
//			{
//				;
//			}
			gor_flag=4;
			//��ˢ��
			
			refresh(1,0);
			break;
		}
		//�Ե��Է�β��
		else if((tempr_x==ltail->x)&&(tempr_y==ltail->y))
		{
			if(Detection_idle() == 1)
		{
			SAudio_flag = 0x0000;			//������������Ч
			Play_List(22);
		}
			diet_flag=1;
			if((templ_x==rtail->x)&&(templ_y==rtail->y))
			{
				;
			}
			else
			{
				templ_s=lscore;
				tempr_s=rscore;
				if(lscore<=1)
				{
					lscore=0;
					rscore+=1;
				}
				else
				{
					lscore=lscore/2;
					rscore+=lscore;
					//̰�������¶���
					lptr=lhead;
					rptr=rhead;
					//��
					for(i=0;i<(2+lscore);i++)
					{
						lptr=lptr->next;
					}
					tempp=lptr->next;
					lptr->next=NULL;
					lptr=tempp;
					while(tempp!=NULL)
					{
						tempp=tempp->next;
						Draw_Point(lptr->x,lptr->y,0x000000,0);	
						free(lptr);
						lptr=tempp;
					}
					//�ҷ�
					tempp=rtail->next;
					for(i=0;i<(rscore-tempr_s);i++)
					{
						rptr= (snake * )malloc(sizeof(snake));
					  tempp->next=rptr;
					  rptr->next=NULL;
						tempp=rptr;
					}
				}
				refresh(1,1);
			}
			break;
		}
		else 
		{
			rptr=rptr->next;
			if(lptr->next ==NULL)
			  ;
		  else
			  lptr=lptr->next ;
		}
	}
}

/*************************************************
*��������	���������ʳ���ȡ�������ڶ���Ԫ��
*��ڲ���	����
*����ֵ		����
*˵��		  ����
**************************************************/
void randfood()
{
	OS_ERR err;
	lptr=lhead;
  rptr=rhead;
	while(lptr->next!=NULL)
		{
			ltail=lptr;
			lptr=lptr->next ;
		}
	while(rptr->next!=NULL)
		{
			rtail=rptr;
			rptr=rptr->next ;
			 
		}
	lptr=lhead;
  rptr=rhead;		
		
	switch(lvflag)
	{
		case 0:
			lvflag=1;
		  eatflag=0;
			tpfood(2);
		  for(i=0;i<2;i++)
		  {
//				sfood[i].x=10+i;
//				sfood[i].y=4;
		    sfood[i].color=0x0000ff;
	      Draw_Point(sfood[i].x,sfood[i].y,sfood[i].color,0);
			}
			Update_Layer();
	    OSTimeDly(10,OS_OPT_TIME_DLY,&err);
		  break;
		case 1:
			if(eatflag==2)
			{
//							if(Detection_idle() == 1)
//		{
			SAudio_flag = 0x0000;			//������������Ч
			Play_List(20);
//		}
				OSTimeDly(600,OS_OPT_TIME_DLY,&err);
				lvflag=2;
				eatflag=0;
				tpfood(4);
				
			}
			for(i=0;i<4;i++)
		    {
//				sfood[i].x=20+i;
//				sfood[i].y=4;
		    sfood[i].color=0x0000ff;
	      Draw_Point(sfood[i].x,sfood[i].y,sfood[i].color,0);
			  }
			  Update_Layer();
	      OSTimeDly(10,OS_OPT_TIME_DLY,&err);
			break;
		case 2:
			if(eatflag==4)
			{
//							if(Detection_idle() == 1)
//		{
			SAudio_flag = 0x0000;			//������������Ч
			Play_List(20);
//		}
				OSTimeDly(600,OS_OPT_TIME_DLY,&err);
				lvflag=3;
				eatflag=0;
				tpfood(8);
				
			}
			for(i=0;i<8;i++)
		    {
//				sfood[i].x=30+i;
//				sfood[i].y=4;
		    sfood[i].color=0x0000ff;
	      Draw_Point(sfood[i].x,sfood[i].y,sfood[i].color,0);
			  }
			  Update_Layer();
	      OSTimeDly(10,OS_OPT_TIME_DLY,&err);
			break;
		case 3:
			if(eatflag==8)
			{
//							if(Detection_idle() == 1)
//		{
			SAudio_flag = 0x0000;			//������������Ч
			Play_List(20);
//		}
				OSTimeDly(600,OS_OPT_TIME_DLY,&err);
				eatflag=0;
				tpfood(8);

//				SAudio_flag = 0;
			}
							for(i=0;i<8;i++)
		    {
		    sfood[i].color=0x0000ff;
	      Draw_Point(sfood[i].x,sfood[i].y,sfood[i].color,0);
			  }
			  Update_Layer();
	      OSTimeDly(10,OS_OPT_TIME_DLY,&err);
			break;
		default:
			break;
	}
	

	
	
	
	
	
	
//	//ʳ��1
//	if(eatf_flag)
//	{
//		Random_t(&f_food.x,&f_food.y);
//		while(lptr->next!=NULL||rptr->next!=NULL)
//	 {
//		 ltail=lptr;
//		 rtail=rptr;
//		//�Ƿ���̰���������ص�
//		if((f_food.x==lptr->x)&&(f_food.y==lptr->y))
//		{
//			lptr=lhead;
//			rptr=rhead;
//			Random_t(&f_food.x,&f_food.y);
//		}
//		else if((f_food.x==rptr->x)&&(f_food.y==rptr->y))
//		{
//			lptr=lhead;
//			rptr=rhead;
//			Random_t(&f_food.x,&f_food.y);
//		}
//		else
//		{
//			if(lptr->next ==NULL)
//			  ;
//		  else
//			  lptr=lptr->next ;
//			if(rptr->next ==NULL)
//			  ;
//		  else
//			  rptr=rptr->next ;
//		}
//		//�Ƿ���ǽ�ص�
//		if(f_food.x==0||f_food.x==47)
//		{
//			Random_t(&f_food.x,&f_food.y);
//			lptr=lhead;
//			rptr=rhead;
//		}
//		else
//			;
//		//�ж��Ƿ�������β�ص�
//		if(((f_food.x==lptr->next->x)&&(f_food.y==lptr->next->y))||((f_food.x==rptr->next->x)&&(f_food.y==rptr->next->y)))
//		{
//			lptr=lhead;
//			rptr=rhead;
//			Random_t(&f_food.x,&f_food.y);
//		}
//	 }
//	 
//	 f_food.color=0x0000ff;
//	 Draw_Point(f_food.x,f_food.y,f_food.color,0);
//   Update_Layer();
//	 OSTimeDly(10,OS_OPT_TIME_DLY,&err);			
//	 eatf_flag=0;
//  }
//	else
//	{
//		while(lptr->next!=NULL)
//		{
//			ltail=lptr;
//			lptr=lptr->next ;
//		}
//	}
//	lptr=lhead;
//  rptr=rhead;
//	//ʳ��2
//	if(eats_flag)
//	{
//		Random_t(&s_food.x,&s_food.y);
//		while(lptr->next!=NULL||rptr->next!=NULL)
//	 {
//		 ltail=lptr;
//		 rtail=rptr;
//		//�Ƿ���̰���������ص�
//		if(((s_food.x==lptr->x)&&(s_food.y==lptr->y))||s_food.x==0||s_food.x==47||((s_food.x==f_food.x)&&(s_food.y==f_food.y)))
//		{
//			lptr=lhead;
//			rptr=rhead;
//			Random_t(&s_food.x,&s_food.y);
//		}
//		else if((s_food.x==rptr->x)&&(s_food.y==rptr->y))
//		{
//			lptr=lhead;
//			rptr=rhead;
//			Random_t(&s_food.x,&s_food.y);
//		}
//		else
//		{
//			if(lptr->next ==NULL)
//			  ;
//		  else
//			  lptr=lptr->next ;
//			if(rptr->next ==NULL)
//			  ;
//		  else
//			  rptr=rptr->next ;
//		}
//		//�ж��Ƿ�������β�ص�
//		if(((s_food.x==lptr->next->x)&&(s_food.y==lptr->next->y))||((s_food.x==rptr->next->x)&&(s_food.y==rptr->next->y)))
//		{
//			lptr=lhead;
//			rptr=rhead;
//			Random_t(&s_food.x,&s_food.y);
//		}
//	 }
//	 
//	 
//	 s_food.color=0x0000ff;
//	 Draw_Point(s_food.x,s_food.y,s_food.color,0);
//   Update_Layer();
//	 OSTimeDly(10,OS_OPT_TIME_DLY,&err);		
//	 eats_flag=0;
//  }
//	else
//	{
//		while(rptr->next!=NULL)
//		{
//			rtail=rptr;
//			rptr=rptr->next ;
//			 
//		}
//	}
}


/*************************************************
*��������	��ˢ��̰����
*��ڲ���	����
*����ֵ		����
*˵��		  ����
**************************************************/
void refresh(int lf,int rf)
{
	OS_ERR err;
	gol_flag=2;
  gor_flag=4;
	lptr=lhead;
	rptr=rhead;
	llength=lscore+3;
	rlength=rscore+3;
	line_l=llength/9;
	row_l=llength%8;
	line_r=rlength/9;
	row_r=rlength%8;
//	line_l=0;
//	row_l=4;
//	line_r=0;
//	row_r=4;
	Draw_Point(f_food.x,f_food.y,0x0000000,0);
	Draw_Point(s_food.x,s_food.y,0x0000000,0);
	//��ˢ��
	if(lf)
	{
	 while(lptr!=NULL)
	 {
		 Draw_Point(lptr->x,lptr->y,0x000000,0);
		 lptr=lptr->next;
	 }
	 lptr=lhead;

	 lhead->x=line_l+1;
	 lhead->y=row_l-1;
	 rfx=line_l+1;
	 rfy=row_l-1;
	 if(line_l==0&&row_l==0)
	 {
		 lhead->y=7;
		 rfy=7;
	 }
	 Draw_Point(lhead->x,lhead->y,lhead->color,0);
	 
	 rfflag=-1;
	 while(lptr->next!=NULL)
	 {
		 rfy+=rfflag;
		if(rfy<0)
		{
			rfy=0;
			rfflag=-rfflag;
			rfx-=1;
		}
		if(rfy>7)
		{
			rfy=7;
			rfflag=-rfflag;
			rfx-=1;
		}
		lptr=lptr->next;
		lptr->x=rfx;
		lptr->y=rfy;
		Draw_Point(lptr->x,lptr->y,clryellow,0);
	 }
  }

	//�ҷ�ˢ��
	if(rf)
	{
	 while(rptr!=NULL)
	 {
		 Draw_Point(rptr->x,rptr->y,0x000000,0);
		 rptr=rptr->next;
	 }
	 rptr=rhead;

	  rhead->x=46-line_r;
	  rhead->y=8-row_r;
	  rfx=46-line_r;
	  rfy=8-row_r;
	 if(line_r==0&&row_r==0)
	 {
		 rhead->y=0;
		 rfy=0;
	 }
	 rfflag=1;
	 Draw_Point(rhead->x,rhead->y,rhead->color,0);
	 while(rptr->next!=NULL)
	 {
		rfy+=rfflag;
		if(rfy<0)
		{
			rfy=0;
			rfflag=-rfflag;
			rfx+=1;
		}
		if(rfy>7)
		{
			rfy=7;
			rfflag=-rfflag;
			rfx+=1;
		}
		rptr=rptr->next;
		rptr->x=rfx;
		rptr->y=rfy;
		Draw_Point(rptr->x,rptr->y,clrazure,0);
	 }
  }
	Update_Layer();
	OSTimeDly(10,OS_OPT_TIME_DLY,&err);	
  eatf_flag=1;
	eats_flag=1;

}
/*************************************************
*��������	��̰������Ϸ����
*��ڲ���	����
*����ֵ		����
*˵��		  ����
**************************************************/
void snakeover(void)
{
	OS_ERR err;
	
//	freelink();
	
				//��Ϸ����
			OSTimeDly(10,OS_OPT_TIME_DLY,&err);
			Clear_screen();
		  OSTimeDly(10,OS_OPT_TIME_DLY,&err);
			Update_Layer();
		  OSTimeDly(10,OS_OPT_TIME_DLY,&err);
	    Play_List(37);
	    DispStrCH(6,"GAME",0xff0000,0);
		  OSTimeDly(10,OS_OPT_TIME_DLY,&err);
			Update_Layer();
		  OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
		  DispStrCH(6,"OVER",0xff0000,0);
		  OSTimeDly(10,OS_OPT_TIME_DLY,&err);
			Update_Layer();
		  OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
			Clear_screen();
		  OSTimeDly(10,OS_OPT_TIME_DLY,&err);
			Update_Layer();
		  OSTimeDly(10,OS_OPT_TIME_DLY,&err);
//	    lscore=11;
//	    rscore=22;
			sprintf(ls,"%d",lscore);
	    sprintf(rs,"%d",rscore);
			DispStrCH(4,ls,clrgreen,0);
			DispStrCH(20,":",clrgreen,0);
			DispStrCH(27,rs,clrgreen,0);
			OSTimeDly(10,OS_OPT_TIME_DLY,&err);
			Update_Layer();
		  OSTimeDly(10,OS_OPT_TIME_DLY,&err);
//			Play_List(255);	
			OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
			over_count=0;
//			Ctl_Data.Mode=0x00;
			snake_flag=0;
			Clear_screen();
		  OSTimeDly(10,OS_OPT_TIME_DLY,&err);
			Update_Layer();
		  OSTimeDly(10,OS_OPT_TIME_DLY,&err);
}

void freelink()
{
	lptr=lhead;
			while(lptr->next!=NULL)
			{
				tempp=lptr;
				lptr=lptr->next;
				free(tempp);
			}
			free(lptr);
			rptr=rhead;
			while(rptr->next!=NULL)
			{
				tempp=rptr;
				rptr=rptr->next;
				free(tempp);
			}
			free(rptr);
			snake_flag =0;
			over_count=0;
}

void tpfood(int snum)
{
	OS_ERR err;
	int j=0;
	int sameflag;
	int temp=0;

	if(snum==2)
	{
		createfood(&sfood[0]);
		createfood(&sfood[1]);
		while((sfood[1].x==sfood[0].x)&&(sfood[1].y==sfood[0].y))
		{
			createfood(&sfood[1]);

		}

	}
	else if(snum==4)
	{
				createfood(&sfood[0]);
		i=1;
		while(sameflag)
		{
			createfood(&sfood[i]);
			temp=i;
			for(j=0;j<temp;j++)
		  {
				if((sfood[i].x==sfood[j].x)&&(sfood[i].y==sfood[j].y))
				{
					i--;
					break;
				}
			}
			if((temp==i)&&(i==3))
				sameflag=0;
			i++;
		}
//		createfood(sfood[0]);
//		createfood(sfood[1]);
//    while((sfood[1].x==sfood[0].x)&&(sfood[1].y==sfood[0].y))
//		{
//			createfood(sfood[1]);
//		}
//		createfood(sfood[2]);
//		while(((sfood[2].x==sfood[0].x)&&(sfood[2].y==sfood[0].y))||((sfood[2].x==sfood[1].x)&&(sfood[2].y==sfood[1].y)))
//		{
//			createfood(sfood[2]);
//		}
//		createfood(sfood[3]);
//		while(((sfood[3].x==sfood[0].x)&&(sfood[3].y==sfood[0].y))||((sfood[3].x==sfood[1].x)&&(sfood[3].y==sfood[1].y))||((sfood[3].x==sfood[2].x)&&(sfood[3].y==sfood[2].y)))
//		{
//			createfood(sfood[3]);
//		}		
	}
	else if(snum==8)
	{
		createfood(&sfood[0]);
		i=1;
		while(sameflag)
		{
			createfood(&sfood[i]);
			temp=i;
			for(j=0;j<temp;j++)
		  {
				if((sfood[i].x==sfood[j].x)&&(sfood[i].y==sfood[j].y))
				{
					i--;
					break;
				}
			}
			if((temp==i)&&(i==7))
				sameflag=0;
			i++;
		}
	}
}

void createfood(food *temfood)
{
	lptr=lhead;
	rptr=rhead;
	Random_t(temfood);
	while(lptr->next!=NULL||rptr->next!=NULL)
	{
		 ltail=lptr;
		 rtail=rptr;
		//�Ƿ���̰���������ص�
		if(((*temfood).x==lptr->x)&&((*temfood).y==lptr->y))
		{
			lptr=lhead;
			rptr=rhead;
			Random_t(temfood);
		}
		else if(((*temfood).x==rptr->x)&&((*temfood).y==rptr->y))
		{
			lptr=lhead;
			rptr=rhead;
			Random_t(temfood);
		}
		else
		{
			if(lptr->next ==NULL)
			  ;
		  else
			  lptr=lptr->next ;
			if(rptr->next ==NULL)
			  ;
		  else
			  rptr=rptr->next ;
		}
		//�Ƿ���ǽ�ص�
		if((*temfood).x==0||(*temfood).x==47)
		{
			Random_t(temfood);
			lptr=lhead;
			rptr=rhead;
		}
		else
			;
		//�ж��Ƿ�������β�ص�
		if((((*temfood).x==lptr->next->x)&&((*temfood).y==lptr->next->y))||(((*temfood).x==rptr->next->x)&&((*temfood).y==rptr->next->y)))
		{
			lptr=lhead;
			rptr=rhead;
			Random_t(temfood);
		}	
	}
}
//void rand_db()
//{
//	OS_ERR err;
//	lptr=lhead;
//  rptr=rhead;
//	//ʳ��1
//	if(eatdb_flag)
//	{
//		Random_t(&db_food.x,&db_food.y);
//		while(lptr->next!=NULL||rptr->next!=NULL)
//	 {
//		 ltail=lptr;
//		 rtail=rptr;
//		//�Ƿ���̰���������ص�
//		if((db_food.x==lptr->x)&&(db_food.y==lptr->y))
//		{
//			lptr=lhead;
//			rptr=rhead;
//			Random_t(&db_food.x,&db_food.y);
//		}
//		else if((db_food.x==rptr->x)&&(db_food.y==rptr->y))
//		{
//			lptr=lhead;
//			rptr=rhead;
//			Random_t(&db_food.x,&db_food.y);
//		}
//		else
//		{
//			if(lptr->next ==NULL)
//			  ;
//		  else
//			  lptr=lptr->next ;
//			if(rptr->next ==NULL)
//			  ;
//		  else
//			  rptr=rptr->next ;
//		}
//		//�Ƿ���ǽ�ص�
//		if(db_food.x==0||db_food.x==47)
//		{
//			Random_t(&db_food.x,&db_food.y);
//			lptr=lhead;
//			rptr=rhead;
//		}
//		else
//			;
//		//�ж��Ƿ�������β�ص�
//		if(((db_food.x==lptr->next->x)&&(db_food.y==lptr->next->y))||((db_food.x==rptr->next->x)&&(db_food.y==rptr->next->y)))
//		{
//			lptr=lhead;
//			rptr=rhead;
//			Random_t(&db_food.x,&db_food.y);
//		}
//	 }
//	 
//	 db_food.color=clrred;
//	 Draw_Point(db_food.x,db_food.y,db_food.color,0);
//   Update_Layer();
//	 OSTimeDly(10,OS_OPT_TIME_DLY,&err);			
//	 eatf_flag=0;
//  }
//	else
//	{
//		while(lptr->next!=NULL)
//		{
//			ltail=lptr;
//			lptr=lptr->next ;
//		}
//	}
//}
//void lvsnake()
//{
//	
//}



//void twinkle(snake *head)
//{
//	OS_ERR err;
//	snake *ptr;
//	u8 clr;
//	u8 headclr;
//	for(i=0;i<6;i++)
//	{
//		sflag=~sflag;
//		if(sflag)
//		{
//			headclr=head->color;
//			clr=head->next->color;
//		}
//		else
//		{
//			headclr=0x000000;
//			clr=0x000000;
//		}
//		ptr=head;
//		Draw_Point(ptr->x,ptr->y,headclr,0);
//		while(lptr->next!=NULL)
//		{
//		 tempp=ptr;
//		 ptr=ptr->next;
//		 Draw_Point(ptr->x,ptr->y,clr,0);
////	 free(tempp);
//		}
//		Update_Layer();
//		OSTimeDly(100,OS_OPT_TIME_DLY,&err);
//	}
//			
//}