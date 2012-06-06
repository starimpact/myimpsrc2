
#include "imp_avd.h"
#define MAX_NUM_AVD_CHANNEL 8
IMP_S32 s32AVDnCh = 0;
IMP_HANDLE IMP_AVD_Create(IMP_S32 s32Width, IMP_S32 s32Height)
{
    IMP_HANDLE hModule = NULL;
	AVD_MODULE *hAvd;
#ifdef TD_LICENSE_CHECK
	IMP_S32 s32Lic = IMP_CheckLicenseByMac();
    if( s32Lic != IMP_STATUS_OK)
	{
		printf("License Check Failed!\n");
		return NULL;
	}
#endif
	s32AVDnCh++;
    if( s32AVDnCh> MAX_NUM_AVD_CHANNEL )
        {
                printf("Reached AVD channel limit!\n");
                return NULL;
        }

    hModule = AVD_Initialize(s32Width,s32Height);
	hAvd = (AVD_MODULE*)hModule;

    if( s32AVDnCh ==1 )
        {
 	    IMP_GA_InitDitherTab();
	}
	AVD_setDefaultUserParameter( &hAvd->Para );

    return hModule;
}

IMP_VOID IMP_AVD_Release(IMP_HANDLE hModule)
{
     AVD_Terminate((AVD_MODULE* )hModule);
	s32AVDnCh--;
	if(s32AVDnCh==0)
	{
     		IMP_GA_FreeDitherTab();
	}
}
#if 1
IMP_VOID IMP_PrintfAvdParameter(IMP_AVD_PARA_S *stAvdPara)
{
	/** 设置AVD红外切换触发方式，0为外部触发，1为自动触发 */
	printf("stAvdPara->stTrigModePara.u32IrTrigMode = %d\n",stAvdPara->stTrigModePara.u32IrTrigMode);
	/** 设置AVD场景切换触发方式，0为外部触发，1为自动触发，2为混合触发 */
	printf("stAvdPara->stTrigModePara.u32ScnTrigMode = %d\n",stAvdPara->stTrigModePara.u32ScnTrigMode);

	/** 3.设置AVD红外检测模式 */
	/** 可见光-红外自动切换平衡级别（1-5级），1为倾向于被判断为可见光模式，5为倾向于被判断为红外模式，默认为3 */
	printf("stAvdPara->stIrModePara.u32BalanceLevel = %d\n",stAvdPara->stIrModePara.u32BalanceLevel);

	/** 4.设置AVD亮度异常检测参数 */
	/** 亮度异常检测功能开关，0为关闭，1为开启 */
	printf("stAvdPara->stBrightAbnmlPara.u32Enable = %d\n",stAvdPara->stBrightAbnmlPara.u32Enable);
	/** 亮度异常检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警 */
	printf("stAvdPara->stBrightAbnmlPara.u32AlarmLevel = %d\n",stAvdPara->stBrightAbnmlPara.u32AlarmLevel);
	/** 亮度异常报警时间，（0-20秒），数值越大需要的报警时间越长 */
	printf("stAvdPara->stBrightAbnmlPara.u32AlarmTime = %d\n",stAvdPara->stBrightAbnmlPara.u32AlarmTime);

	/** 5.设置AVD清晰度检测参数 */
	/** 清晰度检测功能开关，0为关闭，1为开启 */
	printf("stAvdPara->stClarityPara.u32Enable = %d\n",stAvdPara->stClarityPara.u32Enable);
	/** 清晰度检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警 */
	printf("stAvdPara->stClarityPara.u32AlarmLevel = %d\n",stAvdPara->stClarityPara.u32AlarmLevel);
	/** 清晰度检测报警时间，（0-20秒），数值越大需要的报警时间越长 */
	printf("stAvdPara->stClarityPara.u32AlarmTime = %d\n",stAvdPara->stClarityPara.u32AlarmTime);

	/** 6.设置AVD偏色检测参数 */
	/** 偏色检测功能开关，0为关闭，1为开启 */
	printf("stAvdPara->stColorPara.u32Enable = %d\n",stAvdPara->stColorPara.u32Enable);
	/** 偏色检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警 */
	printf("stAvdPara->stColorPara.u32AlarmLevel = %d\n",stAvdPara->stColorPara.u32AlarmLevel);
	/** 偏色检测报警时间，（0-20秒），数值越大需要的报警时间越长 */
	printf("stAvdPara->stColorPara.u32AlarmTime = %d\n",stAvdPara->stColorPara.u32AlarmTime);

	/** 7.设置AVD噪声检测参数 */
	/** 噪声检测功能开关，0为关闭，1为开启 */
	printf("stAvdPara->stNoisePara.u32Enable = %d\n",stAvdPara->stNoisePara.u32Enable);
	/** 噪声检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警 */
	printf("stAvdPara->stNoisePara.u32AlarmLevel = %d\n",stAvdPara->stNoisePara.u32AlarmLevel);
	/** 噪声检测报警时间，（0-20秒），数值越大需要的报警时间越长 */
	printf("stAvdPara->stNoisePara.u32AlarmTime = %d\n",stAvdPara->stNoisePara.u32AlarmTime);

	/** 8.设置AVD画面冻结检测参数 */
	/** 画面冻结检测功能开关，0为关闭，1为开启 */
	printf("stAvdPara->stFreezePara.u32Enable = %d\n",stAvdPara->stFreezePara.u32Enable);
	/** 画面冻结检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警 */
	printf("stAvdPara->stFreezePara.u32AlarmLevel = %d\n",stAvdPara->stFreezePara.u32AlarmLevel);
	/** 画面冻结报警时间，（0-20秒），数值越大需要的报警时间越长 */
	printf("stAvdPara->stFreezePara.u32AlarmTime = %d\n",stAvdPara->stFreezePara.u32AlarmTime);

	/** 9.设置AVD信号缺失检测参数 */
	/** 信号缺失检测功能开关，0为关闭，1为开启 */
	printf("stAvdPara->stNosignalPara.u32Enable = %d\n",stAvdPara->stNosignalPara.u32Enable );
	/** 信号缺失检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警 */
	printf("stAvdPara->stNosignalPara.u32AlarmLevel = %d\n",stAvdPara->stNosignalPara.u32AlarmLevel);
	/** 信号缺失检测报警时间，（0-20秒），数值越大需要的报警时间越长 */
	printf("stAvdPara->stNosignalPara.u32AlarmTime = %d\n",stAvdPara->stNosignalPara.u32AlarmTime );

	/** 10.设置AVD场景变换检测参数 */
	/** 场景变换检测功能开关，0为关闭，1为开启 */
	printf("stAvdPara->stChangePara.u32Enable = %d\n",stAvdPara->stChangePara.u32Enable);
	/** 场景变换检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警 */
	printf("stAvdPara->stChangePara.u32AlarmLevel = %d\n",stAvdPara->stChangePara.u32AlarmLevel );

	/** 11.设置AVD人为干扰检测参数 */
	/** 人为干扰检测功能开关，0为关闭，1为开启 */
	printf("stAvdPara->stInterferePara.u32Enable = %d\n",stAvdPara->stInterferePara.u32Enable);
	/** 人为干扰检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警 */
	printf("stAvdPara->stInterferePara.u32AlarmLevel = %d\n",stAvdPara->stInterferePara.u32AlarmLevel);
	/** 人为干扰检测报警时间，（0-20秒），数值越大需要的报警时间越长 */
	printf("stAvdPara->stInterferePara.u32AlarmTime = %d\n",stAvdPara->stInterferePara.u32AlarmTime);

	/** 12.设置AVD PTZ失控检测参数 */
	/** PTZ失控检测功能开关，0为关闭，1为开启 */
	printf("stAvdPara->stPtzLoseCtlPara.u32Enable = %d\n",stAvdPara->stPtzLoseCtlPara.u32Enable);
	/** 亮PTZ失控检测功能灵敏度档位，1-5档，数值越大越灵敏越易报警 */
	printf("stAvdPara->stPtzLoseCtlPara.u32AlarmLevel = %d\n",stAvdPara->stPtzLoseCtlPara.u32AlarmLevel);
	/** PTZ失控检测报警时间，（0-20秒），数值越大需要的报警时间越长 */
	printf("stAvdPara->stPtzLoseCtlPara.u32AlarmTime = %d\n",stAvdPara->stPtzLoseCtlPara.u32AlarmTime);
}
#endif
IMP_BOOL IMP_AVD_ConfigParameter(IMP_HANDLE hModule, IMP_AVD_PARA_S *pstAvdParaSrc)
{
	AVD_MODULE *hAvd = (AVD_MODULE*)hModule;

    if( pstAvdParaSrc == NULL || hAvd == NULL)
        return IMP_FALSE;

	AVD_checkUserParameter( pstAvdParaSrc );

    memcpy( &hAvd->Para, pstAvdParaSrc, sizeof(IMP_AVD_PARA_S));
 //   IMP_PrintfAvdParameter(&hAvd->Para);
	AVD_setAdvancedParameter( &hAvd->Para, &hAvd->Adv_Para );
    return IMP_TRUE;
}

