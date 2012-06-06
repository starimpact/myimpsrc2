
#include "imp_pea_thread.h"
#include "imp_draw_osd.h"
pthread_t thread[3];

pthread_mutex_t mut;

RESULT_S gstPeaResult;

URP_PARA_S stURPpara;

int master_thread_init_ok = 0;

TDE2_SURFACE_S g_stOsdYSurface;		/**<  Y Surface */
TDE2_SURFACE_S g_stOsdUSurface;		/**<  U Surface */
TDE2_SURFACE_S g_stOsdVSurface;		/**<  V Surface */

typedef enum hiSAMPLE_VO_DIV_MODE
{
    DIV_MODE_1  = 1,    /* 1-screen display */
    DIV_MODE_4  = 4,    /* 4-screen display */
    DIV_MODE_9  = 9,    /* 9-screen display */
    DIV_MODE_16 = 16,   /* 16-screen display */
    DIV_MODE_BUTT
} SAMPLE_VO_DIV_MODE;


#define BT656_WORKMODE VI_WORK_MODE_4D1
#define G_VODEV VO_DEV_SD
//#define printf(...)
/* For PAL or NTSC input and output. */
extern VIDEO_NORM_E gs_enViNorm;
extern VO_INTF_SYNC_E gs_enSDTvMode;
/*****************************************************************************
 Prototype       : SAMPLE_GetViCfg_SD
 Description     : vi configs to input standard-definition video
 Input           : enPicSize     **
                   pstViDevAttr  **
                   pstViChnAttr  **
 Output          : None
 Return Value    :
 Global Variable
    Read Only    :
    Read & Write :
*****************************************************************************/
HI_VOID SAMPLE_GetViCfg_SD(PIC_SIZE_E     enPicSize,
                           VI_PUB_ATTR_S* pstViDevAttr,
                           VI_CHN_ATTR_S* pstViChnAttr)
{
    pstViDevAttr->enInputMode = VI_MODE_BT656;
    pstViDevAttr->enWorkMode = BT656_WORKMODE;
    pstViDevAttr->enViNorm = gs_enViNorm;

    pstViChnAttr->stCapRect.u32Width  = 704;
    pstViChnAttr->stCapRect.u32Height =
        (VIDEO_ENCODING_MODE_PAL == gs_enViNorm) ? 288 : 240;
    pstViChnAttr->stCapRect.s32X =
        (704 == pstViChnAttr->stCapRect.u32Width) ? 8 : 0;
    pstViChnAttr->stCapRect.s32Y = 0;
    pstViChnAttr->enViPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
    pstViChnAttr->bHighPri = HI_FALSE;
    pstViChnAttr->bChromaResample = HI_FALSE;

    /* different pic size has different capture method for BT656. */
    pstViChnAttr->enCapSel = (PIC_D1 == enPicSize || PIC_2CIF == enPicSize) ? \
                             VI_CAPSEL_BOTH : VI_CAPSEL_BOTTOM;
    pstViChnAttr->bDownScale = (PIC_D1 == enPicSize || PIC_HD1 == enPicSize) ? \
                               HI_FALSE : HI_TRUE;

    return;
}
/*****************************************************************************
 Prototype       : SAMPLE_GetVoCfg_VGA_800x600
 Description     : vo configs to output VGA video.
 Input           : pstVoDevAttr       **
                   pstVideoLayerAttr  **
 Output          : None
 Return Value    :
 Global Variable
    Read Only    :
    Read & Write :

*****************************************************************************/
HI_VOID SAMPLE_GetVoCfg_VGA_800x600(VO_PUB_ATTR_S*         pstVoDevAttr,
                                    VO_VIDEO_LAYER_ATTR_S* pstVideoLayerAttr)
{
    /* set vo device attribute and start device. */
    pstVoDevAttr->u32BgColor = VO_BKGRD_BLUE;
    pstVoDevAttr->enIntfType = VO_INTF_VGA;
    pstVoDevAttr->enIntfSync = VO_OUTPUT_800x600_60;

    pstVideoLayerAttr->stDispRect.s32X = 0;
    pstVideoLayerAttr->stDispRect.s32Y = 0;
    pstVideoLayerAttr->stDispRect.u32Width   = 800;
    pstVideoLayerAttr->stDispRect.u32Height  = 600;
    pstVideoLayerAttr->stImageSize.u32Width  = 720;
    pstVideoLayerAttr->stImageSize.u32Height = (VO_OUTPUT_PAL == gs_enSDTvMode) ? 576 : 480;
    pstVideoLayerAttr->u32DispFrmRt = (VO_OUTPUT_PAL == gs_enSDTvMode) ? 25 : 30;
    pstVideoLayerAttr->enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    pstVideoLayerAttr->s32PiPChn = VO_DEFAULT_CHN;

    return;
}
#define BUFRAME 15
VIDEO_FRAME_INFO_S stFrame[BUFRAME];
int Curframe = 0;

