
#include "imp_swing.h"
#include "time.h"
#include "stdlib.h"


IMP_S32 IMP_ShowImg(IMP_UCHAR*srcGray,IMP_S32 Height,IMP_S32 Width,IMP_UCHAR*widoName)
{
	IMP_S32 width;
	IMP_S32 height;
	IMP_UCHAR *P1;
	IMP_UCHAR *P2;
	IplImage*src=cvCreateImage(cvSize(Width,Height),IPL_DEPTH_8U,1);
	
	if(!srcGray)
	{
	  printf("The input of IMP_ShowImg is wrong!\n");
	  if(!src)
	    cvReleaseImage(&src);
	  return 0;
	}
	
	
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
//	cvWaitKey(1);
	cvReleaseImage(&src);
	return 1;
}

IMP_S32 IMP_BinaryImg(IMP_UCHAR *srcGray,IMP_UCHAR *BinMask,IMP_S32 val,IMP_S32 Lenght)
{
	IMP_S32 lenght;
	if (!srcGray||!BinMask)
	{
		printf("The input of IMP_BinaryImg is wrong!\n");
		return 0;
	}

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
IMP_S32 IMP_StatiPix(IMP_UCHAR*srcGray,IMP_UCHAR*statiImg,IMP_S32 ImgH,IMP_S32 ImgW)
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
	IMP_S32 OffNeg[9]={-ImgW-1,-ImgW,-ImgW+1, -1 ,0 ,1, ImgW-1, ImgW,-ImgW+1};
	if (!srcGray||!statiImg)
	{
		printf("The input of IMP_StatiPix is wrong!\n");
		return 0;
	}

	for(height=1;height<ImgH-1;height++)
	{
	  P1=srcGray+height*ImgW;
	  P2=statiImg+height*ImgW;
	  for(width=1;width<ImgW-1;width++)
	  {
	    Num=0;
	    Num+=(P1[width-ImgW-1]>0)?1:0;
        Num+=(P1[width-ImgW]>0)?1:0;
	    Num+=(P1[width-ImgW+1]>0)?1:0;
        Num+=(P1[width-1]>0)?1:0;
        Num+=(P1[width]>0)?1:0;
	    Num+=(P1[width+1]>0)?1:0;
	    Num+=(P1[width+ImgW-1]>0)?1:0;
        Num+=(P1[width+ImgW]>0)?1:0;
	    Num+=(P1[width+ImgW+1]>0)?1:0;
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
	//if (!backGray||!srcGray||!proporMat)
	//{
	//	printf("The input ImgPrortion is wrong!\n");
	//	return 0;
	//}

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

//IMP_S32 IMP_RectSum(IMP_UCHAR*srcGray,IMP_RECT_S widow,IMP_S32 Height,IMP_S32 Width)
//{/*得到图像相应方形的像素和，Height和Width是图像的宽和高*/
//	IMP_UCHAR*p;
//	IMP_S32 width;
//	IMP_S32 widS;
//	IMP_S32 widE;
//	IMP_S32 heiS;
//	IMP_S32 heiE;
//	IMP_S32 heightL;
//	IMP_S32 height;
//	IMP_S32 Ret=0;
	//if (!srcGray)
	//{
	//	printf("The input of IMP_RectSum is wrong!\n");
	//}
//	widS=widow.s16X1;
//	widE=widow.s16X2;
//    heiS=widow.s16Y1;
//	heiE=widow.s16Y2;
//	p=srcGray+heiS*Width;
	//for (height=heiS;height<heiE;height++)
//	{
	//	for (width=widS;width<widE;width++)
		///{
	//		Ret+=p[width];
		//}
		//p=p+Width;
	//}
	//return Ret;
//}

IMP_S32 IMP_RectSum(IMP_UCHAR*srcGray,IMP_RECT_S widow,IMP_S32 Height,IMP_S32 Width)
{/*得到图像相应方形的像素和，Height和Width是图像的宽和高*/
	IMP_UCHAR*p;
	IMP_S32 width;
	IMP_S32 height;
	IMP_S32 Ret=0;
	
	for (height=widow.s16Y1;height<widow.s16Y2;height++)
	{
		p=srcGray+height*Width;
		for (width=widow.s16X1;width<widow.s16X2;width++)
		{
			Ret+=p[width];
		}
	}
	return Ret;
}

IMP_S32 IMP_GetSadImg(IMP_Swing_S*PsModel)
{
	//struct timeval tvStart,tvEnd;//delete
	//double linStart = 0,linEnd = 0,lTime = 0;//delete
	IMP_S32 i;
	IMP_S32 j;
	IMP_S32 ImgH;
	IMP_S32 ImgW;
	IMP_S32 Length;
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
	
	IMP_RECT_S rect;


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
	Pgray=PsModel->pstResult->stImgInGray.pu8Data;
	FoImg=PsModel->pstResult->stDRegionSet.pstImgFgOrg->pu8Data;
	Pbck=PsModel->pstResult->stDRegionSet.pstImgBgGray->pu8Data;
	Length=ImgH*ImgW;
	IMP_ShowImg( Pbck,ImgH,ImgW,"dd");

	if(!PsModel)
	{
		printf("The inPut of GetSadImg is wrong!\n");
		return 0;
	}
	
	
	for(height=1;height<ImgH-1;height++)
	{
		Length=height*ImgW;
		P=Pgray+Length;
		Pz11=(XY22+Length);
		Pg11=(Pbck+Length+ImgW);
		Pz10=(XY21+Length);
		Pg10=(Pbck+Length);
		Pz1_1=((XY20+Length));
		Pg1_1=(Pbck+Length-ImgW);
		Pz01=((XY12+Length));
		Pg01=Pbck+Length+ImgW;
		Pz00=(XY11+Length);
		Pg00=(Pbck+Length);
		Pz0_1=((XY10+Length));
		Pg0_1=(Pbck+Length-ImgW);
		Pz_11=((XY02+Length));
		Pg_11=(Pbck+Length+ImgW);
		Pz_10=((XY01+Length));
		Pg_10=(Pbck+Length);
		Pz_1_1=((XY00+Length));
		Pg_1_1=(Pbck+Length-ImgW);

		for(width=1;width<ImgW-1;width++)
		{
			Pz11[width]=abs(Pg11[width+1]-P[width]);
			Pz10[width]=abs(Pg10[width+1]-P[width]);
			Pz1_1[width]=abs(Pg1_1[width+1]-P[width]);

			Pz01[width]=abs(Pg01[width]-P[width]);
			Pz00[width]=abs(Pg00[width]-P[width]);
			Pz0_1[width]=abs(Pg0_1[width]-P[width]);
		
			Pz_11[width]=abs(Pg_11[width-1]-P[width]);
			Pz_10[width]=abs(Pg_10[width-1]-P[width]);
			Pz_1_1[width]=abs(Pg_1_1[width-1]-P[width]);
		}
	}
	
    //gettimeofday (&tvStart,"usec");//delete
	for(height=2;height<ImgH-2;height++)
	{	
		Pzsad=(zSadMin+height*ImgW);
		Pvsad=(vSadMin+height*ImgW);
		for (width=2;width<ImgW-2;width++)
		{
			vminVal=1000;
			zminVal=1000;

			rect.s16X1=width-1;
			rect.s16X2=width+2;
			rect.s16Y1=height-1;
			rect.s16Y2=height+2;
			i=IMP_RectSum(XY22,rect,ImgH,ImgW)/9;
			rect.s16X1=width-2;
			rect.s16X2=width+1;
			rect.s16Y1=height-2;
			rect.s16Y2=height+1;
			j=IMP_RectSum(XY22,rect,ImgH,ImgW)/9;
			if (i<zminVal)
				zminVal=i;
			if (j<vminVal)
				vminVal=j;

			rect.s16X1=width-1;
			rect.s16X2=width+2;
			rect.s16Y1=height-1;
			rect.s16Y2=height+2;
			i=IMP_RectSum(XY21,rect,ImgH,ImgW)/9;
			rect.s16X1=width-2;
			rect.s16X2=width+1;
			rect.s16Y1=height-1;
			rect.s16Y2=height+2;
			j=IMP_RectSum(XY21,rect,ImgH,ImgW)/9;
			if (i<zminVal)
				zminVal=i;
			if (j<vminVal)
				vminVal=j;

			rect.s16X1=width-1;
			rect.s16X2=width+2;
			rect.s16Y1=height-1;
			rect.s16Y2=height+2;
			i=IMP_RectSum(XY20,rect,ImgH,ImgW)/9;
			rect.s16X1=width-2;
			rect.s16X2=width+1;
			rect.s16Y1=height;
			rect.s16Y2=height+3;
			j=IMP_RectSum(XY20,rect,ImgH,ImgW)/9;
			if (i<zminVal)
				zminVal=i;
			if (j<vminVal)
				vminVal=j;


			rect.s16X1=width-1;
			rect.s16X2=width+2;
			rect.s16Y1=height-1;
			rect.s16Y2=height+2;
			i=IMP_RectSum(XY12,rect,ImgH,ImgW)/9;
			rect.s16X1=width-1;
			rect.s16X2=width+2;
			rect.s16Y1=height-2;
			rect.s16Y2=height+1;
			j=IMP_RectSum(XY12,rect,ImgH,ImgW)/9;
			if (i<zminVal)
				zminVal=i;
			if (j<vminVal)
				vminVal=j;

			rect.s16X1=width-1;
			rect.s16X2=width+2;
			rect.s16Y1=height-1;
			rect.s16Y2=height+2;
			i=IMP_RectSum(XY11,rect,ImgH,ImgW)/9;
			j=IMP_RectSum(XY11,rect,ImgH,ImgW)/9;
			if (i<zminVal)
				zminVal=i;
			if (j<vminVal)
				vminVal=j;

			rect.s16X1=width-1;
			rect.s16X2=width+2;
			rect.s16Y1=height-1;
			rect.s16Y2=height+2;
			i=IMP_RectSum(XY10,rect,ImgH,ImgW)/9;
			rect.s16X1=width-1;
			rect.s16X2=width+2;
			rect.s16Y1=height;
			rect.s16Y2=height+3;
			j=IMP_RectSum(XY10,rect,ImgH,ImgW)/9;
			if (i<zminVal)
				zminVal=i;
			if (j<vminVal)
				vminVal=j;

			rect.s16X1=width-1;
			rect.s16X2=width+2;
			rect.s16Y1=height-1;
			rect.s16Y2=height+2;
			i=IMP_RectSum(XY02,rect,ImgH,ImgW)/9;
			rect.s16X1=width;
			rect.s16X2=width+3;
			rect.s16Y1=height-2;
			rect.s16Y2=height+1;
			j=IMP_RectSum(XY02,rect,ImgH,ImgW)/9;
			if (i<zminVal)
				zminVal=i;
			if (j<vminVal)
				vminVal=j;

			rect.s16X1=width-1;
			rect.s16X2=width+2;
			rect.s16Y1=height-1;
			rect.s16Y2=height+2;
			i=IMP_RectSum(XY01,rect,ImgH,ImgW)/9;
			rect.s16X1=width;
			rect.s16X2=width+3;
			rect.s16Y1=height-1;
			rect.s16Y2=height+2;
			j=IMP_RectSum(XY01,rect,ImgH,ImgW)/9;
			if (i<zminVal)
				zminVal=i;
			if (j<vminVal)
				vminVal=j;

			rect.s16X1=width-1;
			rect.s16X2=width+2;
			rect.s16Y1=height-1;
			rect.s16Y2=height+2;
			i=IMP_RectSum(XY11,rect,ImgH,ImgW)/9;
			rect.s16X1=width;
			rect.s16X2=width+3;
			rect.s16Y1=height;
			rect.s16Y2=height+3;
			j=IMP_RectSum(XY11,rect,ImgH,ImgW)/9;
			if (i<zminVal)
				zminVal=i;
			if (j<vminVal)
				vminVal=j;

			Pzsad[width]=zminVal;
			Pvsad[width]=vminVal;
		}
	}
	
	//gettimeofday (&tvEnd,"usec");//delete
	//linStart = (double)tvStart.tv_usec;  //unit S
   // linEnd = (double)tvEnd.tv_usec;        //unit S
   // lTime = linEnd-linStart;
   // printf("swing=%3.1lfms  \n",lTime/1000);
	
	return 1;
}

IMP_S32 IMP_GetMemSizeSwingModel( PEA_RESULT_S *pstResult)
{
	IMP_S32 width = pstResult->s32Width;
	IMP_S32 height = pstResult->s32Height;
	IMP_S32 size =sizeof(IMP_Swing_S)+(14)*height*width; 	
	return size;
}
IMP_MODULE_HANDLE IMP_CreateSwing(PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource)
{
	IMP_MODULE_HANDLE hModule = NULL;
	IMP_Swing_S *pstModule; 
	IMP_S32	length=pstResult->s32Width*pstResult->s32Height;

	pstModule = (IMP_Swing_S*)IMP_MMAlloc( &pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IMP_Swing_S));	

	pstModule->pstResult = pstResult; 
	pstModule->pstHwResource = pstHwResource; 


	pstModule->p00=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->p01=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->p02=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->p10=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->p11=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->p12=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->p20=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->p21=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->p22=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->inMask=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->swImg=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->zSadmin=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));
	pstModule->vSadmin=(IMP_UCHAR *)IMP_MMAlloc(&pstHwResource->stMemMgr, IMP_MEMBLK_TYPE_SLOW,length*sizeof(IMP_UCHAR));

	while(length--)
	{
		pstModule->inMask[length]=0;
		pstModule->swImg[length]=0;
	}
	hModule = (IMP_MODULE_HANDLE)pstModule;
	return hModule;
}