extern unsigned char *pYData;
void ConvertYUVtoRGB (unsigned char *src0, unsigned char *src1, unsigned char *src2,
                      unsigned char *dst_ori,int width,int height);
static void IMP_Yuv420Split(YUV_IMAGE420_S *image, GRAY_IMAGE_S *YImg, GRAY_IMAGE_S *UImg, GRAY_IMAGE_S *VImg);
#define AVD_TIME_LIMIT 7 * 24 * 3600 * 2

static IMP_VOID AVD_refineResult(IMP_HANDLE hModule, EXT_TRIGGERS_S *pstExtTriggers);
static IMP_VOID	AVD_alarmAnalysis(IMP_HANDLE hModule,IMP_U32 u32ExtScnChg);
extern IMP_BOOL AVD_irAnalysis( AVD_MODULE* avdModule );
AVD_TransfImgIntoPAL(AVD_MODULE *hAvd)
{
    hAvd->RGBImg.s32H = IMP_QCIF_PAL_IMG_HEIGHT;
    hAvd->YImg.s32H = IMP_QCIF_PAL_IMG_HEIGHT;
    hAvd->UImg.s32H = IMP_QCIF_PAL_IMG_HEIGHT/4;
    hAvd->VImg.s32H = IMP_QCIF_PAL_IMG_HEIGHT/4;

    hAvd->YImgLast.s32H = IMP_QCIF_PAL_IMG_HEIGHT;
    hAvd->UImgLast.s32H = IMP_QCIF_PAL_IMG_HEIGHT/4;
    hAvd->VImgLast.s32H = IMP_QCIF_PAL_IMG_HEIGHT/4;

    hAvd->RImg.s32H = IMP_QCIF_PAL_IMG_HEIGHT;
    hAvd->GImg.s32H = IMP_QCIF_PAL_IMG_HEIGHT;
    hAvd->BImg.s32H = IMP_QCIF_PAL_IMG_HEIGHT;
    hAvd->ClrCastImg.s32H = IMP_QCIF_PAL_IMG_HEIGHT;

    hAvd->EdgeImgMag.s32H = IMP_QCIF_PAL_IMG_HEIGHT;
    hAvd->EdgeImgGrad.s32H = IMP_QCIF_PAL_IMG_HEIGHT;
    hAvd->EdgeImg.s32H = IMP_QCIF_PAL_IMG_HEIGHT;

    hAvd->RefYImg.s32H = IMP_QCIF_PAL_IMG_HEIGHT;
    hAvd->RefUImg.s32H = IMP_QCIF_PAL_IMG_HEIGHT/4;
    hAvd->RefVImg.s32H = IMP_QCIF_PAL_IMG_HEIGHT/4;

    hAvd->YImgDiffNxt.s32H = IMP_QCIF_PAL_IMG_HEIGHT;
    hAvd->UImgDiffNxt.s32H = IMP_QCIF_PAL_IMG_HEIGHT/4;
    hAvd->VImgDiffNxt.s32H = IMP_QCIF_PAL_IMG_HEIGHT/4;

    hAvd->YImgDiffNxt2.s32H = IMP_QCIF_PAL_IMG_HEIGHT;

    hAvd->GrayImgDiff.s32H = IMP_QCIF_PAL_IMG_HEIGHT;
#ifdef DEBUG_AVD_SHOW_FRM_DIFF
    hAvd->GrayImgDiff3.s32H = IMP_QCIF_PAL_IMG_HEIGHT;
#endif
    hAvd->UImgDiff.s32H = IMP_QCIF_PAL_IMG_HEIGHT/4;
    hAvd->VImgDiff.s32H = IMP_QCIF_PAL_IMG_HEIGHT/4;

}

