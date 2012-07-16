#include "time.h"
#include "stdlib.h"

#include "cv.h"
#include "highgui.h"

#include "imp_lightremove.h"

IMP_S32 impShowColorMark(IMP_UCHAR *pu8Img, IMP_UCHAR *pu8Mark, IMP_S32 s32W, IMP_S32 s32H);

typedef struct impLightRemove_S
{
	GA_HARDWARE_RS_S  *pstHwResource;
    PEA_RESULT_S *pstResult; 
    IMP_OutputLightRemove_S *pstOutput;
	IMP_S32 s32i;
	IMP_FLOAT fthesh1;
	IMP_FLOAT fthesh2;
	IMP_FLOAT fdelay;
    IMP_FLOAT * apfqueue1[9];
	IMP_UCHAR * apu8queue2[10];
	IMP_UCHAR * pu8H;
	IMP_UCHAR * pu8M;
	IMP_UCHAR * pu8N;
	IMP_UCHAR * pu8T;
	IMP_UCHAR * pu8single;
	IMP_FLOAT * pfV;
	IMP_FLOAT * pfVt0;
	IMP_FLOAT * pfVt1;
	IMP_UCHAR * pu8img;
}IMP_LightRemove_S;


IMP_S32 imp_dif1(IMP_FLOAT * src3,IMP_S32 src4,IMP_S32 row,IMP_S32 col,IMP_FLOAT valu1,IMP_FLOAT valu2)
{
	if(abs((src3+row*src4)[col])>valu2 &&
		abs((src3+(row-1)*src4)[col-1])>valu2 &&
		abs((src3+(row-1)*src4)[col])>valu2 &&
		abs((src3+(row-1)*src4)[col+1])>valu2 &&
		abs((src3+(row)*src4)[col-1])>valu2 &&
		abs((src3+(row)*src4)[col+1])>valu2 &&
		abs((src3+(row+1)*src4)[col-1])>valu2 &&
		abs((src3+(row+1)*src4)[col])>valu2 &&
		abs((src3+(row+1)*src4)[col+1])>valu2)
		return(0);
	else
		if((src3+row*src4)[col]>valu1 &&
			(src3+(row-1)*src4)[col-1]>valu1 &&
			(src3+(row-1)*src4)[col]>valu1 &&
			(src3+(row-1)*src4)[col+1]>valu1 &&
			(src3+(row)*src4)[col-1]>valu1 &&
			(src3+(row)*src4)[col+1]>valu1 &&
			(src3+(row+1)*src4)[col-1]>valu1 &&
			(src3+(row+1)*src4)[col]>valu1 &&
			(src3+(row+1)*src4)[col+1]>valu1)
			return(1);
		else
		{
			
			if((src3+row*src4)[col]<(-valu1) &&
				(src3+(row-1)*src4)[col-1]<(-valu1) &&
				(src3+(row-1)*src4)[col]<(-valu1) &&
				(src3+(row-1)*src4)[col+1]<(-valu1) &&
				(src3+(row)*src4)[col-1]<(-valu1) &&
				(src3+(row)*src4)[col+1]<(-valu1) &&
				(src3+(row+1)*src4)[col-1]<(-valu1) &&
				(src3+(row+1)*src4)[col]<(-valu1) &&
				(src3+(row+1)*src4)[col+1]<(-valu1))
				return(2);
			else
				return(3);
		}
}
IMP_S32 IMP_GetMemSizeAlgModel( PEA_RESULT_S *pstResult )
{
	IMP_S32 width = pstResult->s32Width;
	IMP_S32 height = pstResult->s32Height;
	IMP_S32 size =sizeof(IMP_LightRemove_S)+28*height*width; 	
	return size;
}
IMP_MODULE_HANDLE IMP_CreateLightRemove(PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource)
{
	IMP_MODULE_HANDLE hModule = NULL;
	IMP_LightRemove_S *pstModule;  
	pstModule = (IMP_LightRemove_S*)IMP_MMAlloc( &pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IMP_LightRemove_S));	
	pstModule->pstHwResource = pstHwResource; 
	pstModule->pstResult = pstResult; 
	IMP_OutputLightRemove_S *pstOutput =&pstResult->stOutPutLR; 
    IMP_S32	s32H=pstResult->s32Height;
	IMP_S32 s32W=pstResult->s32Width;
    pstModule->pu8single=pstResult->stImgInGray.pu8Data;
    IMP_S32 i;
    IMP_S32 j;
	for(i=0;i<=8;i++)
	{
		pstModule->apfqueue1[i]=(IMP_FLOAT *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,s32H*s32W*4);
	}
	for(j=0;j<=9;j++)
	{
		pstModule->apu8queue2[j]=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,s32H*s32W);
	}
	pstModule->s32i=0;
	pstModule->pu8H=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,s32H*s32W);
	pstModule->pu8M=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,s32H*s32W);
	pstModule->pu8N=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,s32H*s32W);
	pstModule->pu8T=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,s32H*s32W);
	pstModule->pfV=(IMP_FLOAT *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,s32H*s32W*4);
	pstModule->pfVt0=(IMP_FLOAT *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,s32H*s32W*4);
	pstModule->pfVt1=(IMP_FLOAT *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,s32H*s32W*4);
	pstModule->pu8img=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,s32H*s32W);
    IMP_S32 i1;
    IMP_S32 j1;
    for(i1=0;i1<s32H;i1++)
		for(j1=0;j1<s32W;j1++)
		{
			(pstModule->pu8H+i1*s32W)[j1]=0;
			(pstModule->pu8M+i1*s32W)[j1]=0;
			(pstModule->pu8N+i1*s32W)[j1]=0;
			(pstModule->pu8T+i1*s32W)[j1]=0;
		}
	pstModule->fthesh1=10;
	pstModule->fthesh2=2;
	pstModule->fdelay=20;
	
	pstOutput->pu8img = pstModule->pu8img;
	hModule = (IMP_MODULE_HANDLE)pstModule;
    return hModule;
}

