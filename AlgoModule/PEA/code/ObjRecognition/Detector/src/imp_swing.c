#include "imp_swing.h"
#include "time.h"
#include "stdlib.h"


IMP_S32 IMP_ShowImg(IMP_UCHAR*srcGray,IMP_S32 Height,IMP_S32 Width,IMP_UCHAR*widoName)
{
	IMP_S32 width;
	IMP_S32 height;
	IMP_UCHAR *P1;
	IMP_UCHAR *P2;
	IplImage*src;
	
	if(!srcGray)
	{
	  printf("The input of IMP_ShowImg is wrong!\n");
	  return 0;
	}
	
	src=cvCreateImage(cvSize(Width,Height),IPL_DEPTH_8U,1);
	
	for (height=Height;height>0;height--)
	{
		P1=(IMP_UCHAR*)(src->imageData+(height-1)*src->widthStep);
		P2=srcGray+(height-1)*Width;
		for (width=Width;width>0;width--)
		{
			P1[width-1]=P2[width-1];
		}
	}
	cvShowImage(widoName,src);
	//cvWaitKey(1);
	cvReleaseImage(&src);
	return 1;
}

IMP_S32 IMP_BinaryImg(IMP_UCHAR *srcGray,IMP_UCHAR *BinMask,IMP_S32 val,IMP_S32 Lenght)
{
	IMP_S32 lenght;
	lenght=Lenght;
	
	while(lenght--)
	{
		if (srcGray[lenght]>val)
		{
			BinMask[lenght]=255;
		}
		else
		{
			BinMask[lenght]=0;
		}
	}
	return 1;
}

IMP_S32 IMP_StatiPix(IMP_UCHAR*srcGray,IMP_UCHAR*statiImg,IMP_S32 ImgH,IMP_S32 ImgW)//chengfa xuyao gaijin 
{
	//该操作之前需要对statiImg图像进行归零
	//srcGray图像中像素值不是0，则statiImg相应位置应该+1；
	//srcGray图像中像素值为0，则statiImg相应位置应该-1
	//当数值大于255时，statiImg赋值为255
	//当数值小雨0，statiImg赋值为0
	IMP_U8 *P1;
	IMP_U8 *P2;
	IMP_S32 Num;
	IMP_S32 width;
	IMP_S32 height;
	IMP_S32 OffNeg[6]={-ImgW-1,-ImgW,-ImgW+1, ImgW-1, ImgW,-ImgW+1};
	
	//if (!srcGray||!statiImg)
	//{
	//	printf("The input of IMP_StatiPix is wrong!\n");
	//	return 0;
	//}

	P1=srcGray;
	P2=statiImg;
	for(height=1;height<ImgH-1;height++)
	{
	  //P1=srcGray+height*ImgW;
	  //P2=statiImg+height*ImgW;
	  P1+=ImgW;
	  P2+=ImgW;
	  for(width=1;width<ImgW-1;width++)
	  {
	    Num=0;
	    Num+=P1[width+OffNeg[0]];
        Num+=P1[width+OffNeg[1]];
	    Num+=P1[width+OffNeg[2]];
        Num+=P1[width+OffNeg[3]];
        Num+=P1[width+OffNeg[4]];
        Num+=P1[width+OffNeg[5]];
        Num+=P1[width-1];
        Num+=P1[width];
	    Num+=P1[width+1];
	 
	    if(Num>3||P1[width])
	    {
	      if(P2[width]<255)
	      {
	        P2[width]++;
	      } 
	    }
	    else
	    {
	      if(P2[width]>0)
	      {
	        P2[width]--;
	      }
	    }
	  }
	}

	return 1;
}
IMP_S32 IMP_Proporte(IMP_UCHAR*backGray,IMP_UCHAR*srcGray,IMP_UCHAR*proMat1,IMP_UCHAR*proMat2,IMP_S32 Length)
{
	/*得到两图像像素比值，存放与propotMat中，Length是图像的内存长度*/
	
	IMP_S32 length;
	length=Length;

	while(length--)
	{
		if(srcGray[length]-backGray[length]>0)
		  proMat1[length]=1;
		else
		  proMat1[length]=0;
		if(srcGray[length]-backGray[length]<0)
		  proMat2[length]=1;
		else
		  proMat2[length]=0;
		
	}
	return 1;
}