AVD_TransfImgIntoNTSC(AVD_MODULE *hAvd)
{
    hAvd->RGBImg.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;
    hAvd->YImg.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;
    hAvd->UImg.s32H = IMP_QCIF_NTSC_IMG_HEIGHT/4;
    hAvd->VImg.s32H = IMP_QCIF_NTSC_IMG_HEIGHT/4;

    hAvd->YImgLast.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;
    hAvd->UImgLast.s32H = IMP_QCIF_NTSC_IMG_HEIGHT/4;
    hAvd->VImgLast.s32H = IMP_QCIF_NTSC_IMG_HEIGHT/4;

    hAvd->RImg.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;
    hAvd->GImg.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;
    hAvd->BImg.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;
    hAvd->ClrCastImg.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;

    hAvd->EdgeImgMag.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;
    hAvd->EdgeImgGrad.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;
    hAvd->EdgeImg.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;

    hAvd->RefYImg.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;
    hAvd->RefUImg.s32H = IMP_QCIF_NTSC_IMG_HEIGHT/4;
    hAvd->RefVImg.s32H = IMP_QCIF_NTSC_IMG_HEIGHT/4;

    hAvd->YImgDiffNxt.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;
    hAvd->UImgDiffNxt.s32H = IMP_QCIF_NTSC_IMG_HEIGHT/4;
    hAvd->VImgDiffNxt.s32H = IMP_QCIF_NTSC_IMG_HEIGHT/4;

	hAvd->YImgDiffNxt2.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;

    hAvd->GrayImgDiff.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;
#ifdef DEBUG_AVD_SHOW_FRM_DIFF
    hAvd->GrayImgDiff3.s32H = IMP_QCIF_NTSC_IMG_HEIGHT;
#endif
    hAvd->UImgDiff.s32H = IMP_QCIF_NTSC_IMG_HEIGHT/4;
    hAvd->VImgDiff.s32H = IMP_QCIF_NTSC_IMG_HEIGHT/4;

}