IMP_S32 IMP_ConfigLightRemove(IMP_MODULE_HANDLE hModule, IMP_LightRemovePara_S *pstLRP)
{
    IMP_LightRemove_S *pstModule;
    pstModule = (IMP_LightRemove_S*)hModule;
    return 0;
}

IMP_S32 IMP_ProcessLightRemove(IMP_MODULE_HANDLE hModule)
{
	IMP_LightRemove_S *pstModule;	
	pstModule = (IMP_LightRemove_S*)hModule;
	PEA_RESULT_S *pstResult = pstModule->pstResult;
	IMP_OutputLightRemove_S *pstOutput =&pstResult->stOutPutLR;
	
    IMP_S32 s32H=pstModule->pstResult->s32Height;
	IMP_S32 s32W=pstModule->pstResult->s32Width;
    IMP_FLOAT ** appfqueue1=pstModule->apfqueue1;
	IMP_UCHAR ** appu8queue2=pstModule->apu8queue2;
	IMP_UCHAR * pu8H=pstModule->pu8H;
	IMP_UCHAR * pu8M=pstModule->pu8M;
	IMP_UCHAR * pu8N=pstModule->pu8N;
	IMP_UCHAR * pu8T=pstModule->pu8T;
	IMP_UCHAR * pu8single=pstModule->pu8single;
	IMP_FLOAT * pfV=pstModule->pfV;
	IMP_FLOAT * pfVt0=pstModule->pfVt0;
	IMP_FLOAT * pfVt1=pstModule->pfVt1;
    IMP_UCHAR * pu8img=pstModule->pu8img;
	IMP_FLOAT fthesh1=pstModule->fthesh1;
	IMP_FLOAT fthesh2=pstModule->fthesh2;
	IMP_FLOAT fdelay=pstModule->fdelay;
	IMP_S32 d=pstModule->pstResult->u32FrmTimeCur;
	if(d<=1)
    {
       IMP_S32 i;
       IMP_S32 j;
	   for(i=0;i<s32H;i++)
		   for(j=0;j<s32W;j++)
		   {
			  (appu8queue2[0]+i*s32W)[j]=(pu8single+i*s32W)[j];
		   }
    }
    else
    {
    IMP_S32 i;
    IMP_S32 j;
	for(i=0;i<s32H;i++)
		for(j=0;j<s32W;j++)
		{
			(pu8img+i*s32W)[j]=0;
		}
	pstModule->s32i++;
	if(pstModule->s32i<=9)
	{  
            IMP_S32 i;
            IMP_S32 j;
    	    for(i=0;i<s32H;i++)
	    	for(j=0;j<s32W;j++) 
			{
		    	(appu8queue2[pstModule->s32i]+i*s32W)[j]=(pu8single+i*s32W)[j];
		    	(appfqueue1[pstModule->s32i-1]+i*s32W)[j]=(pu8single+i*s32W)[j]-
				    (appu8queue2[0]+i*s32W)[j];
			}
	}
	else
	{
        IMP_S32 ii;
		for(ii=0;ii<=7;ii++)
		{
            IMP_S32 i;
            IMP_S32 j;
			for(i=0;i<s32H;i++)
			    for(j=0;j<s32W;j++)
			    {
				    (appfqueue1[ii]+i*s32W)[j]=(appfqueue1[ii+1]+i*s32W)[j]+
					     (appu8queue2[0]+i*s32W)[j]-(appu8queue2[1]+i*s32W)[j];
			    }
		}
        IMP_S32 jj;
		for(jj=0;jj<=8;jj++)
		{
            IMP_S32 p1;
            IMP_S32 q1;
			for(p1=0;p1<s32H;p1++)
			    for(q1=0;q1<s32W;q1++)
			    {
				   (appu8queue2[jj]+p1*s32W)[q1]=(appu8queue2[jj+1]+p1*s32W)[q1];
			    }
		}
        IMP_S32 p2;
        IMP_S32 q2;
        for(p2=0;p2<s32H;p2++)
		    for(q2=0;q2<s32W;q2++)
		    {
			   (appu8queue2[9]+p2*s32W)[q2]=(pu8single+p2*s32W)[q2];
			   (appfqueue1[8]+p2*s32W)[q2]=(pu8single+p2*s32W)[q2]-
				    (appu8queue2[0]+p2*s32W)[q2];
		    }
        IMP_S32 i9;
        IMP_S32 j9;
		for(i9=0;i9<s32H;i9++)
			for(j9=0;j9<s32W;j9++)
			{
				if(pstModule->s32i==10)
					(pfVt0+i9*s32W)[j9]=(appfqueue1[8]+i9*s32W)[j9]-
					     (appfqueue1[0]+i9*s32W)[j9];
				else
				{
					(pfV+i9*s32W)[j9]=(appfqueue1[8]+i9*s32W)[j9]-
						(appfqueue1[0]+i9*s32W)[j9];
					(pfVt1+i9*s32W)[j9]=0.75*(pfVt0+i9*s32W)[j9]+
						0.25*(pfV+i9*s32W)[j9];
					(pfVt0+i9*s32W)[j9]=(pfVt1+i9*s32W)[j9];                              
				}
			}
        IMP_S32 i2;
        IMP_S32 j2;
		for(i2=1;i2<s32H-1;i2++)
			for(j2=1;j2<s32W-1;j2++)
			{
				if(pstModule->s32i>18)
				{ 
					if(abs((appfqueue1[8]+i2*s32W)[j2]-(appfqueue1[0]+i2*s32W)[j2])>fthesh1)
					{
						(pu8img+i2*s32W)[j2]=1;
						(pu8H+i2*s32W)[j2]=0;
						(pu8M+i2*s32W)[j2]=0;
					}
					else
					{
						if(imp_dif1(pfVt1,s32W,i2,j2,fthesh2,fthesh1)==1)
						{
      
							(pu8H+i2*s32W)[j2]=(pu8H+i2*s32W)[j2]+1;
							(pu8M+i2*s32W)[j2]=0;
							(pu8img+i2*s32W)[j2]=2;
						}
						else
						{
							if((pu8H+i2*s32W)[j2]!=0)
							{			   
								(pu8img+i2*s32W)[j2]=2;	   
								(pu8N+i2*s32W)[j2]=(pu8N+i2*s32W)[j2]+1;
								if((pu8N+i2*s32W)[j2]>=fdelay)
								{
									(pu8H+i2*s32W)[j2]=0;
									(pu8N+i2*s32W)[j2]=0;
								}
							}
						}
						if(imp_dif1(pfVt1,s32W,i2,j2,fthesh2,fthesh1)==2)
						{
							(pu8M+i2*s32W)[j2]=(pu8M+i2*s32W)[j2]+1;
							(pu8H+i2*s32W)[j2]=0;
							(pu8img+i2*s32W)[j2]=3;	
						}
						else
						{
							if((pu8M+i2*s32W)[j2]!=0)
							{
								(pu8img+i2*s32W)[j2]=3;
								(pu8T+i2*s32W)[j2]=(pu8T+i2*s32W)[j2]+1;
								if((pu8T+i2*s32W)[j2]>=fdelay)
								{
									(pu8M+i2*s32W)[j2]=0;
									(pu8T+i2*s32W)[j2]=0;
								}
							}
						}
					}
				}
				
			}
		}
	}
	
//	memset(pu8img, 0, s32W * s32H);
  //  for(IMP_S32 i3=0;i3<s32H;i3++)
	//	for(IMP_S32 j3=0;j3<s32W;j3++)
	//		if((pu8img+i3*s32W)[j3]==1)
	//		{
	//			(pu8frame+i3*s32W*3)[j3*3]=0;
	//			(pu8frame+i3*s32W*3)[j3*3+1]=255;
	//			(pu8frame+i3*s32W*3)[j3*3+2]=0;
	//		}
	//		else
	//		{
	//			if((pu8img+i3*s32W)[j3]==2)
//				{
//					(pu8frame+i3*s32W*3)[j3*3]=255;
//					(pu8frame+i3*s32W*3)[j3*3+1]=0;
//					(pu8frame+i3*s32W*3)[j3*3+2]=0;
//				}
//				else
//				{
//					if((pu8img+i3*s32W)[j3]==3)
//					{
//						(pu8frame+i3*s32W*3)[j3*3]=0;
//						(pu8frame+i3*s32W*3)[j3*3+1]=0;
//						(pu8frame+i3*s32W*3)[j3*3+2]=255;
//					}
//				}
//			}
	
	impShowColorMark(pu8single, pu8img, s32W, s32H);
	
	return 0;
}
#include "cv.h"
#include "highgui.h"

