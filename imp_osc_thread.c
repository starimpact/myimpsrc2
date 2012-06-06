#include "imp_osc_thread.h"
//#define printf(...)
static void IMP_HiImageConvertToYUV422Image(VIDEO_FRAME_S *pVBuf, YUV_IMAGE422_S *pImage);
static void IMP_OSC_ParaConfig( IMP_MODULE_HANDLE hModule );
static void IMP_PARA_Config( IMP_MODULE_HANDLE hModule, IMP_S32 s32ImgW, IMP_S32 s32ImgH );
extern pthread_mutex_t mut;

extern RESULT_S gstOscResult;

extern URP_PARA_S stURPpara;
extern master_thread_init_ok;
IMP_S32 OSC_ALGO_PROCESS()
{
    IMP_S32 nFrmNum = 0;
    VIDEO_FRAME_INFO_S stFrame;
	VI_DEV OscViDev = 0;
	VI_CHN OscViChn = 0;

	OBJ_S stOscObj;
	LIB_INFO_S stOscLibInfo;
	IMP_HANDLE hOscIMP = &stOscObj;
	MEM_SET_S stOscMems;
	YUV_IMAGE422_S stOscImage;
	RESULT_S stOscResult;
	IMP_S32 s32OscWidth, s32OscHeight;

    IMP_S32 bRet = 0;

    s32OscWidth = IMG_WIDTH;
    s32OscHeight = IMG_HEIGHT;
printf("w:%d,h:%d\n", s32OscWidth, s32OscHeight);
	IMP_YUVImage422Create( &stOscImage, s32OscWidth, s32OscHeight, NULL );

    // 获取库信息
	IMP_GetAlgoLibInfo( hOscIMP, &stOscLibInfo );

	// 获取内存需求
	stOscMems.u32ImgW = s32OscWidth;
	stOscMems.u32ImgH = s32OscHeight;
	IMP_GetMemReq( hOscIMP, &stOscMems );
	IMP_MemsAlloc( &stOscMems );

    if(IMP_OSC_Create( hOscIMP, &stOscMems ) != IMP_STATUS_OK)
	{
        printf("IMP_OSC_Create Error!\n");
	       exit(0);
	}

    // 配置参数
	IMP_OSC_ParaConfig( hOscIMP );

//	IMP_Start( hOscIMP );

    sleep(2);

    while(1)
    {
        if(master_thread_init_ok == 0)
            continue;
        //Get Image From Hisi Interface
        if (HI_MPI_VI_GetFrame(OscViDev, OscViChn, &stFrame))
        {
            printf("HI_MPI_VI_GetFrame err, vi(%d,%d)\n", OscViDev, OscViChn);
            continue;
        }
struct timeval t1, t2;
gettimeofday(&t1, NULL);
        IMP_HiImageConvertToYUV422Image(&stFrame.stVFrame,&stOscImage);

        HI_MPI_VI_ReleaseFrame(OscViDev, OscViChn, &stFrame);

        // Process Image
		IMP_OSC_ProcessImage( hOscIMP, &stOscImage );

        // Get Algo Result
		IMP_OSC_GetResults( hOscIMP, &stOscResult );
gettimeofday(&t2, NULL);
printf("out:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
printf("\n--------------------------\n");
        pthread_mutex_lock(&mut);
        gstOscResult = stOscResult;
        pthread_mutex_unlock(&mut);
        usleep(100000);
    }

	printf("Stopped..................\n");
//	IMP_Stop( hOscIMP, &stOscResult );
	IMP_OSC_Release( hOscIMP );

	IMP_MemsFree( &stOscMems );
	IMP_YUVImage422Destroy( &stOscImage, NULL );

    return bRet;
}

static void IMP_HiImageConvertToYUV422Image(VIDEO_FRAME_S *pVBuf, YUV_IMAGE422_S *pImage)
{
    IMP_S32 w, h;
    IMP_U8 * pVBufVirt_Y;
    IMP_U8 * pVBufVirt_C;
    IMP_U8 * pMemContent;
    IMP_U8 *pY = pImage->pu8Y;
    IMP_U8 *pU = pImage->pu8U;
    IMP_U8 *pV = pImage->pu8V;
    IMP_U8 TmpBuff[1024];
    //IMP_S32 w = 0;
    HI_U32 phy_addr,u32Size;
	HI_CHAR *pUserPageAddr[2];
    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
    HI_U32 u32UvHeight;

    pImage->u32Time = pVBuf->u64pts/(40*1000);

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
    {
        u32Size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*3/2;
        u32UvHeight = pVBuf->u32Height/2;
    }
    else
    {
        u32Size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*2;
        u32UvHeight = pVBuf->u32Height;
    }

    phy_addr = pVBuf->u32PhyAddr[0];

    //printf("phy_addr:%x, u32Size:%d\n", phy_addr, u32Size);
    pUserPageAddr[0] = (HI_U8 *) HI_MPI_SYS_Mmap(phy_addr, u32Size);
    if (NULL == pUserPageAddr[0])
    {
        return;
    }

	pVBufVirt_Y = pUserPageAddr[0];
	pVBufVirt_C = pVBufVirt_Y + (pVBuf->u32Stride[0])*(pVBuf->u32Height);
  //  printf("u32Height:%d:::::pVBuf->u32Width:%d\n",pVBuf->u32Height,pVBuf->u32Width);
    /* Y ----------------------------------------------------------------*/
//    printf("width:%d####height:%d\n",pVBuf->u32Stride[0],pVBuf->u32Height);
    if(pVBuf->u32Stride[0] == 352)
    {

#ifndef OSC_QCIF
        for(h=0; h<pVBuf->u32Height/2; h++)
        {
            pMemContent = pVBufVirt_Y + 2*h*pVBuf->u32Stride[0];
            for( w=0;w<(pVBuf->u32Width/2);w++ )
            {
                TmpBuff[w] = (IMP_U8)(*pMemContent);
                pMemContent+=2;
            }
            memcpy(pImage->pu8Y,TmpBuff,pVBuf->u32Width/2);
            pImage->pu8Y += pVBuf->u32Width/2;
        }
#else
        for(h=0; h<pVBuf->u32Height; h++)
        {
            pMemContent = pVBufVirt_Y + h*pVBuf->u32Stride[0];
            memcpy(pImage->pu8Y,pMemContent,pVBuf->u32Width);
            pImage->pu8Y += pVBuf->u32Width;
        }
#endif
        pImage->pu8Y = pY;

    }
    else if( pVBuf->u32Stride[0] == 704)
    {
        //printf("pVBuf->u32Height%d #### pVBuf->u32Width%d\n",pVBuf->u32Height,pVBuf->u32Width);
#ifdef OSC_QCIF
        for(h=0; h<(pVBuf->u32Height/4); h++)
        {
            pMemContent = pVBufVirt_Y + 4*h*(pVBuf->u32Stride[0]);
            for( w=0;w<(pVBuf->u32Width/4);w++ )
            {
                TmpBuff[w] = (IMP_U8)(*pMemContent);
                pMemContent+=4;
            }
            memcpy(pImage->pu8Y,TmpBuff,pVBuf->u32Width/4);
            pImage->pu8Y += pVBuf->u32Width/4;
        }
#else
        for(h=0; h<(pVBuf->u32Height/2); h++)
        {
            pMemContent = pVBufVirt_Y + 2*h*(pVBuf->u32Stride[0]);
            for( w=0;w<(pVBuf->u32Width/2);w++ )
            {
                TmpBuff[w] = (IMP_U8)(*pMemContent);
                pMemContent+=2;
            }
            memcpy(pImage->pu8Y,TmpBuff,pVBuf->u32Width/2);
            pImage->pu8Y += pVBuf->u32Width/2;
        }
#endif
        pImage->pu8Y = pY;
    }
    HI_MPI_SYS_Munmap(pUserPageAddr[0], u32Size);
}


#define MAX_RULE_DATA_SIZE 64*1024
//modified by mzhang 2011-11-14 14:45:56
static void IMP_OSC_ParaConfig( IMP_MODULE_HANDLE hModule )
{
short awPos1[4][4][2]={{215,102, 300,102, 295,159, 222,165}, {222,105, 262,106, 262,137, 223,138}, {272,108, 287,108, 287,117, 271,119}};
short awPank0203[4][4][2]={181,130, 331,125, 344,188, 184,210};
short awCar1[4][4][2]={157,195, 159,238, 205,232, 192,187};
short awCar2[4][4][2]={60,34, 50,266, 276,259, 177,40};
short awCar3[4][4][2]={81,93, 88,168, 205,166, 188,93};
short awPos[4][4][2];

memcpy(awPos, awCar2, 4 * 2 * 2);
memcpy(awPos + 1, awCar1, 4 * 2 * 2);
memcpy(awPos + 2, awCar3, 4 * 2 * 2);

int dwI;
	URP_OSC_SPECL_REGIONS_S *psr = 0;
	memset(&stURPpara,0,sizeof(URP_PARA_S));
	{
		stURPpara.stConfigPara.s32ImgW = 352;
		stURPpara.stConfigPara.s32ImgH = 288;

		stURPpara.stAdvancePara.s32AdvanceParaEnable = 1;
		stURPpara.stAdvancePara.s32TargetOutputSensitivityLevel = IMP_HIGH_LEVEL_SENSITIVITY;
        stURPpara.stAdvancePara.s32TargetMinSize = -1;
        stURPpara.stAdvancePara.s32TargetMaxSize = -1;
        stURPpara.stAdvancePara.s32TargetMinWidth = -1;
        stURPpara.stAdvancePara.s32TargetMinHeight = -1;
        stURPpara.stAdvancePara.s32TargetMaxWidth = -1;
        stURPpara.stAdvancePara.s32TargetMaxHeight = -1;
//        stURPpara.stAdvancePara.f32TargetMinWHRatio = -1;
//        stURPpara.stAdvancePara.f32TargetMaxWHRatio = -1;
        stURPpara.stAdvancePara.s32TargetMinSpeed = -1;
        stURPpara.stAdvancePara.s32TargetMaxSpeed = -1;


		stURPpara.stRuleSet.astRule[0].u32Enable = 1;
		stURPpara.stRuleSet.astRule[0].u32Valid = 1;
		stURPpara.stRuleSet.astRule[0].u32Mode = IMP_URP_FUNC_OSC; //设定为OSC算法

		stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.s32SceneType = IMP_URP_INDOOR;
		stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.s32CameraType = IMP_URP_COMMON_CAMERA;
		stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.stOscPara.s32TimeMin = 20;
		stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.stOscPara.s32SizeMin = 10;
		stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.stOscPara.s32SizeMax = 10000;

		psr = &stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.astSpclRgs[0];
		psr->s32Valid = 1;
//		{
	//		IMP_S8 *pbyBufName = "HelloOSC";
	//		memcpy(stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.astSpclRgs[0].as8Name, pbyBufName, sizeof(pbyBufName));
//		}

        //定义检测区域
		psr->stOscRg.s32Valid = 1;
		psr->stOscRg.s32PointNum = 4;
        for(dwI = 0; dwI < 4; dwI++)
        {
            psr->stOscRg.astPoint[dwI].s16X = awPos[0][dwI][0];
            psr->stOscRg.astPoint[dwI].s16Y = awPos[0][dwI][1];
        }
//        printf("%d,%d;\n", psr->stOscRg.astPoint[0].s16X, psr->stOscRg.astPoint[0].s16Y);
#if 1
        //定义无效子区域
		psr->astSubRgA.s32Valid = 1;
		psr->astSubRgA.s32PointNum = 4;
        for(dwI = 0; dwI < psr->astSubRgA.s32PointNum; dwI++)
        {
            psr->astSubRgA.astPoint[dwI].s16X = awPos[1][dwI][0];
            psr->astSubRgA.astPoint[dwI].s16Y = awPos[1][dwI][1];
        }
#endif

#if 1
        //定义无效子区域
		psr->astSubRgB.s32Valid = 0;
		psr->astSubRgB.s32PointNum = 4;
        for(dwI = 0; dwI < psr->astSubRgB.s32PointNum; dwI++)
        {
            psr->astSubRgB.astPoint[dwI].s16X = awPos[2][dwI][0];
            psr->astSubRgB.astPoint[dwI].s16Y = awPos[2][dwI][1];
        }
#endif

	}

    printf("IMP_ParaConfig\n");
	IMP_OSC_ConfigArmPeaParameter( hModule, NULL ,&stURPpara );
}