IMP_BOOL IMP_AVD_Process(IMP_HANDLE hModule, YUV_IMAGE420_S *pstImage, 	EXT_TRIGGERS_S *pstExtTriggers )
{
	AVD_MODULE *hAvd = (AVD_MODULE*)hModule;

    // N-P MODE SWITCH
    if(pstImage->s32H == IMP_QCIF_NTSC_IMG_HEIGHT )
    {
        hAvd->PMode = IMP_FALSE;
//        printf("This is N\n");
        AVD_TransfImgIntoNTSC(hAvd);
    }
    else
    {
        hAvd->PMode = IMP_TRUE;
//        printf("This is P\n");
    }

//    printf("AVD:W%d,H%d,uvW%d,uvH%d\n",hAvd->imgW,hAvd->imgH,hAvd->UImg.s32W,hAvd->UImg.s32H);

#ifdef USE_TIME_LICENSE_LIMIT
    static IMP_S32 s32FrameCnt = 0;

    s32FrameCnt++;
    if( s32FrameCnt > AVD_TIME_LIMIT )
    {
        return IMP_FALSE;
    }
#endif

	//保存上帧图像
	IMP_GrayImageClone(&hAvd->YImg,&hAvd->YImgLast);
	IMP_GrayImageClone(&hAvd->UImg,&hAvd->UImgLast);
	IMP_GrayImageClone(&hAvd->VImg,&hAvd->VImgLast);

    //ipConvertImageYUV2RGB(pstImage,&hAvd->RGBImg);
    IMP_Yuv420Split(pstImage, &hAvd->YImg, &hAvd->UImg, &hAvd->VImg);
    //pYData = hAvd->YImg.pu8Data;
    ipConvertYUVtoRGB(pstImage->pu8Y,pstImage->pu8U,pstImage->pu8V,hAvd->RGBImg.pu8Data,pstImage->s32W,pstImage->s32H);

    //pYData = hAvd->YImg.pu8Data;

	AVD_Transform(hAvd,pstExtTriggers->u32ExtScnChg);
   // printf("flag_change = %d\n",hAvd->flag_change);
	// IR切换触发
	if (hAvd->Para.stTrigModePara.u32IrTrigMode == EXT_SCN_CHG)
	{
		hAvd->IR_mode = pstExtTriggers->u32ExtIrState;					  //外部触发
	}
	else
	{
		hAvd->IR_mode = AVD_irAnalysis(hAvd);							  //内部检测
	}
	if(hAvd->IR_mode_old != hAvd->IR_mode)
	{
		hAvd->IR_mode_old = hAvd->IR_mode;	//记录本次IR状态
		hAvd->IR_change = TRUE;
		hAvd->flag_change = SCN_CHANGE;	//切换场景
//		return IMP_TRUE;
	}
	else
	{
		hAvd->IR_mode_old = hAvd->IR_mode;	//记录本次IR状态
		hAvd->IR_change = FALSE;
	}
//   printf("flag_change00 = %d\n",hAvd->flag_change);

	// 场景切换触发
	if (hAvd->Para.stTrigModePara.u32ScnTrigMode == EXT_SCN_CHG)		  //外部触发
	{
		hAvd->flag_change = pstExtTriggers->u32ExtScnChg;
		//printf("flag_change11 = %d\n",hAvd->flag_change);
	}
	else if (hAvd->Para.stTrigModePara.u32ScnTrigMode == AUTO_SCN_CHG)    //内部检测
	{
		hAvd->flag_change = AVD_changeAnalysis(hAvd);
	//	printf("flag_change22= %d\n",hAvd->flag_change);
	}
	else if (hAvd->Para.stTrigModePara.u32ScnTrigMode == BOTH_SCN_CHG)    //兼而有之
	{
		hAvd->flag_change = pstExtTriggers->u32ExtScnChg || AVD_changeAnalysis(hAvd);
	//	printf("flag_change33 = %d\n",hAvd->flag_change);
	}

 	if (pstExtTriggers->u32ExtScnChg)
	{
        memset(&hAvd->stAvdUpperAnalysis,0,sizeof(AVD_Upper_ANALYSIS_S));
	}

	AVD_brightAnalysis(hAvd);
	AVD_clarityAnalysis(hAvd);
	AVD_colorAnalysis(hAvd);
	AVD_noiseAnalysis(hAvd);
	AVD_freezeAnalysis(hAvd);
	AVD_noSignalAnalysis(hAvd);
	AVD_ptzAnalysis(hAvd,pstExtTriggers->u32ExtScnChg);
	AVD_interfereAnalysis(hAvd,pstExtTriggers->u32ExtScnChg);

	// 修正数据结果
	AVD_refineResult(hAvd, pstExtTriggers);

	// 高级报警分析
	AVD_alarmAnalysis(hAvd,pstExtTriggers->u32ExtScnChg);


	AVD_TransfImgIntoPAL(hAvd);

#if defined(DEBUG_AVD_ANALOG_ALARM_INFO)||defined(DEBUG_AVD_DIGITAL_ALARM_INFO)
	printf("\n");
#endif
   return IMP_TRUE;
}

IMP_VOID AVD_AnalogAnalysisProcess(EVT_SET_S *pstEventSet, AVD_SCORE_S *pstScore, IMP_U8 bChange)
{
	IMP_U8 bFlagAlarm = IMP_FALSE;
	IMP_S32 s32AlarmCnt = pstEventSet->s32EventNum;

	if (  pstScore->s32Value > pstScore->u32AlarmTh )		// 单帧判据
	{
		pstScore->u32Score++;
		if (pstScore->u32Score >=pstScore->u32FrmCnt)
		{
			pstScore->u32Score = pstScore->u32FrmCnt;
		}
	}
	else
	{
		if (pstScore->u32Score>0)
		{
			pstScore->u32Score--;
		}
	}
#ifdef DEBUG_AVD_ANALOG_ALARM_INFO
    printf("Value=%3d, AlarmTh=%3d, Score=%3d, Ratio=%3d, FrmCnt=%3d ",pstScore->s32Value,pstScore->u32AlarmTh,pstScore->u32Score,pstScore->u32Ratio,pstScore->u32FrmCnt);
#endif
	if (pstScore->u32Score*100 > pstScore->u32Ratio*pstScore->u32FrmCnt && pstScore->u32FrmCnt > 3)	// 计算异常帧比例
	{
		pstEventSet->astEvents[s32AlarmCnt].u32Type = pstScore->u32Type;
		if (!pstScore->bFlagStart)	// 写入START
		{
			pstEventSet->astEvents[s32AlarmCnt].u32Status = IMP_EVT_STATUS_START;
			pstScore->bFlagStart = IMP_TRUE;
#ifdef DEBUG_AVD_ANALOG_ALARM_INFO
			printf(" START!!");
#endif
		}
		else						// 写入PROCEDURE
		{
			pstEventSet->astEvents[s32AlarmCnt].u32Status = IMP_EVT_STATUS_PROCEDURE;
#ifdef DEBUG_AVD_ANALOG_ALARM_INFO
			printf(" PROC!!");
#endif
		}

		bFlagAlarm = IMP_TRUE;
	}
	else // 小于百分比得分阈值，写入END
	{
		if(pstScore->bFlagStart)
		{
			pstEventSet->astEvents[s32AlarmCnt].u32Type = pstScore->u32Type;
			pstEventSet->astEvents[s32AlarmCnt].u32Status = IMP_EVT_STATUS_END;
			pstScore->bFlagStart = IMP_FALSE;
			bFlagAlarm = IMP_TRUE;
#ifdef DEBUG_AVD_ANALOG_ALARM_INFO
			printf(" END!!");
#endif
		}
	}

	// 场景切换，写入END
	if (pstScore->bFlagStart && bChange )
	{
		pstEventSet->astEvents[s32AlarmCnt].u32Type = pstScore->u32Type;
		pstEventSet->astEvents[s32AlarmCnt].u32Status = IMP_EVT_STATUS_END;
		pstScore->bFlagStart = IMP_FALSE;
		pstScore->u32Score = 0;
		bFlagAlarm = IMP_TRUE;
#ifdef DEBUG_AVD_ANALOG_ALARM_INFO
		printf(" END!!");
#endif
	}
#ifdef DEBUG_AVD_ANALOG_ALARM_INFO
	printf(" \n");
#endif
	if (bFlagAlarm)
	{
#ifdef DEBUG_AVD_ANALOG_ALARM_INFO
	//	printf("a=%d,f=%d,percent=%.3f\n",pstScore->u32Score,pstScore->u32FrmCnt,(float)(pstScore->u32Score/pstScore->u32FrmCnt));
	//	printf("%x = %d\n",pstScore->u32Type,pstEventSet->astEvents[s32AlarmCnt].u32Status );
#endif
		s32AlarmCnt++;
#ifdef DEBUG_AVD_ANALOG_ALARM_INFO
	//	printf("AlarmCnt=%d\n",s32AlarmCnt);
#endif
	}
	pstEventSet->s32EventNum = s32AlarmCnt;
}

