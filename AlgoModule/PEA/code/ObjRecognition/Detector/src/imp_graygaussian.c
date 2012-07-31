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
	IMP_S32 *pstCod;
	IMP_U8 *pu8Dilate;
	IMP_S32	s32H;
	IMP_S32 s32W;	
	IMP_S32 s32Alpha;       //模型学习率
	IMP_S32 s32w_Alpha;     //权重学习率
}IMP_GrayGaussian_S;

IMP_S32 IMP_GetMemSizeGrayGaussian(PEA_RESULT_S *pstResult)
{
	IMP_S32 width = pstResult->s32Width;
	IMP_S32 height = pstResult->s32Height;
	IMP_S32 size = sizeof(IMP_GrayGaussian_S)+4*height*width; 	
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
	pstModule->s32Alpha =45;  //初始化模型学习率
	pstModule->s32w_Alpha =1; //初始化权重学习率
   	IMP_S32 s32row;
   	IMP_S32 s32col;
	pstModule->pstCod = (IMP_S32 *) IMP_MMAlloc( &pstHwResource->stMemMgr,IMP_MEMBLK_TYPE_SLOW,s32H*s32W*4);
	pstModule->pu8Dilate = (IMP_U8 *) IMP_MMAlloc( &pstHwResource->stMemMgr,IMP_MEMBLK_TYPE_SLOW,4*s32H*s32W);
	IMP_GrayImageCreate( &pstOutput->stBkg, s32W, s32H, &pstHwResource->stMemMgr );
    IMP_GrayImageCreate( &pstOutput->stFilter, s32W, s32H, &pstHwResource->stMemMgr );
	hModule = (IMP_MODULE_HANDLE)pstModule;
	return hModule;
}
IMP_VOID filter(IMP_GrayGaussian_S *pstModule);
IMP_VOID SCAN( IMP_U8 *src, IMP_U16 width, IMP_U16 height);
IMP_VOID norm_of_weight( IMP_S32 *ps32w1, IMP_S32 *ps32w2);
IMP_VOID DilateImage ( IMP_U8 *src, IMP_U16 width, IMP_U16 height);

