

#include "stdio.h"
#include "imp_avd_common.h"
#define IP_AVD_HIST_INCLUDE_0_VALUE 1
#define IP_AVD_HIST_EXCLUDE_0_VALUE 0


IMP_HANDLE AVD_Initialize(IMP_S32 width ,IMP_S32 height)
{
    IMP_HANDLE hImp =  NULL;
	IMP_S32 imgW = width;
	IMP_S32 imgH = height;
    AVD_MODULE *avdModule;
    avdModule = (AVD_MODULE*)malloc(sizeof(AVD_MODULE));
	memset(avdModule,0,sizeof(AVD_MODULE));
	avdModule->imgW = imgW;
	avdModule->imgH = imgH;

    avdModule->flag_change = IMP_TRUE;
	avdModule->IR_change = FALSE;

	IMP_RGBImageCreate(&avdModule->RGBImg,imgW,imgH,NULL);
#ifdef DEBUG_AVD_SHOW_PRINTF
    printf("%d,%d\n",avdModule->RGBImg.s32W,avdModule->RGBImg.s32H);
#endif
	IMP_GrayImageCreate(&avdModule->YImg,imgW,imgH,NULL);
	IMP_GrayImageCreate(&avdModule->UImg,imgW,imgH/4,NULL);
	IMP_GrayImageCreate(&avdModule->VImg,imgW,imgH/4,NULL);

	IMP_GrayImageCreate(&avdModule->YImgLast,imgW,imgH,NULL);
	IMP_GrayImageCreate(&avdModule->UImgLast,imgW,imgH/4,NULL);
	IMP_GrayImageCreate(&avdModule->VImgLast,imgW,imgH/4,NULL);

	IMP_GrayImageCreate(&avdModule->RImg,imgW,imgH,NULL);
	IMP_GrayImageCreate(&avdModule->GImg,imgW,imgH,NULL);
	IMP_GrayImageCreate(&avdModule->BImg,imgW,imgH,NULL);
	IMP_RGBImageCreate(&avdModule->ClrCastImg,imgW,imgH,NULL);

	IMP_GrayImageCreate(&avdModule->EdgeImg,imgW,imgH,NULL);
	IMP_GrayImage16Create(&avdModule->EdgeImgGrad,imgW,imgH,NULL);
	IMP_GrayImageCreate(&avdModule->EdgeImgMag,imgW,imgH,NULL);

	IMP_GrayImageCreate(&avdModule->RefYImg,imgW,imgH,NULL);
	IMP_GrayImageCreate(&avdModule->RefUImg,imgW,imgH/4,NULL);
	IMP_GrayImageCreate(&avdModule->RefVImg,imgW,imgH/4,NULL);

	IMP_GrayImageCreate(&avdModule->YImgDiffNxt,imgW,imgH,NULL);
	IMP_GrayImageCreate(&avdModule->UImgDiffNxt,imgW,imgH/4,NULL);
	IMP_GrayImageCreate(&avdModule->VImgDiffNxt,imgW,imgH/4,NULL);
	IMP_GrayImageCreate(&avdModule->YImgDiffNxt2,imgW,imgH,NULL);

	IMP_GrayImageCreate(&avdModule->GrayImgDiff,imgW,imgH,NULL);
#ifdef DEBUG_AVD_SHOW_FRM_DIFF
	IMP_GrayImageCreate(&avdModule->GrayImgDiff3,imgW,imgH,NULL);
#endif
	IMP_GrayImageCreate(&avdModule->UImgDiff,imgW,imgH/4,NULL);
	IMP_GrayImageCreate(&avdModule->VImgDiff,imgW,imgH/4,NULL);



	avdModule->smthScale = 1;
	avdModule->flag_RefImg = FALSE;

	avdModule->signalValue = 0;
	avdModule->clarityValue = 0;
	avdModule->brightHValue = 0;
	avdModule->brightLValue = 0;
	avdModule->noiseValue = 0;
	avdModule->colorValue = 0;
	avdModule->ptzValue = 0;
	avdModule->freezeValue = 0;

    avdModule->smthScale = 1;

	hImp = (IMP_HANDLE)avdModule;
#ifdef DEBUG_AVD_SHOW_PRINTF
	printf("avdModule = %x\n",avdModule);
	printf("avdModule->YImg = %x DATA = %x, w = %d,h = %d\n",&avdModule->YImg,avdModule->YImg.pu8Data,imgW,imgH);
	printf("avdModule->smthScale = %d\n",avdModule->smthScale);
#endif
	return hImp;

}