IMP_VOID AVD_DigitalAnalysisProcess(EVT_SET_S *pstEventSet, AVD_SCORE_S *pstScore, IMP_U8 bChange)
{
	IMP_U8 bFlagAlarm = IMP_FALSE;
	IMP_S32 s32AlarmCnt = pstEventSet->s32EventNum;

	if (  pstScore->s32Value )		// 单帧判据
	{
		pstScore->u32Score++;
		if (pstScore->u32Score >= pstScore->u32FrmCnt)
		{
			pstScore->u32Score = pstScore->u32FrmCnt;
		}
	}
	else
	{
		if (pstScore->u32Score>0)
		{
			pstScore->u32Score--;
		}
	}
#ifdef DEBUG_AVD_DIGITAL_ALARM_INFO
    printf("Value=%3d, AlarmTh=%3d, Score=%3d, Ratio=%3d, FrmCnt=%3d ",pstScore->s32Value,pstScore->u32AlarmTh,pstScore->u32Score,pstScore->u32Ratio,pstScore->u32FrmCnt);
#endif

	if (pstScore->u32Score)			// 计算异常帧比例
	{
		pstEventSet->astEvents[s32AlarmCnt].u32Type = pstScore->u32Type;
		if (!pstScore->bFlagStart)	// 写入START
		{
			pstEventSet->astEvents[s32AlarmCnt].u32Status = IMP_EVT_STATUS_START;
			pstScore->bFlagStart = IMP_TRUE;
#ifdef DEBUG_AVD_DIGITAL_ALARM_INFO
			printf(" START!!");
#endif
		}
		else						// 写入PROCEDURE
		{
			pstEventSet->astEvents[s32AlarmCnt].u32Status = IMP_EVT_STATUS_PROCEDURE;
#ifdef DEBUG_AVD_DIGITAL_ALARM_INFO
			printf(" PROC!!");
#endif
		}

		bFlagAlarm = IMP_TRUE;
	}
	else // 小于百分比得分阈值，写入END
	{
		if(pstScore->bFlagStart)
		{
			pstEventSet->astEvents[s32AlarmCnt].u32Type = pstScore->u32Type;
			pstEventSet->astEvents[s32AlarmCnt].u32Status = IMP_EVT_STATUS_END;
			pstScore->bFlagStart = IMP_FALSE;
			bFlagAlarm = IMP_TRUE;
#ifdef DEBUG_AVD_DIGITAL_ALARM_INFO
			printf(" END!!");
#endif
		}
	}

	// 场景切换，写入END
	if (pstScore->bFlagStart && bChange )
	{
		pstEventSet->astEvents[s32AlarmCnt].u32Type = pstScore->u32Type;
		pstEventSet->astEvents[s32AlarmCnt].u32Status = IMP_EVT_STATUS_END;
		pstScore->bFlagStart = IMP_FALSE;
		pstScore->u32Score = 0;
#ifdef DEBUG_AVD_DIGITAL_ALARM_INFO
		printf(" END!!");
#endif
	}

#ifdef DEBUG_AVD_DIGITAL_ALARM_INFO	
	printf("\n");
#endif

	if (bFlagAlarm)
	{
#ifdef DEBUG_AVD_DIGITAL_ALARM_INFO
	//	printf("a=%d,f=%d,percent=%.3f\n",pstScore->u32Score,pstScore->u32FrmCnt,(float)(pstScore->u32Score/pstScore->u32FrmCnt));
	//	printf("%x = %d\n",pstScore->u32Type,pstEventSet->astEvents[s32AlarmCnt].u32Status );
#endif
		s32AlarmCnt++;
	}
	pstEventSet->s32EventNum = s32AlarmCnt;
	return;
}