IMP_S32 IMP_ReleaseLightRemove(IMP_MODULE_HANDLE hModule)
{
    IMP_LightRemove_S *pstModule;
    MEM_MGR_ARRAY_S *pstMemMgr;
    
	pstModule = (IMP_LightRemove_S*)hModule;
	pstMemMgr = &pstModule->pstHwResource->stMemMgr;
	
//	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->pstOutput);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->pfVt1);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->pfVt0);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->pfV);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->pu8T);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->pu8N);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->pu8M);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->pu8H);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->pu8img);
	
	IMP_S32 i, j;
	for(i=0;i<=8;i++)
	{
		IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->apfqueue1[i]);
	}

	for(j=0;j<=9;j++)
	{
		IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->apu8queue2[j]);
	}	
	
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule);
	
	return 0;
}



IMP_S32 impShowColorMark(IMP_U8 *pu8Img, IMP_U8 *pu8Mark, IMP_S32 s32W, IMP_S32 s32H)
{
	IMP_S32 s32RI, s32CI;
	IMP_S32 s32Oft;
	IplImage *pstColorMark = 0;
	IMP_U8 *pu8ColorData = 0;
	IMP_S32 s32WidthStep;
	
	pstColorMark = cvCreateImage(cvSize(s32W, s32H), 8, 3);
	pu8ColorData = pstColorMark->imageData;
	s32WidthStep = pstColorMark->widthStep;
	
	for (s32RI = 0; s32RI < s32H; s32RI++)
	{
		for (s32CI = 0; s32CI < s32W; s32CI++)
		{
			s32Oft = s32RI * s32W + s32CI;
			
			pu8ColorData[s32RI * s32WidthStep + s32CI * 3 + 0] = pu8Img[s32Oft];
			pu8ColorData[s32RI * s32WidthStep + s32CI * 3 + 1] = pu8Img[s32Oft];
			pu8ColorData[s32RI * s32WidthStep + s32CI * 3 + 2] = pu8Img[s32Oft];
			
			if (pu8Mark[s32Oft] == 1)
			{
				pu8ColorData[s32RI * s32WidthStep + s32CI * 3 + 0] = 0;
				pu8ColorData[s32RI * s32WidthStep + s32CI * 3 + 1] = 255;
				pu8ColorData[s32RI * s32WidthStep + s32CI * 3 + 2] = 0;
			}
			else if (pu8Mark[s32Oft] == 2)
			{
				pu8ColorData[s32RI * s32WidthStep + s32CI * 3 + 0] = 255;
				pu8ColorData[s32RI * s32WidthStep + s32CI * 3 + 1] = 0;
				pu8ColorData[s32RI * s32WidthStep + s32CI * 3 + 2] = 0;
			}
			else if (pu8Mark[s32Oft] == 3)
			{
				pu8ColorData[s32RI * s32WidthStep + s32CI * 3 + 0] = 0;
				pu8ColorData[s32RI * s32WidthStep + s32CI * 3 + 1] = 0;
				pu8ColorData[s32RI * s32WidthStep + s32CI * 3 + 2] = 255;
			}
		}
	}

	cvShowImage("ColorMark", pstColorMark);
	cvReleaseImage(&pstColorMark);

	return 0;
}


