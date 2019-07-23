#include "img.h"
#include <math.h>
#include <bsp.h>
u32 color;
u8 *databuf;    		//���ݶ�ȡ��ŵ�ַ
u8 *bmpbuf;			  	//���ݽ����ַ
BITMAPINFO *pbmp;		//��ʱָ��
u16 x ,y,rowlen,countpix,count,rgb,color_byte;	
u16 br;
u16 readlen=1024;
/*
����ͼ������
Ϊ0��ʧ��
�ɹ�ʹ������Ҫ�ͷ��ڴ�ռ�
pic_memfree(databuf);
pic_memfree(f_model);

*/
ZqImage* open_pic (void)	
{

	FIL* f_model;
	ZqImage* tempimg;
	u8 res;
	Clear_screen();
	Update_Layer();
	
	
	databuf=(u8*)pic_memalloc(readlen);		//����readlen�ֽڵ��ڴ�����
	f_model=(FIL *)pic_memalloc(sizeof(FIL));	//����FIL�ֽڵ��ڴ����� 
	if(f_model==NULL)							//�ڴ�����ʧ��.
	{		 
		pic_memfree(databuf);
	} 	
	
	res=f_open(f_model,"1:/PICTURE/test.bmp",FA_READ);//���ļ�	
		if(res==0)//�򿪳ɹ�.
	{
		f_read(f_model,databuf,readlen,(UINT*)&br);	//����readlen���ֽ�
		if(br!=readlen)readlen=br;	//���һ������
		pbmp=(BITMAPINFO*)databuf;					//�õ�BMP��ͷ����Ϣ   
//		count=pbmp->bmfHeader.bfOffBits;        	//����ƫ��,�õ����ݶεĿ�ʼ��ַ
//		color_byte=pbmp->bmiHeader.biBitCount/8;	//��ɫλ 16/24/32 
//		bmpbuf=databuf+count;//����ƫ��,�õ����ݶεĿ�ʼ��ַ
//		tempimg=(u8*)pic_memalloc(sizeof(unsigned char)*pbmp->bmiHeader.biWidth*pbmp->bmiHeader.biHeight+sizeof(int)*3);		//����ͼ���С�ֽڵ��ڴ�����
		tempimg->height = pbmp->bmiHeader.biHeight;
		tempimg->width = pbmp->bmiHeader.biWidth; 
		tempimg->channels = pbmp->bmiHeader.biBitCount/8;	//��ɫλ 16/24/32 
		tempimg->imageData = databuf+count;//����ƫ��,�õ����ݶεĿ�ʼ��ַ
		f_close(f_model);//�ر��ļ�
		return tempimg;
	}
	return 0;//
}

void show_pic(ZqImage* bmpImg)
{
		x=0;
		y=0;
		rowlen=8;//һ��8������
		countpix=0;
		count=0;
		rgb=0;
		while(count<readlen)
		{
				if(bmpImg->channels==3)   //24λ��ɫͼ
				{
					switch (rgb) 
					{
						case 0:				  
							color=(u32)bmpImg->imageData[count]&0x000000ff; //B
							break ;	   
						case 1: 	 
							color|=(u32)(bmpImg->imageData[count]&0x000000ff)<<8;//G
							break;	  
						case 2 : 
							color|=(u32)(bmpImg->imageData[count]&0x000000ff)<<16;//R	 			
							break ;			
					}   
				}
				rgb++;	  
				count++;
			if(rgb==bmpImg->channels) //ˮƽ�����ȡ��1���������ݺ���ʾ
				{	
					if(x<bmpImg->height)	 					 			   
					{	
						Draw_Point(x,y,color,0); 	
						Update_Layer();						
					}
					countpix++;//�����ۼ�
					x++;//x������һ������ 
					color=0x00000000; 
					rgb=0; 	
				}
				
				if(countpix>=rowlen)//ˮƽ��������ֵ����.����
				{		 
					y++; 	
					x=0; 
					countpix=0;
					color=0x00000000;
					rgb=0;
				}
		}
		


//Clear_screen();
//	Update_Layer();
//Draw_Point(0,0,0xff11D0,0);//RGB
//Draw_Point(0,1,0x00ff00,0);
//Draw_Point(0,2,0x0000ff,0);
//Update_Layer();
}