#define ANALYSISI_TIME 5
static IMP_VOID	AVD_alarmAnalysis(IMP_HANDLE hModule,IMP_U32 u32ExtScnChg)
{
    AVD_MODULE *hAvd = (AVD_MODULE*)hModule;
	AVD_STATCL_DATA_S *pstStatclData = &hAvd->stAvdResult.stStatclData;
	EVT_SET_S *pstEventSet = &hAvd->stAvdResult.stEventSet;
	EVT_SET_S *pstEventSetOld = &hAvd->stAvdResultOld.stEventSet;
	IMP_AVD_PARA_S *pstUsrPara = &hAvd->Para;

	AVD_SCORE_S *pstChangeScore = &hAvd->stAvdUpperAnalysis.stChangeScore;
	AVD_SCORE_S *pstBrightHScore = &hAvd->stAvdUpperAnalysis.stBrightHScore;
	AVD_SCORE_S *pstBrightLScore = &hAvd->stAvdUpperAnalysis.stBrightLScore;
	AVD_SCORE_S *pstColorScore = &hAvd->stAvdUpperAnalysis.stColorScore;
	AVD_SCORE_S *pstClarityScore = &hAvd->stAvdUpperAnalysis.stClarityScore;
	AVD_SCORE_S *pstNoiseScore = &hAvd->stAvdUpperAnalysis.stNoiseScore;
	AVD_SCORE_S *pstFreezeScore = &hAvd->stAvdUpperAnalysis.stFreezeScore;
	AVD_SCORE_S *pstNoSignalScore = &hAvd->stAvdUpperAnalysis.stNoSignalScore;
	AVD_SCORE_S *pstPtzScore = &hAvd->stAvdUpperAnalysis.stPtzScore;
	AVD_SCORE_S *pstInterfereScore = &hAvd->stAvdUpperAnalysis.stInterfereScore;

	IMP_U8 u8FrmCnt = hAvd->prcFrmCnt;
	IMP_U8 flagAlarm = FALSE;

	// 清空
	memset(pstEventSet,0,sizeof(EVT_SET_S));


	// 1.场景变换
	if (hAvd->Para.stChangePara.u32Enable)
	{
		pstChangeScore->s32Value = hAvd->stAvdResult.stStatclData.s32SceneChg;
		//pstChangeScore->u32AlarmTh = 0;
		pstChangeScore->u32FrmCnt = ANALYSISI_TIME;
		//pstChangeScore->u32Ratio = 80;
		pstChangeScore->u32Type = IMP_EVT_TYPE_AlarmSceneChg;
#ifdef	DEBUG_AVD_DIGITAL_ALARM_INFO
		printf("Change: ");
		printf("L=%d, ",hAvd->Para.stChangePara.u32AlarmLevel);
#endif
		AVD_DigitalAnalysisProcess(pstEventSet, pstChangeScore, u32ExtScnChg);
	}

	// 2.信号有无
	if (hAvd->Para.stNosignalPara.u32Enable)
	{
		pstNoSignalScore->s32Value = hAvd->stAvdResult.stStatclData.s32SignalValue;
		//pstNoSignalScore->u32AlarmTh = 0;
		pstNoSignalScore->u32FrmCnt = ANALYSISI_TIME;
		//pstNoSignalScore->u32Ratio = 80;
		pstNoSignalScore->u32Type = IMP_EVT_TYPE_AlarmNoSignal;
#ifdef	DEBUG_AVD_DIGITAL_ALARM_INFO
		printf("NoSgnl: ");
		printf("L=%d, ",hAvd->Para.stNosignalPara.u32AlarmLevel);
#endif
		AVD_DigitalAnalysisProcess(pstEventSet, pstNoSignalScore, u32ExtScnChg);
	}

	// 3.清晰度
	if (hAvd->Para.stClarityPara.u32Enable)
	{
		pstClarityScore->s32Value = hAvd->stAvdResult.stStatclData.s32ClarityValue;
		pstClarityScore->u32AlarmTh = 60;
		pstClarityScore->u32FrmCnt = ANALYSISI_TIME;
		pstClarityScore->u32Ratio = 60;
		pstClarityScore->u32Type = IMP_EVT_TYPE_AlarmClarityAbnml;
#ifdef	DEBUG_AVD_ANALOG_ALARM_INFO
		printf("Clarty: ");
		printf("L=%d, ",hAvd->Para.stClarityPara.u32AlarmLevel);
#endif
		AVD_AnalogAnalysisProcess(pstEventSet, pstClarityScore, u32ExtScnChg);
	}

	// 4.亮度异常
	if (hAvd->Para.stBrightAbnmlPara.u32Enable)
	{
		// 过亮
		pstBrightHScore->s32Value = hAvd->stAvdResult.stStatclData.s32BrightHValue;
		pstBrightHScore->u32AlarmTh = 40;
		pstBrightHScore->u32FrmCnt = ANALYSISI_TIME;
		pstBrightHScore->u32Ratio = 60;
		pstBrightHScore->u32Type = IMP_EVT_TYPE_AlarmBrightAbnml;
#ifdef	DEBUG_AVD_ANALOG_ALARM_INFO
		printf("BrghtH: ");
		printf("L=%d, ",hAvd->Para.stBrightAbnmlPara.u32AlarmLevel);
#endif
		AVD_AnalogAnalysisProcess(pstEventSet, pstBrightHScore, u32ExtScnChg);
		// 过暗
		pstBrightLScore->s32Value = hAvd->stAvdResult.stStatclData.s32BrightLValue;
		pstBrightLScore->u32AlarmTh = 40;
		pstBrightLScore->u32FrmCnt = ANALYSISI_TIME;
		pstBrightLScore->u32Ratio = 60;
		pstBrightLScore->u32Type = IMP_EVT_TYPE_AlarmBrightAbnml;
#ifdef	DEBUG_AVD_ANALOG_ALARM_INFO
		printf("BrghtL: ");
		printf("L=%d, ",hAvd->Para.stBrightAbnmlPara.u32AlarmLevel);
#endif
		AVD_AnalogAnalysisProcess(pstEventSet, pstBrightLScore, u32ExtScnChg);
	}

	// 5.视频偏色
	if (hAvd->Para.stColorPara.u32Enable)
	{
		pstColorScore->s32Value = hAvd->stAvdResult.stStatclData.s32ColorValue;
	//	printf("hAvd->stAvdResult.stStatclData.s32ColorValue = %d\n",hAvd->stAvdResult.stStatclData.s32ColorValue);
		pstColorScore->u32AlarmTh = 40;
		pstColorScore->u32FrmCnt = ANALYSISI_TIME;
		pstColorScore->u32Ratio = 60;
		pstColorScore->u32Type = IMP_EVT_TYPE_AlarmColorAbnml;
#ifdef	DEBUG_AVD_ANALOG_ALARM_INFO
		printf("Colour: ");
		printf("L=%d, ",hAvd->Para.stColorPara.u32AlarmLevel);
#endif
		AVD_AnalogAnalysisProcess(pstEventSet, pstColorScore, u32ExtScnChg);
	}

	// 6.噪声干扰
	if (hAvd->Para.stNoisePara.u32Enable)
	{
		pstNoiseScore->s32Value = hAvd->stAvdResult.stStatclData.s32NoiseValue;
	//	printf("hAvd->stAvdResult.stStatclData.s32NoiseValue = %d\n",hAvd->stAvdResult.stStatclData.s32NoiseValue);
		pstNoiseScore->u32AlarmTh = 40;
		pstNoiseScore->u32FrmCnt = ANALYSISI_TIME;
		pstNoiseScore->u32Ratio = 60;
		pstNoiseScore->u32Type = IMP_EVT_TYPE_AlarmNoise;
#ifdef	DEBUG_AVD_ANALOG_ALARM_INFO
		printf("Noise : ");
		printf("L=%d, ",hAvd->Para.stNoisePara.u32AlarmLevel);
#endif
		AVD_AnalogAnalysisProcess(pstEventSet, pstNoiseScore, u32ExtScnChg);
	}

	// 7.PTZ失控
	if (hAvd->Para.stPtzLoseCtlPara.u32Enable)
	{
		pstPtzScore->s32Value = hAvd->stAvdResult.stStatclData.s32PtzValue;
		//pstPtzScore->u32AlarmTh = 100;
		pstPtzScore->u32FrmCnt = ANALYSISI_TIME;
		//pstPtzScore->u32Ratio = 80;
		pstPtzScore->u32Type = IMP_EVT_TYPE_AlarmPtzLoseCrl;
#ifdef	DEBUG_AVD_DIGITAL_ALARM_INFO
		printf("PTZCtr: ");
		printf("L=%d, ",hAvd->Para.stPtzLoseCtlPara.u32AlarmLevel);
#endif
		AVD_DigitalAnalysisProcess(pstEventSet, pstPtzScore, u32ExtScnChg);
	}

	// 8.画面冻结
	if (hAvd->Para.stFreezePara.u32Enable)
	{
		pstFreezeScore->s32Value = hAvd->stAvdResult.stStatclData.s32FreezValue;
		//pstPtzScore->u32AlarmTh = 0;
		pstFreezeScore->u32FrmCnt = ANALYSISI_TIME;
		//pstFreezeScore->u32Ratio = 80;
		pstFreezeScore->u32Type = IMP_EVT_TYPE_AlarmFreeze;
#ifdef	DEBUG_AVD_DIGITAL_ALARM_INFO
		printf("Freeze: ");
		printf("L=%d, ",hAvd->Para.stFreezePara.u32AlarmLevel);
#endif
		AVD_DigitalAnalysisProcess(pstEventSet, pstFreezeScore, u32ExtScnChg);
	}

	// 9.人为干扰
	if (hAvd->Para.stInterferePara.u32Enable)
	{
		pstInterfereScore->s32Value = hAvd->stAvdResult.stStatclData.s32InterfereValue;
		//pstPtzScore->u32AlarmTh = 0;
		pstInterfereScore->u32FrmCnt = ANALYSISI_TIME;
		//pstInterfereScore->u32Ratio = 80;
		pstInterfereScore->u32Type = IMP_EVT_TYPE_AlarmInterfere;
#ifdef	DEBUG_AVD_DIGITAL_ALARM_INFO
		printf("Intrfr: ");
		printf("L=%d, ",hAvd->Para.stInterferePara.u32AlarmLevel);
#endif
		AVD_DigitalAnalysisProcess(pstEventSet, pstInterfereScore, u32ExtScnChg);
	}

	// 对报警结果进行存档
	memcpy(&hAvd->stAvdResultOld,&hAvd->stAvdResult,sizeof(AVD_RESULT_S));

}