void AVD_Terminate(AVD_MODULE* avdModule)
{
	IMP_RGBImageDestroy(&avdModule->RGBImg,NULL);
	IMP_GrayImageDestroy(&avdModule->YImg,NULL);
	IMP_GrayImageDestroy(&avdModule->UImg,NULL);
	IMP_GrayImageDestroy(&avdModule->VImg,NULL);

	IMP_GrayImageDestroy(&avdModule->YImgLast,NULL);
	IMP_GrayImageDestroy(&avdModule->UImgLast,NULL);
	IMP_GrayImageDestroy(&avdModule->VImgLast,NULL);

	IMP_GrayImageDestroy(&avdModule->RImg,NULL);
	IMP_GrayImageDestroy(&avdModule->GImg,NULL);
	IMP_GrayImageDestroy(&avdModule->BImg,NULL);
	IMP_RGBImageDestroy(&avdModule->ClrCastImg,NULL);

	IMP_GrayImageDestroy(&avdModule->EdgeImg,NULL);
	IMP_GrayImage16Destroy(&avdModule->EdgeImgGrad,NULL);
	IMP_GrayImageDestroy(&avdModule->EdgeImgMag,NULL);

	IMP_GrayImageDestroy(&avdModule->RefYImg,NULL);
	IMP_GrayImageDestroy(&avdModule->RefUImg,NULL);
	IMP_GrayImageDestroy(&avdModule->RefVImg,NULL);

	IMP_GrayImageDestroy(&avdModule->YImgDiffNxt,NULL);
	IMP_GrayImageDestroy(&avdModule->UImgDiffNxt,NULL);
	IMP_GrayImageDestroy(&avdModule->VImgDiffNxt,NULL);
	IMP_GrayImageDestroy(&avdModule->YImgDiffNxt2,NULL);

	IMP_GrayImageDestroy(&avdModule->GrayImgDiff,NULL);

	IMP_GrayImageDestroy(&avdModule->UImgDiff,NULL);
	IMP_GrayImageDestroy(&avdModule->VImgDiff,NULL);



	free(avdModule);
}



