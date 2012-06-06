#include "imp_pea_thread.h"
#include "imp_draw_osd.h"

#define TIME_TEST
#ifdef TIME_TEST
#include <sys/time.h>
#endif
//#define printf(...)
static void IMP_HiImageConvertToYUV422Image(VIDEO_FRAME_S *pVBuf, YUV_IMAGE422_S *pImage);
static void IMP_ShowDebugInfo( RESULT_S *result );
static void IMP_ParaConfig( IMP_MODULE_HANDLE hModule );
static void IMP_PARA_Config( IMP_MODULE_HANDLE hModule, IMP_S32 s32ImgW, IMP_S32 s32ImgH );
static void IMP_PARA_Config1( IMP_MODULE_HANDLE hModule, IMP_S32 s32ImgW, IMP_S32 s32ImgH );
extern pthread_mutex_t mut;

extern RESULT_S gstPeaResult;

extern URP_PARA_S stURPpara;
extern master_thread_init_ok;
#define BUFRAME 15
extern VIDEO_FRAME_INFO_S stFrame[BUFRAME];
extern int Curframe;

extern int Get_YUV_Ready;
extern int lastframe;
extern int drawframe;
IMP_S32 PEA_ALGO_PROCESS()
{
#ifdef TIME_TEST
    //timer
    struct timeval start;
    struct timeval end;
    int timestart,timeend,timeuse;
#endif
    IMP_S32 nFrmNum = 0;
   // VIDEO_FRAME_INFO_S stFrame;
    VI_DEV ViDev = 0;
    VI_CHN ViChn =0;
    HI_U32 uWidth;
    HI_U32 uHeight;

    OBJ_S stObj;
	LIB_INFO_S stLibInfo;
	IMP_HANDLE hIMP = &stObj;
	MEM_SET_S stMems;
	YUV_IMAGE422_S stImage;
	RESULT_S stResult;
	IMP_S32 s32Width, s32Height;

	IMP_S32 bRet = 0;

    int interval = 0;
    int tmp = 0;
    int i = 0;

    s32Width = IMG_WIDTH;
    s32Height = IMG_HEIGHT;

	IMP_YUVImage422Create( &stImage, s32Width, s32Height, NULL );

    // 获取库信息
	IMP_GetAlgoLibInfo( hIMP, &stLibInfo );

	// 获取内存需求
	stMems.u32ImgW = s32Width;
	stMems.u32ImgH = s32Height;
	IMP_GetMemReq( hIMP, &stMems );
	IMP_MemsAlloc( &stMems );

    if(IMP_Create( hIMP, &stMems ) != IMP_STATUS_OK)
	{
        printf("IMP_Create Error!\n");
	       exit(0);
	}

#if defined(IMP_DEBUG_PRINT)
    printf("IMP_Create ok!\n");
#endif
    // 配置参数
	IMP_ParaConfig( hIMP );
	IMP_Start( hIMP );
#if defined(IMP_DEBUG_PRINT)
	printf("IMP_Start ok!\n");
#endif

    sleep(2);
    while(1)
    {
        if(Get_YUV_Ready == 0)
            continue;
        //break;
        drawframe = Curframe;
#ifdef TIME_TEST
        gettimeofday(&start,NULL);//start timer
#endif

        IMP_HiImageConvertToYUV422Image(&stFrame[drawframe].stVFrame,&stImage);

#ifdef TIME_TEST
        gettimeofday(&end,NULL);
        timestart=1000000*start.tv_sec+start.tv_usec;
        timeend=1000000*end.tv_sec+end.tv_usec;
        timeuse=timeend-timestart;
#if defined(IMP_DEBUG_PRINT)
        printf("IMP_HiImageConvertToYUV422Image:time used:%d us\n",timeuse);
#endif
        gettimeofday(&start,NULL);//start timer
#endif

        // Process Image
		IMP_ProcessImage( hIMP, &stImage );

        // Get Algo Result
		IMP_GetResults( hIMP, &stResult );
#ifdef TIME_TEST
        gettimeofday(&end,NULL);
        timestart=1000000*start.tv_sec+start.tv_usec;
        timeend=1000000*end.tv_sec+end.tv_usec;
        timeuse=timeend-timestart;
#if defined(IMP_DEBUG_PRINT)
        printf("IMP_ProcessImage:time used:%d us\n",timeuse);
#endif
        IMP_DrawPeaResult( &stFrame[drawframe].stVFrame, &stResult,IMP_D1,IMP_QCIF );

        //Draw other frame
        interval = drawframe - lastframe;
        if (interval < 0)
                interval += BUFRAME;
        for (i = 1; i < interval; i++)
        {
                lastframe = drawframe - interval + i;
                if (lastframe < 0)
                        lastframe += BUFRAME;

                tmp = lastframe - Curframe;
                if (tmp < 0)
                        tmp += BUFRAME;
                if (tmp < 2)
                        break;
                IMP_DrawPeaResult( &stFrame[lastframe].stVFrame, &stResult,IMP_D1,IMP_QCIF );
        }

        lastframe = drawframe;

#ifdef SHOW_DEBUG_INFO
        IMP_ShowDebugInfo(&stResult);
#endif
        usleep(120000);
    }

	IMP_Stop( hIMP, &stResult );
	IMP_Release( hIMP );

	IMP_MemsFree( &stMems );
	IMP_YUVImage422Destroy( &stImage, NULL );

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
    HI_U32 phy_addr,size;
	HI_CHAR *pUserPageAddr[2];
    PIXEL_FORMAT_E  enPixelFormat = pVBuf->enPixelFormat;
    HI_U32 u32UvHeight;

    pImage->u32Time = pVBuf->u64pts/(40*1000);

    if (PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
    {
        size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*3/2;
        u32UvHeight = pVBuf->u32Height/2;
    }
    else
    {
        size = (pVBuf->u32Stride[0])*(pVBuf->u32Height)*2;
        u32UvHeight = pVBuf->u32Height;
    }

    phy_addr = pVBuf->u32PhyAddr[0];

    //printf("phy_addr:%x, size:%d\n", phy_addr, size);
    pUserPageAddr[0] = (HI_U8 *) HI_MPI_SYS_Mmap(phy_addr, size);
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
#ifdef PEA_QCIF
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
#ifdef PEA_QCIF
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
    HI_MPI_SYS_Munmap(pUserPageAddr[0], size);
}

static void IMP_ShowDebugInfo( RESULT_S *result )
{
    IMP_S32 i,j,k,zone,num1,tmp;

    EVT_DATA_PERIMETER_S *pdataPerimeter;
    EVT_DATA_TRIPWIRE_S *pdataTripwire;
    IMP_POINT_S pts, pte,pt_s,pt_e;

#if defined(IMP_DEBUG_PRINT)
    printf("target num = %d\n",result->stTargetSet.s32TargetNum);
#endif
    for(i=0;i<result->stEventSet.s32EventNum;i++)
    {
        if(result->stEventSet.astEvents[i].u32Status==IMP_EVT_STATUS_START)
        {
            if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmPerimeter)
            {
                zone=result->stEventSet.astEvents[i].u32Zone;
#if defined(IMP_DEBUG_PRINT)
                printf("Target:%d perimeter start \n",result->stEventSet.astEvents[i].u32Id);
#endif
            }
            if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmTripwire)
            {
                zone=result->stEventSet.astEvents[i].u32Zone;
                pdataTripwire=(EVT_DATA_TRIPWIRE_S*)(result->stEventSet.astEvents[i].au8Data);
#if defined(IMP_DEBUG_PRINT)
                printf("Target:%d tripwire start, zone = %d , u32TId = 0x%x\n",result->stEventSet.astEvents[i].u32Id, zone,pdataTripwire->u32TId);
#endif
                pts=pdataTripwire->stRule.stLine.stPs;
                pte=pdataTripwire->stRule.stLine.stPe;

#if defined(IMP_DEBUG_PRINT)
               printf("start: i=%d,pts=(x=%d,y=%d),pte=(x=%d,y=%d)\n",i,pts.s16X,pts.s16Y,pte.s16X,pte.s16Y);
#endif

                tmp = 0;

                for (j = 0; j < IMP_URP_MAX_TRIPWIRE_CNT; j++)
				{
					if (stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].s32Valid &&
                        pts.s16X == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stStartPt.s16X &&
						pts.s16Y == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stStartPt.s16Y &&
						pte.s16X == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stEndPt.s16X &&
						pte.s16Y == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stEndPt.s16Y)
					{
                          tmp++;
					}
                }
#if defined(IMP_DEBUG_PRINT)
                printf("--------------------------start line = %d!----------------------------------\n",tmp);
#endif
            }
        }
        if(result->stEventSet.astEvents[i].u32Status == IMP_EVT_STATUS_PROCEDURE)
        {
            if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmPerimeter)
            {
                zone=result->stEventSet.astEvents[i].u32Zone;
            }
            if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmTripwire)
            {
                zone=result->stEventSet.astEvents[i].u32Zone;
                pdataTripwire=(EVT_DATA_TRIPWIRE_S*)(result->stEventSet.astEvents[i].au8Data);
                pts=pdataTripwire->stRule.stLine.stPs;
                pte=pdataTripwire->stRule.stLine.stPe;
            }
        }
        else if(result->stEventSet.astEvents[i].u32Status==IMP_EVT_STATUS_END)
        {
             if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmPerimeter)
            {
#if defined(IMP_DEBUG_PRINT)
                printf("Target:%d perimeter end \n",result->stEventSet.astEvents[i].u32Id);
#endif
            }
            if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmTripwire)
            {
                pdataTripwire=(EVT_DATA_TRIPWIRE_S*)(result->stEventSet.astEvents[i].au8Data);
                zone=result->stEventSet.astEvents[i].u32Zone;
                if(result->stEventSet.astEvents[i].u32Id == 39)
                {
#if defined(IMP_DEBUG_PRINT)
                    printf("event id: %d == 39\n",result->stEventSet.astEvents[i].u32Id);
#endif
                }
#if defined(IMP_DEBUG_PRINT)
                printf("Target:%d tripwire end, zone = %d , u32TId = 0x%x \n",result->stEventSet.astEvents[i].u32Id, zone, pdataTripwire->u32TId);
#endif
                pts=pdataTripwire->stRule.stLine.stPs;
                pte=pdataTripwire->stRule.stLine.stPe;
#if defined(IMP_DEBUG_PRINT)
                printf("end: i=%d,pts=(x=%d,y=%d),pte=(x=%d,y=%d)\n",i,pts.s16X,pts.s16Y,pte.s16X,pte.s16Y);
#endif

                tmp = 0;
                for (j = 0; j < IMP_URP_MAX_TRIPWIRE_CNT; j++)
				{
					if (stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].s32Valid &&
                        pts.s16X == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stStartPt.s16X &&
						pts.s16Y == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stStartPt.s16Y &&
						pte.s16X == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stEndPt.s16X &&
						pte.s16Y == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stEndPt.s16Y)
					{
                         tmp++;
					}
                }