/*void ipYuv422Split(YUV_IMAGE422_S *image, GRAY_IMAGE_S *YImg, GRAY_IMAGE_S *UImg, GRAY_IMAGE_S *VImg)
{
    IMP_S32 w = image-> s32W;
    IMP_S32 h = image-> s32H;

  //  printf("2.ipYuvSplit\n");
    //Y
    memcpy( YImg->pu8Data, image->pu8Y, w*h );
    //U
    memcpy( UImg->pu8Data, image->pu8U, w*h/2);
    //V
    memcpy( VImg->pu8Data, image->pu8V, w*h/2);
 //   printf("2.ipYuvSplit\n");
}*/

static void IMP_Yuv420Split(YUV_IMAGE420_S *image, GRAY_IMAGE_S *YImg, GRAY_IMAGE_S *UImg, GRAY_IMAGE_S *VImg)
{
    IMP_S32 w = image->s32W;
    IMP_S32 h = image->s32H;
    //Y
 //   printf("(%x %x %x %x %x),(w=%d,h=%d)\n",YImg,YImg->pu8Data,image->pu8Y,UImg->pu8Data,VImg->pu8Data,w,h);
    memcpy( YImg->pu8Data, image->pu8Y, w*h );
    //U
    memcpy( UImg->pu8Data, image->pu8U, w*h/4);
    //V
    memcpy( VImg->pu8Data, image->pu8V, w*h/4);
 //   printf("2.ipYuvSplit\n");
}

IMP_VOID IMP_AVD_GetResult(IMP_HANDLE hModule, AVD_RESULT_S *pstAvdResult)
{
	AVD_MODULE *hAvd = (AVD_MODULE*)hModule;
	memcpy(pstAvdResult,&hAvd->stAvdResult,sizeof(AVD_RESULT_S));
}