IMP_S32 IMP_GetSadImg1(IMP_Swing_S*PsModel)
{
	//将原先的图像增加一个通道
	//增加的一个通道用来存放该
	//像素对应的位置的八邻域的
	//值之和信息
	//struct timeval tvStart,tvEnd;//delete
	//double linStart = 0,linEnd = 0,lTime = 0;//delete
	IMP_S32 i;
	IMP_S32 j;
	IMP_S32 ImgH;
	IMP_S32 ImgW;
	IMP_S32 Wstep;
	IMP_S32 Hstep;
	IMP_S32 Length1;
	IMP_S32 Length2;
	IMP_S32 zminVal;
	IMP_S32 vminVal;
	IMP_S32 width;
	IMP_S32 height;

	IMP_UCHAR*P;
	IMP_UCHAR*Pgray;
	IMP_UCHAR*Pbck;
	IMP_UCHAR*FoImg;
	IMP_UCHAR*Pzsad;
	IMP_UCHAR*Pvsad;
	IMP_UCHAR*zSadMin;
	IMP_UCHAR*vSadMin;
	
	IMP_UCHAR*Pz11;
	IMP_UCHAR*Pz10;
	IMP_UCHAR*Pz1_1;
	IMP_UCHAR*Pz01;
	IMP_UCHAR*Pz00;
	IMP_UCHAR*Pz0_1;
	IMP_UCHAR*Pz_11;
	IMP_UCHAR*Pz_10;
	IMP_UCHAR*Pz_1_1;


	IMP_UCHAR*Pg11;
	IMP_UCHAR*Pg10;
	IMP_UCHAR*Pg1_1;
	IMP_UCHAR*Pg01;
	IMP_UCHAR*Pg00;
	IMP_UCHAR*Pg0_1;
	IMP_UCHAR*Pg_11;
	IMP_UCHAR*Pg_10;
	IMP_UCHAR*Pg_1_1;
	
	IMP_UCHAR*XY00;
	IMP_UCHAR*XY01;
	IMP_UCHAR*XY02;
	IMP_UCHAR*XY10;
	IMP_UCHAR*XY11;
	IMP_UCHAR*XY12;
	IMP_UCHAR*XY20;
	IMP_UCHAR*XY21;
	IMP_UCHAR*XY22;

	XY00=PsModel->p00;
	XY01=PsModel->p01;
	XY02=PsModel->p02;
	XY10=PsModel->p10;
	XY11=PsModel->p11;
	XY12=PsModel->p12;
	XY20=PsModel->p20;
	XY21=PsModel->p21;
	XY22=PsModel->p22;
	
	
	zSadMin=PsModel->zSadmin;
	vSadMin=PsModel->vSadmin;
	ImgH=PsModel->pstResult->s32Height;
	ImgW=PsModel->pstResult->s32Width;
	IMP_GaussImg(PsModel->pstResult->stImgInGray.pu8Data,XY00, ImgH, ImgW);
	IMP_DResizeImg(XY00,PsModel->srcGray,ImgH,ImgW);
	IMP_GaussImg(PsModel->pstResult->stDRegionSet.pstImgBgGray->pu8Data,XY00, ImgH, ImgW);
	IMP_DResizeImg(XY00,PsModel->bckImg,ImgH,ImgW);
	IMP_DResizeImg(PsModel->pstResult->stDRegionSet.pstImgFgOrg->pu8Data,PsModel->forImg,ImgH,ImgW);
	//Pgray=PsModel->pstResult->stImgInGray.pu8Data;
	//FoImg=PsModel->pstResult->stDRegionSet.pstImgFgOrg->pu8Data;
	Pgray=PsModel->srcGray;
	Pbck=PsModel->bckImg;
	FoImg=PsModel->forImg;
	
	
	//Pbck=PsModel->pstResult->stDRegionSet.pstImgBgGray->pu8Data;
	ImgH=ImgH>>1;
	ImgW=ImgW>>1;
	
	Length1=0;
	Length2=0;
	Wstep=ImgW+ImgW;
	for(height=1;height<ImgH-1;height++)
	{
		//Length=height*ImgW;
		Length1+=ImgW;
		Length2+=Wstep;
		P=Pgray+Length1;
		Pz11=(XY22+Length2);
		Pg11=(Pbck+Length1+ImgW);
		Pz10=(XY21+Length2);
		Pg10=(Pbck+Length1);
		Pz1_1=((XY20+Length2));
		Pg1_1=(Pbck+Length1-ImgW);
		Pz01=((XY12+Length2));
		Pg01=Pbck+Length1+ImgW;
		Pz00=(XY11+Length2);
		Pg00=(Pbck+Length1);
		Pz0_1=((XY10+Length2));
		Pg0_1=(Pbck+Length1-ImgW);
		Pz_11=((XY02+Length2));
		Pg_11=(Pbck+Length1+ImgW);
		Pz_10=((XY01+Length2));
		Pg_10=(Pbck+Length1);
		Pz_1_1=((XY00+Length2));
		Pg_1_1=(Pbck+Length1-ImgW);

		for(width=1;width<ImgW-1;width++)
		{
		    Hstep=width+width;
		    
			Pz11[Hstep]=abs(Pg11[width+1]-P[width]);
			Pz10[Hstep]=abs(Pg10[width+1]-P[width]);
			Pz1_1[Hstep]=abs(Pg1_1[width+1]-P[width]);

			Pz01[Hstep]=abs(Pg01[width]-P[width]);
			Pz00[Hstep]=abs(Pg00[width]-P[width]);
			Pz0_1[Hstep]=abs(Pg0_1[width]-P[width]);
		
			Pz_11[Hstep]=abs(Pg_11[width-1]-P[width]);
			Pz_10[Hstep]=abs(Pg_10[width-1]-P[width]);
			Pz_1_1[Hstep]=abs(Pg_1_1[width-1]-P[width]);
		}
	}
	
	
	
	P=XY22;
	Length1=ImgW+ImgW;
	for(height=1;height<ImgH-1;height++)
	{
	    Wstep=0;
		Wstep+=2;
		P+=Length1;
		for(width=1;width<ImgW-1;width++)
		{
			P[Wstep+1]=(P[Wstep+2]+P[Wstep]+P[Wstep-2]+P[Wstep+Length1+2]+P[Wstep+Length1]
		    +P[Wstep+Length1-2]+P[Wstep-Length1+2]+P[Wstep-Length1]+P[Wstep-Length1-2])>>3;
			Wstep+=2;
		}
	}
	
	P=XY21;
	for(height=1;height<ImgH-1;height++)
	{
	    Wstep=0;
		Wstep+=2;
		P+=Length1;
		
		for(width=1;width<ImgW-1;width++)
		{
			P[Wstep+1]=(P[Wstep+2]+P[Wstep]+P[Wstep-2]+P[Wstep+Length1+2]+P[Wstep+Length1]
			  +P[Wstep+Length1-2]+P[Wstep-Length1+2]+P[Wstep-Length1]+P[Wstep-Length1-2])>>3;
			Wstep+=2;
		}
	}
	
	P=XY20;
	for(height=1;height<ImgH-1;height++)
	{
	    Wstep=0;
		Wstep+=2;
		P+=Length1;
		for(width=1;width<ImgW-1;width++)
		{
			P[Wstep+1]=(P[Wstep+2]+P[Wstep]+P[Wstep-2]+P[Wstep+Length1+2]+P[Wstep+Length1]
			  +P[Wstep+Length1-2]+P[Wstep-Length1+2]+P[Wstep-Length1]+P[Wstep-Length1-2])>>3;
			Wstep+=2;
		}
	}
	
	P=XY12;
	for(height=1;height<ImgH-1;height++)
	{
	    Wstep=0;
		Wstep+=2;
		P+=Length1;
		for(width=1;width<ImgW-1;width++)
		{
			P[Wstep+1]=(P[Wstep+2]+P[Wstep]+P[Wstep-2]+P[Wstep+Length1+2]+P[Wstep+Length1]
			  +P[Wstep+Length1-2]+P[Wstep-Length1+2]+P[Wstep-Length1]+P[Wstep-Length1-2])>>3;
			Wstep+=2;
		}
	}
	
	P=XY11;
	for(height=1;height<ImgH-1;height++)
	{
	    Wstep=0;
		Wstep+=2;
		P+=Length1;
		for(width=1;width<ImgW-1;width++)
		{
			P[Wstep+1]=(P[Wstep+2]+P[Wstep]+P[Wstep-2]+P[Wstep+Length1+2]+P[Wstep+Length1]
			  +P[Wstep+Length1-2]+P[Wstep-Length1+2]+P[Wstep-Length1]+P[Wstep-Length1-2])>>3;
			Wstep+=2;
		}
	}
	
	P=XY10;
	for(height=1;height<ImgH-1;height++)
	{
	    Wstep=0;
		Wstep+=2;
		P+=Length1;
		for(width=1;width<ImgW-1;width++)
		{
			P[Wstep+1]=(P[Wstep+2]+P[Wstep]+P[Wstep-2]+P[Wstep+Length1+2]+P[Wstep+Length1]
			  +P[Wstep+Length1-2]+P[Wstep-Length1+2]+P[Wstep-Length1]+P[Wstep-Length1-2])>>3;
			Wstep+=2;
		}
	}
	
	P=XY02;
	for(height=1;height<ImgH-1;height++)
	{
	    Wstep=0;
		Wstep+=2;
		P+=Length1;
		for(width=1;width<ImgW-1;width++)
		{
			P[Wstep+1]=(P[Wstep+2]+P[Wstep]+P[Wstep-2]+P[Wstep+Length1+2]+P[Wstep+Length1]
			  +P[Wstep+Length1-2]+P[Wstep-Length1+2]+P[Wstep-Length1]+P[Wstep-Length1-2])>>3;
			Wstep+=2;
		}
	}
	
	P=XY01;
	for(height=1;height<ImgH-1;height++)
	{
	    Wstep=0;
		Wstep+=2;
		P+=Length1;
		for(width=1;width<ImgW-1;width++)//P 3
		{
			P[Wstep+1]=(P[Wstep+2]+P[Wstep]+P[Wstep-2]+P[Wstep+Length1+2]+P[Wstep+Length1]
			  +P[Wstep+Length1-2]+P[Wstep-Length1+2]+P[Wstep-Length1]+P[Wstep-Length1-2])>>3;
			Wstep+=2;
		}
	}
	
	P=XY00;
	for(height=1;height<ImgH-1;height++)
	{
	    Wstep=0;
		Wstep+=2;
		P+=Length1;
		for(width=1;width<ImgW-1;width++)
		{
			P[Wstep+1]=(P[Wstep+2]+P[Wstep]+P[Wstep-2]+P[Wstep+Length1+2]+P[Wstep+Length1]
			  +P[Wstep+Length1-2]+P[Wstep-Length1+2]+P[Wstep-Length1]+P[Wstep-Length1-2])>>3;
			Wstep+=2;
		}
	}
	
	
	//gettimeofday (&tvStart,"usec");//delete
	Pzsad=zSadMin+ImgW;
	Pvsad=vSadMin+ImgW;
	Length1=ImgW<<1;
	Length2=Length1;
	for(height=2;height<ImgH-2;height++)
	{
	    Wstep=0;
		Wstep+=2;
		Pzsad+=ImgW;
		Pvsad+=ImgW;
		Length2+=Length1;
		
		for(width=2;width<ImgW-2;width++)
		{
			zminVal=1000;
			vminVal=1000;
		
			Wstep+=2;	
			i=Length2+Wstep+1;
			if(XY22[i]<zminVal)
			{
				zminVal=XY22[i];
			}
			
			if(XY21[i]<zminVal)
			{
				zminVal=XY21[i];
			}
			
			if(XY20[i]<zminVal)
			{
				zminVal=XY20[i];
			}
			
			if(XY12[i]<zminVal)
			{
				zminVal=XY12[i];
			}
			
			if(XY11[i]<zminVal)
			{
				zminVal=XY11[i];
			}
			
			if(XY10[i]<zminVal)
			{
				zminVal=XY10[i];
			}
			
			if(XY02[i]<zminVal)
			{
				zminVal=XY02[i];
			}
			
			if(XY01[i]<zminVal)
			{
				zminVal=XY01[i];
			}
			
			if(XY00[i]<zminVal)
			{
				zminVal=XY00[i];
			}
			Pzsad[width]=zminVal;
			
			
			
			if(XY22[Length2-Length1+Wstep-1]<vminVal)
			{
				vminVal=XY22[Length2-Length1+Wstep-1];
			}
			
			if(XY21[Length2+Wstep-1]<vminVal)
			{
				vminVal=XY21[Length2+Wstep-1];
			}
			
			if(XY20[Length2+Length1+Wstep-1]<vminVal)
			{
				vminVal=XY20[Length2+Length1+Wstep-1];
			}
			
			if(XY12[Length2-Length1+Wstep+1]<vminVal)
			{
				vminVal=XY12[Length2-Length1+Wstep+1];
			}
			
			if(XY11[Length2+Wstep+1]<vminVal)
			{
				vminVal=XY11[Length2+Wstep+1];
			}
			
			if(XY10[Length2+Length1+Wstep+1]<vminVal)
			{
				vminVal=XY10[Length2+Length1+Wstep+1];
			}
			
			if(XY02[Length2-Length1+Wstep+3]<vminVal)
			{
				vminVal=XY02[Length2-Length1+Wstep+3];
			}
			
			if(XY01[Length2+Wstep+3]<vminVal)
			{
				vminVal=XY01[Length2+Wstep+3];
			}
			
			if(XY00[Length2+Length1+Wstep+3]<vminVal)
			{
				vminVal=XY00[Length2+Length1+Wstep+3];
			}
			Pvsad[width]=vminVal;

		}
	}
	
	//gettimeofday (&tvEnd,"usec");//delete
	//linStart = (double)tvStart.tv_usec;//unit S
    //linEnd = (double)tvEnd.tv_usec;//unit S
    //lTime = linEnd-linStart;
    //printf("swing=%3.1lfms  \n",lTime/1000);
	return 1;
}