extern IMP_U8 * pYData;
void AVD_Transform(AVD_MODULE* avdModule,IMP_U32 u32ExtScnChg)
{
 //   pthread_mutex_lock(&mut);
#ifdef TIME_TEST
    //timer
    struct timeval start;
    struct timeval timer1,timer2,timer3,timer4,timer5,timer6;
    struct timeval end;
    IMP_S32 timestart,timeend,timeuse;
    IMP_S32 t1,t2,t3,t4,t5,t6;
    IMP_S32 delta_t1,delta_t2,delta_t3,delta_t4,delta_t5,delta_t6,delta_t7;
#endif

#ifdef TIME_TEST
        gettimeofday(&start,NULL);//start timer
#endif

	IMP_S32 i;
	RGB_IMAGE_S* pRGBImg = &avdModule->RGBImg;
	GRAY_IMAGE_S* pGrayImg = &avdModule->YImg;

	GRAY_IMAGE_S* pYImg = &avdModule->YImg;
	GRAY_IMAGE_S* pUImg = &avdModule->UImg;
	GRAY_IMAGE_S* pVImg = &avdModule->VImg;

	GRAY_IMAGE_S* pYImgLast = &avdModule->YImgLast;								///< 上一帧Y图
	GRAY_IMAGE_S* pUImgLast = &avdModule->UImgLast;								///< 上一帧U图
	GRAY_IMAGE_S* pVImgLast = &avdModule->VImgLast;								///< 上一帧V图

	GRAY_IMAGE_S* pBImg = &avdModule->BImg;
	GRAY_IMAGE_S* pGImg = &avdModule->GImg;
	GRAY_IMAGE_S* pRImg = &avdModule->RImg;

	GRAY_IMAGE_S* pEdgeImg = &avdModule->EdgeImg;
	GRAY_IMAGE16_S* pEdgeImgGrad = &avdModule->EdgeImgGrad;
	GRAY_IMAGE_S* pEdgeImgMag = &avdModule->EdgeImgMag;

	GRAY_IMAGE_S* pRefGrayImg = &avdModule->RefYImg;
	GRAY_IMAGE_S* pRefUImg = &avdModule->RefUImg;
	GRAY_IMAGE_S* pRefVImg = &avdModule->RefVImg;

	GRAY_IMAGE_S* pYImgDiffNxt = &avdModule->YImgDiffNxt;
	GRAY_IMAGE_S* pUImgDiffNxt = &avdModule->UImgDiffNxt;
	GRAY_IMAGE_S* pVImgDiffNxt = &avdModule->VImgDiffNxt;

	GRAY_IMAGE_S* pYImgDiffNxt2 = &avdModule->YImgDiffNxt2;

	GRAY_IMAGE_S* pGrayImgDiff = &avdModule->GrayImgDiff;
#ifdef DEBUG_AVD_SHOW_FRM_DIFF
	GRAY_IMAGE_S* pGrayImgDiff3 = &avdModule->GrayImgDiff3;
#endif
	GRAY_IMAGE_S* pUImgDiff = &avdModule->UImgDiff;
	GRAY_IMAGE_S* pVImgDiff = &avdModule->VImgDiff;

	AVD_HISTOGROUP* pHistGrayImg = &avdModule->histGrayImg;
	AVD_HISTOGROUP* pHistYImgDiffNxt = &avdModule->histYImgDiffNxt;
	AVD_HISTOGROUP* pHistUImgDiffNxt = &avdModule->histUImgDiffNxt;
	AVD_HISTOGROUP* pHistVImgDiffNxt = &avdModule->histVImgDiffNxt;
	AVD_HISTOGROUP* pHistYImgDiffNxt2 = &avdModule->histYImgDiffNxt2;
	AVD_HISTOGROUP* pHistGrayImgDiff = &avdModule->histGrayImgDiff;

#ifdef DEBUG_AVD_SHOW_FRM_DIFF
	AVD_HISTOGROUP* pHistGrayImgDiff3 = &avdModule->histGrayImgDiff3;
#endif
	AVD_HISTOGROUP* pHistEdgeImg = &avdModule->histEdgeImg;

	AVD_HISTOGROUP* pHistBImg = &avdModule->histBImg;
	AVD_HISTOGROUP* pHistGImg = &avdModule->histGImg;
	AVD_HISTOGROUP* pHistRImg = &avdModule->histRImg;

	AVD_HISTOGROUP* pHistUImg = &avdModule->histUImg;
	AVD_HISTOGROUP* pHistVImg = &avdModule->histVImg;
	AVD_HISTOGROUP* pHistUImgDiff = &avdModule->histUImgDiff;
	AVD_HISTOGROUP* pHistVImgDiff = &avdModule->histVImgDiff;

	IMP_U16 img_w = pRGBImg->s32W;
	IMP_U16 img_h = pRGBImg->s32H;
	IMP_S32 smthScale = avdModule->smthScale;

	IMP_S32 freeze_th;

	IMP_U8 *p1,*p2;

//    printf("4.AVD_Transform\n");
//////////////////////////////////////////////////////////////////////////
#ifdef TIME_TEST
        gettimeofday(&timer1,NULL);//timer1
#endif

//////////////////////////////////////////////////////////////////////////

//printf("11111\n");
	if (avdModule->flag_change || u32ExtScnChg)
	{
#ifdef DEBUG_AVD_SHOW_PRINTF
		 printf("flag_change\n");
#endif
		avdModule->flag_change = FALSE;
		avdModule->flag_RefImg = FALSE;
		avdModule->prcFrmCnt = 0;
		if(u32ExtScnChg)
			memset(&avdModule->stAvdUpperAnalysis,0,sizeof(AVD_Upper_ANALYSIS_S));
#ifdef DEBUG_AVD_SHOW_PRINTF
		printf("*************Scene Change!!!*************\n");
#endif
	}
//printf("22222\n");
	avdModule->prcFrmCnt++;


	if (!avdModule->flag_RefImg)
	{

        IMP_GrayImageClone(pYImg,pRefGrayImg);
        IMP_GrayImageClone(pUImg,pRefUImg);
        IMP_GrayImageClone(pVImg,pRefVImg);

		avdModule->flag_RefImg = TRUE;
	}
//printf("33333\n");

#ifdef TIME_TEST
        gettimeofday(&timer2,NULL);//timer2
#endif
	//////////////////////////////////////////////////////////////////////////

	//printf("pBImg->pu8Data=%d,pBImg->pu8Data=D,pBImg->pu8Data=%d,img_w = %d,img_h = %d");
	ipSplit( pRGBImg->pu8Data, pBImg->pu8Data, pGImg->pu8Data, pRImg->pu8Data, img_w, img_h );

//printf("44444\n");
  //  pYData = pGImg->pu8Data;


	ipGray2RGB( pGrayImg->pu8Data, pRGBImg->pu8Data, img_w, img_h );
//printf("55555\n");

//	cvSmooth(pGrayImg,pGrayImgSmth,CV_GAUSSIAN,smthScale*2+1,0,0,0);

#ifdef TIME_TEST
        gettimeofday(&timer3,NULL);//timer3
#endif
	//////////////////////////////////////////////////////////////////////////

	avd_canny_process( pGrayImg->pu8Data,
					  ( IMP_S8*)(pEdgeImgGrad->ps16Data),
					   pEdgeImgMag->pu8Data,
					   pEdgeImg->pu8Data,
					   0, 0, img_w, img_h, img_w, img_h,
					   0 );

#ifdef TIME_TEST
        gettimeofday(&timer4,NULL);//timer1
#endif

	ipImageSub(	pGrayImg->pu8Data, img_w,
				pRefGrayImg->pu8Data, img_w,
				pGrayImgDiff->pu8Data, img_w,
				img_w, img_h);


	ipImageSub(	pGrayImg->pu8Data, img_w,
				pYImgLast->pu8Data, img_w,
				pYImgDiffNxt->pu8Data, img_w,
				img_w, img_h);



	ipImageThreshold (	pGrayImgDiff->pu8Data, img_w,
						pGrayImgDiff->pu8Data, img_w,
						img_w, img_h, 30, 255, 0, 0);  //pGrayImgDiff   For Change T=30


	if(avdModule->Para.stFreezePara.u32AlarmLevel == LEVEL5)
	{
	    freeze_th = 15;
	}
	else if(avdModule->Para.stFreezePara.u32AlarmLevel == USER_LEVEL)
	{
		freeze_th = 15;
	}
	else
	{
        freeze_th = (avdModule->Para.stFreezePara.u32AlarmLevel-1)*2;
    }

	ipImageThreshold (	pYImgDiffNxt->pu8Data, img_w,
						pYImgDiffNxt2->pu8Data, img_w,
						img_w, img_h, freeze_th, 255, 0, 0);   //pYImgDiffNxt2  For Freeze T=1

	ipImageThreshold (	pYImgDiffNxt->pu8Data, img_w,
						pYImgDiffNxt->pu8Data, img_w,
						img_w, img_h, 15, 255, 0, 0);  //pYImgDiffNxt   For Noise T=15



#ifdef DEBUG_AVD_SHOW_FRM_DIFF
	ipImageSub(	pYImgDiffNxt->pu8Data, img_w,
				pYImgDiffNxt2->pu8Data, img_w,
				pGrayImgDiff3->pu8Data, img_w,
				img_w, img_h);
#endif


    //pYData = pRefGrayImg->pu8Data;

#ifdef TIME_TEST
        gettimeofday(&timer5,NULL);//timer5
#endif

	ipImageSub(	pRefUImg->pu8Data, img_w,
				pUImg->pu8Data, img_w,
				pUImgDiff->pu8Data, img_w,
				img_w, img_h/4);

	ipImageSub(	pRefVImg->pu8Data, img_w,
				pVImg->pu8Data, img_w,
				pVImgDiff->pu8Data, img_w,
				img_w, img_h/4);


	ipImageSub(	pUImg->pu8Data, img_w,
				pUImgLast->pu8Data, img_w,
				pUImgDiffNxt->pu8Data, img_w,
				img_w, img_h/4);

	ipImageSub(	pVImg->pu8Data, img_w,
				pVImgLast->pu8Data, img_w,
				pVImgDiffNxt->pu8Data, img_w,
				img_w, img_h/4);


	ipImageThreshold (	pUImgDiff->pu8Data, img_w,
						pUImgDiff->pu8Data, img_w,
						img_w, img_h/4, 20, 255, 0, 0);

	ipImageThreshold (	pVImgDiff->pu8Data, img_w,
						pVImgDiff->pu8Data, img_w,
						img_w, img_h/4, 20, 255, 0, 0);

	ipImageThreshold (	pUImgDiffNxt->pu8Data, img_w,
						pUImgDiffNxt->pu8Data, img_w,
						img_w, img_h/4, 20, 255, 0, 0);

	ipImageThreshold (	pVImgDiffNxt->pu8Data, img_w,
						pVImgDiffNxt->pu8Data, img_w,
						img_w, img_h/4, 20, 255, 0, 0);


 //   pYData = pGrayImgDiff->pu8Data;
#ifdef TIME_TEST
        gettimeofday(&timer6,NULL);//timer6
#endif


#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("Gray:\n");
#endif
	ipCalHist2(pGrayImg,pHistGrayImg,IP_AVD_HIST_INCLUDE_0_VALUE);

#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("Edge:\n");
#endif
 	ipCalHist2(pEdgeImg,pHistEdgeImg,IP_AVD_HIST_EXCLUDE_0_VALUE);
// 	ipCalHist(pGrayImgSmthEdge,pHistGrayImgSmthEdge,IP_AVD_HIST_EXCLUDE_0_VALUE);

#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("Diff1:\n");
#endif
	ipCalHist2(pGrayImgDiff,pHistGrayImgDiff,IP_AVD_HIST_INCLUDE_0_VALUE);


#ifdef DEBUG_AVD_SHOW_FRM_DIFF
	ipCalHist2(pGrayImgDiff3,pHistGrayImgDiff3,IP_AVD_HIST_INCLUDE_0_VALUE);
#endif

#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("Red:\n");
#endif
	ipCalHist2(pRImg,pHistRImg,IP_AVD_HIST_INCLUDE_0_VALUE);
#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("Green:\n");
#endif
	ipCalHist2(pGImg,pHistGImg,IP_AVD_HIST_INCLUDE_0_VALUE);
#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("Blue:\n");
#endif
	ipCalHist2(pBImg,pHistBImg,IP_AVD_HIST_INCLUDE_0_VALUE);

#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("U:\n");
#endif
	ipCalHist2(pUImg,pHistUImg,IP_AVD_HIST_INCLUDE_0_VALUE);

#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("V:\n");
#endif
	ipCalHist2(pVImg,pHistVImg,IP_AVD_HIST_INCLUDE_0_VALUE);

#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("U Diff:\n");
#endif
	ipCalHist2(pUImgDiff,pHistUImgDiff,IP_AVD_HIST_INCLUDE_0_VALUE);

#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("V Diff:\n");
#endif
	ipCalHist2(pVImgDiff,pHistVImgDiff,IP_AVD_HIST_INCLUDE_0_VALUE);


#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("Y Diff Nxt:\n");
#endif


	ipCalHist2(pYImgDiffNxt,pHistYImgDiffNxt,IP_AVD_HIST_INCLUDE_0_VALUE);

#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("U Diff Nxt:\n");
#endif
	ipCalHist2(pUImgDiffNxt,pHistUImgDiffNxt,IP_AVD_HIST_INCLUDE_0_VALUE);

#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("V Diff Nxt:\n");
#endif
	ipCalHist2(pVImgDiffNxt,pHistVImgDiffNxt,IP_AVD_HIST_INCLUDE_0_VALUE);

#ifdef SHOW_DEBUG_HISTOGRAM_INFO
	printf("Y Diff Nxt2:\n");
#endif
	ipCalHist2(pYImgDiffNxt2,pHistYImgDiffNxt2,IP_AVD_HIST_INCLUDE_0_VALUE);

#ifdef TIME_TEST
        gettimeofday(&end,NULL);
        timestart=1000000*start.tv_sec+start.tv_usec;
        t1=1000000*timer1.tv_sec+timer1.tv_usec;
        t2=1000000*timer2.tv_sec+timer2.tv_usec;
        t3=1000000*timer3.tv_sec+timer3.tv_usec;
        t4=1000000*timer4.tv_sec+timer4.tv_usec;
        t5=1000000*timer5.tv_sec+timer5.tv_usec;
        t6=1000000*timer6.tv_sec+timer6.tv_usec;
        timeend=1000000*end.tv_sec+end.tv_usec;
        timeuse=timeend-timestart;
        delta_t1=t1-timestart;
        delta_t2=t2-t1;
        delta_t3=t3-t2;
        delta_t4=t4-t3;
        delta_t5=t5-t4;
        delta_t6=t6-t5;
        delta_t7=timeend-t6;
        printf("AVD_Transform function:time used:%d us\n",timeuse);
        printf("delta t1:%d us\n",delta_t1);
        printf("delta t2:%d us\n",delta_t2);
        printf("delta t3:%d us\n",delta_t3);
        printf("delta t4:%d us\n",delta_t4);
        printf("delta t5:%d us\n",delta_t5);
        printf("delta t6:%d us\n",delta_t6);
        printf("delta t7:%d us\n",delta_t7);
#endif


}