static IMP_VOID IMP_FusionResult(AVD_RESULT_S *pstAvdResult);
IMP_VOID AVD_refineResult(IMP_HANDLE hModule, EXT_TRIGGERS_S *pstExtTriggers)
{
	AVD_MODULE *hAvd = (AVD_MODULE*)hModule;
	AVD_RESULT_S *pstAvdResult = &hAvd->stAvdResult;

	pstAvdResult->stStatclData.s32SceneChg = hAvd->flag_change;
    pstAvdResult->stStatclData.s32ClarityValue = hAvd->clarityValue;
    pstAvdResult->stStatclData.s32BrightHValue = hAvd->brightHValue;
    pstAvdResult->stStatclData.s32BrightLValue = hAvd->brightLValue;
    pstAvdResult->stStatclData.s32ColorValue = hAvd->colorValue;
    pstAvdResult->stStatclData.s32NoiseValue = hAvd->noiseValue;
    pstAvdResult->stStatclData.s32FreezValue = hAvd->freezeValue;
	pstAvdResult->stStatclData.s32SignalValue = hAvd->signalValue;
    pstAvdResult->stStatclData.s32PtzValue = hAvd->ptzValue;
	pstAvdResult->stStatclData.s32InterfereValue = hAvd->interfereValue;

	// 使能项清空输出数据
	if(!hAvd->Para.stChangePara.u32Enable)
		pstAvdResult->stStatclData.s32SceneChg = 0;
	if(!hAvd->Para.stBrightAbnmlPara.u32Enable)
	{
		pstAvdResult->stStatclData.s32BrightHValue = 0;
		pstAvdResult->stStatclData.s32BrightLValue = 0;
	}
	if(!hAvd->Para.stClarityPara.u32Enable)
		pstAvdResult->stStatclData.s32ClarityValue = 0;
	if(!hAvd->Para.stColorPara.u32Enable)
		pstAvdResult->stStatclData.s32ColorValue = 0;
	if(!hAvd->Para.stNoisePara.u32Enable)
		pstAvdResult->stStatclData.s32NoiseValue = 0;
	if(!hAvd->Para.stFreezePara.u32Enable)
		pstAvdResult->stStatclData.s32FreezValue = 0;
	if(!hAvd->Para.stNosignalPara.u32Enable)
		pstAvdResult->stStatclData.s32SignalValue = 0;
	if(!hAvd->Para.stPtzLoseCtlPara.u32Enable)
		pstAvdResult->stStatclData.s32PtzValue = 0;
	if(!hAvd->Para.stInterferePara.u32Enable)
		pstAvdResult->stStatclData.s32InterfereValue = 0;

	// 场景切换外部触发，清空报警
	if (
		(  (  hAvd->Para.stTrigModePara.u32ScnTrigMode == EXT_SCN_CHG
		   || hAvd->Para.stTrigModePara.u32ScnTrigMode == BOTH_SCN_CHG )
		&& pstExtTriggers->u32ExtScnChg == SCN_CHANGE )
		|| hAvd->IR_change
	   )
	{
		pstAvdResult->stStatclData.s32SceneChg = 0;
		pstAvdResult->stStatclData.s32SignalValue = 0;
		pstAvdResult->stStatclData.s32ClarityValue = 0;
		pstAvdResult->stStatclData.s32BrightHValue = 0;
		pstAvdResult->stStatclData.s32BrightLValue = 0;
		pstAvdResult->stStatclData.s32ColorValue = 0;
		pstAvdResult->stStatclData.s32NoiseValue = 0;
		pstAvdResult->stStatclData.s32FreezValue = 0;
		pstAvdResult->stStatclData.s32PtzValue = 0;
		pstAvdResult->stStatclData.s32InterfereValue = 0;
	}
    IMP_FusionResult(pstAvdResult);

}
static IMP_VOID IMP_FusionResult(AVD_RESULT_S *pstAvdResult)
{
//     if(pstAvdResult->stStatclData.s32SceneChg)
//     {
//         pstAvdResult->stStatclData.s32SignalValue = 0;
//         pstAvdResult->stStatclData.s32ClarityValue = 0;
//         pstAvdResult->stStatclData.s32BrightHValue = 0;
//         pstAvdResult->stStatclData.s32BrightLValue = 0;
//         pstAvdResult->stStatclData.s32ColorValue = 0;
//         pstAvdResult->stStatclData.s32NoiseValue = 0;
//         pstAvdResult->stStatclData.s32FreezValue = 0;
//  //       pstAvdResult->stStatclData.s32PtzValue = 0;
// 		pstAvdResult->stStatclData.s32InterfereValue = 0;
//     }
    if(pstAvdResult->stStatclData.s32SignalValue)
    {
        pstAvdResult->stStatclData.s32ClarityValue = 0;
        pstAvdResult->stStatclData.s32BrightHValue = 0;
        pstAvdResult->stStatclData.s32BrightLValue = 0;
        pstAvdResult->stStatclData.s32ColorValue = 0;
        pstAvdResult->stStatclData.s32NoiseValue = 0;
        pstAvdResult->stStatclData.s32FreezValue = 0;
        pstAvdResult->stStatclData.s32PtzValue = 0;
		pstAvdResult->stStatclData.s32InterfereValue = 0;
    }
    if(pstAvdResult->stStatclData.s32FreezValue)
    {
        pstAvdResult->stStatclData.s32ClarityValue = 0;
        pstAvdResult->stStatclData.s32BrightHValue = 0;
        pstAvdResult->stStatclData.s32BrightLValue = 0;
        pstAvdResult->stStatclData.s32ColorValue = 0;
        pstAvdResult->stStatclData.s32NoiseValue = 0;
        pstAvdResult->stStatclData.s32PtzValue = 0;
		pstAvdResult->stStatclData.s32InterfereValue = 0;
    }
}