IMP_S32 IMP_GetMemSizeSwingModel( PEA_RESULT_S *pstResult)
{
	IMP_S32 width = pstResult->s32Width>>1;
	IMP_S32 height = pstResult->s32Height>>1;
	IMP_S32 size =sizeof(IMP_Swing_S)+(31)*height*width; 	
	return size;
}

IMP_MODULE_HANDLE IMP_CreateSwing(PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource)
{
	IMP_MODULE_HANDLE hModule = NULL;
	IMP_Swing_S *pstModule; 
	IMP_S32	length=(pstResult->s32Width>>1)*(pstResult->s32Height>>1);

	pstModule = (IMP_Swing_S*)IMP_MMAlloc( &pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IMP_Swing_S));	

	pstModule->pstResult = pstResult; 
	pstModule->pstHwResource = pstHwResource; 


	pstModule->p00=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,5*length*sizeof(IMP_UCHAR));
	pstModule->p01=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,2*length*sizeof(IMP_UCHAR));
	pstModule->p02=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,2*length*sizeof(IMP_UCHAR));
	pstModule->p10=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,2*length*sizeof(IMP_UCHAR));
	pstModule->p11=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,5*length*sizeof(IMP_UCHAR));
	pstModule->p12=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,2*length*sizeof(IMP_UCHAR));
	pstModule->p20=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,2*length*sizeof(IMP_UCHAR));
	pstModule->p21=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,2*length*sizeof(IMP_UCHAR));
	pstModule->p22=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,2*length*sizeof(IMP_UCHAR));
	pstModule->inMask=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->swImg=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->zSadmin=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->vSadmin=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->srcGray=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->forImg=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->bckImg=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	//pstModule->vSadmin=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));

	while(length--)
	{
		pstModule->forImg[length]=0;
		pstModule->inMask[length]=0;
		pstModule->swImg[length]=0;
	}
	hModule = (IMP_MODULE_HANDLE)pstModule;
	return hModule;
}