IMP_S32 IMP_ProcessSwing(IMP_MODULE_HANDLE hModule)
{
	struct timeval tvStart,tvEnd;//delete
	double linStart = 0,linEnd = 0,lTime = 0;//delete

	IMP_Swing_S *pstModule;	
	PEA_RESULT_S *pstResult;

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
	IMP_UCHAR * pc5;
	IMP_UCHAR * pc6;

	IMP_UCHAR * p00;
	IMP_UCHAR * p01;
	IMP_UCHAR * p02;
	IMP_UCHAR * p10;
	IMP_UCHAR * p11;
	IMP_UCHAR * p12;
	IMP_UCHAR * p20;
	IMP_UCHAR * p21;
	IMP_UCHAR * p22;
	IMP_UCHAR * srcGray;
	IMP_UCHAR * bckGray;
	IMP_UCHAR * forGray;
	IMP_UCHAR * zSadmin;
	IMP_UCHAR * vSadmin;
	IMP_UCHAR * inMask;
	IMP_UCHAR * swImg;

	
	pstModule = (IMP_Swing_S*)hModule;
	p00=pstModule->p00;
	p01=pstModule->p01;
	p02=pstModule->p02;
	p10=pstModule->p10;
	p11=pstModule->p11;
	p12=pstModule->p12;
	p22=pstModule->p22;
	//pff=pstModule->pff;
	swImg=pstModule->swImg;
	inMask=pstModule->inMask;
	zSadmin=pstModule->zSadmin;
	vSadmin=pstModule->vSadmin;
	pstResult = pstModule->pstResult;
	ImgH=pstResult->s32Height;
	ImgW=pstResult->s32Width;
	Lenght=ImgW*ImgH;
	srcGray=pstResult->stImgInGray.pu8Data;
	//bckGray=pstModule->bkImg;
	bckGray=pstResult->stDRegionSet.pstImgBgGray->pu8Data;
	forGray=pstResult->stDRegionSet.pstImgFgOrg->pu8Data;
	gettimeofday (&tvStart,"usec");//delete
	//IMP_Proporte(bckGray,srcGray,p11,Lenght);
	IMP_GetSadImg(pstModule);
	IMP_Proporte(bckGray,srcGray,p11,p22,Lenght);
	
	//将前景的摇摆区域去除
	for (height=1;height<ImgH-1;height++)
	{
	    Step=height*ImgW;
		pc1=(p00+Step);
		pc2=(zSadmin+Step);
		pc3=(vSadmin+Step);
		pc5=(swImg+Step);
		pc6=(forGray+Step);
		
		
		
		for (width=1;width<ImgW-1;width++)
		{	
			LowNum=0;
			UpNum=0;
			
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
			//printf("\n %d %d \n",UpNum,LowNum);
			if (pc2[width]>2&&pc2[width]<20)
			{
				if (abs(pc2[width]-pc3[width])<6&&abs(LowNum-UpNum)<6)
					pc1[width]=255;
				else
					pc1[width]=0;
			}
			else
			{
				pc1[width]=0;
			}		

		//	if (abs(LowNum-UpNum)>=6)
		//	{
		//		pc1[width]=0;
		//	}

			if(pc6[width])
			{
				if (pc5[width]!=0&&pc2[width]<25)//&&pc2[width]<30
				{
					pc6[width]=0;
				}
			}
			
		}
		//IMP_ShowImg(p00,ImgH,ImgW,"midmid");
	}
	
	gettimeofday (&tvEnd,"usec");//delete
	linStart = (double)tvStart.tv_usec;//unit S
    linEnd = (double)tvEnd.tv_usec;//unit S
    lTime = linEnd-linStart;
    printf("swing=%3.1lfms  \n",lTime/1000);


	IMP_StatiPix(p00,inMask,ImgH,ImgW);
	IMP_BinaryImg(inMask,swImg,40,Lenght);
	IMP_ShowImg(forGray,ImgH,ImgW,"result");
	IMP_ShowImg(bckGray,ImgH,ImgW,"HJBK");
	IMP_ShowImg(swImg,ImgH,ImgW,"swImg");

	return 1;
}

IMP_S32 IMP_ReleaseSwing(IMP_MODULE_HANDLE hModule)
{
	IMP_Swing_S *pstModule;
	MEM_MGR_ARRAY_S *pstMemMgr;

	pstModule = (IMP_Swing_S*)hModule;
	pstMemMgr = &pstModule->pstHwResource->stMemMgr;

	//IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->pstOutput);
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
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule);
	return 1;
}








