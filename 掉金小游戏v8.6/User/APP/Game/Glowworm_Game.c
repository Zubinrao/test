#include "Glowworm_Game.h"
#include "Drawing.h"
#include "Control.h"
#include "WavPlay.h"
#include "string.h"
#include "math.h"
#include "ADC.h"

/*�ⲿ�����ģ�����ɼ����ݣ���������������������*/
extern __IO u16 AD_Value[CH_N*Sampling_N];

static u8  Motion;								//���ΰ��˶�������
static u8 	Gwm_Number;							//ө�������
static u8	Gwm_state_flag[GWM_NUMBER_MAX];		//ө���״̬��� 0-������ 1-����
static u8	Gwm_direction[GWM_NUMBER_MAX];		//ө����´��˶����� 0-���� 1-�� 2-�� 3-�� 4-���� 5-����
static u8	Gwm_Position[GWM_NUMBER_MAX][2];	//ө���x,yλ�ü�¼��
static u8	Gwm_Birth_P[GWM_NUMBER_MAX];		//ө������λ�ü�¼�� 0-���ұ߳��� 1-����߳���
static u8	Gwm_Level[GWM_NUMBER_MAX];			//ө������ȼ�¼��
static u8   GWM_Change[GWM_NUMBER_MAX];			//���ȱ仯�����¼�� 0-���ȼ�С 1-��������

static const u32 Gwm_Brightness[LEVEL]={
0x00000000,0x00060900,0x000C1100,0x00121A00,0x00172200,0x001D2B00,
0x00233300,0x00293C00,0x002F4400,0x00354D00,0x003B5500,0x00415E00,
0x00466600,0x004C6F00,0x00527700,0x00588000,0x005E8800,0x00649100,
0x006A9900,0x006FA200,0x0075AA00,0x007BB300,0x0081BB00,0x0087C300,
0x008DCC00,0x0093D500,0x0099DD00,0x009EE600,0x00A4EE00,0x00AAF700,0x00B0FF00};

u8	Count[10];							//���ü�����

/*����: ��Ϸ������ʼ��
 *���룺void
 *�����void
 */
void Glowworm_Game_Init(void)
{
	Gwm_Number = 0;
	
	memset(Gwm_state_flag,0,GWM_NUMBER_MAX);
//	memset(Gwm_direction,0,GWM_NUMBER_MAX);
//	memset(Gwm_Position,0,GWM_NUMBER_MAX*2);
//	memset(Gwm_Level,LEVEL-1,GWM_NUMBER_MAX);
//	memset(GWM_Change,0,GWM_NUMBER_MAX);
	
	Draw_LineX(0,59,15,NEST_COLOUR);
	Draw_LineX(0,59,14,NEST_COLOUR);
}

/*����: ���������
 *���룺x-y �������Χ (y > x)
 *����������������
 */
u8 Random(u8 x,u8 y)
{
	u8 buf;
	static u16	Indexes=0;
	static u16	Indexes_max = CH_N*Sampling_N;
	
	buf = AD_Value[Indexes] % (y-x+1) + x;
	Indexes++;
	if(Indexes == Indexes_max)
		Indexes = 0;
	
	return buf;
}