IMP_S32 IMP_ProcessSwing(IMP_MODULE_HANDLE hModule)
{
	IMP_Swing_S *pstModule;	
	PEA_RESULT_S *pstResult;

	struct timeval tvStart,tvEnd;//delete
	
	double linStart = 0,linEnd = 0,lTime = 0;//delete
	IMP_S32 val;
	IMP_S32 Step;
	IMP_S32 ImgH;
	IMP_S32 ImgW;
	IMP_S32 width;
	IMP_S32 height;
	IMP_S32 Lenght;
	IMP_S32 LowNum;
	IMP_S32 UpNum;

	IMP_UCHAR * pc1;
	IMP_UCHAR * pc2;
	IMP_UCHAR * pc3;
	IMP_UCHAR * pc4;
	IMP_UCHAR * pc5;
	IMP_UCHAR * pc6;
	
	IMP_UCHAR * p11;
	IMP_UCHAR * p22;

	gettimeofday (&tvStart,"usec");//delete
	pstModule = (IMP_Swing_S*)hModule;
	p11=pstModule->p11;
	p22=pstModule->p22;
	pstResult = pstModule->pstResult;
	ImgH=pstResult->s32Height>>1;
	ImgW=pstResult->s32Width>>1;
	Lenght=ImgW*ImgH;
	
	IMP_GetSadImg1(pstModule);
	IMP_Proporte(pstModule->bckImg,pstModule->srcGray,p11,p22,Lenght);
	
	//将前景的摇摆区域去除
	pc1=(pstModule->p00+ImgW);
	pc2=(pstModule->zSadmin+ImgW);
	pc3=(pstModule->vSadmin+ImgW);
	//pc4=(pstModule->swImg+ImgW)
	pc5=(pstModule->swImg+ImgW);
	pc6=(pstModule->forImg+ImgW);
	Step=0;
	for (height=1;height<ImgH-1;height++)
	{
	
	
		pc1+=Step;
		pc2+=Step;
		pc3+=Step;
		pc5+=Step;
		pc6+=Step;
	    Step=ImgW;
		
		for (width=1;width<ImgW-1;width++)
		{	
			UpNum=0;
			LowNum=0;
				
			UpNum+=(p11+Step-ImgW)[width-1];
			UpNum+=(p11+Step-ImgW)[width];
			UpNum+=(p11+Step-ImgW)[width+1];
			UpNum+=(p11+Step)[width-1];
			UpNum+=(p11+Step)[width+1];
			UpNum+=(p11+Step+ImgW)[width-1];
			UpNum+=(p11+Step+ImgW)[width];
			UpNum+=(p11+Step+ImgW)[width+1];
			LowNum+=(p22+Step-ImgW)[width-1];
			LowNum+=(p22+Step-ImgW)[width];
			LowNum+=(p22+Step-ImgW)[width+1];
			LowNum+=(p22+Step)[width-1];
			LowNum+=(p22+Step)[width+1];
			LowNum+=(p22+Step+ImgW)[width-1];
			LowNum+=(p22+Step+ImgW)[width];
			LowNum+=(p22+Step+ImgW)[width+1];
			
			val=abs(pc2[width]-pc3[width])<6&&abs(LowNum-UpNum)<6;
			
			if (pc2[width]>1&&pc2[width]<12)//2//20
			{
				//val=abs(pc2[width]-pc3[width])<6&&abs(LowNum-UpNum)<6;
				if (val)//6//6
				{
					pc1[width]=1;
				}
				else
				{
					pc1[width]=0;
				}		
			}
			else
			{
				pc1[width]=0;
				//pc6[width]=0;
			}
					
			if(val||pc5[width])//val||pc5[width]
			{
				if(pc2[width]>12&&abs(pc2[width]-pc3[width])<6)
				    pc6[width]=255;
				else
				 	pc6[width]=0;		
			}
			else
			{
				pc6[width]=0;
			}
			
		}
	}
	
	

	IMP_StatiPix(pstModule->p00,pstModule->inMask,ImgH,ImgW);
	IMP_BinaryImg(pstModule->inMask,pstModule->swImg,40,Lenght);
	
	IMP_DePix(pstModule->forImg,ImgH,ImgW,100);
	//IMP_ShowImg(pstModule->forImg,ImgH,ImgW,"hesult2");
	
	//IMP_UResizeImg(pstModule->forImg,pstModule->p00,ImgH,ImgW);
	IMP_UResizeImg(pstModule->swImg,pstModule->p00,ImgH,ImgW);
	IMP_UResizeImg(pstModule->forImg,pstModule->p11,ImgH,ImgW);
	IMP_ShowImg(pstModule->p00,ImgH<<1,ImgW<<1,"Swing");
	IMP_ShowImg(pstModule->p11,ImgH<<1,ImgW<<1,"Deduction");
	ImgH=ImgH<<1;
	ImgW=ImgW<<1;
	pc1=pstModule->p00;
	pc2=pstModule->p11;
	pc3=pstResult->stDRegionSet.pstImgFgOrg->pu8Data;
	Step=ImgH*ImgW;
	
	while(Step--)
	{
		if(pc3[Step])
		{
			if(pc1[Step])
			{
				if(!pc2[Step])
				pc3[Step]=0;
			}
			
		}
	}
	IMP_ShowImg(pc3,ImgH,ImgW,"Result");
	gettimeofday (&tvEnd,"usec");//delete
	linStart = (double)tvStart.tv_usec;//unit S
    linEnd = (double)tvEnd.tv_usec;//unit S
    lTime = linEnd-linStart;
    printf("swing=%3.1lfms  \n",lTime/1000);
	return 1;
}


