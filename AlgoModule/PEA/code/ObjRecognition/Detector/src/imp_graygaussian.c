#include "time.h"
#include "stdlib.h"
#include "imp_graygaussian.h"
#include "math.h"
#include "time.h"
#include "sys/time.h"

struct timeval start,end;
typedef struct impGrayGaussian_S
{
	GA_HARDWARE_RS_S  *pstHwResource;
    PEA_RESULT_S *pstResult; 
    IMP_OutputGrayGaussian_S *pstOutput;
	GRAY_IMAGE_S stModel1; //高斯模型1
	GRAY_IMAGE_S stModel2; //高斯模型2
	GRAY_IMAGE_S stModel3; //高斯模型3
	GRAY_IMAGE_S stFilter;
	IMP_S32	s32H;
	IMP_S32 s32W;
	IMP_S32 s32Alpha;     //模型学习率
	IMP_S32 s32w_Alpha;   //权重学习率
	GRAY_IMAGE_S stWeight1; //保存模型1权重
	GRAY_IMAGE_S stWeight2; //保存模型2权重
	GRAY_IMAGE_S stWeight3; //保存模型3权重
}IMP_GrayGaussian_S;

IMP_S32 IMP_GetMemSizeGrayGaussian(PEA_RESULT_S *pstResult)
{
	IMP_S32 width = pstResult->s32Width;
	IMP_S32 height = pstResult->s32Height;
	IMP_S32 size =sizeof(IMP_GrayGaussian_S)+24*height*width; 	
	return size;
}

IMP_MODULE_HANDLE IMP_CreateGrayGaussian(PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource) //创建内存
{
	IMP_MODULE_HANDLE hModule = NULL;
	IMP_GrayGaussian_S *pstModule;  
	pstModule = (IMP_GrayGaussian_S*)IMP_MMAlloc( &pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IMP_GrayGaussian_S));	
	pstModule->pstHwResource = pstHwResource; 
	pstModule->pstResult = pstResult; 
	IMP_OutputGrayGaussian_S *pstOutput =&pstResult->stOutPutGG; 
	pstModule->pstOutput = pstOutput;
    IMP_S32	s32H=pstResult->s32Height;
	IMP_S32 s32W=pstResult->s32Width;
	pstModule->s32Alpha=13;  //初始化模型学习率
	pstModule->s32w_Alpha=1; //初始化权重学习率
    IMP_S32 s32row;
    IMP_S32 s32col;
    IMP_GrayImageCreate( &pstModule->stWeight1, s32W, s32H, &pstHwResource->stMemMgr );
    IMP_GrayImageCreate( &pstModule->stWeight2, s32W, s32H, &pstHwResource->stMemMgr );
    IMP_GrayImageCreate( &pstModule->stWeight3, s32W, s32H, &pstHwResource->stMemMgr );
    IMP_GrayImageCreate( &pstModule->stModel1, s32W, s32H, &pstHwResource->stMemMgr );
    IMP_GrayImageCreate( &pstModule->stModel2, s32W, s32H, &pstHwResource->stMemMgr );
    IMP_GrayImageCreate( &pstModule->stModel3, s32W, s32H, &pstHwResource->stMemMgr );
    IMP_GrayImageCreate( &pstModule->stFilter, s32W, s32H, &pstHwResource->stMemMgr );
    IMP_GrayImageCreate( &pstModule->pstOutput->stBkg, s32W, s32H, &pstHwResource->stMemMgr );
    IMP_GrayImageCreate( &pstModule->pstOutput->stFilter, s32W, s32H, &pstHwResource->stMemMgr );
	IMP_UCHAR *pu8Weight1=pstModule->stWeight1.pu8Data;
	IMP_UCHAR *pu8Weight2=pstModule->stWeight2.pu8Data;
	IMP_UCHAR *pu8Weight3=pstModule->stWeight3.pu8Data;
	pstOutput->stBkg = pstModule->stModel1;
	pstOutput->stFilter = pstModule->stFilter;
	for(s32row=s32H-1;s32row>=0;s32row--)
		for(s32col=s32W-1;s32col>=0;s32col--)
		{
			(pu8Weight1+s32row*s32W)[s32col]=43;   //初始化权重(0-128)
			(pu8Weight2+s32row*s32W)[s32col]=43;
			(pu8Weight3+s32row*s32W)[s32col]=43;
		}			
	hModule = (IMP_MODULE_HANDLE)pstModule;
	//printf("hello CreateGrayGaussian.\n");
	return hModule;
}
IMP_VOID sort(IMP_S32 s32a ,IMP_S32 s32b,IMP_S32 s32c,IMP_S32 *ps32Max,IMP_S32 *ps32Middle,IMP_S32 *ps32Min);
IMP_VOID norm_of_weight(IMP_S32 s32w1,IMP_S32 s32w2,IMP_S32 s32w3,IMP_S32 *ps32w1,IMP_S32 *ps32w2,IMP_S32 *ps32w3);
IMP_S32 get_new_weight(IMP_S32 s32w,IMP_S32 s32w_Alpha);
IMP_VOID filter(IMP_GrayGaussian_S *pstModule);
IMP_S32 impSaveImage(IMP_U8 *pu8Img, IMP_S32 s32W, IMP_S32 s32H, IMP_U8 *pu8Name);