void ipCalHist2(GRAY_IMAGE_S* pImg, AVD_HISTOGROUP* pHistGroup, IMP_S32 flag_0)
{
	IMP_S32 i,j;
	IMP_S32 imgW = pImg->s32W;
	IMP_S32 imgH = pImg->s32H;
	IMP_S32 pixelCnt = imgH*imgW;

	IMP_U8* dat;
	IMP_U8 val;

	IMP_S32 sumDev;

	IpHistogram* histAll = &pHistGroup->histAll;
	IMP_S32* meanAll = &pHistGroup->meanAll;
	IMP_S32* stdDevAll = &pHistGroup->stdDevAll;

 	memset(pHistGroup,0,sizeof(AVD_HISTOGROUP));
	sumDev = 0;

	dat = pImg->pu8Data;
	for (j=0;j<imgH;j++)
	{
		for (i=0;i<imgW;i++)
		{
			val = dat[i];

			if(flag_0 || val)
			{
				histAll->bin[val>>IP_HIST_LEVEL_B]++;
				histAll->volume++;
 			}
			meanAll[0] += (IMP_S32)val;
		}
		dat += imgW;
	}


	histAll->max = histAll->bin[0];
	for (j=1;j<IP_HIST_LEVEL;j++)
	{
		if (histAll->bin[j] > histAll->max)
		{
			histAll->max = histAll->bin[j];
		}
	}



	meanAll[0] /= pixelCnt;

#ifdef SHOW_DEBUG_HISTOGRAM_INFO

	printf("Mean: ");
	for (i=0;i<IP_BLOCK_CNT;i++)
	{
		printf("%d ",meanPart[i]);
	}
	printf("- %d\n",meanAll[0]);

#endif


	dat = pImg->pu8Data;
	for (j=0;j<imgH;j++)
	{
		for (i=0;i<imgW;i++)
		{
			stdDevAll[0] += abs(dat[i]-meanAll[0]);
		}
		dat += imgW;
	}

	stdDevAll[0] /= pixelCnt;

#ifdef SHOW_DEBUG_HISTOGRAM_INFO

	printf("StdDev: ");
	for (i=0;i<IP_BLOCK_CNT;i++)
	{
		printf("%d ",stdDevPart[i]);
	}
	printf("- %d\n",stdDevAll[0]);
	printf("\n");
#endif
}