IMP_S32 IMP_ReleaseSwing(IMP_MODULE_HANDLE hModule)
{
	IMP_Swing_S *pstModule;
	MEM_MGR_ARRAY_S *pstMemMgr;

	pstModule = (IMP_Swing_S*)hModule;
	pstMemMgr = &pstModule->pstHwResource->stMemMgr;

	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->p00);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->p01);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->p02);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->p10);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->p11);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->p12);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->p20);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->p21);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->p22);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->inMask);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->swImg);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->zSadmin);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->vSadmin);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->srcGray);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->forImg);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->bckImg);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule);
	return 1;
}

IMP_S32 IMP_GaussImg(IMP_UCHAR*srcGray,IMP_UCHAR*dst,IMP_S32 ImgH,IMP_S32 ImgW)
{
	IMP_S32 val;
	IMP_S32 width;
	IMP_S32 height;
	//IMP_S32 Step;
	IMP_UCHAR*P1;
	IMP_UCHAR*P2;
	
	if(!srcGray||!dst)
	{
		printf("The input of IMP_GaussImg is wrong!\n");
		return 0;
	}	
	
	P1=srcGray;
	P2=dst;
	for(height=1;height<ImgH-1;height++)
	{
		P1+=ImgW;
		P2+=ImgW;
		for(width=1;width<ImgW-1;width++)
		{
			val=0;
			val=(P1[width+1]<<1)+(P1[width]<<2)+(P1[width-1]<<1)+(P1[width+ImgW-1])+(P1[width+ImgW]<<1)+
			               (P1[width+ImgW+1])+(P1[width-ImgW-1])+(P1[width-ImgW]<<1)+(P1[width-ImgW+1]);
			P2[width]=val>>4;
		}
	}
	return 1;
}