#if defined(IMP_DEBUG_PRINT)
                printf("--------------------------end line = %d!----------------------------------\n",tmp);
#endif
            }
        }
    }
}

void print_stURPpara(void)
{
	printf("=========URP_PARA_S.URP_CONFIG_PARA_S===========\n");
	printf("  s32Version=%d\n",stURPpara.stConfigPara.s32Version);
	printf("  s32ImgW=%d\n",stURPpara.stConfigPara.s32ImgW);
	printf("  s32ImgH=%d\n",stURPpara.stConfigPara.s32ImgH);
	printf("==========URP_PARA_S.URP_RULE_SET_S=============\n");
	int i;
	for(i=0;i<IMP_URP_MAX_NUM_RULE_NUM;i++)
	{
		printf("u32Enable=%d u32Valid=%d u32Level=%d u32Mode=%s name=%s u32Mode=%d\n",stURPpara.stRuleSet.astRule[i].u32Enable,stURPpara.stRuleSet.astRule[i].u32Valid,stURPpara.stRuleSet.astRule[i].u32Level,((stURPpara.stRuleSet.astRule[i].u32Mode&IMP_FUNC_PERIMETER)==0)?"绊线":"周界",stURPpara.stRuleSet.astRule[i].aName,stURPpara.stRuleSet.astRule[i].u32Mode);
		if (stURPpara.stRuleSet.astRule[i].u32Enable!=0)
		{
			if ((stURPpara.stRuleSet.astRule[i].u32Mode&IMP_FUNC_PERIMETER) != 0)
			{
				printf("周界规则:URP_PERIMETER_RULE_PARA_S \n");
				printf("目标类型限制=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stPerimeterRulePara.s32TypeLimit);
				printf("目标类别:人=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stPerimeterRulePara.s32TypeHuman);
				printf("目标类别:车=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stPerimeterRulePara.s32TypeVehicle);
				printf("周界模式=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stPerimeterRulePara.s32Mode);

				printf("最小像素距离=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stPerimeterRulePara.stLimitPara.s32MinDist);
				printf("最短时间=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stPerimeterRulePara.stLimitPara.s32MinTime);
				printf("是否做方向限制=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stPerimeterRulePara.stLimitPara.s32DirectionLimit);
				printf("禁止方向角度=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stPerimeterRulePara.stLimitPara.s32ForbiddenDirection);

				printf("周界边界区域 边界点数目=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stPerimeterRulePara.stLimitPara.stBoundary.s32BoundaryPtNum);
				int j;
				for (j=0; j<stURPpara.stRuleSet.astRule[i].stPara.stPerimeterRulePara.stLimitPara.stBoundary.s32BoundaryPtNum; j++)
					printf("周界边界区域 边界点%d坐标 x=%d y=%d\n",j,stURPpara.stRuleSet.astRule[i].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[j].s16X,stURPpara.stRuleSet.astRule[i].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[j].s16Y);
			}
			else if ((stURPpara.stRuleSet.astRule[i].u32Mode&IMP_FUNC_TRIPWIRE) != 0)
			{
				printf("绊线规则:URP_TRIPWIRE_RULE_PARA_S\n");
				printf("目标类型限制=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stTripwireRulePara.s32TypeLimit);
				printf("目标类别:人=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stTripwireRulePara.s32TypeHuman);
				printf("目标类别:车=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stTripwireRulePara.s32TypeVehicle);
				int j;
				for (j=0; j<IMP_URP_MAX_TRIPWIRE_CNT; j++)
				{
					if (stURPpara.stRuleSet.astRule[i].stPara.stTripwireRulePara.astLines[j].s32Valid == 1)
					{
						printf("绊线是否为双向绊线=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stTripwireRulePara.astLines[j].s32IsDoubleDirection);
						printf("单绊线禁止方向角度=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stTripwireRulePara.astLines[j].s32ForbiddenDirection);
						printf("单绊线位置 起点x=%d y=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stTripwireRulePara.astLines[j].stLine.stStartPt.s16X,stURPpara.stRuleSet.astRule[i].stPara.stTripwireRulePara.astLines[j].stLine.stStartPt.s16Y);
						printf("单绊线位置 终点x=%d y=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stTripwireRulePara.astLines[j].stLine.stEndPt.s16X,stURPpara.stRuleSet.astRule[i].stPara.stTripwireRulePara.astLines[j].stLine.stEndPt.s16Y);
					}
				}
				printf("最小像素距离=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stTripwireRulePara.stLimitPara.s32MinDist);
				printf("最短时间=%d\n",stURPpara.stRuleSet.astRule[i].stPara.stTripwireRulePara.stLimitPara.s32MinTime);
			}
			else
				printf("其他规则%d\n",stURPpara.stRuleSet.astRule[i].u32Mode);
		}
		else
			printf("规则%d未使能\n",i);
		printf("------------------------------------------------\n");
	}
	printf("========URP_PARA_S.URP_ADVANCE_PARA_S===========\n");
	printf("  s32AdvanceParaEnable=%d\n",stURPpara.stAdvancePara.s32AdvanceParaEnable);
	printf("  s32TargetMinSize=%d\n",stURPpara.stAdvancePara.s32TargetMinSize);
	printf("  s32TargetMaxSize=%d\n",stURPpara.stAdvancePara.s32TargetMaxSize);
	printf("  s32TargetMinWidth=%d\n",stURPpara.stAdvancePara.s32TargetMinWidth);
	printf("  s32TargetMaxWidth=%d\n",stURPpara.stAdvancePara.s32TargetMaxWidth);
	printf("  s32TargetMinHeight=%d\n",stURPpara.stAdvancePara.s32TargetMinHeight);
	printf("  s32TargetMaxHeight=%d\n",stURPpara.stAdvancePara.s32TargetMaxHeight);
	printf("  f32TargetMinWHRatio=%f\n",stURPpara.stAdvancePara.f32TargetMinWHRatio);
	printf("  f32TargetMaxWHRatio=%f\n",stURPpara.stAdvancePara.f32TargetMaxWHRatio);
	printf("  s32TargetMinSpeed=%d\n",stURPpara.stAdvancePara.s32TargetMinSpeed);
	printf("  s32TargetMaxSpeed=%d\n",stURPpara.stAdvancePara.s32TargetMaxSpeed);
	printf("  s32TargetOutputSensitivityLevel=%d\n",stURPpara.stAdvancePara.s32TargetOutputSensitivityLevel);
}

#define MAX_RULE_DATA_SIZE 64*1024
static void IMP_ParaConfig( IMP_MODULE_HANDLE hModule )
{
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
        stURPpara.stAdvancePara.f32TargetMinWHRatio = -1;
        stURPpara.stAdvancePara.f32TargetMaxWHRatio = -1;
        stURPpara.stAdvancePara.s32TargetMinSpeed = -1;
        stURPpara.stAdvancePara.s32TargetMaxSpeed = -1;


		stURPpara.stRuleSet.astRule[0].u32Enable = 0;
		stURPpara.stRuleSet.astRule[0].u32Valid = 0;
		stURPpara.stRuleSet.astRule[0].u32Mode |= IMP_FUNC_PERIMETER;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32Mode = IMP_URP_PMODE_INTRUSION;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32TypeLimit = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32TypeHuman = 1;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32TypeVehicle = 1;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.s32DirectionLimit = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.s32ForbiddenDirection = 180;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.s32MinDist = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.s32MinTime = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.s32BoundaryPtNum = 8;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[0].s16X = 85;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[0].s16Y = 201;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[1].s16X = 331;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[1].s16Y = 136;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[2].s16X = 336;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[2].s16Y = 184;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[3].s16X = 334;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[3].s16Y = 263;
        stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[4].s16X = 337;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[4].s16Y = 194;
        stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[5].s16X = 286;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[5].s16Y = 113;
        stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[6].s16X = 167;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[6].s16Y = 134;
        stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[7].s16X = 92;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[7].s16Y = 167;

		stURPpara.stRuleSet.astRule[1].u32Enable = 1;
		stURPpara.stRuleSet.astRule[1].u32Valid = 1;
		stURPpara.stRuleSet.astRule[1].u32Mode |= IMP_FUNC_TRIPWIRE;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.s32TypeLimit = 0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.s32TypeHuman =1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.s32TypeVehicle =1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.stLimitPara.s32MinDist=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.stLimitPara.s32MinTime=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].s32ForbiddenDirection=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].s32IsDoubleDirection=1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].s32Valid=1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stStartPt.s16X=176;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stStartPt.s16Y=20;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stEndPt.s16X=176;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stEndPt.s16Y=270;

		stURPpara.stRuleSet.astRule[2].u32Enable = 1;
		stURPpara.stRuleSet.astRule[2].u32Valid = 1;
		stURPpara.stRuleSet.astRule[2].u32Mode |= IMP_FUNC_PERIMETER;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.s32Mode = IMP_URP_PMODE_INTRUSION;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.s32TypeLimit = 0;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.s32TypeHuman = 1;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.s32TypeVehicle = 1;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.s32DirectionLimit = 0;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.s32ForbiddenDirection = 180;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.s32MinDist = 0;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.s32MinTime = 0;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.s32BoundaryPtNum = 4;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[0].s16X = 1;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[0].s16Y = 1;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[1].s16X = 1;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[1].s16Y = 239;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[2].s16X = 351;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[2].s16Y = 239;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[3].s16X = 351;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[3].s16Y = 1;
	}
	IMP_ConfigArmPeaParameter( hModule, NULL ,&stURPpara );
}
static void IMP_PARA_Config1( IMP_MODULE_HANDLE hModule, IMP_S32 s32ImgW, IMP_S32 s32ImgH )
{
	EXTERNAL_PARA_S *pstPara = IMP_PARA_Alloc( IMP_PARA_RULE_BUFLEN,
		IMP_PARA_ADVBUF_BUFCNT, IMP_PARA_ADVBUF_BUFLEN, NULL );

	memset(&stURPpara,0,sizeof(URP_PARA_S));
	{
		stURPpara.stConfigPara.s32ImgW = 352;
		stURPpara.stConfigPara.s32ImgH = 288;
		stURPpara.stRuleSet.astRule[0].u32Enable = 0;
		stURPpara.stRuleSet.astRule[0].u32Valid = 0;
		stURPpara.stRuleSet.astRule[0].u32Mode |= IMP_FUNC_PERIMETER;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32Mode = IMP_URP_PMODE_INTRUSION;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32TypeLimit = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32TypeHuman = 1;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32TypeVehicle = 1;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.s32DirectionLimit = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.s32ForbiddenDirection = 180;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.s32MinDist = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.s32MinTime = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.s32BoundaryPtNum = 4;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[0].s16X = 80;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[0].s16Y = 80;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[1].s16X = 80;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[1].s16Y = 200;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[2].s16X = 270;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[2].s16Y = 200;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[3].s16X = 270;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[3].s16Y = 80;

		stURPpara.stRuleSet.astRule[1].u32Enable = 1;
		stURPpara.stRuleSet.astRule[1].u32Valid = 1;
		stURPpara.stRuleSet.astRule[1].u32Mode |= IMP_FUNC_TRIPWIRE;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.s32TypeLimit = 0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.s32TypeHuman =1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.s32TypeVehicle =1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.stLimitPara.s32MinDist=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.stLimitPara.s32MinTime=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].s32ForbiddenDirection=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].s32IsDoubleDirection=1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].s32Valid=1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stStartPt.s16X=15;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stStartPt.s16Y=20;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stEndPt.s16X=176;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stEndPt.s16Y=270;

        stURPpara.stConfigPara.s32ImgW = 352;
		stURPpara.stConfigPara.s32ImgH = 288;
		stURPpara.stRuleSet.astRule[2].u32Enable = 0;
		stURPpara.stRuleSet.astRule[2].u32Valid = 0;
		stURPpara.stRuleSet.astRule[2].u32Mode |= IMP_FUNC_PERIMETER;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.s32Mode = IMP_URP_PMODE_INTRUSION;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.s32TypeLimit = 0;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.s32TypeHuman = 1;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.s32TypeVehicle = 1;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.s32DirectionLimit = 0;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.s32ForbiddenDirection = 180;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.s32MinDist = 0;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.s32MinTime = 0;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.s32BoundaryPtNum = 4;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[0].s16X = 1;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[0].s16Y = 1;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[1].s16X = 1;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[1].s16Y = 287;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[2].s16X = 351;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[2].s16Y = 287;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[3].s16X = 351;
		stURPpara.stRuleSet.astRule[2].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[3].s16Y = 1;
	}
	IMP_PARA_Init( pstPara, NULL, NULL, s32ImgW, s32ImgH, NULL );
	IMP_ConfigArmPeaParameter( hModule, pstPara ,&stURPpara );
	IMP_PARA_Free( pstPara, IMP_PARA_ADVBUF_BUFCNT, NULL );
}
static void IMP_PARA_Config( IMP_MODULE_HANDLE hModule, IMP_S32 s32ImgW, IMP_S32 s32ImgH )
{
	FILE * ConfigFileFd;
	EXTERNAL_PARA_S *pstPara = IMP_PARA_Alloc( IMP_PARA_RULE_BUFLEN,
	IMP_PARA_ADVBUF_BUFCNT, IMP_PARA_ADVBUF_BUFLEN, NULL );

	memset(&stURPpara,0,sizeof(URP_PARA_S));
	ConfigFileFd = fopen("URP_PARA_ConfigFile.dat","r");
	if (ConfigFileFd != NULL)
	{
		if (fread(&stURPpara,1,sizeof(URP_PARA_S),ConfigFileFd) != sizeof(URP_PARA_S))
			printf("未读到配置文件\n");
		fclose(ConfigFileFd);
	}
//	print_stURPpara();

	IMP_PARA_Init( pstPara, NULL, NULL, s32ImgW, s32ImgH, NULL );
	IMP_ConfigArmPeaParameter( hModule, pstPara ,&stURPpara );
	IMP_PARA_Free( pstPara, IMP_PARA_ADVBUF_BUFCNT, NULL );
}