void AVD_rgb2yuv444( IMP_U8 *rgbDat, IMP_U8 *yDat, IMP_U8 *uDat, IMP_U8 *vDat, IMP_U16 width, IMP_U16 height)
{
	IMP_FLOAT y,u,v;
	IMP_U16 i,j;
	IMP_U8* rgb_d = rgbDat;
	IMP_U8* y_d = yDat;
	IMP_U8* u_d = uDat;
	IMP_U8* v_d = vDat;

	IMP_U8 tr,tg,tb;
	IMP_U16 w = width;
	IMP_U16 h = height;

	// There was a Bug before;
	// The Bug has been corrected;

	for(j=0;j<h;j++)
	{
		for(i=0;i<w;i++)
		{
			tb=rgb_d[i*3];
			tg=rgb_d[i*3+1];
			tr=rgb_d[i*3+2];
			y=0.257*tr+0.504*tg+0.098*tb+16;
			u=-0.148*tr-0.291*tg+0.439*tb+128;
			v=0.439*tr-0.368*tg-0.071*tb+128;
			if( yDat )
				y_d[i]=y<16?16:(y>235?235:y);
			if( uDat )
				u_d[i]=u<16?16:(u>240?240:u);
			if( vDat )
				v_d[i]=v<16?16:(v>240?240:v);
		}
		rgb_d+=w*3;
		y_d += w;
		u_d += w;
		v_d += w;
	}
}