IMP_S32 IMP_DResizeImg(IMP_UCHAR*srcGray,IMP_UCHAR*dst,IMP_S32 ImgH,IMP_S32 ImgW)
{
/*/输入的src图像是已经经过高斯平滑处理的图像/*/
	IMP_S32 width;
	IMP_S32 height;
	IMP_S32 Step;
	IMP_S32 Step1;
	IMP_UCHAR*P1;
	IMP_UCHAR*P2;
	
	if(!srcGray||!dst)
	{
		printf("The input of IMIP_ResizeImg is wrong!\n");
		return 0;
	}	
	
	Step=ImgH>>1;
	Step1=ImgW>>1;
	P1=srcGray;
	P2=dst;
	for(height=0;height<Step;height++)
	{
		P1+=ImgW;
		for(width=0;width<Step1;width++)
		{
			P2[width]=P1[(width<<1)+1];
		}
		P1+=ImgW;
		P2+=Step1;
	}
	return 1;
}

IMP_S32 IMP_UResizeImg(IMP_UCHAR*srcGray,IMP_UCHAR*dst,IMP_S32 ImgH,IMP_S32 ImgW)
{
	/*/对图像上采样，将图像尺寸放大一倍/*/
	IMP_S32 width;
	IMP_S32 height;
	IMP_S32 Step1;
	IMP_S32 Step2;
	IMP_UCHAR*P1;
	IMP_UCHAR*P2;
	
	if(!srcGray||!dst)
	{
		printf("The input of IMP_UresizeImg is wrong!\n");
		return 0;
	}
	
	Step1=ImgW<<1;
	Step2=ImgW<<2;
	P1=srcGray;
	P2=dst;
	for(height=0;height<ImgH;height++)
	{
		for(width=0;width<ImgW;width++)
		{
			P2[width<<1]=P1[width];
			P2[(width<<1)+1]=P1[width];
			P2[Step1+(width<<1)]=P1[width];
			P2[Step1+(width<<1)+1]=P1[width];
		}
		P1+=ImgW;
		P2+=Step2;
	}
	return 1;
}

IMP_S32 IMP_DePix(IMP_UCHAR*srcGray,IMP_S32 ImgH,IMP_S32 ImgW,IMP_S32 val)
{
	IMP_S32 val1;
	IMP_S32 width;
	IMP_S32 height;
	IMP_UCHAR*PC;
	
	
	if(!srcGray)
	{
		printf("The input of IMP_DePix is wrong!\n");
		return 0;
	}
	
	PC=srcGray;
	for(height=1;height<ImgH-1;height++)
	{
		PC+=ImgW;
		for(width=1;width<ImgW-1;width++)
		{
			if(PC[width])
			{
				val1=PC[width-1]+PC[width+1]+PC[width-ImgW-1]+PC[width-ImgW]+PC[width-ImgW+1]
				+PC[width+ImgW-1]+PC[width+ImgW]+PC[width+ImgW+1];
				if(val1<val)
				PC[width]=0;
			}
		}
	}
	return 1;	
}