int Get_YUV_Ready = 0;
int lastframe = 0;
int drawframe = 0;
HI_S32 SAMPLE_VIO_1Screen_VoVGA()
{
    VB_CONF_S stVbConf   = {0};
    HI_S32 s32ViChnTotal = 1;
    HI_S32 s32VoChnTotal = 1;
    VO_DEV VoDev;
    VO_CHN VoChn;
    VI_DEV ViDev;
    VI_CHN ViChn;
    VI_PUB_ATTR_S stViDevAttr;
    VI_CHN_ATTR_S stViChnAttr;
    VO_PUB_ATTR_S stVoDevAttr;
    VO_VIDEO_LAYER_ATTR_S stVideoLayerAttr;
  //  VIDEO_FRAME_INFO_S stFrame;
    HI_S32 s32Ret = HI_SUCCESS;
    int iRet = 0;
    int i = 0;
    stVbConf.astCommPool[0].u32BlkSize = 704 * 576 * 2;
    stVbConf.astCommPool[0].u32BlkCnt  = 20;
    stVbConf.astCommPool[1].u32BlkSize = 384 * 288 * 2;
    stVbConf.astCommPool[1].u32BlkCnt = 20;
    if (HI_SUCCESS != SAMPLE_InitMPP(&stVbConf))
    {
       return -1;
    }

    /* Config VI to input standard-definition video */
    ViDev = 0;
    SAMPLE_GetViCfg_SD(PIC_D1, &stViDevAttr, &stViChnAttr);

    s32Ret = SAMPLE_StartViByChn(s32ViChnTotal, &stViDevAttr, &stViChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    /* display VGA video on vo HD divice. */
    VoDev = VO_DEV_HD;
    SAMPLE_GetVoCfg_VGA_800x600(&stVoDevAttr, &stVideoLayerAttr);
    s32Ret = SAMPLE_StartVo(s32VoChnTotal, VoDev, &stVoDevAttr, &stVideoLayerAttr);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    master_thread_init_ok = 1;
    while(1)
    {
        //HI_MPI_VENC_SendFrame(7, 7, stFrame[i].u32PoolId, (VIDEO_FRAME_S *)(&(stFrame[i])), HI_TRUE);
        IMP_DrawPeaRule(&stFrame[i].stVFrame,&stURPpara);
        HI_MPI_VO_SendFrame( VoDev,0,&stFrame[i]);
        HI_MPI_VI_ReleaseFrame(ViDev, 0, &stFrame[i]);

        //HI_MPI_VI_ReleaseFrame(g_iVCAChn/4, g_iVCAChn % 4, &(stFrame[i]));

        iRet = HI_MPI_VI_GetFrame(ViDev, 0, &(stFrame[i]));

        if(iRet == HI_SUCCESS )
        {
                Curframe = i;
                Get_YUV_Ready = 1;
        }
        else
        {
                printf(0xFF0000, "HI_MPI_VI_GetFrame error, return %x", iRet);
                //Td_Log_Msg(LOG_LEVEL_WARN, "HI_MPI_VI_GetFrame error\n");
        }

        i++;
        if (i >= BUFRAME)
                i = 0;

        usleep(10);
    }

    /* Display on HD screen. */
 /*   s32Ret = HI_MPI_VI_BindOutput(ViDev, 0, VoDev, 0);
         if (HI_SUCCESS != s32Ret)
        {
            printf("bind vi2vo failed, vi(%d,%d),vo(%d,%d)!\n",
                   ViDev, ViChn, VoDev, VoChn);
            return HI_FAILURE;
        }*/

    /*printf("start VI to VO preview \n");
    while(1)
    {
        if (HI_MPI_VI_GetFrame(ViDev, 0, &stFrame))
        {
            printf("main thread err\n");
            printf("HI_MPI_VI_GetFrame err, vi(%d,%d)\n", ViDev, ViChn);
            return -1;
        }
        IP_OUTPUT_RESULT(&stFrame.stVFrame, &g_IP_Result);
        HI_MPI_VO_SendFrame( VoDev,0,&stFrame);
        HI_MPI_VI_ReleaseFrame(ViDev, 0, &stFrame);

    }*/
    return HI_SUCCESS;
}
/*****************************************************************************
 Prototype       : SAMPLE_GetVoCfg_1280x1024_4Screen
 Description     : vo configs to output 1280*1024 on HD.
 Input           : pstVoDevAttr       **
                   pstVideoLayerAttr  **
 Output          : None
 Return Value    :
 Global Variable
    Read Only    :
    Read & Write :

*****************************************************************************/
HI_VOID SAMPLE_GetVoCfg_1280x1024_4Screen(VO_PUB_ATTR_S*         pstVoDevAttr,
                                    VO_VIDEO_LAYER_ATTR_S* pstVideoLayerAttr)
{
    HI_U32 u32InPicHeight = 0;


    pstVoDevAttr->u32BgColor = VO_BKGRD_BLUE;
    pstVoDevAttr->enIntfType = VO_INTF_VGA;
    pstVoDevAttr->enIntfSync = VO_OUTPUT_1280x1024_60;

    pstVideoLayerAttr->stDispRect.s32X = 0;
    pstVideoLayerAttr->stDispRect.s32Y = 0;
    pstVideoLayerAttr->stDispRect.u32Width   = 1280;
    pstVideoLayerAttr->stDispRect.u32Height  = 1024;

    pstVideoLayerAttr->stImageSize.u32Width  = 1280;
    u32InPicHeight = (VO_OUTPUT_PAL == gs_enSDTvMode) ? 576 : 480;
    pstVideoLayerAttr->stImageSize.u32Height = u32InPicHeight*2;
    pstVideoLayerAttr->u32DispFrmRt = (VO_OUTPUT_PAL == gs_enSDTvMode) ? 25 : 30;
    pstVideoLayerAttr->enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    pstVideoLayerAttr->s32PiPChn = VO_DEFAULT_CHN;

    return;
}

HI_S32 SAMPLE_VIO_TDE_1Screen_VoVGA()
{
    VB_CONF_S stVbConf   = {0};

    HI_S32 s32ViChnTotal = 1;

    HI_S32 s32VoChnTotal = 1;

    VO_DEV VoDev;

    VO_CHN VoChn;

    VI_DEV ViDev;

    VI_CHN ViChn;

    VI_PUB_ATTR_S stViDevAttr;

    VI_CHN_ATTR_S stViChnAttr;

    VO_PUB_ATTR_S stVoDevAttr;

    VO_VIDEO_LAYER_ATTR_S stVideoLayerAttr;

    VIDEO_FRAME_INFO_S stFrame;

    HI_S32 s32Ret = HI_SUCCESS;

    HI_S32 u32PhyAddr;

    stVbConf.astCommPool[0].u32BlkSize = 704 * 576 * 2;

    stVbConf.astCommPool[0].u32BlkCnt  = 20;

    stVbConf.astCommPool[1].u32BlkSize = 384 * 288 * 2;

    stVbConf.astCommPool[1].u32BlkCnt = 20;

    if (HI_SUCCESS != SAMPLE_InitMPP(&stVbConf))
    {
       return -1;
    }

    /* Config VI to input standard-definition video */
    ViDev = 0;

    SAMPLE_GetViCfg_SD(PIC_D1, &stViDevAttr, &stViChnAttr);

    s32Ret = SAMPLE_StartViByChn(s32ViChnTotal, &stViDevAttr, &stViChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    /* display VGA video on vo HD divice. */

    VoDev = VO_DEV_HD;

    SAMPLE_GetVoCfg_VGA_800x600(&stVoDevAttr, &stVideoLayerAttr);

    s32Ret = SAMPLE_StartVo(s32VoChnTotal, VoDev, &stVoDevAttr, &stVideoLayerAttr);

    if (HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    printf("start VI to VO preview \n");

    master_thread_init_ok = 1;

    sleep(3);

	/** 1. 打开TDE */
    HI_TDE2_Open();

    /** 2. 创建Y U V OSD surface */
    g_stOsdYSurface.enColorFmt = TDE2_COLOR_FMT_ARGB1555;

    g_stOsdYSurface.bYCbCrClut = 1;

    g_stOsdYSurface.bAlphaMax255 = HI_TRUE;

    g_stOsdYSurface.enColorSpaceConv = TDE2_ITU_R_BT601_VIDEO;

    g_stOsdUSurface.enColorFmt = TDE2_COLOR_FMT_ARGB1555;

    g_stOsdUSurface.bYCbCrClut = 0;

    g_stOsdUSurface.bAlphaMax255 = HI_TRUE;

    g_stOsdUSurface.enColorSpaceConv = TDE2_ITU_R_BT601_VIDEO;

    g_stOsdVSurface.enColorFmt = TDE2_COLOR_FMT_ARGB1555;

    g_stOsdVSurface.bYCbCrClut = 1;

    g_stOsdVSurface.bAlphaMax255 = HI_TRUE;

    g_stOsdVSurface.enColorSpaceConv = TDE2_ITU_R_BT601_VIDEO;

    while(1)
    {
        TGT_SET_S	*target_set = &gstPeaResult.stTargetSet;
        EVT_SET_S *event_set = &gstPeaResult.stEventSet;
        IMP_S32 s32TargetNum = target_set->s32TargetNum;

		/** 获取图像 */
        if (HI_MPI_VI_GetFrame(ViDev, 0, &stFrame))
        {
            printf("HI_MPI_VI_GetFrame err, vi(%d,%d)\n", ViDev, 0);
            return -1;
        }
        if(s32TargetNum > 0 )
        {
            TDE_HANDLE s32Handle;

			/* 3. 开始TDE任务 */
            s32Handle = HI_TDE2_BeginJob();
            if(HI_ERR_TDE_INVALID_HANDLE == s32Handle)
            {
                printf("start job failed!\n");
                continue;
            }
			/** 4. 配置YUV Surface数据 长度、宽度、stride、物理地址等 */
            u32PhyAddr = stFrame.stVFrame.u32PhyAddr[0];

            g_stOsdYSurface.u32Width = stFrame.stVFrame.u32Width;

            g_stOsdYSurface.u32Height = stFrame.stVFrame.u32Height;

            g_stOsdYSurface.u32Stride = stFrame.stVFrame.u32Stride[0];

            g_stOsdYSurface.u32PhyAddr = u32PhyAddr;

            g_stOsdUSurface.u32Width = stFrame.stVFrame.u32Width>>1;

            g_stOsdUSurface.u32Height = stFrame.stVFrame.u32Height>>1;

            g_stOsdUSurface.u32Stride = stFrame.stVFrame.u32Stride[1];

            g_stOsdUSurface.u32PhyAddr = stFrame.stVFrame.u32PhyAddr[1]+1;

            g_stOsdVSurface = g_stOsdUSurface;

            g_stOsdVSurface.u32PhyAddr = stFrame.stVFrame.u32PhyAddr[1];

			/** 5. 通过TDE绘制修改YUV数据显示PEA数据结果 TDE只支持绘制矩形区域、直线等*/
            IMP_TDE_DrawPeaResult( s32Handle, &g_stOsdYSurface,&g_stOsdVSurface, &gstPeaResult,IMP_D1,IMP_QCIF );

			/** 6. 直接修改YUV数据显示PEA数据结果 绘制轨迹线、用户配置的规则信息等*/
            IMP_DrawPeaResult( &stFrame.stVFrame, &gstPeaResult,IMP_D1,IMP_QCIF );

            /** 7. 结束TDE任务 */
            s32Ret = HI_TDE2_EndJob(s32Handle, HI_FALSE, HI_TRUE, 10);
            if(s32Ret < 0)
            {

                printf("Line:%d,HI_TDE2_EndJob failed,ret=0x%x!\n", __LINE__, s32Ret);

                HI_TDE2_CancelJob(s32Handle);
                return ;
            }
        }
		/** 8. 将修改后的YUV数据送给VO设备 */
        HI_MPI_VO_SendFrame( VoDev,0,&stFrame);

		/** 9. 释放图像 */
        HI_MPI_VI_ReleaseFrame(ViDev, 0, &stFrame);
    }
	/** 10. 关闭TDE */
    HI_TDE2_Close();

    return HI_SUCCESS;
}
void SERVICE_PROCESS()
{
    return;
    while(1)
    {
        int cnt = 3000000;
        int i = 0;
        int sum = 0;
        for( i = 0; i < cnt;i++ )
        {
            sum = sum+1;
        }
        usleep(100);
    }

}
void thread_create(void)
{
     int temp;
     memset(&thread, 0, sizeof(thread));

     if((temp = pthread_create(&thread[0], NULL, SAMPLE_VIO_1Screen_VoVGA, NULL)) != 0)
         printf(" main thread create error\n");
     else
          printf("main thread create ok\n");

     if((temp = pthread_create(&thread[1], NULL, PEA_ALGO_PROCESS, NULL)) != 0)
         printf("pea algo thread create error\n");
     else
          printf("pea algo thread create ok\n");

    if((temp = pthread_create(&thread[2], NULL, SERVICE_PROCESS, NULL)) != 0)
         printf("pea algo thread create error\n");
     else
          printf("pea algo thread create ok\n");
}
void thread_wait(void)
{
    if(thread[0] !=0)
    {
        pthread_join(thread[0],NULL);
    }
    if(thread[1] !=0)
    {
        pthread_join(thread[1],NULL);
    }
    if(thread[2] !=0)
    {
        pthread_join(thread[2],NULL);
    }
}
void main()
{

    pthread_mutex_init(&mut,NULL);

    thread_create();

    thread_wait();

	return 0;
}