IMP_S32 AVD_2ThTrans(IMP_S32 thL, IMP_S32 thH, IMP_S32 input)
{
	int y1 = 30;
	int y2 = 60;
	int a = thL;
	int b = thH;
	int x = input;
	int y;

	y = (int)((float)((y2-y1)*x+y1*b-y2*a)/(b-a+0.00001f));

// 	if( input < thL ) input = 0;
// 	else if(input > thH ) input = 100;
// 	else input = (input - thL)*100/(thH - thL);  //线性拉伸

	return y;
}



void ipSplit( IMP_U8 *rgbDat, IMP_U8 *bDat, IMP_U8 *gDat, IMP_U8 *rDat, IMP_U16 width, IMP_U16 height)
{
	IMP_S32 i,j;
	IMP_S32 w = width;
	IMP_S32 h = height;
//	IMP_U8 tmp;
    IMP_U8 *rgbDatTmp = rgbDat;
    IMP_U8 *bDatTmp = bDat;
    IMP_U8 *gDatTmp = gDat;
    IMP_U8 *rDatTmp = rDat;
#ifdef DEBUG_AVD_SHOW_PRINTF
    printf("width = %d,height= %d\n",width,height);
#endif
	for(j=0;j<h;j++)
	{
		for(i=0;i<w;i++)
		{
			//if( bDatTmp )
				bDatTmp[i]=rgbDatTmp[i*3];
			//	printf("%d",bDatTmp[i]);
			//	bDatTmp[i] =255;
			//if( gDatTmp )
				gDatTmp[i]=rgbDatTmp[i*3+1];
            //    gDatTmp[i] =128;
			//if( rDatTmp )
				rDatTmp[i]=rgbDatTmp[i*3+2];
		}
		rgbDatTmp+=w*3;
		bDatTmp += w;
		gDatTmp += w;
		rDatTmp += w;
	}
	//flip
/*	*bDatTmp = bDat;
	for(j = 0; j< w * h / 2;j++)
	{
        tmp = *bDatTmp;
        *bDatTmp = *(bDatTmp + w * h - j - 1) ;
        *(bDatTmp + w * h - j - 1) = tmp;
	}*/
}


void ipGray2RGB( IMP_U8 *grayDat, IMP_U8 *rgbDat, IMP_U16 width, IMP_U16 height)
{
	IMP_S32 i,j;
	IMP_S32 w = width;
	IMP_S32 h = height;
// There is BuGug;
	for(j=0;j<h;j++)
	{
		for(i=0;i<w;i++)
		{
			rgbDat[i*3]=grayDat[i];
			rgbDat[i*3+1]=grayDat[i];
			rgbDat[i*3+2]=grayDat[i];
		}
		rgbDat+=w*3;
		grayDat+=w;
	}
}