IMP_S32 IMP_ProcessGrayGaussian(IMP_MODULE_HANDLE hModule)
{
	gettimeofday(&start,NULL);
	IMP_GrayGaussian_S *pstModule;	
	pstModule = (IMP_GrayGaussian_S*)hModule;
	IMP_S32 s32row; //所在行位置
	IMP_S32 s32col; //所在列位置
    IMP_S32 s32H = pstModule->pstResult->s32Height ;
	IMP_S32 s32W = pstModule->pstResult->s32Width ;
	IMP_S32 s32Alpha = pstModule->s32Alpha ;
   	IMP_S32 s32w_Alpha = pstModule->s32w_Alpha ;
	IMP_S32 *ps32Codval = pstModule->pstCod ;
	IMP_U8 u8Scenceval;
	IMP_U8 *pu8Scence = pstModule->pstResult->stImgInGray.pu8Data ;
	IMP_U8 *pu8Bkg = pstModule->pstOutput->stBkg.pu8Data ;
	IMP_U8 *pu8Sub = pstModule->pu8Dilate ;
	IMP_S32 s32d = pstModule->pstResult->u32FrmTimeCur ;  //帧号
	if(s32d==0)                                       //取第一帧信息
	{
	   for(s32row=s32H-1;s32row>=0;s32row--)
		   for(s32col=s32W-1;s32col>=0;s32col--)
		   {
			u8Scenceval = (pu8Scence+s32row*s32W)[s32col];
			(ps32Codval+s32row*s32W)[s32col]= (127<<24)|(127<<16)|(u8Scenceval<<8)|(u8Scenceval);  //初始化
		   }
    }
	IMP_S32 s32weight1 ;  //当前模型1权重
	IMP_S32 s32weight2 ;  //当前模型2权重
	IMP_S32 s32model1 ;
	IMP_S32 s32model2 ;
	IMP_S32 s32Distance_model1 ;
	IMP_S32 s32Distance_model2 ;
	IMP_S32 s32weight;       
	IMP_U8 u8tmp;
	IMP_S32 s32sumval;
	for(s32row=s32H-1;s32row>=0;s32row--)
	{
		IMP_S32 *ptr_cod = (IMP_S32*)(ps32Codval+s32row*s32W);
		IMP_U8 *ptr_scence = (IMP_U8*)(pu8Scence+s32row*s32W);
		IMP_U8 *ptr_sub = (IMP_U8*)(pu8Sub+s32row*s32W*4) ;
		for(s32col=s32W-1;s32col>=0;s32col--)
		{
			s32sumval = ptr_cod[s32col];
			s32model1 = (s32sumval&0xFF00)>>8;
			s32model2 = s32sumval&0x00FF;
			s32Distance_model1 = abs(s32model1-ptr_scence[s32col]);
			s32Distance_model2 = abs(s32model2-ptr_scence[s32col]);
			if(s32Distance_model1-10<0)
				ptr_sub[(s32col<<2)+0] = 0 ;
			else
				ptr_sub[(s32col<<2)+0] = 255 ;					
			if(s32Distance_model2-10<0)
				ptr_sub[(s32col<<2)+1] = 0 ;
			else
				ptr_sub[(s32col<<2)+1] = 255 ;	
			ptr_sub[(s32col<<2)+2] = 0 ;	
			ptr_sub[(s32col<<2)+3] = 0 ;			
		}	
	}
	SCAN( pu8Sub, s32W, s32H);
	DilateImage( pu8Sub, s32W, s32H);		
	for( s32row = s32H-1; s32row >= 0; s32row--)
	{
		IMP_U8 *ptr_sub = (IMP_U8*)(pu8Sub+s32row*s32W*4) ;
		IMP_S32 *ptr_cod = (IMP_S32*)(ps32Codval+s32row*s32W);
		IMP_U8 *ptr_scence = (IMP_U8*)(pu8Scence+s32row*s32W);
		IMP_U8 *ptr_bkg = (IMP_U8*)(pu8Bkg+s32row*s32W);
		for( s32col=s32W-1; s32col>=0; s32col--)
		{	
			s32sumval = ptr_cod[s32col];
			s32weight1 = (s32sumval&0xFF000000)>>24;
			s32weight2 = (s32sumval&0x00FF0000)>>16;
			s32model1 = (s32sumval&0xFF00)>>8;
			s32model2 =  s32sumval&0x00FF;		
			if(ptr_sub[(s32col<<2)+0]==0)
			{
				if(s32weight1-255<0)
						s32weight1++;	
				s32model1 = ((s32model1<<7)+s32Alpha*(ptr_scence[s32col]-s32model1)+64 )>>7;
			}
			s32weight2 = 255 -s32weight1;
			if(ptr_sub[(s32col<<2)+3]==0)
			{
				if(s32weight2-255<0)
				{
					if((s32d&2)==0)
						s32weight2++;
				}
				s32model2=((s32model2<<7)+s32Alpha*(ptr_scence[s32col]-s32model2) +64 )>>7;
			}
			if((ptr_sub[(s32col<<2)+0]==255)&&(ptr_sub[(s32col<<2)+3]==255))
			{
				s32weight2 = s32w_Alpha;
				s32model2 = ptr_scence[s32col];
			}
			s32weight1 = 255 -s32weight2;
			if(s32row==60&&s32col==171)
					printf(" weight1: %d--weight2: %d--model1:%d--model2:%d\n",s32weight1,s32weight2,s32model1,s32model2);
			if(s32weight2-s32weight1>0)
			{
				s32weight = 0;
				s32weight = s32weight2;
				s32weight2 = s32weight1;
				s32weight1 = s32weight;
				u8tmp = s32model2;
				s32model2 = s32model1;
				s32model1 = u8tmp;
			}	
			(ps32Codval+s32row*s32W)[s32col] = (((IMP_U8)s32weight1)<<24)|(((IMP_U8)s32weight2)<<16)|(((IMP_U8)s32model1)<<8)|((IMP_U8)s32model2); 
			ptr_bkg[s32col] = (IMP_U8)s32model1;					
		}
	}
	ipShowGrayImage(s32W, s32H, pu8Bkg, "hellotrl1");
	filter(pstModule);
	ipShowGrayImage(s32W, s32H, pstModule->pstOutput->stFilter.pu8Data, "hellofilter");
	gettimeofday(&end,NULL);
	IMP_S32 timeuse = end.tv_usec - start.tv_usec; 
	printf("time %d\n", timeuse);
}


IMP_S32 IMP_ReleaseGrayGaussian(IMP_MODULE_HANDLE hModule)
{
	IMP_GrayGaussian_S *pstModule;	
	pstModule = (IMP_GrayGaussian_S*)hModule;
	GA_HARDWARE_RS_S *pstHwResource; 
	IMP_GrayImageDestroy( &pstModule->pstOutput->stFilter, &pstHwResource->stMemMgr );   //释放内存
	IMP_GrayImageDestroy( &pstModule->pstOutput->stBkg, &pstHwResource->stMemMgr );
	IMP_MMFree(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->pu8Dilate);	
	IMP_MMFree(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->pstCod);
    IMP_MMFree(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule);
}