ZqImage* imscale(ZqImage* bmpImg,double dy,double dx)
{
     //ͼƬ���Ŵ���
    ZqImage* bmpImgSca;
    int width = 0;
    int height = 0;
    int channels = 1;
    int step = 0;
    int Sca_step = 0;
    int i, j, k;
	  int pre_i,pre_j,after_i,after_j;//����ǰ��Ӧ�����ص�����
	
    width = bmpImg->width;
    height = bmpImg->height;
    channels = bmpImg->channels;
    //��ʼ�����ź�ͼƬ����Ϣ
    bmpImgSca = (ZqImage*)pic_memalloc(sizeof(ZqImage));
    bmpImgSca->channels = channels;
    bmpImgSca->width = (int)(bmpImg->width*dy + 0.5);
    bmpImgSca->height = (int)(bmpImg->height*dx + 0.5);
    step = channels * width;
    Sca_step = channels * bmpImgSca->width;
    bmpImgSca->imageData = (unsigned char*)pic_memalloc(sizeof(unsigned char)*bmpImgSca->width*bmpImgSca->height*channels);

		if (channels == 3)
    {
        //��ʼ������ͼ��
        for(i=0; i<bmpImgSca->height; i++)
        {
            for(j=0; j<bmpImgSca->width; j++)
            {
                for(k=0; k<3; k++)
                {
                    bmpImgSca->imageData[(bmpImgSca->height-1-i)*Sca_step+j*3+k] = 0;
                }
            }
        }
        //����任
        for(i = 0;i < bmpImgSca->height;i++)
        {
            for(j = 0;j < bmpImgSca->width;j++)
            {
                after_i = i;
                after_j = j;
                pre_i = (int)(after_i / dx + 0.5);
                pre_j = (int)(after_j / dy + 0.5);
                if(pre_i >= 0 && pre_i < height && pre_j >= 0 && pre_j < width)//��ԭͼ��Χ��
                    for(k=0; k<3; k++)
                    {
                        bmpImgSca->imageData[i * Sca_step + j*3 +k] = bmpImg->imageData[pre_i * step + pre_j*3 + k];
                    }
            }
        }
    }
    return bmpImgSca;
}

ZqImage* imrotate(ZqImage* bmpImg,int Angle)
{
     //ͼƬ��ת����
    ZqImage* bmpImgRot;
    double angle;//Ҫ��ת�Ļ�����
    int width = 0;
    int height = 0;
    int step = 0;
    int Rot_step = 0;
    int channels = 1;
    int i, j, k;
		int midX_pre,midY_pre,midX_aft,midY_aft;//��תǰ������ĵ������
		int pre_i,pre_j,after_i,after_j;//��תǰ���Ӧ�����ص�����
 
    width = bmpImg->width;
    height = bmpImg->height;
    channels = bmpImg->channels;
    midX_pre = width / 2;
    midY_pre = height / 2;
    angle = 1.0 * Angle * PI / 180;
    //��ʼ����ת��ͼƬ����Ϣ
    bmpImgRot = (ZqImage*)pic_memalloc(sizeof(ZqImage));
    bmpImgRot->channels = channels;
    bmpImgRot->width = bmpImg->width;
    bmpImgRot->height = bmpImg->height;
    midX_aft =bmpImgRot->width / 2;
    midY_aft = bmpImgRot->height / 2;
    step = channels * width;
    Rot_step = channels * bmpImgRot->width;
    bmpImgRot->imageData = (unsigned char*)pic_memalloc(sizeof(unsigned char)*bmpImgRot->width*bmpImgRot->height*channels);

		
    if (channels == 3)
    {
        //��ʼ����תͼ��
        for(i=0; i<bmpImgRot->height; i++)
        {
            for(j=0; j<bmpImgRot->width; j++)
            {
                for(k=0; k<3; k++)
                {
                    bmpImgRot->imageData[(bmpImgRot->height-1-i)*Rot_step+j*3+k] = 0;
                }
            }
        }
        //����任
        for(i = 0;i < bmpImgRot->height;i++)
        {
            for(j = 0;j < bmpImgRot->width;j++)
            {
                after_i = i - midX_aft;
                after_j = j - midY_aft;
                pre_i = (int)(cos((double)angle) * after_i - sin((double)angle) * after_j) + midX_pre;
                pre_j = (int)(sin((double)angle) * after_i + cos((double)angle) * after_j) + midY_pre;

                if(pre_i >= 0 && pre_i < height && pre_j >= 0 && pre_j < width)//��ԭͼ��Χ��
                    for(k=0; k<3; k++)
                    {
                        bmpImgRot->imageData[i * Rot_step + j*3 +k] = bmpImg->imageData[pre_i * step + pre_j*3 + k];
                    }
            }
        }
    }
    return bmpImgRot;
}
//��̬�����ڴ�
void *pic_memalloc (u32 size)			
{
//	return (void*)mymalloc(SRAMIN,size);
	return (void*)malloc(size);
}
//�ͷ��ڴ�
void pic_memfree (void* mf)		 
{
	free(mf);
//	myfree(SRAMIN,mf);
}
	