void Glowworm_Game(void)
{
	u8 i,x,y;
	u32 buf;
	
	Count[0] += 20;
	if(Count[0] >= REFRESH_INTERVAL)
	{
		Count[0] = 0;
		
		/*ͨ�����ΰ���ٶȿ���ө�������*/
		Motion = fabs(DMP_Data.Angular_Velocity)/15;
		if(Motion < GWM_NUMBER_MIN)	Motion = GWM_NUMBER_MIN;
		else if(Motion > GWM_NUMBER_MAX) Motion = GWM_NUMBER_MAX;
		
		/*����ө����˶�����*/
		for(i=0;i<GWM_NUMBER_MAX;i++)
		{
			if(Gwm_state_flag[i] != 0)
			{
				x = Gwm_Position[i][0];
				y = Gwm_Position[i][1];
				
				if(Gwm_Birth_P[i] == 0)									//���ұ߳�����ө���
				{
					if(y == 15)
						Draw_Point(x,y,NEST_COLOUR,0);					//ө���ӳ�Ѩ������ָ���Ѩ��ɫ
					else
						Draw_Point(x,y,0x000000,0);						//�����һ�ε��˶��켣
				
					/*����ө����˶�����*/
					switch(Gwm_direction[i])
					{
						case 0:											//����
						{
							break;
						}
						case 1:											//���˶�
						{
							if(x > 0)
							{
								x--;
								Gwm_Position[i][0] = x;
							}
							else										//ө�����ʧ
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							
							break;
						}
						case 2:											//���˶�
						{
							if(y > 0)
							{
								if(y == 15)	y = 7;
								else y--;
								Gwm_Position[i][1] = y;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							
							break;
						}
						case 3:											//���˶�
						{
							if(x < (LED_X-1))
							{
								x++;
								Gwm_Position[i][0] = x;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							
							break;
						}
						case 4:											//����
						{
							if((y > 0) && (x > 0))
							{
								x--;
								if(y == 15)	y = 7;
								else y--;
								
								Gwm_Position[i][0] = x;
								Gwm_Position[i][1] = y;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							break;
						}
						case 5:											//����
						{
							if((y > 0) && (x < (LED_X-1)))
							{
								x++;
								if(y == 15)	y = 7;
								else y--;
								
								Gwm_Position[i][0] = x;
								Gwm_Position[i][1] = y;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							break;
						}
						default:
							break;
					}
				}
				else if(Gwm_Birth_P[i] == 1)							//����߳�����ө���
				{
					if(y == 14)
						Draw_Point(x,y,NEST_COLOUR,0);
					else
						Draw_Point(x,y,0x000000,0);
					
					switch(Gwm_direction[i])
					{
						case 0:											//����
						{
							break;
						}
						case 1:											//���˶�
						{
							if(x < (LED_X-1))
							{
								x++;
								Gwm_Position[i][0] = x;
							}
							else										//ө�����ʧ
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							
							break;
						}
						case 2:											//���˶�
						{
							if(y != (LED_Y-1))
							{
								if(y == 14)	y = 0;
								else y++;
								Gwm_Position[i][1] = y;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							
							break;
						}
						case 3:											//���˶�
						{
							if(x > 0)
							{
								x--;
								Gwm_Position[i][0] = x;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							
							break;
						}
						case 4:											//����
						{
							if((y != (LED_Y-1)) && (x < (LED_X-1)))
							{
								x++;
								if(y == 14)	y = 0;
								else y++;
								
								Gwm_Position[i][0] = x;
								Gwm_Position[i][1] = y;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							break;
						}
						case 5:											//����
						{
							if((y != (LED_Y-1)) && (x > 0))
							{
								x--;
								if(y == 14)	y = 0;
								else y++;
								
								Gwm_Position[i][0] = x;
								Gwm_Position[i][1] = y;
							}
							else
							{
								Gwm_state_flag[i] = 0;
								if(Gwm_Number != 0)
									Gwm_Number--;
							}
							break;
						}
						default:
							break;
					}
				}
			}
		}
		
		/*����ө�����һ�ε��˶�����*/
		for(i=0;i<GWM_NUMBER_MAX;i++)
		{
			Gwm_direction[i] = Random(0,5);
		}
		
		/*����ө����������*/
		if(Gwm_Number < Motion)
		{
			for(i=0;i<Motion;i++)
			{
				/*��ѯ�Ѿ���ʧ�˵�ө���*/
				if(Gwm_state_flag[i] == 0)
				{
					Gwm_Birth_P[i] = Random(0,1);				//����ө���ĳ�������
					Gwm_Position[i][0] = Random(0,LED_X-1);		//����ө���ĳ���λ�� X
					if(Gwm_Birth_P[i] == 0)
						Gwm_Position[i][1] = 15;				//����ө���ĳ���λ�� Y
					else
						Gwm_Position[i][1] = 14;
					Gwm_Level[i] = Random(0,LEVEL-1);			//�����������
					GWM_Change[i] = Random(0,1);				//�������ȱ仯�������
					Gwm_direction[i] = 2;						//����ө����´��˶�����
					Gwm_Number++;								//��¼ө�������
					Gwm_state_flag[i] = 1;						//���ө����Ѿ�����
				}
			}
		}
	}
	
	/*���ȿ���*/
	for(i=0;i<GWM_NUMBER_MAX;i++)
	{
		if(Gwm_state_flag[i] !=0 )
		{
			x = Gwm_Position[i][0];
			y = Gwm_Position[i][1];
			
			buf = Gwm_Brightness[Gwm_Level[i]];		//��ȡ��ʱ�̵�����
			if(GWM_Change[i] !=0 )					//��һʱ�̵���������
			{
				if(Gwm_Level[i] >= (LEVEL-1))
					GWM_Change[i] = 0;				//���ȷ�ת
				else
					Gwm_Level[i]++;
			}
			else									//��һʱ�̵����ȼ�С
			{
				if(Gwm_Level[i] <= 0)
					GWM_Change[i] = 1;
				else
					Gwm_Level[i]--;
			}
			
			Draw_Point(x,y,buf,0);
		}
	}
	Update_Layer();
}