IMP_S32 IMP_ProcessGrayGaussian(IMP_MODULE_HANDLE hModule)
{
	gettimeofday(&start,NULL);
	IMP_GrayGaussian_S *pstModule;	
	pstModule = (IMP_GrayGaussian_S*)hModule;
	IMP_S32 s32row; //所在行位置
	IMP_S32 s32col; //所在列位置
    IMP_S32 s32H = pstModule->pstResult->s32Height;
	IMP_S32 s32W = pstModule->pstResult->s32Width;
	IMP_S32 s32Alpha = pstModule->s32Alpha;
    IMP_S32 s32w_Alpha = pstModule->s32w_Alpha;
	IMP_U8 *pu8Weight1 = pstModule->stWeight1.pu8Data;
	IMP_U8 *pu8Weight2 = pstModule->stWeight2.pu8Data;
	IMP_U8 *pu8Weight3 = pstModule->stWeight3.pu8Data;
	IMP_U8 *pu8Model1 = pstModule->stModel1.pu8Data;
	IMP_U8 *pu8Model2 = pstModule->stModel2.pu8Data;
	IMP_U8 *pu8Model3 = pstModule->stModel3.pu8Data;
	IMP_U8 *pu8Scence = pstModule->pstResult->stImgInGray.pu8Data;
	IMP_S32 s32d = pstModule->pstResult->u32FrmTimeCur;  //帧号
	if(s32d-1==0)                                       //取第一帧信息
	{
	   for(s32row=s32H-1;s32row>=0;s32row--)
		   for(s32col=s32W-1;s32col>=0;s32col--)
		   {
			(pu8Model1+s32row*s32W)[s32col]=(pu8Scence+s32row*s32W)[s32col];
			(pu8Model2+s32row*s32W)[s32col]=(pu8Scence+s32row*s32W)[s32col];
			(pu8Model3+s32row*s32W)[s32col]=(pu8Scence+s32row*s32W)[s32col];  
		   }
    }
	else
	{
	IMP_S32 s32Distance_model1 ;
	IMP_S32 s32Distance_model2 ;
	IMP_S32 s32Distance_model3 ;
	IMP_S32 s32w;
	IMP_S32 s32max;
	IMP_S32 s32middle;
	IMP_S32 s32min;
	IMP_S32 s32w_max;
	IMP_S32 s32w_middle;
	IMP_S32 s32w_min;
	IMP_S32 s32w1;       //当前模型1权重
	IMP_S32 s32w2;       //当前模型2权重
	IMP_S32 s32w3;       //当前模型3权重
	IMP_S32 s32norm_w1;  //归一化后模型1权重
	IMP_S32 s32norm_w2;  //归一化后模型2权重
	IMP_S32 s32norm_w3;  //归一化后模型3权重
	IMP_U8 u8tmp;
	for(s32row=s32H-1;s32row>=0;s32row--)
	{
		uchar *ptr_scence = (uchar*)(pu8Scence+s32row*s32W);
		uchar *ptr_model1 = (uchar*)(pu8Model1+s32row*s32W);
		uchar *ptr_model2 = (uchar*)(pu8Model2+s32row*s32W);
		uchar *ptr_model3 = (uchar*)(pu8Model3+s32row*s32W);
		for(s32col=s32W-1;s32col>=0;s32col--)
		{				
			s32Distance_model1 = abs(ptr_model1[s32col]-ptr_scence[s32col]);
			s32Distance_model2 = abs(ptr_model2[s32col]-ptr_scence[s32col]);
			s32Distance_model3 = abs(ptr_model3[s32col]-ptr_scence[s32col]);
			sort(s32Distance_model1,s32Distance_model2,s32Distance_model3,&s32max,&s32middle,&s32min);
			s32w1 = (pu8Weight1+s32row*s32W)[s32col];
			s32w2 = (pu8Weight2+s32row*s32W)[s32col];
			s32w3 = (pu8Weight3+s32row*s32W)[s32col];
			if(s32min-10>0)
			{
				s32w3 = s32w_Alpha;
				norm_of_weight(s32w1,s32w2,s32w3,&s32norm_w1,&s32norm_w2,&s32norm_w3);
				ptr_model3[s32col] = ptr_scence[s32col];
				(pu8Weight1+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w1;
				(pu8Weight2+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w2;
				(pu8Weight3+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w3;
			}
			else
			{				
				if(s32min-s32Distance_model1==0)
				{
					s32w1 = get_new_weight(s32w1,s32w_Alpha);
					norm_of_weight(s32w1,s32w2,s32w3,&s32norm_w1,&s32norm_w2,&s32norm_w3);
					ptr_model1[s32col]=((128-s32Alpha)*(ptr_model1[s32col])+s32Alpha*(ptr_scence[s32col]) )>>7;	
					(pu8Weight1+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w1;
					(pu8Weight2+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w2;
					(pu8Weight3+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w3;
				}
				else if(s32min-s32Distance_model2==0)
				{
					s32w2 = get_new_weight(s32w2,s32w_Alpha);
					norm_of_weight(s32w1,s32w2,s32w3,&s32norm_w1,&s32norm_w2,&s32norm_w3);
					sort(s32norm_w1,s32norm_w2,s32norm_w3,&s32w_max,&s32w_middle,&s32w_min);
					if(s32norm_w2-s32w_middle==0)
					{
						ptr_model2[s32col]=((128-s32Alpha)*(ptr_model2[s32col])+s32Alpha*(ptr_scence[s32col]) )>>7;	
						(pu8Weight1+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w1;
						(pu8Weight2+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w2;
						(pu8Weight3+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w3;
					}
					else if(s32norm_w2-s32w_max==0)
					{
						s32w = s32norm_w2;
						s32norm_w2 = s32norm_w1;
						s32norm_w1 = s32w;
						u8tmp = ptr_model2[s32col];
						ptr_model2[s32col] = ptr_model1[s32col];
						ptr_model1[s32col]=((128-s32Alpha)*u8tmp+s32Alpha*(ptr_scence[s32col]) )>>7;			
						(pu8Weight1+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w1;
						(pu8Weight2+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w2;
						(pu8Weight3+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w3;
					}
				}
				else if(s32min-s32Distance_model3==0)
				{
					s32w3 = s32w3+s32w_Alpha ;
					norm_of_weight(s32w1,s32w2,s32w3,&s32norm_w1,&s32norm_w2,&s32norm_w3);
					sort(s32norm_w1,s32norm_w2,s32norm_w3,&s32w_max,&s32w_middle,&s32w_min);
					if(s32norm_w3-s32w_min==0)
					{
						ptr_model3[s32col]=((128-s32Alpha)*(ptr_model3[s32col])+s32Alpha*(ptr_scence[s32col]) )>>7;
						(pu8Weight1+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w1;
						(pu8Weight2+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w2;
						(pu8Weight3+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w3;
					}
					else if(s32norm_w3-s32w_middle==0)
					{
						s32w = s32norm_w3;
						s32norm_w3 = s32norm_w2;
						s32norm_w2 = s32w;
						u8tmp = ptr_model3[s32col];
						ptr_model3[s32col] = ptr_model2[s32col];						
						ptr_model2[s32col]=((128-s32Alpha)*u8tmp+s32Alpha*(ptr_scence[s32col]) )>>7;
						(pu8Weight1+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w1;
						(pu8Weight2+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w2;
						(pu8Weight3+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w3;
					}
					else if(s32norm_w3-s32w_max==0)
					{
						s32w = s32norm_w3;
						s32norm_w3 = s32norm_w2; 
						s32norm_w2 = s32norm_w1;
						s32norm_w1 = s32w;
						u8tmp = ptr_model3[s32col];
						ptr_model3[s32col] = ptr_model2[s32col];
						ptr_model2[s32col] = ptr_model1[s32col];
						ptr_model1[s32col]=((128-s32Alpha)*u8tmp+s32Alpha*(ptr_scence[s32col]) )>>7;
						(pu8Weight1+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w1;
						(pu8Weight2+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w2;
						(pu8Weight3+s32row*s32W)[s32col] = (IMP_UCHAR)s32norm_w3;
					}
				}
			}
			
		}
	}
	
	ipShowGrayImage(s32W, s32H, pu8Model1, "hellotrl1");
//	impSaveImage(pu8Model1, s32W, s32H, "hellotrl1.bmp");
	filter(pstModule);
	ipShowGrayImage(s32W, s32H, pstModule->stFilter.pu8Data, "hellofilter");
	}	
	//printf("hello ProcessGrayGaussian.\n");
	gettimeofday(&end,NULL);
	IMP_S32 timeuse = end.tv_usec - start.tv_usec; 
//	if(s32d==125) 
	printf("ProcessGrayGaussian time:%d\n", timeuse);
}

/*IMP_S32 impSaveImage(IMP_U8 *pu8Img, IMP_S32 s32W, IMP_S32 s32H, IMP_U8 *pu8Name)
{
	IMP_S32 s32RI, s32CI;
	IMP_S32 s32Oft, s32Oft2;
	IMP_S32 s32Step;
	
	IplImage *pstImg = cvCreateImage(cvSize(s32W, s32H), 8, 1);
	s32Step = pstImg->widthStep;
	
	for (s32RI = 0; s32RI < s32H; s32RI++)
	{
		for (s32CI = 0; s32CI < s32W; s32CI++)
		{
			s32Oft = s32RI * s32W + s32CI;
			s32Oft2 = s32RI * s32Step + s32CI;
			pstImg->imageData[s32Oft2] = pu8Img[s32Oft];
		}
	}
	cvSaveImage(pu8Name, pstImg, 0);
	cvReleaseImage(&pstImg);
	
	return 0;
}
*/

IMP_S32 IMP_ReleaseGrayGaussian(IMP_MODULE_HANDLE hModule)
{
	IMP_GrayGaussian_S *pstModule;	
	pstModule = (IMP_GrayGaussian_S*)hModule;
	GA_HARDWARE_RS_S *pstHwResource;
	IMP_GrayImageDestroy( &pstModule->pstOutput->stFilter, &pstHwResource->stMemMgr );     //释放内存
	IMP_GrayImageDestroy( &pstModule->pstOutput->stBkg, &pstHwResource->stMemMgr ); 
	IMP_GrayImageDestroy( &pstModule->stFilter, &pstHwResource->stMemMgr );
	IMP_GrayImageDestroy( &pstModule->stModel3, &pstHwResource->stMemMgr );
	IMP_GrayImageDestroy( &pstModule->stModel2, &pstHwResource->stMemMgr );
	IMP_GrayImageDestroy( &pstModule->stModel1, &pstHwResource->stMemMgr );
	IMP_GrayImageDestroy( &pstModule->stWeight3, &pstHwResource->stMemMgr );
	IMP_GrayImageDestroy( &pstModule->stWeight2, &pstHwResource->stMemMgr );
	IMP_GrayImageDestroy( &pstModule->stWeight1, &pstHwResource->stMemMgr );
    IMP_MMFree(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule);
	printf("bye bye ProcessGrayGaussian.\n");

}
IMP_VOID sort(IMP_S32 s32a ,IMP_S32 s32b,IMP_S32 s32c,IMP_S32 *ps32Max,IMP_S32 *ps32Middle,IMP_S32 *ps32Min)
{
	if(((s32a-s32b)*(s32a-s32c))<0)
	{
		*ps32Middle = s32a;
		if(s32a-s32b>0)
		{
			*ps32Max = s32c; 
			*ps32Min = s32b;
		}
		else
		{
			*ps32Max = s32b; 
			*ps32Min = s32c;
		}
	}
	else
	{
		if(s32a-s32b<0)
		{
			*ps32Min = s32a;
			if(s32b-s32c>0)
			{
				*ps32Max = s32b;
				*ps32Middle = s32c;
			}
			else
			{
				*ps32Max = s32c;
				*ps32Middle = s32b;
			}
		}
		else
		{
			*ps32Max = s32a;
			if(s32b-s32c>0)
			{
				*ps32Middle = s32b;
				*ps32Min = s32c;
			}
			else
			{
				*ps32Middle = s32c;
				*ps32Min = s32b;
			}
		}
	}
}
IMP_VOID norm_of_weight(IMP_S32 s32w1,IMP_S32 s32w2,IMP_S32 s32w3,IMP_S32 *ps32w1,IMP_S32 *ps32w2,IMP_S32 *ps32w3)
{
	IMP_S32 s32w_sum = s32w1+s32w2+s32w3;
	*ps32w1 = 128*s32w1/s32w_sum;
	*ps32w2 = 128*s32w2/s32w_sum;
	*ps32w3 = 128*s32w3/s32w_sum;
}
IMP_S32 get_new_weight(IMP_S32 s32w,IMP_S32 s32w_Alpha)
{
	s32w = ((128-s32w_Alpha)*s32w+128*s32w_Alpha + 64)>>7;
	return(s32w);
} 
IMP_VOID filter(IMP_GrayGaussian_S *pstModule)
{
	IMP_S32 s32W = pstModule->pstResult->s32Width;
	IMP_S32 s32H = pstModule->pstResult->s32Height;
	IMP_U8 *pu8Scence=pstModule->pstResult->stImgInGray.pu8Data;
	IMP_U8 *pu8Bkg=pstModule->stModel1.pu8Data;
	IMP_U8 *pu8Filter=pstModule->stFilter.pu8Data;
	IMP_S32 s32row;
	IMP_S32 s32col;
	IMP_S32 s32Intensity_sub;
	for(s32row=s32H-1;s32row>=0;s32row--)
	{
		IMP_U8 *ptr_scence = (IMP_U8*)(pu8Scence+s32row*s32W);
        IMP_U8 *ptr_bkg= (IMP_U8*)(pu8Bkg+s32row*s32W);
		IMP_U8 *ptr_filter= (IMP_U8*)(pu8Filter+s32row*s32W);
		for(s32col=s32W-1;s32col>=0;s32col--)
		{	
			 s32Intensity_sub =abs(ptr_scence[s32col]-ptr_bkg[s32col]);
			if(s32Intensity_sub-12<0)
			{
				ptr_filter[s32col]=0;
			}
			else
			{
				ptr_filter[s32col]=255;
			}
		}
	}
}