IMP_VOID filter(IMP_GrayGaussian_S *pstModule)
{
	IMP_S32 s32W = pstModule->pstResult->s32Width;
	IMP_S32 s32H = pstModule->pstResult->s32Height;
	IMP_U8 *pu8Scence=pstModule->pstResult->stImgInGray.pu8Data;
	IMP_S32 s32threshold = 0;
	s32threshold = pstModule->pstResult->s32Noise;
	s32threshold = ((s32threshold<<1)+s32threshold)>>1;
	IMP_U8 *pu8Bkg = pstModule->pstOutput->stBkg.pu8Data;
	IMP_U8 *pu8Filter = pstModule->pstOutput->stFilter.pu8Data;
	IMP_S32 s32row;
	IMP_S32 s32col;
	IMP_S32 s32Intensity_sub;
	for(s32row=s32H-1;s32row>=0;s32row--)
	{
		IMP_U8 *ptr_scence = (IMP_U8*)(pu8Scence+s32row*s32W);
		IMP_U8 *ptr_bkg = (IMP_U8*)(pu8Bkg+s32row*s32W);
		IMP_U8 *ptr_filter = (IMP_U8*)(pu8Filter+s32row*s32W);
		for(s32col=s32W-1;s32col>=0;s32col--)
		{	
			s32Intensity_sub =abs(ptr_scence[s32col]-ptr_bkg[s32col]);
			if(s32Intensity_sub-s32threshold<0)
			{
				ptr_filter[s32col]=0;
				continue;
			}
			ptr_filter[s32col]=255;			
		}
	}
}

IMP_VOID SCAN( IMP_U8 *src,IMP_U16 width, IMP_U16 height)
{
	IMP_S32 x, y, result1,result2;
	IMP_S32 widthM1 = width - 2;
	IMP_S32 heightM1 = height - 2;
	IMP_U8 *psnegr,*psneg,*ps,*psplu,*psplur;
	psnegr = src;	// skip one stride
	psneg = src +(width<<2);
	ps = src +(width<<3);
	psplu = src +(width<<3)+(width<<2);
	psplur = src +(width<<4);
	for ( y = 2; y < heightM1; y++ )
	{
		psnegr +=8 ;
		psneg +=8 ;
		ps +=8 ;
		psplu +=8 ;
		psplur +=8 ;							
		for ( x = 2; x < widthM1; x++)
		{
			if((*ps)==255)
			{
				result1 = 0;
				result1 |= *(psneg -4);
				result1 |= *(psnegr);
				result1 |= *(psneg +4);
				result1 |= *(ps - 8);
				result1 |= *(ps + 8);
				result1 |= *(psplu -4);
				result1 |= *(psplur);
				result1 |= *(psplu +4);
				(*(ps+2)) = result1&(*ps);
			}
			if((*(ps+1))==255)
			{
				result2 = 0;
				result2 |= *((psneg +1) -4);
				result2 |= *(psnegr +1);
				result2 |= *((psneg +1) +4);
				result2 |= *((ps+1) - 8);
				result2 |= *((ps+1) + 8);
				result2 |= *((psplu +1) -4);
				result2 |= *(psplur+1);
				result2 |= *((psplu+1) +4);
				(*(ps+3)) = result2&(*(ps+1));
			}
			
			psnegr +=4 ;
			psneg +=4 ;
			ps +=4 ;
			psplu +=4 ;
			psplur +=4 ;		
		}
		psnegr +=8 ;
		psneg +=8 ;
		ps +=8 ;
		psplu +=8 ;
		psplur +=8 ;
	}
}

IMP_VOID DilateImage ( IMP_U8 *src, IMP_U16 width, IMP_U16 height)
{
	IMP_S32 x, y, result1, result2, result3, result4;

	IMP_S32 widthM1 = width - 1;
	IMP_S32 heightM1 = height - 1;
	IMP_U8 *psneg,*ps,*psplu;
	psneg = src;	// skip one stride
	ps = src +(width<<2);
	psplu = src +(width<<3);
	for ( y = 1; y < heightM1; y++ )
	{
		psneg +=4 ;
		ps +=4 ;
		psplu +=4 ;
		for ( x = 1; x < widthM1; x++)
		{
			result3 = 0;
			result3 |= *((psneg +2) -4);
			result3 |= *(psneg +2);
			result3 |= *((psneg +2) +4);
			result3 |= *((ps +2) - 4);
			result3 |= *(ps +2);
			result3 |= *((ps +2) +4);
			result3 |= *((psplu +2) -4);
			result3 |= *(psplu +2);
			result3 |= *((psplu +2) +4);
			(*(ps+0)) = (result3 !=0? 255 : 0);			
			psneg +=4 ;
			ps +=4 ;
			psplu +=4 ;	
		}
		psneg +=4 ;
		ps +=4 ;
		psplu +=4 ;
	}
}


